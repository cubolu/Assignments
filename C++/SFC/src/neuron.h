/**
 * neuron.h
 * Project:     SFC Assignment, Restricted Boltzmann machine
 * Author:      Jakub Lukac
 * e-mail:      xlukac09@stud.fit.vutbr.cz
 * Created:     24.11.2018
 *
 */

#ifndef SFC_NEURON_H
#define SFC_NEURON_H

#include <vector>
#include <functional>
#include <memory>
#include <random>

#include "exceptions.h"
#include "activation_funcs.h"


// Pre-declaration
class Connection;

class Neuron {
public:
    Neuron(double state, const std::function<double(double)> &activation_func);

    Neuron(double state, const std::function<double(double)> &activation_func, std::mt19937_64 *generator);

    double get_state() const;

    std::vector<std::shared_ptr<Connection>> &get_connections();

    void add_connection(const std::shared_ptr<Connection> &connection);

    virtual void update_state();

    virtual void initialize(double) { throw BrainError("Neuron: Cannot be initialized."); };

protected:
    double m_state;
    std::vector<std::shared_ptr<Connection>> m_connections;

    const std::function<double(double)> &m_activation_func;
    std::mt19937_64 *m_generator;
    std::uniform_real_distribution<> m_distribution;
};

class VisibleNeuron : public Neuron {
public:
    VisibleNeuron(double initial_state, const std::function<double(double)> &activation_func, std::mt19937_64 *generator);

    void initialize(double initial_state) override;

private:
    double m_initial_state;
};

class HiddenNeuron : public Neuron {
public:
    HiddenNeuron(double state, const std::function<double(double)> &activation_func, std::mt19937_64 *generator);

    void initialize(double) override { throw BrainError("HiddenNeuron: Cannot be initialized."); };
};

class BiasNeuron : public Neuron {
public:
    BiasNeuron() : Neuron(1.0, ActivationFunctions::ALWAYS_ONE) {}

    void
    update_state() override { throw BrainError("BiasNeuron: Cannot update state of bias neuron, it's always 1."); }

    void initialize(double) override { throw BrainError("BiasNeuron: Cannot be initialized, it's always 1."); };
};

/** ***************************** Connections ****************************** */
class Connection {
public:
    double get_weight() const;

    void set_weight(double weight);

    bool update_weight(double data_set_size, double learning_coefficient, double learning_threshold);

    /// Value needs to be normalized
    unsigned get_weight_positive_gradient() const;

    /// Value needs to be normalized
    unsigned get_weight_negative_gradient() const;

    void reset_weight_gradients();

    virtual bool is_input(const Neuron &current_neuron) const = 0;

    virtual bool is_output(const Neuron &current_neuron) const = 0;

    virtual double get_state_of_connected_neuron(const Neuron &current_neuron) const = 0;

    virtual void update_positive_gradient() = 0;

    virtual void update_negative_gradient() = 0;

protected:
    double m_weight{1.0};

    /// Contrastive Divergence
    unsigned m_weight_positive_gradient{0u};
    unsigned m_weight_negative_gradient{0u};

    void reset_weight_positive_gradient();

    void reset_weight_negative_gradient();
};

class BidirectionalStaticConnection : public Connection {
public:
    static std::shared_ptr<Connection>
    connect_neurons(Neuron &left_neuron, Neuron &right_neuron, double initial_weight);

    BidirectionalStaticConnection(Neuron &left_neuron, Neuron &right_neuron);

    BidirectionalStaticConnection(Neuron &left_neuron, Neuron &right_neuron, double initial_weight);

    bool is_input(const Neuron &current_neuron) const override;

    bool is_output(const Neuron &current_neuron) const override;

    double get_state_of_connected_neuron(const Neuron &current_neuron) const override;

    void update_positive_gradient() override;

    void update_negative_gradient() override;

private:
    Neuron &m_left_neuron;
    Neuron &m_right_neuron;
};

#endif //SFC_NEURON_H
