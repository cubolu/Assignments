/**
 * neuron.cpp
 * Project:     SFC Assignment, Restricted Boltzmann machine
 * Author:      Jakub Lukac
 * e-mail:      xlukac09@stud.fit.vutbr.cz
 * Created:     24.11.2018
 *
 */

#include <iostream>
#include <iomanip>
#include "neuron.h"

Neuron::Neuron(double state, const std::function<double(double)> &activation_func, std::mt19937_64 *generator)
        : m_state(state), m_activation_func(activation_func), m_generator(generator), m_distribution(0.0, 1.0) {}

Neuron::Neuron(double state, const std::function<double(double)> &activation_func)
        : m_state(state), m_activation_func(activation_func), m_generator(nullptr) {}

double Neuron::get_state() const {
    return m_state;
}

void Neuron::add_connection(const std::shared_ptr<Connection> &connection) {
    m_connections.push_back(connection);
}

void Neuron::update_state() {
    /**
     * State update:
     *      Compute activation energy:
     *          e_i = sum(States_of_connected_neurons * connection_weights)
     *      Apply activation function:
     *          p_i = sigmoid_func(e_i), p_i in (0, 1)
     *      New neuron state:
     *          with probability p_i new_state = 1, with probability (1 - p_i) new_state = 0
     */
    double activation_energy = 0.0;

    for (const auto &connection: m_connections) {
        activation_energy += connection.get()->get_weight() * connection.get()->get_state_of_connected_neuron(*this);
    }

    double activation_probability = m_activation_func(activation_energy);

    if (m_distribution(*m_generator) < activation_probability) {
        m_state = 1.0;
    } else {
        m_state = 0.0;
    }
}

std::vector<std::shared_ptr<Connection>> &Neuron::get_connections() {
    return m_connections;
}

VisibleNeuron::VisibleNeuron(double initial_state, const std::function<double(double)> &activation_func, std::mt19937_64 *generator)
        : Neuron(initial_state, activation_func, generator), m_initial_state(initial_state) {}

void VisibleNeuron::initialize(double initial_state) {
    m_state = initial_state;
    m_initial_state = initial_state;
}

HiddenNeuron::HiddenNeuron(double state, const std::function<double(double)> &activation_func, std::mt19937_64 *generator)
        : Neuron(state, activation_func, generator) {}

/** ***************************** Connections ****************************** */

double Connection::get_weight() const {
    return m_weight;
}

void Connection::set_weight(double weight) {
    Connection::m_weight = weight;
}

bool Connection::update_weight(double data_set_size, double learning_coefficient, double learning_threshold) {
    double learning_result = learning_coefficient *
                             ((static_cast<double>(m_weight_positive_gradient) -
                               static_cast<double>(m_weight_negative_gradient)) / data_set_size);

    reset_weight_gradients(); // duplicate update call will result in no change of the weight

#ifndef NDEBUG_TRACE
    std::cout << "\t" << std::fixed << std::setprecision(4) << m_weight << " + "
              << std::fixed << std::setprecision(4) << learning_result
              << " (" << (1.0 * learning_coefficient < learning_result ? "FAIL" : "ok") << ")" << std::endl;
#endif

    m_weight += learning_result;
    return learning_threshold < learning_result;
}

unsigned Connection::get_weight_positive_gradient() const {
    return m_weight_positive_gradient;
}

void Connection::reset_weight_positive_gradient() {
    Connection::m_weight_positive_gradient = 0u;
}

unsigned Connection::get_weight_negative_gradient() const {
    return m_weight_negative_gradient;
}

void Connection::reset_weight_negative_gradient() {
    Connection::m_weight_negative_gradient = 0u;
}

void Connection::reset_weight_gradients() {
    reset_weight_positive_gradient();
    reset_weight_negative_gradient();
}

std::shared_ptr<Connection>
BidirectionalStaticConnection::connect_neurons(Neuron &left_neuron, Neuron &right_neuron, double initial_weight) {
    std::shared_ptr<Connection> connection_ptr =
            std::make_shared<BidirectionalStaticConnection>(left_neuron, right_neuron, initial_weight);

    left_neuron.add_connection(connection_ptr);
    right_neuron.add_connection(connection_ptr);
    return connection_ptr;
}

BidirectionalStaticConnection::BidirectionalStaticConnection(Neuron &left_neuron, Neuron &right_neuron)
        : m_left_neuron(left_neuron), m_right_neuron(right_neuron) {
    set_weight(1.0);
}

BidirectionalStaticConnection::BidirectionalStaticConnection(
        Neuron &left_neuron, Neuron &right_neuron, double initial_weight) : m_left_neuron(left_neuron),
                                                                            m_right_neuron(right_neuron) {

    set_weight(initial_weight);
}

bool BidirectionalStaticConnection::is_input(const Neuron &current_neuron) const {
#ifndef NDEBUG
    if (std::addressof(current_neuron) == std::addressof(m_left_neuron)
        || std::addressof(current_neuron) == std::addressof(m_right_neuron)) {
        return true;
    } else {
        throw BrainError("BidirectionalStaticConnection: Broken connection.");
    }
#else
    return true;
#endif
}

bool BidirectionalStaticConnection::is_output(const Neuron &current_neuron) const {
#ifndef NDEBUG
    if (std::addressof(current_neuron) == std::addressof(m_left_neuron)
        || std::addressof(current_neuron) == std::addressof(m_right_neuron)) {
        return true;
    } else {
        throw BrainError("BidirectionalStaticConnection: Broken connection.");
    }
#else
    return true;
#endif
}

double BidirectionalStaticConnection::get_state_of_connected_neuron(const Neuron &current_neuron) const {
    if (std::addressof(current_neuron) == std::addressof(m_left_neuron)) {
        return m_right_neuron.get_state();
    } else if (std::addressof(current_neuron) == std::addressof(m_right_neuron)) {
        return m_left_neuron.get_state();
    } else {
        throw BrainError("BidirectionalStaticConnection: Broken connection, not valid connection for current neuron.");
    }
}

void BidirectionalStaticConnection::update_positive_gradient() {
    if (m_right_neuron.get_state() == m_left_neuron.get_state()) {
        ++m_weight_positive_gradient;
    }
}

void BidirectionalStaticConnection::update_negative_gradient() {
    if (m_right_neuron.get_state() == m_left_neuron.get_state()) {
        ++m_weight_negative_gradient;
    }
}
