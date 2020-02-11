/**
 * ldap.h
 * Project:     ISA Assignment, LDAP Server
 * Compiled:    gcc version 4.9.3 (Ubuntu 16.04)
 * Author:      Jakub Lukac
 * e-mail:      xlukac09@stud.fit.vutbr.cz
 * Created:     21-10-2017
 *
 */

#ifndef LDAPSERVER_LDAP_H
#define LDAPSERVER_LDAP_H

#include <exception>
#include <fstream>
#include <string>
#include <map>
#include "ldap_ans1_parser.h"
#include "ldap_filter.h"

struct LdapError : public std::exception {
    explicit LdapError(std::string &&msg) : m_msg(msg) {

    }
    const char * what() const throw () override {
        return m_msg.c_str();
    }

    std::string m_msg;
};

class Ldap {
public:
    Ldap(int welcome_socket, const std::string& file_name);
    ~Ldap();

    bool is_clientActive();

    void process_request();

private:
    LdapASN1Parser m_parser;                /// ASN1 language protocol parser
    std::ifstream m_databaseFile;     /// data base file - csv file

    bool m_unbind;
    int m_messageID;
    LDAP_OPERATION m_requestedOperation;
    LdapFilter * m_filter;

    void process_bind();
    void process_unbind();
    void process_search();

    std::map<std::string,std::string> split_db_row(std::string &str);
};


#endif //LDAPSERVER_LDAP_H
