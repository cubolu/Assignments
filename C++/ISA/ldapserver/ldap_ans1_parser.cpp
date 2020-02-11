/**
 * ldap_asn1_parser.cpp
 * Project:     ISA Assignment, LDAP Server
 * Compiled:    gcc version 4.9.3 (Ubuntu 16.04)
 * Author:      Jakub Lukac
 * e-mail:      xlukac09@stud.fit.vutbr.cz
 * Created:     21-10-2017
 *
 */

#include "ldap_ans1_parser.h"

#include <iomanip>
#include <sstream>
#include <iostream>
#include <cstring>
#include <algorithm>
#include <iterator>
#include <climits>

#include <sys/socket.h> // Core socket functions and data structures
#include <unistd.h>     // Close socket function

LdapASN1Parser::LdapASN1Parser(const int socketHandle) : m_socketHandle(socketHandle) {

}

LdapASN1Parser::~LdapASN1Parser() {
    // correctly shut down connection
    bool is_not_correctly_shut = shutdown(m_socketHandle, SHUT_RDWR) == -1;
    close(m_socketHandle);

    if (is_not_correctly_shut) {
        std::cerr << "error: Shutdown failed: " << std::strerror(errno) << std::endl;
        throw SocketError(std::strerror(errno));
    }
}

void LdapASN1Parser::clear() {
    m_data.clear();
    m_ldap_dn.clear();
    m_attributes.clear();
}

/** ******************************** Request ******************************* **/

int LdapASN1Parser::parse_ldapMessage() {
    // get message length
    unsigned int message_len = get_sequence();
    // get the rest of the message
    m_data = get_data(message_len);
    // get message ID
    return get_integer(m_data);
}

LDAP_OPERATION LdapASN1Parser::parse_operation() {
    // validate data size which is needed
    check_data_size(m_data, 2);

    // get operation
    auto op = static_cast<LDAP_OPERATION>(m_data.front());
    unsigned char len;
    switch (op) {
        case LDAP_OPERATION::BINDREQUEST:
        case LDAP_OPERATION::UNBINDREQUEST:
        case LDAP_OPERATION::SEARCHREQUEST:
            m_data.pop_front();
            len = m_data.front();
            m_data.pop_front();
            parse_operation_body(op, len);
            // controls
            return op;
        default:
            // build the error message
            std::stringstream error_msg;
            error_msg << "Unknown operation: \'0x" << std::hex << std::setfill('0')
                      << std::setw(2) << static_cast<unsigned int>(op)
                      << "\'";
            throw ParserError(error_msg.str());;
    }
}

void LdapASN1Parser::parse_operation_body(LDAP_OPERATION op, unsigned char len) {
    check_data_size(m_data, len);
    switch (op) {
        case LDAP_OPERATION::BINDREQUEST:
            parse_bindrequest();
            break;
        case LDAP_OPERATION::UNBINDREQUEST:
            parse_unbindrequest();
            break;
        case LDAP_OPERATION::SEARCHREQUEST:
            parse_searchrequest();
            break;
    }
}

void LdapASN1Parser::parse_bindrequest() {
    int version = get_integer(m_data);
    m_ldap_dn = get_string(m_data);
#ifndef NDEBUG
    std::cout << "info: " << "Bind request params:" << std::endl;
    std::cout << "\t" << "version: " << version << std::endl;
    std::cout << "\t" << "DN: " << m_ldap_dn << std::endl;
#endif

    check_data_size(m_data, 2);
    auto front = static_cast<LDAP_TOKEN>(m_data.front());
    m_data.pop_front();
    if (front == LDAP_TOKEN::BINDSIMPLE) {
        unsigned char len = m_data.front();
        m_data.pop_front();
        std::string password = to_string(m_data, len);

#ifndef NDEBUG
        std::cout << "info: " << "Simple bind with password \"" << password << "\" (accepting any)" << std::endl;
#endif
    } else if (front == LDAP_TOKEN::BINDSASL) {
        std::cout << "BINDSASL..." << std::endl;
        throw ParserError("Not implemented!");
    } else {
        // build the error message
        std::stringstream error_msg;
        error_msg << "Bind request failed, unknown token: \'0x"<< std::hex << std::setfill('0')
                  << std::setw(2) << static_cast<unsigned int>(front)
                  << "\'";
        throw ParserError(error_msg.str());
    }
}

void LdapASN1Parser::parse_unbindrequest() {
    // no operation needed
}

