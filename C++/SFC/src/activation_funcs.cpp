/**
 * activation_funcs.cpp
 * Project:     SFC Assignment, Restricted Boltzmann machine
 * Author:      Jakub Lukac
 * e-mail:      xlukac09@stud.fit.vutbr.cz
 * Created:     28.11.2018
 *
 */

#include <cmath>
#include "activation_funcs.h"

const std::function<double(double)> ActivationFunctions::ALWAYS_ZERO = {[](double value) { return 0.0; }};
const std::function<double(double)> ActivationFunctions::ALWAYS_ONE = {[](double value) { return 1.0; }};

const std::function<double(double)> ActivationFunctions::SIGMOID = {[](double value) {
    return 1.0 / ( 1.0 + std::exp(-value) );
}};
const std::function<double(double)> ActivationFunctions::FAST_SIGMOID = {[](double value) {
    return 1.0 / ( 1.0 + std::abs(value) );
}};
