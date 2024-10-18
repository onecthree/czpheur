#include <php.h>
#include <ext/standard/info.h>
#include <php_zpheur.h>

#include <string.h>
#include <zpheur.h>
#include <include/runtime.h>
#include "HeaderTrait_arginfo.h"

void local_set_header( char* header_key, char* value_src )
{

    size_t     serve_len = 0;

    serve_len = snprintf(NULL, 0, "%s: %s", header_key, value_src);
    char* serve_src = (char*)malloc(serve_len);

    php_sprintf(serve_src, "%s: %s", header_key, value_src);
    char* copy_serve_src = (char*)malloc(serve_len);
    strncpy(copy_serve_src, serve_src, serve_len);

    sapi_header_line ctr = {0};
    ctr.line = copy_serve_src;
    ctr.line_len = serve_len;
    sapi_header_op(SAPI_HEADER_REPLACE, &ctr);

    cleanup:
        free(serve_src);
}

// PHP_METHOD(HeaderTrait, __construct)
// {
//     ZEND_PARSE_PARAMETERS_NONE();
// }

PHP_METHOD(HeaderTrait, set)
{
    zval*        header = 0;

    char*        value_src = NULL;
    size_t       value_len = 0;

    ZEND_PARSE_PARAMETERS_START(2, 2)
        Z_PARAM_ZVAL(header)
        Z_PARAM_STRING(value_src, value_len)
    ZEND_PARSE_PARAMETERS_END();
       
    if(! value_src )
    {
        zend_error(E_ERROR, "value_src must be filled, with use inline header");
    }       
    
    local_set_header(Z_STRVAL_P(header), value_src);

    // RETURN_ZVAL(getThis(), 0, 0);
}

PHP_METHOD(HeaderTrait, redirect)
{
    char*        url_src = NULL;
    size_t       url_len = 0;

    zval*        is_permanent = NULL;

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
    sapi_header_op(SAPI_HEADER_REPLACE, &ctr);

    url_src[url_len] = '\0';
    size_t redirect_target_len = snprintf(NULL, 0, "Location: %s", url_src);
    char redirect_target_src[redirect_target_len];
    php_sprintf(redirect_target_src, "Location: %s", url_src);

    ctr.line = redirect_target_src;
    ctr.line_len = redirect_target_len;
    sapi_header_op(SAPI_HEADER_REPLACE, &ctr);

    RETURN_ZVAL(getThis(), 0, 0);
}

ZEND_MINIT_FUNCTION(Zpheur_Schemes_Http_Message_HeaderTrait)
{
    zend_class_entry ce;

    INIT_NS_CLASS_ENTRY(ce, "Zpheur\\Schemes\\Http\\Message", "HeaderTrait", zpheur_schemes_http_message_headertrait_class_method);
    zpheur_schemes_http_message_headertrait_class_entry = zend_register_internal_class(&ce);
    zpheur_schemes_http_message_headertrait_class_entry->ce_flags |= ZEND_ACC_TRAIT;
    zpheur_schemes_http_message_headertrait_class_entry->ce_flags |= ZEND_ACC_NO_DYNAMIC_PROPERTIES;

    zend_declare_property_long(zpheur_schemes_http_message_headertrait_class_entry, "statusCode", sizeof("statusCode") - 1, 200, ZEND_ACC_PUBLIC);

    return SUCCESS;
}