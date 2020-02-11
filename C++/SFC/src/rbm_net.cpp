/**
 * rbm_net.cpp
 * Project:     SFC Assignment, Restricted Boltzmann machine
 * Author:      Jakub Lukac
 * e-mail:      xlukac09@stud.fit.vutbr.cz
 * Created:     1.12.2018
 *
 */

#include <iostream>
#include <cmath>
#include "rbm_net.h"

RBMNet::RBMNet(unsigned hidden_layer_size) : RBMNet(IMAGE_ROWS * IMAGE_COLS, hidden_layer_size) {}

RBMNet::RBMNet(unsigned visible_layer_size, unsigned hidden_layer_size)
        : m_generator(m_random_device()), m_weight_distribution(WEIGHT_DISTRIBUTION_ARGS) {

    m_visible_layer.reserve(visible_layer_size + 1);
    m_hidden_layer.reserve(visible_layer_size + 1);

    /// Create layers and initialize them
    for (unsigned i = 0; i < visible_layer_size; ++i) {
        m_visible_layer.push_back(std::make_shared<VisibleNeuron>(NEURON_INITIAL_STATE, ACTIVATION_FUNCTION,
                                                                  &m_generator));
    }

    for (unsigned i = 0; i < hidden_layer_size; ++i) {
        m_hidden_layer.push_back(std::make_shared<HiddenNeuron>(NEURON_INITIAL_STATE, ACTIVATION_FUNCTION,
                                                                &m_generator));
    }

    connect_layers(m_visible_layer, m_hidden_layer);

    /// Create biases
    m_visible_layer_bias_ptr = std::make_shared<BiasNeuron>();
    m_hidden_layer_bias_ptr = std::make_shared<BiasNeuron>();

    /// Connect biases with neurons of specific layer
    connect_bias_to_visible_layer(*m_visible_layer_bias_ptr.get(), m_visible_layer);
    connect_bias_to_hidden_layer(*m_hidden_layer_bias_ptr.get(), m_hidden_layer);
}

void RBMNet::connect_layers(std::vector<std::shared_ptr<Neuron>> &first, std::vector<std::shared_ptr<Neuron>> &second) {
    for (auto &visible_neuron_ptr: first) {
        for (auto &hidden_neuron_ptr: second) {
            m_all_connections.push_back(
                    BidirectionalStaticConnection::connect_neurons(*visible_neuron_ptr.get(), *hidden_neuron_ptr.get(),
                                                                   m_weight_distribution(m_generator)));
        }
    }
}

void RBMNet::connect_bias_to_visible_layer(Neuron &bias, std::vector<std::shared_ptr<Neuron>> &layer) {
    for (auto &neuron_ptr: layer) {
        m_all_connections.push_back(
                BidirectionalStaticConnection::connect_neurons(bias, *neuron_ptr.get(),
                                                               m_weight_distribution(m_generator)));
    }
}

void RBMNet::connect_bias_to_hidden_layer(Neuron &bias, std::vector<std::shared_ptr<Neuron>> &layer) {
    for (auto &neuron_ptr: layer) {
        m_all_connections.push_back(
                BidirectionalStaticConnection::connect_neurons(bias, *neuron_ptr.get(), 0.0));
    }
}

