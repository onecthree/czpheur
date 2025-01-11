#include <php.h>
#include <ext/standard/info.h>
#include <php_zpheur.h>

#include <string.h>
#include <zpheur.h>
#include <include/onecstr.h>
#include <include/runtime.h>
#include "HeaderTrait_arginfo.h"

void local_set_header( char* key_src, size_t key_len, char* value_src, size_t value_len )
{
    // "e.g. content-type: application/json"
    onec_string* header;
    onec_string_init(header);

    onec_string_append(header, 3, key_src, ": ", value_src);

    sapi_header_line ctr = {0};
    ctr.line = header->val;
    ctr.line_len = header->len;
    sapi_header_op(SAPI_HEADER_REPLACE, &ctr);

    onec_string_release(header);
}

PHP_METHOD(HeaderTrait, set)
{
    char* key_src = NULL;
    size_t key_len = 0;
    char* value_src = NULL;
    size_t value_len = 0;

    ZEND_PARSE_PARAMETERS_START(2, 2)
        Z_PARAM_STRING(key_src, key_len)
        Z_PARAM_STRING(value_src, value_len)
    ZEND_PARSE_PARAMETERS_END();
       
    if(! value_src )
    {
        zend_error(E_ERROR, "value_src must be filled, with use inline header");
    }       
    
    local_set_header(key_src, key_len, value_src, value_len);
}

PHP_METHOD(HeaderTrait, redirect)
{
    char* url_src = NULL;
    size_t url_len = 0;
    zval* is_permanent = NULL;

    ZEND_PARSE_PARAMETERS_START(1, 2)
        Z_PARAM_STRING(url_src, url_len)
        Z_PARAM_OPTIONAL
        Z_PARAM_ZVAL(is_permanent)
    ZEND_PARSE_PARAMETERS_END();
    
    sapi_header_line ctr = {0};

    if( is_permanent && zval_get_long(is_permanent) )
    {
        ctr.line = "HTTP/1.1 301 Moved Permanently";
        ctr.line_len = sizeof("HTTP/1.1 301 Moved Permanently") - 1;
    }
    else
    {
        ctr.line = "HTTP/1.1 307 Temporary Redirect";
        ctr.line_len = sizeof("HTTP/1.1 307 Temporary Redirect") - 1;
    }
    // Replacing header response
    sapi_header_op(SAPI_HEADER_REPLACE, &ctr);

    onec_string* header;
    onec_string_init(header);

    onec_string_append(header, 2, "Location: ", url_src);

    ctr.line = header->val;
    ctr.line_len = header->len;
    // Add location redirect
    sapi_header_op(SAPI_HEADER_REPLACE, &ctr); 

    onec_string_release(header);

    RETURN_ZVAL(getThis(), 0, 0);
}

ZEND_MINIT_FUNCTION(Zpheur_Schemes_WebSocket_Message_HeaderTrait)
{
    zend_class_entry ce;

    INIT_NS_CLASS_ENTRY(ce, "Zpheur\\Schemes\\WebSocket\\Message", "HeaderTrait", zpheur_schemes_websocket_message_headertrait_class_method);
    zpheur_schemes_websocket_message_headertrait_class_entry = zend_register_internal_class(&ce);
    zpheur_schemes_websocket_message_headertrait_class_entry->ce_flags |= ZEND_ACC_TRAIT;
    zpheur_schemes_websocket_message_headertrait_class_entry->ce_flags |= ZEND_ACC_NO_DYNAMIC_PROPERTIES;

    zend_declare_property_long(zpheur_schemes_websocket_message_headertrait_class_entry, "statusCode", sizeof("statusCode") - 1, 200, ZEND_ACC_PUBLIC);

    return SUCCESS;
}