void LdapASN1Parser::parse_searchrequest() {
    std::string base_object = get_string(m_data);       // string
    unsigned char scope = get_enum(m_data);             // enum
    unsigned char deref_aliases = get_enum(m_data);     // enum
    m_size_limit = get_integer(m_data);                 // integer
    int time_limit = get_integer(m_data);               // integer
    bool types_only = get_bool(m_data);                 // bool

    // get filter
    m_filter = parse_filter(m_data);

    // get sequence of strings
    unsigned int len = get_sequence(m_data);
    std::deque<unsigned char> attributes_data = get_data(m_data, len);
    while (!attributes_data.empty()) {
        m_attributes.push_back(get_string(attributes_data));
    }

#ifndef NDEBUG
    std::cout << "info: " << "Search request params:" << std::endl;
    std::cout << "\t" << "baseObject: " << (base_object.empty() ? "(empty)" : base_object) << std::endl;
    std::cout << "\t" << "scope: " << static_cast<int>(scope) << std::endl;
    std::cout << "\t" << "derefAliases: " << static_cast<int>(deref_aliases) << std::endl;
    std::cout << "\t" << "sizeLimit: " << m_size_limit << std::endl;
    std::cout << "\t" << "timeLimit: " << time_limit << std::endl;
    std::cout << "\t" << "typesOnly: " << std::boolalpha << types_only << std::noboolalpha << std::endl;
    std::cout << "\t" << "filter: " << filter_to_str(m_filter.first);
    DEBUG_DEQUE(m_filter.second, "", "\t\t0x");
    std::cout << "\t" << "AttributeDescList: [ ";
    for (const auto &s : m_attributes)
        std::cout << s << (s != m_attributes[m_attributes.size()-1] ? ", " : "");
    std::cout << " ]" << std::endl;
#endif
}

std::pair<LDAP_FILTER, std::deque<unsigned char>> LdapASN1Parser::parse_filter(std::deque<unsigned char> &data) {
    std::pair<LDAP_FILTER, std::deque<unsigned char>> filter;

    auto filter_type = static_cast<LDAP_FILTER>(data.front());
    switch (filter_type) {
        case LDAP_FILTER::AND:
        case LDAP_FILTER::OR:
        case LDAP_FILTER::NOT:
        case LDAP_FILTER::EQUALITYMATCH:
        case LDAP_FILTER::SUBSTRING:
        case LDAP_FILTER::PRESENT:
            filter.first = filter_type;
            break;
        default:
            // build the error message
            std::stringstream error_msg;
            error_msg << "Unknown operation: \'0x" << std::hex << std::setfill('0')
                      << std::setw(2) << static_cast<unsigned int>(filter_type)
                      << "\'";
            throw ParserError(error_msg.str());;
    }
    data.pop_front();   // pop filter type

    unsigned int len = data.front();
    data.pop_front();
    filter.second = get_data(data, len);
    return std::move(filter);
}

std::deque<unsigned char> LdapASN1Parser::get_data(size_t len) {
    std::deque<unsigned char> data;
    unsigned char buf[len];

    ssize_t n_recv = recv(m_socketHandle, buf, len, MSG_WAITALL);
    if (n_recv == -1) {
        std::cerr << "error: " << errno << ", " << std::strerror(errno) << std::endl;
        throw SocketError(std::strerror(errno));
    } else if (n_recv == 0 && len != 0){
        throw SocketEOF();
    } else if (n_recv != len ) {
        // build the error message
        std::stringstream error_msg;
        error_msg << "Cannot read needed data of demanded length, read: " << n_recv << " expected: " << len;
        throw ParserError(error_msg.str());
    }

#ifndef NDEBUG_TRACE
    std::stringstream ss;
    ss << std::hex << std::setfill('0');
    for (size_t i = 0; i < n_recv; ++i)
        ss << std::setw(2) << (unsigned int) buf[i];
    std::cout << "trace: ##################################" << std::endl;
    std::cout << "trace: read: " << n_recv << std::endl;
    std::cout << "trace: hex dump: 0x" << ss.str() << std::endl;
    std::cout << "trace: ##################################" << std::endl;
#endif

    data.assign(buf, buf + len);
    return std::move(data); // always successful, otherwise throw exception
}

std::deque<unsigned char> LdapASN1Parser::get_data(std::deque<unsigned char> &data, size_t len) {
    // validate data size which is needed
    check_data_size(data, len);

    std::deque<unsigned char> truncate_data;
    std::move(data.cbegin(), (data.cbegin() + len), std::back_inserter(truncate_data));
    data.erase(data.begin(), (data.begin() + len));

    return std::move(truncate_data); // always successful, otherwise throw exception
}

