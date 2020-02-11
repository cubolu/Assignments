/**
 * exceptions.h
 * Project:     SFC Assignment, Restricted Boltzmann machine
 * Author:      Jakub Lukac
 * e-mail:      xlukac09@stud.fit.vutbr.cz
 * Created:     27.11.2018
 *
 */

#ifndef SFC_EXCEPTIONS_H
#define SFC_EXCEPTIONS_H

#include <exception>
#include <string>


// Internal neural exception
struct BrainError : public std::exception {
    explicit BrainError(std::string &&msg) : m_msg(msg) {

    }
    const char * what() const noexcept override {
        return m_msg.c_str();
    }

    std::string m_msg;
};

#endif //SFC_EXCEPTIONS_H
