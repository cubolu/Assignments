/**
 * asn1_types.h
 * Project:     ISA Assignment, LDAP Server
 * Compiled:    gcc version 4.9.3 (Ubuntu 16.04)
 * Author:      Jakub Lukac
 * e-mail:      xlukac09@stud.fit.vutbr.cz
 * Created:     14-10-2017
 *
 */

#ifndef LDAPSERVER_ASN1_TYPES_H
#define LDAPSERVER_ASN1_TYPES_H

enum class ASN1_TYPE : unsigned char {
    BOOL                = 0x01,
    INTEGER             = 0x02,
    OCTET_STRING        = 0x04,
    ENUM                = 0x0A,
    SEQUENCE            = 0x30,
    SET                 = 0x31,
};


#endif //LDAPSERVER_ASN1_TYPES_H