bool LdapASN1Parser::get_bool(std::deque<unsigned char> &data) {
    // validate data size which is needed
    check_data_size(data, 3);

    auto front = static_cast<ASN1_TYPE>(data.front());
    if (front != ASN1_TYPE::BOOL) {
        // build the error message
        std::stringstream error_msg;
        error_msg << "Boolean not detected, unknown token: \'0x" << std::hex << std::setfill('0')
                  << std::setw(2) << static_cast<unsigned int>(front)
                  << "\'";
        throw ParserError(error_msg.str());
    }
    data.pop_front();   // pop operation
    data.pop_front();   // pop length which should be 0x01
    return static_cast<bool>(to_int(data, 1));
}

int LdapASN1Parser::get_integer(std::deque<unsigned char> &data) {
    // validate data size which is needed
    check_data_size(data, 2);

    auto front = static_cast<ASN1_TYPE>(data.front());
    if (front != ASN1_TYPE::INTEGER) {
        // build the error message
        std::stringstream error_msg;
        error_msg << "Integer not detected, unknown token: \'0x" << std::hex << std::setfill('0')
                  << std::setw(2) << static_cast<unsigned int>(front)
                  << "\'";
        throw ParserError(error_msg.str());
    }
    data.pop_front();   // pop operation

    unsigned char len = data.front();
    data.pop_front();
    return to_int(data, len);
}

std::string LdapASN1Parser::get_string(std::deque<unsigned char> &data) {
    // validate data size which is needed
    check_data_size(data, 2);

    auto front = static_cast<ASN1_TYPE>(data.front());
    if (front != ASN1_TYPE::OCTET_STRING) {
        // build the error message
        std::stringstream error_msg;
        error_msg << "String not detected, unknown token: \'0x" << std::hex << std::setfill('0')
                  << std::setw(2) << static_cast<unsigned int>(front)
                  << "\'";
        throw ParserError(error_msg.str());
    }
    data.pop_front();   // pop operation

    unsigned char len = data.front();
    data.pop_front();

    return std::move(to_string(data, len));
}

unsigned char LdapASN1Parser::get_enum(std::deque<unsigned char> &data) {
    // validate data size which is needed
    check_data_size(data, 3);

    auto front = static_cast<ASN1_TYPE>(data.front());
    if (front != ASN1_TYPE::ENUM) {
        // build the error message
        std::stringstream error_msg;
        error_msg << "Boolean not detected, unknown token: \'0x" << std::hex << std::setfill('0')
                  << std::setw(2) << static_cast<unsigned int>(front)
                  << "\'";
        throw ParserError(error_msg.str());
    }
    data.pop_front();   // pop operation
    data.pop_front();   // pop length which should be 0x01
    return static_cast<unsigned char>(to_int(data, 1));
}

unsigned int LdapASN1Parser::get_sequence() {
    // get 0x30 followed by length
    std::deque<unsigned char> data = get_data(2);
    return get_sequence(data);
}

unsigned int LdapASN1Parser::get_sequence(std::deque<unsigned char> &data) {
    // validate data size which is needed
    check_data_size(data, 2);

    auto front = static_cast<ASN1_TYPE>(data.front());
    if (front != ASN1_TYPE::SEQUENCE) {
        // build the error message
        std::stringstream error_msg;
        error_msg << "Sequence not detected, unknown token: \'0x" << std::hex << std::setfill('0')
                  << std::setw(2) << static_cast<unsigned int>(front)
                  << "\'";
        throw ParserError(error_msg.str());
    }
    data.pop_front();   // pop operation

    unsigned char len = data.front();
    data.pop_front();
    return len;
}

const int LdapASN1Parser::get_size_limit() {
    return m_size_limit;
}

LdapFilter * LdapASN1Parser::get_filter() {
    return get_filter(m_filter);
}

LdapFilter * LdapASN1Parser::get_filter(std::pair<LDAP_FILTER, std::deque<unsigned char>> filter) {
    LdapFilterAnd *f_and;
    LdapFilterOr *f_or;
    switch (filter.first) {
        case LDAP_FILTER::AND:
            f_and = new LdapFilterAnd();
            while (!filter.second.empty()) {
                f_and->add_filter(get_filter(parse_filter(filter.second)));
            }
            return f_and;
        case LDAP_FILTER::OR:
            f_or = new LdapFilterOr();
            while (!filter.second.empty()) {
                f_or->add_filter(get_filter(parse_filter(filter.second)));
            }
            return f_or;
        case LDAP_FILTER::NOT:
            return new LdapFilterNot(get_filter(parse_filter(filter.second)));
        case LDAP_FILTER::EQUALITYMATCH:
            return new LdapFilterEqMatch(get_string(filter.second), get_string(filter.second));
        case LDAP_FILTER::SUBSTRING:
            //std::cout << "DEBUG: Not implemented" << std::endl;
            throw ParserError("Not implemented");
        case LDAP_FILTER::PRESENT:
            return new LdapFilterPresent(to_string(filter.second, filter.second.size()));
    }
}

