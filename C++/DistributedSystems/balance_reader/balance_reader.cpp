//============================================================================
// Name        : balance_reader.cpp
// Author      : Gray, Cameron C.
//             : Jakub Lukac
// Version     : 1.0
// Copyright   : ICP-3029, School of Computer Science, Bangor University
// Description : Base Program - Read a Balance File (CSV Format) and display
//             : the final balance at the end of processing.
//============================================================================

#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>
#include <iterator>
#include <vector>
#include <iomanip>
#include <cmath>
#include <getopt.h>
#include <pthread.h>

using namespace std;

typedef struct compact_arg {
	char* file_name;
	vector<ifstream::pos_type>* lines_positions;
	size_t start_line;
	size_t chunk_size;
} compact_arg_t;


/*
 * Split a std::string based on a supplied delimeter.
 * s - the string to split.
 * c - the delimeter character.
 * v - the std::vector to place the elements in.
 *
 */
void split(const string& s, char c, vector<string>& v) {
	string::size_type i = 0;
	string::size_type j = s.find(c);

	while (j != string::npos) {
		v.push_back(s.substr(i, j - i));
		i = ++j;
		j = s.find(c, j);

		if (j == string::npos)
			v.push_back(s.substr(i, s.length()));
	}
}


/*
 * Read file line by line.
 */
double count_values(ifstream& file, vector<ifstream::pos_type>& lines_positions, size_t start_line, size_t chunk_size) {
	// Set start position of input stream
	file.seekg(lines_positions[start_line]);

	// DEBUG
	size_t i = 0;

	string line;
	double sum = 0.0;
	size_t end_line = start_line + chunk_size;
	if (file.is_open()) {
		for (size_t current_line = start_line; current_line < end_line; ++current_line) {
			++i;
			vector<string> parts;
			getline(file, line);

			// ... and process out the 4th element in the CSV.
			split(line, ',', parts);
			if (parts.size() != 3) {
				cerr << "error: Unable to process line " << current_line
						<< ", line is malformed. " << parts.size()
						<< " parts found."<< endl;
				continue;
			}

			// Add this value to the account running total.
			sum += atof(parts[2].c_str());
		}
		file.close();
	} else {
		cerr << "error: Unable to open file." << endl;
		exit(EXIT_FAILURE);  // End main thread which results in ending the main process and killing all threads
	}

	// DEBUG
	cout << "processed lines:" << i << '\n';
	return sum;
}


/*
 * Thread code.
 */
void* exec_count_values(void* arg) {
	compact_arg_t* args = (compact_arg_t*) arg;
	// Open the file in each thread
	ifstream file(args->file_name);
	double* sum = (double*)malloc(sizeof(double));

	if (file.is_open() && sum != nullptr) {
		// Run count for thread's chunk
		*sum = count_values(file, *args->lines_positions, args->start_line, args->chunk_size);
		free(arg);  // alocated by run_threads
		return sum;
	} else {
		cerr << "Unable to execute the thread routine." << endl;
		exit(EXIT_FAILURE);  // End main thread which results in ending the main process and killing all threads
	}
}


/*
 * Run multiple threads.
 *
 * return size of chunk for main thread job, main thread runs first chunk
 */
