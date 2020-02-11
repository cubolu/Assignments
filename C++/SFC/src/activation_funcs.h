/**
 * activation_funcs.h
 * Project:     SFC Assignment, Restricted Boltzmann machine
 * Author:      Jakub Lukac
 * e-mail:      xlukac09@stud.fit.vutbr.cz
 * Created:     27.11.2018
 *
 */

#ifndef SFC_ACTIVATION_FUNCS_H
#define SFC_ACTIVATION_FUNCS_H

#include <functional>


class ActivationFunctions {
public:
    static const std::function<double(double)> ALWAYS_ZERO;
    static const std::function<double(double)> ALWAYS_ONE;

    // Logistic function - sigmoid curve
    static const std::function<double(double)> SIGMOID;
    static const std::function<double(double)> FAST_SIGMOID;
};

#endif //SFC_ACTIVATION_FUNCS_H
