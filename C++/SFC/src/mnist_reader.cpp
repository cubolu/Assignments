/**
 * mnist_reader.cpp
 * Project:     SFC Assignment, Restricted Boltzmann machine
 * Author:      Jakub Lukac
 * e-mail:      xlukac09@stud.fit.vutbr.cz
 * Created:     25.11.2018
 *
 */

#include <algorithm>
#include <stdexcept>
#include "mnist_reader.h"

int convert_from_high_endian(int i) {
    unsigned char byte1, byte2, byte3, byte4;
    byte1 = i & 255;
    byte2 = (i >> 8) & 255;
    byte3 = (i >> 16) & 255;
    byte4 = (i >> 24) & 255;
    return ((int) byte1 << 24) + ((int) byte2 << 16) + ((int) byte3 << 8) + byte4;
}

std::vector<cv::Mat> read_mnist_data_set(std::string &file_name) {
    std::ifstream file(file_name, std::ios::binary);
    return read_mnist_data_set(file, 0);
}

std::vector<cv::Mat> read_mnist_data_set(std::string &file_name, int number_of_images_to_read) {
    std::ifstream file(file_name, std::ios::binary);
    return read_mnist_data_set(file, number_of_images_to_read);
}

std::vector<cv::Mat> read_mnist_data_set(std::ifstream &file) {
    return read_mnist_data_set(file, 0);
}

std::vector<cv::Mat> read_mnist_data_set(std::ifstream &file, int number_of_images_to_read) {
    std::vector<cv::Mat> mnist_data_set;
    if (file.is_open()) {
        int magic_number = 0;
        int number_of_images = 0;
        int number_of_rows = 0;
        int number_of_cols = 0;
        unsigned char pixel;


        /// Parse metadata
        file.read((char *) &magic_number, sizeof(magic_number));
        magic_number = convert_from_high_endian(magic_number);

        file.read((char *) &number_of_images, sizeof(number_of_images));
        number_of_images = convert_from_high_endian(number_of_images);
        if (number_of_images > 0) {
            number_of_images = std::min(number_of_images, number_of_images_to_read);
        }

        file.read((char *) &number_of_rows, sizeof(number_of_rows));
        number_of_rows = convert_from_high_endian(number_of_rows);

        file.read((char *) &number_of_cols, sizeof(number_of_cols));
        number_of_cols = convert_from_high_endian(number_of_cols);

        /// Parse images
        for (int i = 0; i < number_of_images; ++i) {
            cv::Mat image = cv::Mat::zeros(number_of_rows, number_of_cols, CV_8UC1);
            for (int r = 0; r < number_of_rows; ++r) {
                for (int c = 0; c < number_of_cols; ++c) {
                    file.read((char *) &pixel, sizeof(pixel));
                    image.at<unsigned char>(r, c) = pixel;
                }
            }
            mnist_data_set.push_back(image);
        }
    } else {
        throw std::runtime_error("Cannot open MNIST data set file.");
    }
    return mnist_data_set;
}

void test_loading_of_data_set(std::string &file_name) {
    std::cout << "Testing data set loading..." << std::endl;

    // Measure time
    auto start = std::chrono::system_clock::now();

    std::vector<cv::Mat> images(read_mnist_data_set(file_name, 0));

    auto end = std::chrono::system_clock::now();

    // Print duration
    std::chrono::duration<double> elapsed_seconds = end - start;
    std::cout << "elapsed time: " << elapsed_seconds.count() << "s" << std::endl
              << "read images: " << images.size() << std::endl;

    // Show image example
    cv::imshow("Example image 1", images.front());
    cv::imshow("Example image 2", images.back());
    cv::waitKey();
}