size_t run_threads(
		char* file_name,
		size_t line_count,
		vector<ifstream::pos_type>* lines_positions,
		pthread_t threads[],
		size_t number_of_threads) {

	size_t chunk_count = number_of_threads + 1; // +1 chunk for main thread
	size_t chunk_remider = line_count % chunk_count;
	size_t chunk_size = line_count / chunk_count;

	compact_arg_t* args;
	size_t current_start_line = chunk_size;  // skipping the first chunk
	size_t current_chunk_size;
	for (size_t i = 0; i < number_of_threads; ++i) {
		// Add one more line to chunk if there is remider
		current_chunk_size = chunk_size + (chunk_remider != 0 ? --chunk_remider, 1 : 0);

		if ((args = (compact_arg_t*)malloc(sizeof(compact_arg_t))) == nullptr) {
           exit(EXIT_FAILURE);  // End main thread which results in ending the main process and killing all threads
        }
		args->file_name = file_name;            args->lines_positions = lines_positions;
		args->start_line = current_start_line;  args->chunk_size = current_chunk_size;
		if (pthread_create(&threads[i], nullptr, &exec_count_values, args)) {
			cerr << "error: Cannot create or start the thread number \"" << i << "\"." << endl;
			exit(EXIT_FAILURE);  // End main thread which results in ending the main process and killing all threads
		}

		// Update start line for next chunk
		current_start_line += current_chunk_size;
	}

	return chunk_size;
}


/*
 * Join multiple theads and accumulate results.
 */
void join_threads(double& sum, pthread_t threads[], size_t number_of_threads) {
	void* result;
	for (size_t i = 0; i < number_of_threads; ++i) {
		pthread_join(threads[i], &result);
		sum += *(double*)result;
		free(result);
	}
}


/*
 * Main entry point.
 *
 * This program expects one argument, the name of the file to process.
 *
 * argc - the number of command-line arguments.
 * argv - command line arguments array.
 */
int main(int argc, char* argv[]) {
	// Default number of paralel jobs
	unsigned jobs = 4;
	// Name of a file in CSV format
	char* file_name;

	// Parse parameters
	int opt;
	try {
	    while ((opt = getopt(argc, argv, "j:")) != -1) {
	        switch (opt) {
	            case 'j':
                    jobs = std::stoul(optarg);
					if (jobs == 0) {
						std::cerr << "error: Incorrect number of threads." << std::endl;
				        throw exception();
					}
					break;
	            default:
					throw exception();
	                break;
	        }   // end switch
	    }   // end while
		if (argc < 2 || optind != argc-1) {
	        std::cerr << "error: Incorrect options." << std::endl;
	        throw exception();
	    }
		file_name = argv[optind];
	} catch ( ... ) {
		std::cerr << "Usage: " << argv[0] << " [-j threads] FILE" << std::endl;
		return EXIT_FAILURE;
	}

	// Open the file again and process line by line.
	ifstream myfile(file_name);
	if (!myfile.is_open()) {
		cerr << "Unable to open file." << endl;
		return EXIT_FAILURE;
	}

	// Total balance
	double runningTotal = 0.0;

	// Count the number of lines in the file.
	// New line characters will be skipped unless we stop it from happening:
	myfile.unsetf(ios_base::skipws);

	vector<ifstream::pos_type> positions;  // the byte position of each line
	std::istream_iterator<char> eos;  // default constructor constructs an end-of-stream iterator
	std::istream_iterator<char> file_iter(myfile);
	size_t line_count = 0;
	positions.push_back(myfile.tellg());
	while (file_iter != eos) {
		if (*file_iter++ == '\n') {
			positions.push_back(myfile.tellg());
			++line_count;
		}
    }

	// Handle files that do not have an ending newline character
	myfile.clear();
	myfile.seekg(-1, ios::end);
	char lastChar;
	myfile.get(lastChar);
	if (lastChar != '\r' && lastChar != '\n')
		line_count++;

	// Reset the position in the file and the stream attributes
	myfile.setf(ios_base::skipws);
	myfile.clear();
	myfile.seekg(0, ios::beg);

	// Start threads
	unsigned number_of_threads = jobs - 1;  // -1 main thread is already running
	pthread_t threads[number_of_threads];
	size_t chunk_size = run_threads(file_name, line_count, &positions, threads, number_of_threads);

	// Run one job in main thread
	runningTotal = count_values(myfile, positions, 0, chunk_size);

	// Join  results
	join_threads(runningTotal, threads, number_of_threads);

	// Print out the total balance
	cout << setprecision(2) << fixed << runningTotal << endl;

	return EXIT_SUCCESS;
}
