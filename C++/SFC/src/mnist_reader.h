/**
 * mnist_reader.h
 * Project:     SFC Assignment, Restricted Boltzmann machine
 * Author:      Jakub Lukac
 * e-mail:      xlukac09@stud.fit.vutbr.cz
 * Created:     25.11.2018
 *
 */

#ifndef SFC_MNIST_READER_H
#define SFC_MNIST_READER_H

#include <math.h>
#include <fstream>
#include <chrono>
#include <opencv2/opencv.hpp>


std::vector<cv::Mat> read_mnist_data_set(std::ifstream &file);

std::vector<cv::Mat> read_mnist_data_set(std::ifstream &file, int number_of_images_to_read);

std::vector<cv::Mat> read_mnist_data_set(std::string &file_name);

std::vector<cv::Mat> read_mnist_data_set(std::string &file_name, int number_of_images_to_read);

void test_loading_of_data_set(std::string &file_name);

#endif //SFC_MNIST_READER_H
