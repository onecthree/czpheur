#include <php.h>
#include <ext/standard/info.h>
#include <php_zpheur.h>

#include <string.h>
#include <stdio.h>
#include <dirent.h>
#include <include/onecstr.h>
#include <zpheur.h>
#include <include/runtime.h>
#include <include/libstr.h>
#include <include/furouter.h>
#include "Route_arginfo.h"


#define LOCALVERB_GET       716984
#define LOCALVERB_POST      80798384
#define LOCALVERB_PUT       808584
#define LOCALVERB_PATCH     8065846772
#define LOCALVERB_DELETE    686976698469
#define LOCALVERB_OPTIONS   79808473797883

PHP_METHOD(Route, __construct)
{
    ZEND_PARSE_PARAMETERS_NONE();

    zval* routes = zend_this_read_property("routes");
    array_init(routes); 

    HashTable* get;
    ALLOC_HASHTABLE(get);
    zend_hash_init(get, 0, NULL, ZVAL_PTR_DTOR, 0);
    zval target_get;
    ZVAL_ARR(&target_get, get);
    zend_hash_str_update(Z_ARR_P(routes), "GET", sizeof("GET") - 1, &target_get);

    HashTable* post;
    ALLOC_HASHTABLE(post);
    zend_hash_init(post, 0, NULL, ZVAL_PTR_DTOR, 0);
    zval target_post;
    ZVAL_ARR(&target_post, post);
    zend_hash_str_update(Z_ARR_P(routes), "POST", sizeof("POST") - 1, &target_post);

    HashTable* put;
    ALLOC_HASHTABLE(put);
    zend_hash_init(put, 0, NULL, ZVAL_PTR_DTOR, 0);
    zval target_put;
    ZVAL_ARR(&target_put, put);
    zend_hash_str_update(Z_ARR_P(routes), "PUT", sizeof("PUT") - 1, &target_put);

    HashTable* patch;
    ALLOC_HASHTABLE(patch);
    zend_hash_init(patch, 0, NULL, ZVAL_PTR_DTOR, 0);
    zval target_patch;
    ZVAL_ARR(&target_patch, patch);
    zend_hash_str_update(Z_ARR_P(routes), "PATCH", sizeof("PATCH") - 1, &target_patch);

    HashTable* deletes;
    ALLOC_HASHTABLE(deletes);
    zend_hash_init(deletes, 0, NULL, ZVAL_PTR_DTOR, 0);
    zval target_deletes;
    ZVAL_ARR(&target_deletes, deletes);
    zend_hash_str_update(Z_ARR_P(routes), "DELETE", sizeof("DELETE") - 1, &target_deletes);

    HashTable* options;
    ALLOC_HASHTABLE(options);
    zend_hash_init(options, 0, NULL, ZVAL_PTR_DTOR, 0);
    zval target_options;
    ZVAL_ARR(&target_options, options);
    zend_hash_str_update(Z_ARR_P(routes), "OPTIONS", sizeof("OPTIONS") - 1, &target_options);

    HashTable* middlewares;
    ALLOC_HASHTABLE(middlewares);
    zend_hash_init(middlewares, 0, NULL, ZVAL_PTR_DTOR, 0);
    zval target_middlewares;
    ZVAL_ARR(&target_middlewares, middlewares);
    zend_this_update_property("middlewares", &target_middlewares);
}

PHP_METHOD(Route, setCacheFilepath)
{
    char*   route_src = NULL;
    size_t  route_len = 0;

    char*   middleware_src = NULL;
    size_t  middleware_len = 0;

    ZEND_PARSE_PARAMETERS_START(2, 2)
        Z_PARAM_STRING(route_src, route_len)
        Z_PARAM_STRING(middleware_src, middleware_len)
    ZEND_PARSE_PARAMETERS_END();

    zval route_filepath;
    ZVAL_STRINGL(&route_filepath, route_src, route_len);
    zend_this_update_property("route_filepath", &route_filepath);

    zval middleware_filepath;
    ZVAL_STRINGL(&middleware_filepath, middleware_src, middleware_len);
    zend_this_update_property("middleware_filepath", &middleware_filepath);

    RETURN_ZVAL(getThis(), 1, 0);
}   

