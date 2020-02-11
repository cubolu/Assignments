/**
 * ldap_filter.cpp
 * Project:     ISA Assignment, LDAP Server
 * Compiled:    gcc version 4.9.3 (Ubuntu 16.04)
 * Author:      Jakub Lukac
 * e-mail:      xlukac09@stud.fit.vutbr.cz
 * Created:     18-11-2017
 *
 */

#include "ldap_filter.h"

LdapFilterAnd::~LdapFilterAnd() {
    for (auto &f : m_filters)
        delete f;
}

bool LdapFilterAnd::match(const std::map<std::string, std::string> &row) {
    for (auto &f : m_filters) {
        if ( ! f->match(row) ) return false; // lazy evaluation
    }
    return true;
}

void LdapFilterAnd::add_filter(LdapFilter *filter) {
    m_filters.push_back(filter);
}

LdapFilterOr::~LdapFilterOr() {
    for (auto &f : m_filters)
        delete f;
}

bool LdapFilterOr::match(const std::map<std::string, std::string> &row) {
    for (auto &f : m_filters) {
        if ( f->match(row) ) return true; // lazy evaluation
    }
    return false;
}

void LdapFilterOr::add_filter(LdapFilter *filter) {
    m_filters.push_back(filter);
}

LdapFilterNot::LdapFilterNot(LdapFilter *filter)
        : m_filter(filter) {}

bool LdapFilterNot::match(const std::map<std::string, std::string> &row) {
    return !m_filter->match(row);
}

LdapFilterNot::~LdapFilterNot() {
    delete m_filter;
}

LdapFilterEqMatch::LdapFilterEqMatch(std::string m_attributeDesc, std::string m_assertionValue)
        : m_attributeDesc(std::move(m_attributeDesc)), m_assertionValue(std::move(m_assertionValue)) {}

bool LdapFilterEqMatch::match(const std::map<std::string, std::string> &row) {
    row.find(m_attributeDesc);
    auto column = row.find(m_attributeDesc);
    if(column != row.end())
        return column->second == m_assertionValue;

    return false;
}

LdapFilterPresent::LdapFilterPresent(std::string m_present)
        : m_present(std::move(m_present)) {}

bool LdapFilterPresent::match(const std::map<std::string, std::string> &row) {
    return true;
}
