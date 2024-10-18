#include <php.h>
#include <ext/standard/info.h>
#include <php_zpheur.h>

#include <string.h>
#include <zpheur.h>
#include <include/runtime.h>
#include "ErrorRequest_arginfo.h"

PHP_METHOD(ErrorRequest, foo)
{
    ZEND_PARSE_PARAMETERS_NONE();

    sapi_header_line ctr = {0};

    ctr.line = "content-type: text/plain";
    ctr.line_len = sizeof("content-type: text/plain") - 1;

    sapi_header_op(SAPI_HEADER_REPLACE, &ctr);

    php_printf("hello all");
}

ZEND_MINIT_FUNCTION(Zpheur_Schemes_Http_Responder_ErrorRequest)
{
    zend_class_entry ce;

    INIT_NS_CLASS_ENTRY(ce, "Zpheur\\Schemes\\Http\\Responder", "ErrorRequest", zpheur_schemes_http_responder_errorrequest_class_method);
    zpheur_schemes_http_responder_errorrequest_class_entry = zend_register_internal_class(&ce);
    zpheur_schemes_http_responder_errorrequest_class_entry->ce_flags |= ZEND_ACC_TRAIT;
    zpheur_schemes_http_responder_errorrequest_class_entry->ce_flags |= ZEND_ACC_ALLOW_DYNAMIC_PROPERTIES;

    return SUCCESS;
}