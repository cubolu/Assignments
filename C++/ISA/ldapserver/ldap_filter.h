/**
 * ldap_filter.h
 * Project:     ISA Assignment, LDAP Server
 * Compiled:    gcc version 4.9.3 (Ubuntu 16.04)
 * Author:      Jakub Lukac
 * e-mail:      xlukac09@stud.fit.vutbr.cz
 * Created:     18-11-2017
 *
 */

#ifndef LDAPSERVER_LDAP_FILTER_H
#define LDAPSERVER_LDAP_FILTER_H

#include <string>
#include <vector>
#include <map>
#include "ldap_protocol.h"

class LdapFilter {
public:
    virtual bool match(const std::map<std::string, std::string> &row) = 0;
};

class LdapFilterAnd : public LdapFilter {
public:
    ~LdapFilterAnd();

    bool match(const std::map<std::string, std::string> &row) override;
    void add_filter(LdapFilter *filter);

private:
    std::vector<LdapFilter *> m_filters;
};

class LdapFilterOr : public LdapFilter {
public:
    ~LdapFilterOr();

    bool match(const std::map<std::string, std::string> &row) override;
    void add_filter(LdapFilter *filter);

private:
    std::vector<LdapFilter *> m_filters;
};

class LdapFilterNot : public LdapFilter {
public:
    explicit LdapFilterNot(LdapFilter *filter);
    ~LdapFilterNot();

    bool match(const std::map<std::string, std::string> &row) override;

private:
    LdapFilter *m_filter;
};

class LdapFilterEqMatch : public LdapFilter {
public:
    LdapFilterEqMatch(std::string m_attributeDesc, std::string m_assertionValue);

    bool match(const std::map<std::string, std::string> &row) override;

private:
    std::string m_attributeDesc;
    std::string m_assertionValue;
};

class LdapFilterSubstring : public LdapFilter {
public:
    bool match(const std::map<std::string, std::string> &row) override; // TODO

private:
    // TODO
};

class LdapFilterPresent : public LdapFilter {
public:
    explicit LdapFilterPresent(std::string m_present);

    bool match(const std::map<std::string, std::string> &row) override;

private:
    std::string m_present;
};

#endif //LDAPSERVER_LDAP_FILTER_H
