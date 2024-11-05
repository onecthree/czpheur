#include <php.h>
#include <ext/standard/info.h>
#include <php_zpheur.h>

#include <string.h>
#include <zpheur.h>
#include <include/runtime.h>
#include "Request_arginfo.h"


PHP_METHOD(Request, __construct)
{
    zval*       request;
    zval*       query;
    zval*       cookies;
    zval*       attributes;
    zval*       files;
    zval*       server;
    zval*       headers;

    ZEND_PARSE_PARAMETERS_START(6, 6)
        Z_PARAM_ARRAY(request)
        Z_PARAM_ARRAY(query)
        Z_PARAM_ARRAY(cookies)
        Z_PARAM_ARRAY(attributes)
        Z_PARAM_ARRAY(files)
        Z_PARAM_ARRAY(server)
    ZEND_PARSE_PARAMETERS_END();

    /* {{{ REQUEST */
    zend_object* input_object = php_class_init(
        "Zpheur\\Schemes\\Http\\Foundation\\InputBag",
        sizeof("Zpheur\\Schemes\\Http\\Foundation\\InputBag") - 1
    );
    {
        zval* params___construct = safe_emalloc(1, sizeof(zval), 0);
        ZVAL_ZVAL(&params___construct[0], request, 1, 0);

        php_class_call_constructor(input_object, 1, params___construct);
    }

    zval target_input;
    ZVAL_OBJ(&target_input, input_object);
    zend_this_update_property("input", &target_input);
    /* }}} */   

    /* {{{ QUERY */
    zend_object* query_object = php_class_init(
        "Zpheur\\Schemes\\Http\\Foundation\\InputBag",
        sizeof("Zpheur\\Schemes\\Http\\Foundation\\InputBag") - 1
    );
    {
        zval* params___construct = safe_emalloc(1, sizeof(zval), 0);
        ZVAL_ZVAL(&params___construct[0], query, 1, 0);

        php_class_call_method(query_object, "__construct", sizeof("__construct") - 1, 1, params___construct, 0);
    }

    zval target_query;
    ZVAL_OBJ(&target_query, query_object);
    zend_this_update_property("query", &target_query);
    /* }}} */

    /* {{{ COOKIES */
    zend_object* cookies_object = php_class_init(
        "Zpheur\\Schemes\\Http\\Foundation\\InputBag",
        sizeof("Zpheur\\Schemes\\Http\\Foundation\\InputBag") - 1
    );
    {
        zval* params___construct = safe_emalloc(1, sizeof(zval), 0);
        ZVAL_ZVAL(&params___construct[0], cookies, 1, 0);

        php_class_call_method(cookies_object, "__construct", sizeof("__construct") - 1, 1, params___construct, 0);
    }

    zval target_cookies;
    ZVAL_OBJ(&target_cookies, cookies_object);
    zend_this_update_property("cookies", &target_cookies);
    /* }}} */

    /* {{{ ATTRIBUTES */
    zend_object* attributes_object = php_class_init(
        "Zpheur\\Schemes\\Http\\Foundation\\ParameterBag",
        sizeof("Zpheur\\Schemes\\Http\\Foundation\\ParameterBag") - 1
    );
    {
        zval* params___construct = safe_emalloc(1, sizeof(zval), 0);
        ZVAL_ZVAL(&params___construct[0], attributes, 1, 0);

        php_class_call_method(attributes_object, "__construct", sizeof("__construct") - 1, 1, params___construct, 0);
    }

    zval target_attributes;
    ZVAL_OBJ(&target_attributes, attributes_object);
    zend_this_update_property("attributes", &target_attributes);
    /* }}} */   

    zend_this_update_property("files", files);


    /* {{{ HEADERS */
    zend_object* header_object = php_class_init(
        "Zpheur\\Schemes\\Http\\Foundation\\HeaderBag",
        sizeof("Zpheur\\Schemes\\Http\\Foundation\\HeaderBag") - 1
    );
    {
        zval* params___construct = safe_emalloc(1, sizeof(zval), 0);
        ZVAL_ZVAL(&params___construct[0], server, 1, 0);

        php_class_call_method(header_object, "__construct", sizeof("__construct") - 1, 1, params___construct, 0);
    }

    zval target_headers;
    ZVAL_OBJ(&target_headers, header_object);
    zend_this_update_property("headers", &target_headers);
    /* }}} */


    /* {{{ SERVER */
    zend_object* server_object = php_class_init(
        "Zpheur\\Schemes\\Http\\Foundation\\ServerBag",
        sizeof("Zpheur\\Schemes\\Http\\Foundation\\ServerBag") - 1
    );
    {
        zval* params___construct = safe_emalloc(1, sizeof(zval), 0);
        ZVAL_ZVAL(&params___construct[0], server, 1, 0);

        php_class_call_method(server_object, "__construct", sizeof("__construct") - 1, 1, params___construct, 0);
    }

    zval target_server;
    ZVAL_OBJ(&target_server, server_object);
    zend_this_update_property("server", &target_server);
    /* }}} */
}