PHP_METHOD(Route, middleware)
{
    zval*   middleware;

    ZEND_PARSE_PARAMETERS_START(1, 1);
        Z_PARAM_ZVAL(middleware)
    ZEND_PARSE_PARAMETERS_END();

    zval middlewares_wrapper;
    array_init(&middlewares_wrapper);

    if( Z_TYPE_P(middleware) != IS_ARRAY )
        php_error_docref(NULL, E_ERROR, "Argument 01 must be array");

    ZEND_HASH_FOREACH_VAL_IND(Z_ARR_P(middleware), zval* value)
    {
        zval middleware;
        array_init(&middleware);

        if( zend_hash_num_elements(Z_ARR_P(value)) != 2 )
            php_error_docref(NULL, E_ERROR, "Array of arg must be 2 argument");          

        if( Z_TYPE_P(value) != IS_ARRAY )
            php_error_docref(NULL, E_ERROR, "Middleware must be array"); 

        if(! zend_hash_str_exists(Z_ARR_P(value), "dest", sizeof("dest") - 1) )
            php_error_docref(NULL, E_ERROR, "Middleware destination do not exists");

        if(! zend_hash_str_exists(Z_ARR_P(value), "param", sizeof("param") - 1) )
            php_error_docref(NULL, E_ERROR, "Middleware param do not exists");      

        bool  target_valid = false;
        zval* middleware_classname = zend_hash_str_find(Z_ARR_P(value), "dest", sizeof("dest") - 1);

        if( !middleware_classname || Z_TYPE_P(middleware_classname) != IS_STRING )
            php_error_docref(NULL, E_ERROR, "Array of arg 01 must be string");            

        /* Check if method exists */

        zend_object* middleware_class_init = php_class_init(Z_STRVAL_P(middleware_classname));

        ZEND_HASH_FOREACH_PTR(&middleware_class_init->ce->function_table, zend_function* target_function) 
        {
            int is_public = (target_function->common.fn_flags & ZEND_ACC_PUBLIC);
            int name_equalied = zend_string_equals_cstr(target_function->common.function_name, "__invoke", sizeof("__invoke") - 1);

            if( is_public && name_equalied )
            {
                target_valid = true;
                break;
            }   
        } ZEND_HASH_FOREACH_END();        

        if(! target_valid )
            php_error_docref(NULL, E_ERROR, "Call to undefined method %s::%s()", Z_STRVAL_P(middleware_classname), "__invoke");

        /* -- Check if method exists -- */

        zval* parameters = zend_hash_str_find(Z_ARR_P(value), "param", sizeof("param") - 1);

        if( Z_TYPE_P(parameters) != IS_ARRAY )
            php_error_docref(NULL, E_ERROR, "Array of arg 2 must be array");  

        ZEND_HASH_FOREACH_KEY_VAL_IND(Z_ARR_P(parameters), zend_ulong d_index, zend_string* s_index, zval* arg_value)
        {
            zend_ulong arg_type = Z_TYPE_P(arg_value);

            if(! s_index )
                php_error_docref(NULL, E_ERROR, "Middleware argument must be indexed with string");  

            switch( arg_type )
            {
                // case IS_ARRAY:
                case IS_STRING:
                case IS_LONG:
                case IS_DOUBLE:
                case IS_TRUE:
                case IS_FALSE:
                break;
                default:
                    php_error_docref(NULL, E_ERROR,
                        "Middleware argument must be string, numeric, or boolean"
                        // "Middleware argument must be array, string, numeric, or boolean"
                    );  
                break;
            }
        }
        ZEND_HASH_FOREACH_END();

        zval target_middleware;
        array_init(&target_middleware);

        zval target_parameters;
        ZVAL_COPY(&target_parameters, parameters);

        add_next_index_zval(&target_middleware, middleware_classname);
        add_next_index_zval(&target_middleware, &target_parameters);

        add_next_index_zval(&middlewares_wrapper, &target_middleware);
    }
    ZEND_HASH_FOREACH_END();

    zval*  action_class = zend_this_read_property("action_class");
    zval*  action_method = zend_this_read_property("action_method");

    char*  action_class_src = Z_STRVAL_P(action_class);
    char*  action_method_src = Z_STRVAL_P(action_method);

    zval* middlewares = zend_this_read_property("middlewares");

    onec_string* middleware_target_name;
    onec_string_init(middleware_target_name);

    onec_string_append(middleware_target_name, 3, action_class_src, "::", action_method_src);

    zend_hash_str_update(Z_ARR_P(middlewares), middleware_target_name->val, middleware_target_name->len, &middlewares_wrapper);

    onec_string_release(middleware_target_name);

    RETURN_ZVAL(getThis(), 1, 0);
}

