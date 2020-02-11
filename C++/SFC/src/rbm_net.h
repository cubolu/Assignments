/**
 * rbm_net.h
 * Project:     SFC Assignment, Restricted Boltzmann machine
 * Author:      Jakub Lukac
 * e-mail:      xlukac09@stud.fit.vutbr.cz
 * Created:     1.12.2018
 *
 */

#ifndef SFC_RBM_NET_H
#define SFC_RBM_NET_H


#include <vector>
#include <memory>
#include <random>
#include <opencv2/opencv.hpp>

#include "neuron.h"

#define NEURON_INITIAL_STATE        0.0
#define ACTIVATION_FUNCTION         ActivationFunctions::SIGMOID

#define WEIGHT_DISTRIBUTION         std::normal_distribution        // standard distributions from <memory>
#define WEIGHT_DISTRIBUTION_ARGS    0.0, 0.1

#define IMAGE_ROWS                  28
#define IMAGE_COLS                  28
#define IMAGE_THRESHOLD             100

class RBMNet {
public:
    explicit RBMNet(unsigned hidden_layer_size);

    void train(
            const std::vector<cv::Mat> &data, double learning_coefficient, double learning_threshold, unsigned epochs);

    unsigned long get_visible_layer_size() { return m_visible_layer.size(); }

    unsigned long get_hidden_layer_size() { return m_hidden_layer.size(); }

    cv::Mat reconstruct(const cv::Mat &data, unsigned number_of_passes);

    void print_structure();

    void show_weights(std::string window_title);

    void save_weights_as_image(std::string file_name);

private:
    std::vector<std::shared_ptr<Neuron>> m_visible_layer;
    std::vector<std::shared_ptr<Neuron>> m_hidden_layer;
    std::shared_ptr<Neuron> m_visible_layer_bias_ptr;
    std::shared_ptr<Neuron> m_hidden_layer_bias_ptr;
    std::vector<std::shared_ptr<Connection>> m_all_connections;

    std::random_device m_random_device;
    std::mt19937_64 m_generator;
    WEIGHT_DISTRIBUTION<> m_weight_distribution;

    RBMNet(unsigned visible_layer_size, unsigned hidden_layer_size);

    void connect_layers(std::vector<std::shared_ptr<Neuron>> &first, std::vector<std::shared_ptr<Neuron>> &second);

    void connect_bias_to_visible_layer(Neuron &bias, std::vector<std::shared_ptr<Neuron>> &layer);

    void connect_bias_to_hidden_layer(Neuron &bias, std::vector<std::shared_ptr<Neuron>> &layer);

    void print_layer(std::vector<std::shared_ptr<Neuron>> &layer);

    std::vector<cv::Mat> get_weights_as_matrices(std::vector<std::shared_ptr<Neuron>> &layer);

    cv::Mat convert_matrices_to_image(const std::vector<cv::Mat> &weights_per_neurons);

    cv::Mat extract_matrix_from_layer_states(std::vector<std::shared_ptr<Neuron>> &layer);
};


#endif //SFC_RBM_NET_H