std::string LdapASN1Parser::to_string(std::deque<unsigned char> &data, size_t len) {
    // validate data size which is needed
    check_data_size(data, len);
    std::string s;
    for (size_t i = 0; i < len; ++i) {
        s += data.front();
        data.pop_front();
    }
    return std::move(s);
}

int LdapASN1Parser::to_int(std::deque<unsigned char> &data, size_t size) {
    // validate data size
    if (size < 1 || size > 4 || data.size() < size) {
        // build the error message
        std::stringstream error_msg;
        error_msg << "Integer size out of range, size: \'0x" << std::hex << std::setfill('0')
                  << std::setw(2) << static_cast<unsigned int>(size)
                  << "\'";
        throw ParserError(error_msg.str());
    }

    int result = 0;
    for (unsigned int i = 0; i < size; ++i) {
        result += (static_cast<int>(data.front()) << 8*(size - (i+1)));
        data.pop_front();
    }
    return result;
}

void LdapASN1Parser::check_data_size(const std::deque<unsigned char> &data, size_t size) {
    if (data.size() < size) {
        // build the error message
        std::stringstream error_msg;
        error_msg << "Data size not sufficient, actual size: " << data.size() << " needed size: " << size;
        throw ParserError(error_msg.str());
    }
}

/** ******************************* Response ******************************* **/

void LdapASN1Parser::send_ldapMessage(int msg_id, std::deque<unsigned char> &&protocol_op) {
    std::deque<unsigned char> msg_id_data = build_int(msg_id);
    std::deque<unsigned char> msg{
            static_cast<unsigned char>(LDAP_TOKEN::LDAPMESSAGE),
            static_cast<unsigned char>(protocol_op.size() + msg_id_data.size()),
    };
    msg.insert(msg.end(), msg_id_data.cbegin(), msg_id_data.cend());
    msg.insert(msg.end(), protocol_op.cbegin(), protocol_op.cend());

    for (auto c : msg) {
        ssize_t n_res = send(m_socketHandle, &c, 1, 0);
        if (n_res == -1) {
            std::cerr << "error: " << std::strerror(errno) << std::endl;
            throw SocketError(std::strerror(errno));
        }
    }

#ifndef NDEBUG
    DEBUG_DEQUE(msg, "info: LdapMessage sent", "\t0x");
#endif
}

std::deque<unsigned char> LdapASN1Parser::build_ldapResult(LDAP_RESULT_CODE res_code, std::string &&matched_dn) {
    std::deque<unsigned char> res;
    std::deque<unsigned char> res_code_deque = build_enum(static_cast<unsigned char>(res_code));
    std::deque<unsigned char> matched_dn_deque = build_string(matched_dn);
    std::deque<unsigned char> error_str_deque = build_string("");
    res.insert(res.end(), res_code_deque.cbegin(), res_code_deque.cend());
    res.insert(res.end(), matched_dn_deque.cbegin(), matched_dn_deque.cend());
    res.insert(res.end(), error_str_deque.cbegin(), error_str_deque.cend());

    return res;
}

std::deque<unsigned char> LdapASN1Parser::build_bindResponse(std::deque<unsigned char> &&ldap_result) {
    std::deque<unsigned char> res{
            static_cast<unsigned char>(LDAP_TOKEN::BINDRESPONSE),
            static_cast<unsigned char>(ldap_result.size())
    };
    res.insert(res.end(), ldap_result.cbegin(), ldap_result.cend());
    return res;
}