PHP_METHOD(Route, add)
{
    zend_ulong method = 0;

    char*   uri_src = NULL;
    size_t  uri_len = 0;

    zval*   action;

    ZEND_PARSE_PARAMETERS_START(3, 3)
        Z_PARAM_LONG(method)
        Z_PARAM_STRING(uri_src, uri_len)
        Z_PARAM_ZVAL(action)
    ZEND_PARSE_PARAMETERS_END();

    char*    http_method_src = NULL;
    size_t   http_method_len = 0;

    char*    target_class_src = NULL;
    size_t   target_class_len = 0;

    char*    target_method_src = NULL;
    size_t   target_method_len = 0;

    zval*           target_class;
    zval*           target_method;
    zend_object*    target_class_init;
    bool            target_valid = false;

    switch( method )
    {
        // case 7874:
        case LOCALVERB_GET: 
            http_method_src = "GET";
            http_method_len = sizeof("GET") - 1;
        break;
        // case 88814:
        case LOCALVERB_POST:
            http_method_src = "POST";
            http_method_len = sizeof("POST") - 1;
        break;
        // case 8934:
        case LOCALVERB_PUT:
            http_method_src = "PUT";
            http_method_len = sizeof("PUT") - 1;
        break;
        // case 874142:
        case LOCALVERB_PATCH:
            http_method_src = "PATCH";
            http_method_len = sizeof("PATCH") - 1;
        break;
        // case 7573809:
        case LOCALVERB_DELETE:
            http_method_src = "DELETE";
            http_method_len = sizeof("DELETE") - 1;
        break;
        // case 87921763:
        case LOCALVERB_OPTIONS:
            http_method_src = "OPTIONS";
            http_method_len = sizeof("OPTIONS") - 1;
        break;
        default:
            php_error_docref(NULL, E_ERROR, "Method are undefined");
        break;
    }

    if( action && Z_TYPE_P(action) == IS_ARRAY )
    {   
        if(! (target_class = zend_hash_index_find(Z_ARR_P(action), 0)) )
            php_error_docref(NULL, E_ERROR, "01 argument are undefined");

        if( Z_TYPE_P(target_class) != IS_STRING )
            php_error_docref(NULL, E_ERROR, "01 argument must be string");            

        zend_string* target_class_zs = zval_get_string(target_class);
        target_class_src = target_class_zs->val;
        target_class_len = target_class_zs->len;

        target_class_init = php_class_init(target_class_src);

        if(! (target_method = zend_hash_index_find(Z_ARR_P(action), 1)) )
            php_error_docref(NULL, E_ERROR, "02 argument are undefined");

        if( Z_TYPE_P(target_method) != IS_STRING )
            php_error_docref(NULL, E_ERROR, "01 argument must be string");          


        zend_string* target_method_zs = zval_get_string(target_method);
        target_method_src = target_method_zs->val;
        target_method_len = target_method_zs->len;
    }
    else
    {
        if( Z_TYPE_P(action) != IS_STRING )
            php_error_docref(NULL, E_ERROR, "01 argument must be string");    

        target_class_init = php_class_init(target_class_src);

        zend_string* target_class_zs = zval_get_string(target_class);
        target_class_src = target_class_zs->val;
        target_class_len = target_class_zs->len;

        target_method_src = "__invoke";
        target_method_len = sizeof("__invoke") - 1;
    }

    ZEND_HASH_FOREACH_PTR(&target_class_init->ce->function_table, zend_function* value) 
    {
        int is_public = (value->common.fn_flags & ZEND_ACC_PUBLIC);
        int name_equalied = zend_string_equals_cstr(value->common.function_name, target_method_src, target_method_len);

        if( is_public && name_equalied )
        {
            target_valid = true;
            break;
        }   
    } ZEND_HASH_FOREACH_END();

    if(! target_valid )
        php_error_docref(NULL, E_ERROR, "Call to undefined method %s::%s()", target_class_src, target_method_src);

    onec_string* compiled_route = 
        static_furouter_route_uri_parse(uri_src, target_class_src, target_method_src);

    zend_this_update_property("action_class", target_class);

    zend_this_update_property("action_method", target_method);    

    zval target_action;
    array_init(&target_action);

    add_next_index_zval(&target_action, target_class);
    add_next_index_zval(&target_action, target_method);

    zval* routes = zend_this_read_property("routes");
    zval* routes_per_method = zend_hash_str_find(Z_ARR_P(routes), http_method_src, http_method_len);

    zend_hash_str_update(Z_ARR_P(routes_per_method), compiled_route->val, compiled_route->len - 1, &target_action);

    RETURN_ZVAL(getThis(), 1, 0);
}

