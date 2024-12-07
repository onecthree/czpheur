#include <php.h>
#include <ext/standard/info.h>
#include <php_zpheur.h>

#include <string.h>
#include <zpheur.h>
#include <include/runtime.h>
#include <include/onecstr.h>
#include <Zpheur/Schemes/Http/Foundation/ParameterBag_arginfo.h>
#include "Request_arginfo.h"


zend_object_handlers request_object_handlers;

typedef struct _request_common_t
{
    zend_object* input; // Need release
    zend_object* query; // Need release
    zend_object* cookie; // Need release
    zend_object* attribute; // Need release
    zend_object* header; // Need release
    zend_object* server; // Need release
} request_common_t;

typedef struct _request_object
{
    request_common_t* common;
    zend_object std;
} request_object;

void free_request_object(zend_object* object)
{
    request_object* instance = ZPHEUR_GET_OBJECT(request_object, object);

    zend_object_std_dtor(&instance->std);
    if( instance->common )
    {
        if( instance->common->input != NULL )
            zend_object_release(instance->common->input);
        if( instance->common->query != NULL )
            zend_object_release(instance->common->query);
        if( instance->common->cookie != NULL )
            zend_object_release(instance->common->cookie);
        // if( instance->common->attribute != NULL )
        //     zend_object_release(instance->common->attribute);
        // if( instance->common->header != NULL )
        //     zend_object_release(instance->common->header);
        if( instance->common->server != NULL )
            zend_object_release(instance->common->server);

        efree(instance->common);       
    }
}

zend_object* create_request_object( zend_class_entry* ce )
{
    request_object* object = 
        ecalloc(1, sizeof(request_object) + zend_object_properties_size(ce));

    zend_object_std_init(&object->std, ce);
    object_properties_init(&object->std, ce);

    memcpy(&request_object_handlers, zend_get_std_object_handlers(), sizeof(request_object_handlers));
    request_object_handlers.offset = XtOffsetOf(request_object, std);
    request_object_handlers.free_obj = free_request_object;
    object->std.handlers = &request_object_handlers;
    object->common = ecalloc(1, sizeof(request_common_t));
    object->common->input = NULL;
    object->common->query = NULL;
    object->common->cookie = NULL;
    object->common->attribute = NULL;
    object->common->header = NULL;
    object->common->server = NULL;

    return &object->std;
}

