/**
 * ldap_asn1_parser.h
 * Project:     ISA Assignment, LDAP Server
 * Compiled:    gcc version 4.9.3 (Ubuntu 16.04)
 * Author:      Jakub Lukac
 * e-mail:      xlukac09@stud.fit.vutbr.cz
 * Created:     21-10-2017
 *
 */

#ifndef LDAPSERVER_ASN1_PARSER_H
#define LDAPSERVER_ASN1_PARSER_H

#include <string>
#include <exception>
#include <deque>
#include <vector>
#include <utility>

#include "asn1_types.h"
#include "ldap_protocol.h"
#include "ldap_filter.h"

// Debug macro
#ifndef NDEBUG
#define DEBUG_DEQUE(data, msg, prefix)      std::cout << (msg) << " (" << (data).size() << ")" << std::endl << (prefix);\
                                            for (auto d : (data)) \
                                                std::cout << std::hex << std::setfill('0') << std::setw(2) << (unsigned int) d; \
                                            std::cout << std::dec << std::endl
#else
#define DEBUG_DEQUE(data, msg, prefix)
#endif

#define CN              "cn"
#define UID             "uid"
#define MAIL            "mail"

struct ParserError : public std::exception {
    explicit ParserError(std::string &&msg) : m_msg(msg) {

    }
    const char * what() const throw () override {
        return m_msg.c_str();
    }

    std::string m_msg;
};

struct SocketError : public std::exception {
    explicit SocketError(const char *msg) : m_msg(msg) {

    }
    const char * what() const throw () override {
        return m_msg;
    }

    const char *m_msg;
};

struct SocketEOF : public std::exception {

};

class LdapASN1Parser {
public:
    explicit LdapASN1Parser(int socketHandle);
    ~LdapASN1Parser();

    void clear();

    int parse_ldapMessage();
    LDAP_OPERATION parse_operation();

    static bool get_bool(std::deque<unsigned char> &data);
    static int get_integer(std::deque<unsigned char> &data);
    static std::string get_string(std::deque<unsigned char> &data);
    static unsigned char get_enum(std::deque<unsigned char> &data);
    static unsigned int get_sequence(std::deque<unsigned char> &data);

    const int get_size_limit();
    LdapFilter * get_filter();

    void send_ldapMessage(int msg_id, std::deque<unsigned char> &&protocol_op);
    std::deque<unsigned char> build_ldapResult(LDAP_RESULT_CODE res_code, std::string &&matched_dn);
    std::deque<unsigned char> build_serverSaslCreds();
    std::deque<unsigned char> build_bindResponse(std::deque<unsigned char> &&ldap_result);
    std::deque<unsigned char> build_bindResponse(std::deque<unsigned char> &&ldap_result, std::deque<unsigned char> &&sasl);
    std::deque<unsigned char> build_searchEntry(const std::map<std::string, std::string> &row);
    std::deque<unsigned char> build_searchDone(std::deque<unsigned char> &&ldap_result);

    static const std::string op_to_str(LDAP_OPERATION op);
    static const std::string filter_to_str(LDAP_FILTER filter);

private:
    const int m_socketHandle;                   /// socket handle to access network connection

    std::deque<unsigned char> m_data;
    std::string m_ldap_dn;
    int m_size_limit;
    std::pair<LDAP_FILTER, std::deque<unsigned char>> m_filter;
    //std::deque<unsigned char> m_filter_data;
    //LDAP_FILTER m_filter_type;
    std::vector<std::string> m_attributes;

    void parse_operation_body(LDAP_OPERATION op, unsigned char len);
    void parse_bindrequest();
    void parse_unbindrequest();
    void parse_searchrequest();
    std::pair<LDAP_FILTER, std::deque<unsigned char>> parse_filter(std::deque<unsigned char> &data);
    LdapFilter * get_filter(std::pair<LDAP_FILTER, std::deque<unsigned char>> filter);

    std::deque<unsigned char> get_data(size_t len);
    std::deque<unsigned char> get_data(std::deque<unsigned char> &data, size_t len);
    unsigned int get_sequence();

    std::deque<unsigned char> build_int(int val);
    std::deque<unsigned char> build_string(std::string str);
    std::deque<unsigned char> build_enum(unsigned char val);
    std::deque<unsigned char> build_sequence(std::deque<unsigned char> &&data);
    std::deque<unsigned char> build_set(std::deque<unsigned char> &&data);

    static std::string to_string(std::deque<unsigned char> &data, size_t len);
    static int to_int(std::deque<unsigned char> &data, size_t size);

    static void check_data_size(const std::deque<unsigned char> &data, size_t size);
};


#endif //LDAPSERVER_ASN1_PARSER_H