PHP_METHOD(Route, save)
{
    ZEND_PARSE_PARAMETERS_NONE();

    zval* route_filepath        = zend_this_read_property("route_filepath"); 
    zval* middleware_filepath   = zend_this_read_property("middleware_filepath");
    zval* routes                = zend_this_read_property("routes");
    zval* middlewares           = zend_this_read_property("middlewares");

    onec_string* route_serialized;
    onec_string_init(route_serialized);

    onec_string* middleware_serialized;
    onec_string_init(middleware_serialized);

    onec_string_append(route_serialized, 1, "<?php\n\nreturn\n[\n");
    ZEND_HASH_FOREACH_STR_KEY_VAL(Z_ARR_P(routes), zend_string* method, zval* route)
    {
        onec_string_append(route_serialized, 3, "    '", method->val, "' =>\n    [\n");
        ZEND_HASH_FOREACH_STR_KEY_VAL(Z_ARR_P(route), zend_string* uri, zval* action)
        {
            zval* action_class = zend_hash_index_find(Z_ARR_P(action), 0);
            zval* action_method = zend_hash_index_find(Z_ARR_P(action), 1);
            onec_string_append(route_serialized, 9, "        '", uri->val, "'", " => ", "['", Z_STRVAL_P(action_class), "', '", Z_STRVAL_P(action_method), "'],\n");
        }
        ZEND_HASH_FOREACH_END();
        onec_string_append(route_serialized, 1, "    ],\n");
    }
    ZEND_HASH_FOREACH_END();
    onec_string_append(route_serialized, 1, "];");

    onec_string_append(middleware_serialized, 1, "<?php\n\nreturn\n[\n");
    ZEND_HASH_FOREACH_STR_KEY_VAL(Z_ARR_P(middlewares), zend_string* middleware_action, zval* middleware_etc)
    {
        onec_string_append(middleware_serialized, 3, "    '", middleware_action->val, "' =>\n    [\n");
        ZEND_HASH_FOREACH_NUM_KEY_VAL(Z_ARR_P(middleware_etc), zend_ulong etc_index, zval* middleware_target)
        {
            zval* action_class = zend_hash_index_find(Z_ARR_P(middleware_target), 0);
            zval* action_param = zend_hash_index_find(Z_ARR_P(middleware_target), 1);

            onec_string_append(middleware_serialized, 3, "        [ '", Z_STRVAL_P(action_class), "', [\n");
            ZEND_HASH_FOREACH_STR_KEY_VAL(Z_ARR_P(action_param), zend_string* param, zval* value)
            {
                switch( Z_TYPE_P(value) )
                {
                    case IS_STRING:
                        // php_var_dump(value, 0);
                        onec_string_append(middleware_serialized, 5, "                '", param->val, "' => '", Z_STRVAL_P(value), "',\n");
                    break;
                    case IS_LONG:
                        onec_string_append(middleware_serialized, 5, "                '", param->val, "' => ", zend_long_to_str(zval_get_long(value))->val, ",\n");
                    break;
                    case IS_TRUE:
                        onec_string_append(middleware_serialized, 3, "                '", param->val, "' => true,\n");
                    break;
                    case IS_FALSE:
                        onec_string_append(middleware_serialized, 3, "                '", param->val, "' => false,\n");
                    break;
                }
            }
            ZEND_HASH_FOREACH_END();
            onec_string_append(middleware_serialized, 1, "            ]\n        ],\n");
        }
        ZEND_HASH_FOREACH_END();
        onec_string_append(middleware_serialized, 1, "    ],\n");
    }
    ZEND_HASH_FOREACH_END();
    onec_string_append(middleware_serialized, 1, "];");

    php_stream* route_file = php_stream_open_wrapper_ex(Z_STRVAL_P(route_filepath), "w", 0 | REPORT_ERRORS, NULL, NULL);
    php_stream_write(route_file, route_serialized->val, route_serialized->len);

    php_stream* middleware_file = php_stream_open_wrapper_ex(Z_STRVAL_P(middleware_filepath), "w", 0 | REPORT_ERRORS, NULL, NULL);
    php_stream_write(middleware_file, middleware_serialized->val, middleware_serialized->len);

    onec_string_release(route_serialized);
    onec_string_release(middleware_serialized);
}