std::deque<unsigned char> LdapASN1Parser::build_searchEntry(const std::map<std::string, std::string> &row) {
    std::deque<unsigned char> entry;
    std::deque<unsigned char> attributes_data;
    // string ObjectName
    std::deque<unsigned char> obj_str = build_string(m_ldap_dn);
    entry.insert(entry.end(), obj_str.cbegin(), obj_str.cend());

    if (m_attributes.empty()) // no attributes lead to returning all attributes
        m_attributes = {CN, UID, MAIL};
    for (const auto &attr : m_attributes) {
        // sequence of Partial AttributeList
        std::deque<unsigned char> item_data = build_string(attr);
        std::deque<unsigned char> val = build_string(row.at(attr));
        std::deque<unsigned char> vals = build_set(std::move(val)); // always only one in the used db

        item_data.insert(item_data.end(), vals.cbegin(), vals.cend());
        std::deque<unsigned char> attribute = build_sequence(std::move(item_data));

        attributes_data.insert(attributes_data.end(), attribute.cbegin(), attribute.cend());
    }
    std::deque<unsigned char> attributes = build_sequence(std::move(attributes_data));
    entry.insert(entry.end(), attributes.cbegin(), attributes.cend());

    std::deque<unsigned char> res{
            static_cast<unsigned char>(LDAP_TOKEN::SEARCHRESULTENTRY),
            static_cast<unsigned char>(entry.size())
    };
    res.insert(res.end(), entry.cbegin(), entry.cend());
    return res;
}

std::deque<unsigned char> LdapASN1Parser::build_searchDone(std::deque<unsigned char> &&ldap_result) {
    std::deque<unsigned char> res{
            static_cast<unsigned char>(LDAP_TOKEN::SEARCHRESULTDONE),
            static_cast<unsigned char>(ldap_result.size())
    };
    res.insert(res.end(), ldap_result.cbegin(), ldap_result.cend());
    return res;
}

std::deque<unsigned char> LdapASN1Parser::build_int(int val) {
    std::deque<unsigned char> res{ static_cast<unsigned char>(ASN1_TYPE::INTEGER) };
    size_t byte_size = 4;
    if (val >> 24 == 0) { --byte_size; }
    if (val >> 16 == 0) { --byte_size; }
    if (val >> 8  == 0) { --byte_size; }

    res.push_back(static_cast<unsigned char>(byte_size));
    for (size_t i = 0; i < byte_size; ++i) {
        res.push_back(static_cast<unsigned char>(val >> 8*(byte_size - (i+1))));
    }
    return res;
}

std::deque<unsigned char> LdapASN1Parser::build_string(std::string str) {
    if (str.size() > UCHAR_MAX) {
        // build the error message
        std::stringstream error_msg;
        error_msg << "String is too long, size: " << str.size();
        throw ParserError(error_msg.str());
    }
    auto size = static_cast<unsigned char>(str.size());

    std::deque<unsigned char> res{
            static_cast<unsigned char>(ASN1_TYPE::OCTET_STRING),
            size
    };
    for (auto &c : str)
        res.emplace_back(c);
    return res;
}

std::deque<unsigned char> LdapASN1Parser::build_enum(unsigned char val) {
    std::deque<unsigned char> res{
            static_cast<unsigned char>(ASN1_TYPE::ENUM),
            0x01,
            val
    };
    return res;
}

std::deque<unsigned char> LdapASN1Parser::build_sequence(std::deque<unsigned char> &&data) {
    std::deque<unsigned char> res{
            static_cast<unsigned char>(ASN1_TYPE::SEQUENCE),
            static_cast<unsigned char>(data.size())
    };
    res.insert(res.end(), data.cbegin(), data.cend());
    return res;
}

std::deque<unsigned char> LdapASN1Parser::build_set(std::deque<unsigned char> &&data) {
    std::deque<unsigned char> res{
            static_cast<unsigned char>(ASN1_TYPE::SET),
            static_cast<unsigned char>(data.size())
    };
    res.insert(res.end(), data.cbegin(), data.cend());
    return res;
}

const std::string LdapASN1Parser::op_to_str(LDAP_OPERATION op) {
    switch (op) {
        case LDAP_OPERATION::BINDREQUEST:
            return "BINDREQUEST";
        case LDAP_OPERATION::UNBINDREQUEST:
            return "UNBINDREQUEST";
        case LDAP_OPERATION::SEARCHREQUEST:
            return "SEARCHREQUEST";
    }
    return "(unknown)";
}

const std::string LdapASN1Parser::filter_to_str(LDAP_FILTER filter) {
    switch (filter) {
        case LDAP_FILTER::AND:
            return "AND";
        case LDAP_FILTER::OR:
            return "OR";
        case LDAP_FILTER::NOT:
            return "NOT";
        case LDAP_FILTER::EQUALITYMATCH:
            return "EQUALITYMATCH";
        case LDAP_FILTER::SUBSTRING:
            return "SUBSTRING";
        case LDAP_FILTER::PRESENT:
            return "PRESENT";
    }
    return "(unknown)";
}
