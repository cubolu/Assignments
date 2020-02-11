/**
 * ldap_protocol.h
 * Project:     ISA Assignment, LDAP Server
 * Compiled:    gcc version 4.9.3 (Ubuntu 16.04)
 * Author:      Jakub Lukac
 * e-mail:      xlukac09@stud.fit.vutbr.cz
 * Created:     14-10-2017
 *
 */

#ifndef LDAPSERVER_LDAP_PROTOCOL_H
#define LDAPSERVER_LDAP_PROTOCOL_H

#include "asn1_types.h"

enum class LDAP_TOKEN : unsigned char {
    LDAPMESSAGE         = static_cast<unsigned char>(ASN1_TYPE::SEQUENCE),
    LDAPRESULT          = static_cast<unsigned char>(ASN1_TYPE::ENUM),
    BINDREQUEST         = 0x60,
    BINDRESPONSE        = 0x61,
    UNBINDREQUEST       = 0x42, // length 0x00
    SEARCHREQUEST       = 0x63,
    SEARCHRESULTENTRY   = 0x64,
    SEARCHRESULTDONE    = 0x65,

    BINDSIMPLE          = 0x80,
    BINDSASL            = 0xA3,
};

/// Supported operation only
enum class LDAP_OPERATION : unsigned char {
    BINDREQUEST         = static_cast<unsigned char>(LDAP_TOKEN::BINDREQUEST),
    UNBINDREQUEST       = static_cast<unsigned char>(LDAP_TOKEN::UNBINDREQUEST),
    SEARCHREQUEST       = static_cast<unsigned char>(LDAP_TOKEN::SEARCHREQUEST),
};

/// Supported filter types only
enum class LDAP_FILTER : unsigned char {
    AND                 = 0xA0,
    OR                  = 0xA1,
    NOT                 = 0xA2,
    EQUALITYMATCH       = 0xA3,
    SUBSTRING           = 0xA4,
    PRESENT             = 0x87,
};


enum class LDAP_RESULT_CODE : unsigned char {
    SUCCESS             = 0x00,
    OPERATIONSERROR     = 0x01,
    PROTOCOLERROR       = 0x02,
    ///...
    SIZELIMITEXCEEDED   = 0x04,
    ///...
    INVALIDCREDENTIALS  = 0x49,
    ///...
};


#endif //LDAPSERVER_LDAP_PROTOCOL_H
