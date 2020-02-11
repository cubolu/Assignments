/**
 * ldap.cpp
 * Project:     ISA Assignment, LDAP Server
 * Compiled:    gcc version 4.9.3 (Ubuntu 16.04)
 * Author:      Jakub Lukac
 * e-mail:      xlukac09@stud.fit.vutbr.cz
 * Created:     21-10-2017
 *
 */

#include "ldap.h"

#include <iomanip>
#include <sstream>
#include <iostream>
#include <cstring>

Ldap::Ldap(const int welcome_socket, const std::string &file_name) : m_parser(welcome_socket),
                                                                     m_databaseFile(file_name),
                                                                     m_unbind(false),
                                                                     m_filter(nullptr) {
    if (!m_databaseFile.good()) {
        throw std::invalid_argument("Invalid file, error occurred while opening file: '" + file_name + "'.");
    }
}

Ldap::~Ldap() {
    delete m_filter;
}

bool Ldap::is_clientActive() {
    if (m_unbind) {
        // connection was terminated
        return false;
    }

    m_parser.clear();

    try {
        // parse msg
        m_messageID = m_parser.parse_ldapMessage();
        // parse request operation
        m_requestedOperation = m_parser.parse_operation();

#ifndef NDEBUG
        std::cout << "info: message ID: " << m_messageID << std::endl;
        std::cout << "info: message Op: " << LdapASN1Parser::op_to_str(m_requestedOperation) << std::endl;
#endif

        return true;
    } catch (const SocketEOF& e) {
        // empty - return false

#ifndef NDEBUG
        std::cout << "info: Connection was terminated by client" << m_messageID << std::endl;
#endif
    }
    return false;
}

void Ldap::process_request() {
    switch (m_requestedOperation) {
        case LDAP_OPERATION::BINDREQUEST:
            process_bind();
            break;
        case LDAP_OPERATION::UNBINDREQUEST:
            process_unbind();
            break;
        case LDAP_OPERATION::SEARCHREQUEST:
            process_search();
            // if search get filter >> parse filter
            // call parse filter
            // call parse fields
            break;
    }
}

void Ldap::process_bind() {
    m_parser.send_ldapMessage(
            m_messageID,
            m_parser.build_bindResponse(
                    m_parser.build_ldapResult(
                            LDAP_RESULT_CODE::SUCCESS,
                            ""
                    )
                    // [serverSaslCreds]
            )
    );
}

void Ldap::process_unbind() {
    m_unbind = true;

#ifndef NDEBUG
    std::cout << "info: unbind: "<< std::boolalpha << m_unbind << std::noboolalpha << std::endl;
#endif
}

void Ldap::process_search() {
    // prepare filter
    m_filter = m_parser.get_filter();

    // init size limit
    int size_limit = m_parser.get_size_limit();
    int sent_entries_count = 0;

    // get data
    std::string line;
    while (std::getline(m_databaseFile, line)) {
        std::map<std::string, std::string> row = split_db_row(line);
        if (m_filter->match(row)) {
            ++sent_entries_count;

#ifndef NDEBUG
            std::cout << "info: sending: ";
            std::cout << "([ " << CN << " => " << row.at(CN) << " ], "
                      << "[ " << UID << " => " << row.at(UID) << " ], "
                      << "[ " << MAIL << " => " << row.at(MAIL) << " ])"
                      << std::endl;
#endif

            // send matched entries
            m_parser.send_ldapMessage(
                    m_messageID,
                    m_parser.build_searchEntry(row)
            );

            // check size limit
            if (size_limit && sent_entries_count >= size_limit)
                break;
        }
    }

    // search done
    m_parser.send_ldapMessage(
            m_messageID,
            m_parser.build_searchDone(
                    m_parser.build_ldapResult(
                            LDAP_RESULT_CODE::SUCCESS,
                            ""
                    )
            )
    );
}

std::map<std::string, std::string> Ldap::split_db_row(std::string &str) {
    std::map<std::string, std::string> row;
    // cn                   uid                 mail
    // Am...;               xam...;             xam...@stud.fit.vutbr.cz
    // ...
    std::string data;
    for (auto key: {CN, UID, MAIL}) {
        data = str.substr(0, str.find(';')); // if not present, empty string is returned
        row.insert(std::pair<std::string,std::string>(key, data));
        str.erase(0, data.size() + 1); // remove parsed data from line (plus ";")
    }
    return std::move(row);
}
