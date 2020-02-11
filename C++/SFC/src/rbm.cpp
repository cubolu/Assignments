/**
 * rbm.cpp
 * Project:     SFC Assignment, Restricted Boltzmann machine
 * Author:      Jakub Lukac
 * e-mail:      xlukac09@stud.fit.vutbr.cz
 * Created:     24.11.2018
 *
 */

#include <iostream>
#include <vector>
#include <random>
#include <chrono>
#include <algorithm>
#include <string>
#include <cstdlib>
#include <unistd.h>

#include "neuron.h"
#include "mnist_reader.h"
#include "rbm_net.h"

#define NONE_ZOOM       0
#define NORMAL_ZOOM     3
#define BIG_ZOOM        5
#define BIGGER_ZOOM     8

std::vector<cv::Mat> generate_data_set(unsigned number_of_samples, unsigned rows, unsigned cols) {
    std::vector<cv::Mat> data_set;
    std::random_device random_device;
    std::mt19937 generator(random_device());
    std::uniform_int_distribution<> distribution(0, 255);

    for (unsigned i = 0; i < number_of_samples; ++i) {
        cv::Mat image = cv::Mat::zeros(rows, cols, CV_8UC1);
        for (unsigned r = 0; r < rows; ++r) {
            for (unsigned c = 0; c < cols; ++c) {
                image.at<unsigned char>(r, c) = static_cast<unsigned char>(distribution(generator));
            }
        }
        data_set.push_back(image);
    }

    return data_set;
}

cv::Mat resize(cv::Mat &original_image, double scale) {
    cv::Mat resized_image(static_cast<int>(original_image.rows * scale),
                          static_cast<int>(original_image.cols * scale),
                          original_image.type());
    cv::resize(original_image, resized_image, resized_image.size(), 0, 0, CV_INTER_NN);
    return resized_image;
}

void print_help(char *program_name) {
    std::cout << "Usage: " << program_name << " [-h] [-o] [-n NUMBER_OF_HIDDEN_NEURONS]" << std::endl
              << "       [-r LEARNING_RATE] [-t LEARNING_THRESHOLD] [-e NUMBER_OF_EPOCHS]" << std::endl
              << "       [-s NUMBER_OF_TRAINING_SAMPLES_TO_READ] [-i INDEX]" << std::endl
              << "       [-w WEIGHTS_FILE_NAME] [-b ORIGINAL_IMAGE_FILE_NAME] [-a RECONSTRUCTED_IMAGE_FILE_NAME]"
              << std::endl << std::endl
              << "  example: ./rbm -e 50 -s 500 -i 3 -w weights -b original_image -a reconstructed_image"
              << std::endl;
}

int main(int argc, char *argv[]) {
    /// Defaults
    unsigned number_of_hidden_neurons = 225;

    double learning_coefficient = 0.1; // (0, 1)
    double learning_threshold = 0.001;
    unsigned epochs = 100;

    std::string training_data_file_name = "data/train-images-idx3-ubyte";
    std::string test_data_file_name = "data/t10k-images-idx3-ubyte";
    std::string weights_file_name;
    std::string before_reconstruction_file_name;
    std::string after_reconstruction_file_name;

    unsigned number_of_training_samples_to_read = 100;
    unsigned index_of_image_for_reconstruction = 0;
    unsigned number_of_reconstruction_passes = 1;

    unsigned zoom_factor = BIG_ZOOM;

    bool file_outputs_only = false;

    // parse arguments
    int opt;
    while ((opt = getopt(argc, argv, "hon:r:t:e:s:i:w:b:a:")) != -1) {
        switch (opt) {
            case 'h':
                print_help(argv[0]);
                return EXIT_SUCCESS;
                break;
            case 'n':
                number_of_hidden_neurons = static_cast<unsigned>(std::stoi(optarg));
                break;
            case 'r':
                learning_coefficient = std::stod(optarg);
                break;
            case 't':
                learning_threshold = std::stod(optarg);
                break;
            case 'e':
                epochs = static_cast<unsigned>(std::stoi(optarg));
                break;
            case 's':
                number_of_training_samples_to_read = static_cast<unsigned>(std::stoi(optarg));
                break;
            case 'i':
                index_of_image_for_reconstruction = static_cast<unsigned>(std::stoi(optarg));
                break;
            case 'o':
                file_outputs_only = true;
                break;
            case 'w':
                weights_file_name = optarg;
                break;
            case 'b':
                before_reconstruction_file_name = optarg;
                break;
            case 'a':
                after_reconstruction_file_name = optarg;
                break;
            case '?':
            default:
                print_help(argv[0]);
                return EXIT_FAILURE;
                break;
        }
    }

    /// Initialize Restricted Boltzmann Machine with MNIST data set
    const std::vector<cv::Mat> training_data_set = read_mnist_data_set(training_data_file_name,
                                                                       number_of_training_samples_to_read);
    if (training_data_set.empty()
        || training_data_set.back().rows != IMAGE_ROWS
        || training_data_set.back().cols != IMAGE_COLS) {

        throw BrainError("rbm: Input data not not supported. Note: Only two dimensional data allowed.");
    }
    RBMNet simple_net(number_of_hidden_neurons);

    // Measure time
    auto start(std::chrono::system_clock::now());

    simple_net.train(training_data_set, learning_coefficient, learning_threshold, epochs);

    auto end(std::chrono::system_clock::now());

    /// Print duration
    std::chrono::duration<double> elapsed_seconds(end - start);
    std::cout << "Training elapsed time: " << elapsed_seconds.count() << "s" << std::endl;

    /// Reconstruction
    const std::vector<cv::Mat> test_data_set = read_mnist_data_set(test_data_file_name,
                                                                   index_of_image_for_reconstruction + 1);
    unsigned index = std::min(index_of_image_for_reconstruction, static_cast<unsigned>(test_data_set.size() - 1));

    cv::Mat test_image = test_data_set.at(index);
    cv::Mat reconstructed_test_image = simple_net.reconstruct(test_image, number_of_reconstruction_passes);

    /// Apply scaling and output
    if (!weights_file_name.empty())
        simple_net.save_weights_as_image(weights_file_name + ".png");

    test_image = resize(test_image, zoom_factor);
    reconstructed_test_image.convertTo(reconstructed_test_image, CV_8U, 255);
    // Negative image
    // reconstructed_test_image.forEach<unsigned char>(
    //         [](unsigned char &p, const int *) -> void { p = static_cast<unsigned char>(255u) - p; }
    // );
    reconstructed_test_image = resize(reconstructed_test_image, zoom_factor);

    if (!before_reconstruction_file_name.empty()) {
        imwrite(before_reconstruction_file_name + ".png", test_image);
    }
    if (!after_reconstruction_file_name.empty()) {
        imwrite(after_reconstruction_file_name + ".png", reconstructed_test_image);
    }

    if (!file_outputs_only) {
        simple_net.show_weights("Trained weights");
        cv::imshow("Reconstructed - original", test_image);
        cv::imshow("Reconstructed - scaled", reconstructed_test_image);

        cv::waitKey(); /// Press any key to exit
    }

    return EXIT_SUCCESS;
}