void RBMNet::train(
        const std::vector<cv::Mat> &data, double learning_coefficient, double learning_threshold, unsigned epochs) {

    /// Verify data
    if (!data.empty()
        && get_visible_layer_size() != (static_cast<unsigned long>(data.back().rows) * data.back().cols)) {
        throw BrainError("RBMNet: Data dimensions not compatible with the net.");
    }

    /// Training algorithm
    for (; epochs != 0; --epochs) {
        std::cout << "\rEpochs left: " << epochs << "..." << std::flush;
        for (auto &data_sample : data) {
            // POSITIVE PHASE
            // for all vectors in training data set do:
            // // reinitialize the visible neurons
            // // update hidden neurons
            // // update positive gradient

            // set visible neurons to the data set vector
            const cv::Mat reshaped_data_sample = data_sample.reshape(0, 1);
            cv::MatConstIterator_<unsigned char> data_sample_iterator = reshaped_data_sample.begin<unsigned char>();
            for (auto &visible_neuron_ptr: m_visible_layer) {
                /// Initialize visible layer. Convert pixel value to double in range (0.0, 1.0)
                visible_neuron_ptr.get()->initialize((*data_sample_iterator++) < IMAGE_THRESHOLD ? 0.0 : 1.0);
            }

            // update hidden neurons
            for (auto &hidden_neuron_ptr: m_hidden_layer) {
                hidden_neuron_ptr.get()->update_state();
            }

            // update positive gradient, accumulate it through the data set
            for (auto &connection_ptr: m_all_connections) {
                connection_ptr.get()->update_positive_gradient();
            }

            // NEGATIVE PHASE
            // for all vectors in training data set do:
            // // update visible neurons
            // // update hidden neurons
            // // update negative gradient

            // update visible neurons
            for (auto &visible_neuron_ptr: m_visible_layer) {
                visible_neuron_ptr.get()->update_state();
            }

            // update hidden neurons
            for (auto &hidden_neuron_ptr: m_hidden_layer) {
                hidden_neuron_ptr.get()->update_state();
            }

            // update negative gradient, accumulate it through the data set
            for (auto &connection_ptr: m_all_connections) {
                connection_ptr.get()->update_negative_gradient();
            }
        }

        // UPDATE WEIGHTS
        bool weights_changed = false;
        for (auto &connection_ptr: m_all_connections) {
            weights_changed |=
                    connection_ptr.get()->update_weight(data.size(), learning_coefficient, learning_threshold);
                    /// update will also reset gradients
        }

        // if any m_weight changed go back
        if (!weights_changed) break;
    }
    std::cout << "\rEpochs left: " << epochs << std::endl << std::flush;
}

cv::Mat RBMNet::reconstruct(const cv::Mat &data, unsigned number_of_passes) {
    /// Verify data could be reconstructed
    if (!data.empty()
        && get_visible_layer_size() != (static_cast<unsigned long>(data.rows) * data.cols)) {
        throw BrainError("RBMNet: Data cannot be reconstructed, not compatible with the net dimensions.");
    }

    // set initial visible neurons to the data vector values
    const cv::Mat reshaped_data_sample = data.reshape(0, 1);
    cv::MatConstIterator_<unsigned char> data_sample_iterator = reshaped_data_sample.begin<unsigned char>();
    for (auto &visible_neuron_ptr: m_visible_layer) {
        /// Initialize visible layer. Convert pixel value to double in range (0.0, 1.0)
        visible_neuron_ptr.get()->initialize(*data_sample_iterator++ / 255.0);
    }

    /// Reconstruct data
    for (unsigned i = 0; i < number_of_passes; ++i) {
        // update hidden neurons
        for (auto &hidden_neuron_ptr: m_hidden_layer) {
            hidden_neuron_ptr.get()->update_state();
        }

        // update visible neurons
        for (auto &visible_neuron_ptr: m_visible_layer) {
            visible_neuron_ptr.get()->update_state();
        }
    }

    /// Extract result
    cv::Mat resulting_data = extract_matrix_from_layer_states(m_visible_layer);
    int dimensions[]{IMAGE_ROWS, IMAGE_COLS};
    return resulting_data.reshape(0, 2, dimensions);
}

void RBMNet::print_structure() {
    std::cout << "Network: (" << get_visible_layer_size() << ", " << get_hidden_layer_size() << ")" << std::endl;
    std::cout << "Visible layer, last neuron is bias of hidden layer:" << std::endl;
    print_layer(m_visible_layer);
    std::cout << "Hidden layer, last neuron is bias of visible layer:" << std::endl;
    print_layer(m_hidden_layer);
}