PHP_METHOD(Request, parseQuery)
{
    zval*         zv_query;
    char*         is_index_srch = NULL;
    size_t        is_index_len = 0;

    ZEND_PARSE_PARAMETERS_START(2, 2)
    Z_PARAM_ARRAY(zv_query)
    Z_PARAM_STRING(is_index_srch, is_index_len)
    ZEND_PARSE_PARAMETERS_END();

    bool begin = false;

    HashTable *ht_retval;
    ALLOC_HASHTABLE(ht_retval);
    zend_hash_init(ht_retval, 0, NULL, ZVAL_PTR_DTOR, 0);

    if( is_index_len > 1 && is_index_srch[1] == '?' )
    {
        begin = true;
    }

    ZEND_HASH_FOREACH_KEY_VAL_IND(Z_ARR_P(zv_query), zend_ulong num_index, zend_string* index, zval* value)
    {
        if( !begin )
        {
            begin = true;
            continue;
        }

        if(! index )
        {
            size_t buffer_len = snprintf(NULL, 0, "%ld", num_index);
            char buffer[buffer_len + 1];
            buffer[buffer_len] = '\0';

            index = zend_string_init(buffer, buffer_len, 0);
        }

        zend_hash_str_add(ht_retval, index->val, index->len, value);   
    }
    ZEND_HASH_FOREACH_END();

    RETURN_ARR(ht_retval);
}

PHP_METHOD(Request, parseHeader)
{
    zval*         global_server;

    ZEND_PARSE_PARAMETERS_START(1, 1)
    Z_PARAM_ZVAL(global_server)
    ZEND_PARSE_PARAMETERS_END();

    zend_object *retval_obj = php_class_init("stdClass", sizeof("stdClass") - 1);

    ZEND_HASH_FOREACH_STR_KEY_VAL(Z_ARR_P(global_server), zend_string* index, zval* value )
    {
        /*
        *   Stylized case to chamelCase from UPPER_CASE
        */
        if( strncmp(index->val, "HTTP_", 5) == 0 )
        {
            char*      buffer = (char*)malloc(2);
            size_t     buffer_size = 2;
            size_t     buffer_index = 1;
            char       ch;

            buffer[0] = tolower(index->val[5]);

            for( int indexOfHeader = 6; indexOfHeader < index->len; ++indexOfHeader )
            {
                ch = tolower(index->val[indexOfHeader]);

                if( buffer_index >= buffer_size )
                {
                    buffer_size *= 2;
                    char*   _buffer = (char*)realloc(buffer, buffer_size);

                    if(! _buffer )
                        zend_error(E_ERROR, "Unexpected error [E-BRE-1]");

                    buffer = _buffer;
                }

                if( ch == '_' )
                {
                    ch = index->val[++indexOfHeader];
                }

                buffer[buffer_index++] = ch;
            }

            buffer[buffer_index] = '\0';
            zend_update_property(NULL, retval_obj, buffer, buffer_index, value);
        }        
    }
    ZEND_HASH_FOREACH_END();

    RETURN_OBJ(retval_obj);
}

ZEND_MINIT_FUNCTION(Zpheur_Schemes_Http_Foundation_Request)
{
    zend_class_entry ce;

    INIT_NS_CLASS_ENTRY(ce, "Zpheur\\Schemes\\Http\\Foundation", "Request", zpheur_schemes_http_foundation_request_class_method);
    zpheur_schemes_http_foundation_request_class_entry = zend_register_internal_class(&ce);
    zpheur_schemes_http_foundation_request_class_entry->ce_flags |= ZEND_ACC_ALLOW_DYNAMIC_PROPERTIES;

    zend_declare_property_null(zpheur_schemes_http_foundation_request_class_entry, "input", sizeof("input") - 1, ZEND_ACC_PUBLIC);
    zend_declare_property_null(zpheur_schemes_http_foundation_request_class_entry, "query", sizeof("query") - 1, ZEND_ACC_PUBLIC);
    zend_declare_property_null(zpheur_schemes_http_foundation_request_class_entry, "cookies", sizeof("cookies") - 1, ZEND_ACC_PUBLIC);
    zend_declare_property_null(zpheur_schemes_http_foundation_request_class_entry, "attributes", sizeof("attributes") - 1, ZEND_ACC_PUBLIC);
    zend_declare_property_null(zpheur_schemes_http_foundation_request_class_entry, "files", sizeof("files") - 1, ZEND_ACC_PUBLIC);
    zend_declare_property_null(zpheur_schemes_http_foundation_request_class_entry, "server", sizeof("server") - 1, ZEND_ACC_PUBLIC);
    zend_declare_property_null(zpheur_schemes_http_foundation_request_class_entry, "headers", sizeof("headers") - 1, ZEND_ACC_PUBLIC);

    return SUCCESS;
}