PHP_METHOD(Route, tryVerbFromString)
{
    char*   http_verb_src = NULL;
    size_t  http_verb_len = 0;

    ZEND_PARSE_PARAMETERS_START(1, 1)
        Z_PARAM_STRING(http_verb_src, http_verb_len)
    ZEND_PARSE_PARAMETERS_END();

    unsigned long http_verb_repnum =
        str_to_repnum(http_verb_src, http_verb_len, LOCALVERB_OPTIONS);

    switch( http_verb_repnum )
    {
        case LOCALVERB_GET:
        case LOCALVERB_POST:
        case LOCALVERB_PUT:
        case LOCALVERB_PATCH:
        case LOCALVERB_DELETE:
        case LOCALVERB_OPTIONS:
            RETURN_LONG(http_verb_repnum); 
        break;
    }

    RETURN_NULL();
}

ZEND_MINIT_FUNCTION(Zpheur_Schemes_Http_Routing_Route)
{
    zend_class_entry ce;

    INIT_NS_CLASS_ENTRY(ce, "Zpheur\\Schemes\\Http\\Routing", "Route", zpheur_schemes_http_routing_route_class_method);
    zpheur_schemes_http_routing_route_class_entry = zend_register_internal_class(&ce);
    zpheur_schemes_http_routing_route_class_entry->ce_flags |= ZEND_ACC_NO_DYNAMIC_PROPERTIES;

    zend_declare_property_null(zpheur_schemes_http_routing_route_class_entry, "route_filepath", sizeof("route_filepath") - 1, ZEND_ACC_PUBLIC);
    zend_declare_property_null(zpheur_schemes_http_routing_route_class_entry, "middleware_filepath", sizeof("middleware_filepath") - 1, ZEND_ACC_PUBLIC);

    zend_declare_property_null(zpheur_schemes_http_routing_route_class_entry, "routes", sizeof("routes") - 1, ZEND_ACC_PROTECTED);
    zend_declare_property_null(zpheur_schemes_http_routing_route_class_entry, "middlewares", sizeof("middlewares") - 1, ZEND_ACC_PUBLIC);

    zend_declare_property_null(zpheur_schemes_http_routing_route_class_entry, "target_method", sizeof("target_method") - 1, ZEND_ACC_PUBLIC);
    zend_declare_property_null(zpheur_schemes_http_routing_route_class_entry, "target_uri", sizeof("target_uri") - 1, ZEND_ACC_PUBLIC);
    zend_declare_property_null(zpheur_schemes_http_routing_route_class_entry, "action_class", sizeof("action_class") - 1, ZEND_ACC_PUBLIC);
    zend_declare_property_null(zpheur_schemes_http_routing_route_class_entry, "action_method", sizeof("action_method") - 1, ZEND_ACC_PUBLIC);

    // zend_declare_class_constant_long(zpheur_schemes_http_routing_route_class_entry, "GET", sizeof("GET") - 1, 7874);
    // zend_declare_class_constant_long(zpheur_schemes_http_routing_route_class_entry, "POST", sizeof("POST") - 1, 88814);
    // zend_declare_class_constant_long(zpheur_schemes_http_routing_route_class_entry, "PUT", sizeof("PUT") - 1, 8934);
    // zend_declare_class_constant_long(zpheur_schemes_http_routing_route_class_entry, "PATCH", sizeof("PATCH") - 1, 874142);
    // zend_declare_class_constant_long(zpheur_schemes_http_routing_route_class_entry, "DELETE", sizeof("DELETE") - 1, 7573809);
    // zend_declare_class_constant_long(zpheur_schemes_http_routing_route_class_entry, "OPTIONS", sizeof("OPTIONS") - 1, 87921763);

    zend_declare_class_constant_long(zpheur_schemes_http_routing_route_class_entry, "GET", sizeof("GET") - 1, LOCALVERB_GET);
    zend_declare_class_constant_long(zpheur_schemes_http_routing_route_class_entry, "POST", sizeof("POST") - 1, LOCALVERB_POST);
    zend_declare_class_constant_long(zpheur_schemes_http_routing_route_class_entry, "PUT", sizeof("PUT") - 1, LOCALVERB_PUT);
    zend_declare_class_constant_long(zpheur_schemes_http_routing_route_class_entry, "PATCH", sizeof("PATCH") - 1, LOCALVERB_PATCH);
    zend_declare_class_constant_long(zpheur_schemes_http_routing_route_class_entry, "DELETE", sizeof("DELETE") - 1, LOCALVERB_DELETE);
    zend_declare_class_constant_long(zpheur_schemes_http_routing_route_class_entry, "OPTIONS", sizeof("OPTIONS") - 1, LOCALVERB_OPTIONS);

    return SUCCESS;
}