void RBMNet::print_layer(std::vector<std::shared_ptr<Neuron>> &layer) {
    unsigned cnt = 0;
    for (const auto &neuron_ptr: layer) {
        for (const auto &connection_ptr: neuron_ptr.get()->get_connections()) {
            std::cout << "*  " << neuron_ptr.get()->get_state()
                      << " <--" << "|" << connection_ptr.get()->get_weight() << "|--> "
                      << connection_ptr.get()->get_state_of_connected_neuron(*neuron_ptr.get())
                      << (++cnt % 5 == 0 ? (cnt = 0, "\n") : "  ");
        }
        std::cout << (cnt != 0 ? (cnt = 0, "\n") : "") << "####" << std::endl;
    }
}

void RBMNet::show_weights(std::string window_title) {
    cv::Mat weights = convert_matrices_to_image(get_weights_as_matrices(m_hidden_layer));
    cv::imshow(window_title, weights);
}

void RBMNet::save_weights_as_image(std::string file_name) {
    cv::Mat image = convert_matrices_to_image(get_weights_as_matrices(m_hidden_layer));
    image.convertTo(image, CV_8U, 255);
    imwrite(file_name, image);
}

std::vector<cv::Mat> RBMNet::get_weights_as_matrices(std::vector<std::shared_ptr<Neuron>> &layer) {
    std::vector<cv::Mat> weights;
    weights.reserve(layer.size());

    for (const auto &neuron_ptr: layer) {
        int dimensions = static_cast<int>(neuron_ptr.get()->get_connections().size() - 1); // minus bias connection
        cv::Mat neuron_weights = cv::Mat::zeros(1, &dimensions, CV_64FC1);

        auto first = neuron_ptr.get()->get_connections().cbegin();
        auto second_the_last = --neuron_ptr.get()->get_connections().cend(); // skip the last bias connection
        int index = 0;
        for (const auto &connection_ptr: std::vector<std::shared_ptr<Connection>>(first, second_the_last)) {
            neuron_weights.at<double>(index++) = connection_ptr.get()->get_weight();
        }
        weights.push_back(neuron_weights);
    }
    return weights;
}

cv::Mat RBMNet::convert_matrices_to_image(const std::vector<cv::Mat> &weights_per_neurons) {
    /// Reshape matrices
    std::vector<cv::Mat> reshaped_matrices;
    reshaped_matrices.reserve(weights_per_neurons.size());
    int dimensions[]{IMAGE_ROWS, IMAGE_COLS};
    for (auto &matrix : weights_per_neurons) {
        reshaped_matrices.push_back(matrix.reshape(0, 2, dimensions));
    }

    /// Determine matrices layout
    int images_grid_size = static_cast<int>(std::ceil(std::sqrt(weights_per_neurons.size())));
    cv::Mat image = cv::Mat::zeros(images_grid_size * IMAGE_ROWS, images_grid_size * IMAGE_COLS, CV_64FC1);

    /// Construct image
    int current_col = 0;
    int current_row = 0;
    for (auto &matrix : reshaped_matrices) {
        cv::normalize(matrix, matrix, 0.0, 1.0, cv::NORM_MINMAX, CV_64FC1);
        cv::Mat targetROI = image(cv::Rect(current_col, current_row, IMAGE_COLS, IMAGE_ROWS));
        matrix.copyTo(targetROI);

        // Update next image position
        current_col += IMAGE_COLS;
        if ((current_col + IMAGE_COLS) > image.cols) {
            current_col = 0;
            current_row += IMAGE_ROWS;
        }
    }

    return image;
}

cv::Mat RBMNet::extract_matrix_from_layer_states(std::vector<std::shared_ptr<Neuron>> &layer) {
    int dimensions = static_cast<int>(layer.size());
    cv::Mat neuron_states = cv::Mat::zeros(1, &dimensions, CV_64FC1);
    int index = 0;
    for (const auto &neuron_ptr: layer) {
        neuron_states.at<double>(index++) = neuron_ptr.get()->get_state();
    }
    return neuron_states;
}