PHP_METHOD(Request, __construct)
{
    HashTable*       input;
    HashTable*       query;
    HashTable*       cookie;
    HashTable*       attribute;
    HashTable*       files;
    HashTable*       server;
    HashTable*       headers;

    ZEND_PARSE_PARAMETERS_START(6, 6)
        Z_PARAM_ARRAY_HT(input)
        Z_PARAM_ARRAY_HT(query)
        Z_PARAM_ARRAY_HT(cookie)
        Z_PARAM_ARRAY_HT(attribute)
        Z_PARAM_ARRAY_HT(files)
        Z_PARAM_ARRAY_HT(server)
    ZEND_PARSE_PARAMETERS_END();

    request_object* instance = 
        ZPHEUR_ZVAL_GET_OBJECT(request_object, getThis());

    /* {{{ REQUEST */
    zend_object* input_object = php_class_init(
        "Zpheur\\Schemes\\Http\\Foundation\\InputBag",
        sizeof("Zpheur\\Schemes\\Http\\Foundation\\InputBag") - 1
        // "Zpheur\\Schemes\\Http\\Foundation\\ParameterBag",
        // sizeof("Zpheur\\Schemes\\Http\\Foundation\\ParameterBag") - 1
    );
    do {
        zval* params___construct = safe_emalloc(1, sizeof(zval), 0);
        zval arr_input; ZVAL_ARR(&arr_input, input);
        params___construct[0] = arr_input;

        php_class_call_constructor(input_object, 1, params___construct);
        efree(params___construct);
    } while(0);

    instance->common->input = input_object;
    /* }}} */   

    /* {{{ QUERY */
    zend_object* query_object = php_class_init(
        "Zpheur\\Schemes\\Http\\Foundation\\InputBag",
        sizeof("Zpheur\\Schemes\\Http\\Foundation\\InputBag") - 1
        // "Zpheur\\Schemes\\Http\\Foundation\\ParameterBag",
        // sizeof("Zpheur\\Schemes\\Http\\Foundation\\ParameterBag") - 1
    );
    do {
        zval* params___construct = safe_emalloc(1, sizeof(zval), 0);
        zval arr_query; ZVAL_ARR(&arr_query, query);
        params___construct[0] = arr_query;

        php_class_call_constructor(query_object, 1, params___construct);
        efree(params___construct);
    } while(0);

    instance->common->query = query_object;
    /* }}} */

    // /* {{{ cookie */
    zend_object* cookie_object = php_class_init(
        "Zpheur\\Schemes\\Http\\Foundation\\InputBag",
        sizeof("Zpheur\\Schemes\\Http\\Foundation\\InputBag") - 1
        // "Zpheur\\Schemes\\Http\\Foundation\\ParameterBag",
        // sizeof("Zpheur\\Schemes\\Http\\Foundation\\ParameterBag") - 1
    );
    do {
        zval* params___construct = safe_emalloc(1, sizeof(zval), 0);
        zval arr_cookie; ZVAL_ARR(&arr_cookie, cookie);
        params___construct[0] = arr_cookie;

        php_class_call_constructor(cookie_object, 1, params___construct);
        efree(params___construct);
    } while(0);

    instance->common->cookie = cookie_object;
    // /* }}} */

    // /* {{{ attribute */
    // zend_object* attribute_object = php_class_init(
    //     "Zpheur\\Schemes\\Http\\Foundation\\ParameterBag",
    //     sizeof("Zpheur\\Schemes\\Http\\Foundation\\ParameterBag") - 1
    // );
    // {
    //     zval* params___construct = safe_emalloc(1, sizeof(zval), 0);
    //     // ZVAL_ZVAL(&params___construct[0], attribute, 1, 0);
    //     params___construct[0] = *attribute;

    //     php_class_call_constructor(attribute_object, 1, params___construct);
    //     efree(params___construct);
    // }

    // instance->common->attribute = attribute_object;
    // /* }}} */   

    // // zend_this_update_property("files", files);


    // /* {{{ HEADERS */
    // zend_object* header_object = php_class_init(
    //     "Zpheur\\Schemes\\Http\\Foundation\\HeaderBag",
    //     sizeof("Zpheur\\Schemes\\Http\\Foundation\\HeaderBag") - 1
    // );
    // {
    //     zval* params___construct = safe_emalloc(1, sizeof(zval), 0);
    //     // ZVAL_ZVAL(&params___construct[0], server, 1, 0);
    //     params___construct[0] = *server;

    //     php_class_call_constructor(header_object, 1, params___construct);
    //     efree(params___construct);
    // }

    // instance->common->header = header_object;
    // /* }}} */


    // /* {{{ SERVER */
    // Is actually use ServerBag
    zend_object* server_object = php_class_init(
        "Zpheur\\Schemes\\Http\\Foundation\\InputBag",
        sizeof("Zpheur\\Schemes\\Http\\Foundation\\InputBag") - 1
        // "Zpheur\\Schemes\\Http\\Foundation\\ParameterBag",
        // sizeof("Zpheur\\Schemes\\Http\\Foundation\\ParameterBag") - 1
    );
    do {
        zval* params___construct = safe_emalloc(1, sizeof(zval), 0);
        zval arr_server; ZVAL_ARR(&arr_server, server);
        params___construct[0] = arr_server;

        php_class_call_constructor(server_object, 1, params___construct);
        efree(params___construct);
    } while(0);

    instance->common->server = server_object;
    /* }}} */
}

PHP_METHOD(Request, __get)
{
    char* property_src = NULL;
    size_t property_len = 0;

    ZEND_PARSE_PARAMETERS_START(1, 1)
        Z_PARAM_STRING(property_src, property_len)
    ZEND_PARSE_PARAMETERS_END();

    request_object* instance = 
        ZPHEUR_ZVAL_GET_OBJECT(request_object, getThis());  

    if( strncmp("input", property_src, sizeof("input") - 1) == 0 )
    {
        // zval copy; ZVAL_OBJ(&copy, instance->common->input);
        instance->common->input->gc.refcount++; // Increase refcount for staying
        RETURN_OBJ(instance->common->input);
        // RETURN_ZVAL(&copy, 1, 0);
    }

    if( strncmp("query", property_src, sizeof("query") - 1) == 0 )
    {
        // zval copy; ZVAL_OBJ(&copy, instance->common->query);
        instance->common->query->gc.refcount++; // Increase refcount for staying
        RETURN_OBJ(instance->common->query);
        // RETURN_ZVAL(&copy, 1, 0);
    }

    if( strncmp("cookie", property_src, sizeof("cookie") - 1) == 0 )
    {
        // zval copy; ZVAL_OBJ(&copy, instance->common->cookie);
        instance->common->cookie->gc.refcount++; // Increase refcount for staying
        RETURN_OBJ(instance->common->cookie);
        // RETURN_ZVAL(&copy, 1, 0);
    }

    if( strncmp("server", property_src, sizeof("server") - 1) == 0 )
    {
        // zval copy; ZVAL_OBJ(&copy, instance->common->server);
        instance->common->server->gc.refcount++; // Increase refcount for staying
        RETURN_OBJ(instance->common->server);
        // RETURN_ZVAL(&copy, 1, 0);
    }

    RETURN_NULL();
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
    zval* global_server;

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
    zpheur_schemes_http_foundation_request_class_entry->create_object = create_request_object;

    return SUCCESS;
}
