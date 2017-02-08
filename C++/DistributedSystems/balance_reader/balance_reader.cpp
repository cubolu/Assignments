//============================================================================
// Name        : balance_reader.cpp
// Author      : Gray, Cameron C.
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
using namespace std;

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
 * Main entry point.
 *
 * This program expects one argument, the name of the file to process.
 *
 * argc - the number of command-line arguments.
 * argv - command line arguments array.
 */
int main(int argc, char* argv[]) {
	// Check the number of parameters
	if (argc < 2) {
		// Tell the user how to run the program
		cerr << "Usage: " << argv[0] << " filename" << endl;
		/* "Usage messages" are a conventional way of telling the user
		 * how to run a program if they enter the command incorrectly.
		 */
		return 1;
	}

	// Open the file again and process line by line.
	string line;
	ifstream myfile(argv[1]);
	double runningTotal = 0.0;

	// Count the number of lines in the file.
	// New line characters will be skipped unless we stop it from happening:
	myfile.unsetf(ios_base::skipws);

	unsigned line_count = std::count(std::istream_iterator<char>(myfile),
			std::istream_iterator<char>(), '\n');

	// Handle files that do not have an ending newline character
	myfile.clear();
	myfile.seekg(-1, ios::end);
	char lastChar;
	myfile.get(lastChar);
	if (lastChar != '\r' && lastChar != '\n')
		line_count++;

	// Reset the position in the file and the stream attributes.
	myfile.setf(ios_base::skipws);
	myfile.clear();
	myfile.seekg(0, ios::beg);

	// Read in, line by line...
	if (myfile.is_open()) {
		for (unsigned current_line = 0; current_line < line_count; current_line++) {
			vector<string> parts;
			getline(myfile, line);

			// ... and process out the 4th element in the CSV.
			split(line, ',', parts);
			if (parts.size() != 3) {
				cerr << "Unable to process line " << current_line
						<< ", line is malformed. " << parts.size()
                        << " parts found."<< endl;
				continue;
			}

			// Add this value to the account running total.
			runningTotal += atof(parts[2].c_str());
		}
		myfile.close();
	} else {
		cerr << "Unable to open file";
		return 1;
	}

	// Print out the total balance.
	cout << setprecision(2) << fixed << runningTotal << endl;

	return 0;
}
