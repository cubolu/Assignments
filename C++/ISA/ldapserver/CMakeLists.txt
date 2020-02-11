cmake_minimum_required(VERSION 3.6)
project(ldapserver)

set(CMAKE_CXX_STANDARD 11)

add_definitions(-DNDEBUG)
add_definitions(-DNDEBUG_TRACE)

set(SOURCE_FILES myldap.cpp ldap.cpp ldap.h ldap_ans1_parser.cpp ldap_ans1_parser.h ldap_protocol.h asn1_types.h ldap_filter.cpp ldap_filter.h)
add_executable(myldap ${SOURCE_FILES})