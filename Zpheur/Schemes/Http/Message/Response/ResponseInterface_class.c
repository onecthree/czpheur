#include <php.h>
#include <ext/standard/info.h>
#include <php_zpheur.h>

#include <string.h>
#include <zpheur.h>
#include <include/runtime.h>
#include "ResponseInterface_arginfo.h"


PHP_METHOD(ResponseInterface, send)
{
    // Interface method (empty body)
}

PHP_METHOD(ResponseInterface, json)
{
    // Interface method (empty body)
}

ZEND_MINIT_FUNCTION(Zpheur_Schemes_Http_Message_Response_ResponseInterface)
{
    zend_class_entry ce;

    INIT_NS_CLASS_ENTRY(ce, "Zpheur\\Schemes\\Http\\Message\\Response", "ResponseInterface", zpheur_schemes_http_message_response_responseinterface_class_method);
    zpheur_schemes_http_message_response_responseinterface_class_entry = zend_register_internal_class(&ce);
    zpheur_schemes_http_message_response_responseinterface_class_entry->ce_flags |= ZEND_ACC_INTERFACE;
    zpheur_schemes_http_message_response_responseinterface_class_entry->ce_flags |= ZEND_ACC_NO_DYNAMIC_PROPERTIES;

    zend_declare_class_constant_stringl(zpheur_schemes_http_message_response_responseinterface_class_entry, "SERVER", sizeof("SERVER") - 1, "Server", sizeof("Server") - 1);
    zend_declare_class_constant_stringl(zpheur_schemes_http_message_response_responseinterface_class_entry, "CACHE_CONTROL", sizeof("CACHE_CONTROL") - 1, "Cache-Control", sizeof("Cache-Control") - 1);
    zend_declare_class_constant_stringl(zpheur_schemes_http_message_response_responseinterface_class_entry, "AGE", sizeof("AGE") - 1, "Age", sizeof("Age") - 1);
    zend_declare_class_constant_stringl(zpheur_schemes_http_message_response_responseinterface_class_entry, "ETAG", sizeof("ETAG") - 1, "ETag", sizeof("ETag") - 1);
    zend_declare_class_constant_stringl(zpheur_schemes_http_message_response_responseinterface_class_entry, "LAST_MODIFIED", sizeof("LAST_MODIFIED") - 1, "Last-Modified", sizeof("Last-Modified") - 1);
    zend_declare_class_constant_stringl(zpheur_schemes_http_message_response_responseinterface_class_entry, "DATE", sizeof("DATE") - 1, "DATE", sizeof("Date") - 1);
    zend_declare_class_constant_stringl(zpheur_schemes_http_message_response_responseinterface_class_entry, "CONTENT_TYPE", sizeof("CONTENT_TYPE") - 1, "Content-Type", sizeof("Content-Type") - 1);
    zend_declare_class_constant_stringl(zpheur_schemes_http_message_response_responseinterface_class_entry, "CSRF_TOKEN", sizeof("CSRF_TOKEN") - 1, "X-CSRF-Token", sizeof("X-CSRF-Token") - 1);

    zend_declare_class_constant_stringl(zpheur_schemes_http_message_response_responseinterface_class_entry, "DATETIME_FORMAT", sizeof("DATETIME_FORMAT") - 1, "D, d M Y H:i:s \\G\\M\\T", sizeof("D, d M Y H:i:s \\G\\M\\T") - 1);

    return SUCCESS;
}