#include <php.h>
#include <ext/standard/info.h>
#include <php_zpheur.h>

#include <Zend/zend_attributes.h>
#include <ext/spl/spl_iterators.h>
#include <Zend/zend_interfaces.h>

#include <string.h>
#include <stdlib.h>
#include <include/onecstr.h>
#include <zpheur.h>
#include <include/runtime.h>
#include <zpheur.h>
#include "Middleware_arginfo.h"


PHP_METHOD(Middleware, __construct)
{
    zval* middlewares;

    ZEND_PARSE_PARAMETERS_START(1, 1)
        Z_PARAM_ZVAL(middlewares);
    ZEND_PARSE_PARAMETERS_END();

    if( middlewares == NULL || Z_TYPE_P(middlewares) != IS_ARRAY )
        array_init(middlewares);

    zend_this_update_property("middlewares", middlewares);


    zval globals_empty;
    array_init(&globals_empty);
    zend_this_update_property("globals", &globals_empty);
}

PHP_METHOD(Middleware, setGlobalLists)
{
    zval*   globals_list;

    ZEND_PARSE_PARAMETERS_START(1, 1)
        Z_PARAM_ARRAY(globals_list)
    ZEND_PARSE_PARAMETERS_END();

    if( Z_TYPE_P(globals_list) != IS_ARRAY )
        php_error_docref(NULL, E_ERROR, "Argument 01 must be array");

    ZEND_HASH_FOREACH_VAL_IND(Z_ARR_P(globals_list), zval* value)
    {
        if( Z_TYPE_P(value) != IS_ARRAY )
            php_error_docref(NULL, E_ERROR, "Middleware must be array"); 

        if( zend_hash_num_elements(Z_ARR_P(value)) != 2 )
            php_error_docref(NULL, E_ERROR, "Array of arg must be 2 argument");                

        bool  target_valid = false;
        zval* middleware_class = zend_hash_index_find(Z_ARR_P(value), 0);

        if( !middleware_class || Z_TYPE_P(middleware_class) != IS_STRING )
            php_error_docref(NULL, E_ERROR, "Array of arg 01 must be string");            

        zend_string* _middleware_class = zval_get_string(middleware_class);
        zend_object* middleware_class_init = php_class_init(_middleware_class->val, _middleware_class->len);

        ZEND_HASH_FOREACH_PTR(&middleware_class_init->ce->function_table, zend_function* value) 
        {
            int is_public = (value->common.fn_flags & ZEND_ACC_PUBLIC);
            int name_equalied = zend_string_equals_cstr(value->common.function_name, "__invoke", sizeof("__invoke") - 1);

            if( is_public && name_equalied )
            {
                target_valid = true;
                break;
            }   
        } ZEND_HASH_FOREACH_END();        

        if(! target_valid )
            php_error_docref(NULL, E_ERROR, "Call to undefined method %s::%s()", Z_STRVAL_P(middleware_class), "__invoke");


        zval* argument = zend_hash_index_find(Z_ARR_P(value), 1);

        if( Z_TYPE_P(argument) != IS_ARRAY )
            php_error_docref(NULL, E_ERROR, "Array of arg 2 must be array");  

        ZEND_HASH_FOREACH_KEY_VAL_IND(Z_ARR_P(argument), zend_ulong d_index, zend_string* s_index, zval* arg_value)
        {
            zend_ulong arg_type = Z_TYPE_P(arg_value);

            if(! s_index )
                php_error_docref(NULL, E_ERROR, "Middleware argument must be indexed with string");  

            switch( arg_type )
            {
                case IS_STRING: case IS_LONG: case IS_DOUBLE: case IS_TRUE: case IS_FALSE: break;
                default:
                    php_error_docref(NULL, E_ERROR, "Middleware argument must be string, numeric, or boolean");  
                break;
            }
        } ZEND_HASH_FOREACH_END();
    } ZEND_HASH_FOREACH_END();

    zend_this_update_property("globals", globals_list);

    RETURN_ZVAL(getThis(), 1, 0);
}

PHP_METHOD(Middleware, withActionCall)
{
    char*   class_src = NULL;
    size_t  class_len = 0;

    char*   method_src = NULL;
    size_t  method_len = 0;

    bool    static_call = false;

    ZEND_PARSE_PARAMETERS_START(2, 3)
        Z_PARAM_STRING(class_src, class_len)
        Z_PARAM_STRING(method_src, method_len)
        Z_PARAM_OPTIONAL
        Z_PARAM_BOOL(static_call)
    ZEND_PARSE_PARAMETERS_END();

    zval* middleware_globals    = zend_this_read_property("globals");
    zval* middlewares           = zend_this_read_property("middlewares");

    if( static_call )
    {
        zend_object*    target_class = php_class_init(class_src, class_len);
        zend_function*  target_method;
        bool            target_method_found = false;

        ZEND_HASH_FOREACH_PTR(&target_class->ce->function_table, zend_function* value) 
        {
            int is_public = (value->common.fn_flags & ZEND_ACC_PUBLIC);
            int name_equalied = zend_string_equals_cstr(value->common.function_name, method_src, method_len);

            if( is_public && name_equalied )
            {
                target_method = value;
                target_method_found = true;
                break;
            }   
        } ZEND_HASH_FOREACH_END();


        if(! target_method_found )
            php_error_docref(NULL, E_ERROR, "Method %s::%s not found", class_src, method_src);

        /* Class attributes */
        if( target_class->ce->attributes )
        {
            ZEND_HASH_FOREACH_PTR(target_class->ce->attributes, zend_attribute* class_attributes_value)
            {
                zval middleware;
                array_init(&middleware);

                zval dest;
                HashTable *argument;
                ALLOC_HASHTABLE(argument);
                zend_hash_init(argument, 0, NULL, ZVAL_PTR_DTOR, 0);

                if( zend_string_equals_cstr(class_attributes_value->name, "Middleware", sizeof("Middleware") - 1) )
                {
                    zval middleware;
                    array_init(&middleware);
                    zval arguments;
                    array_init(&arguments);


                    if(! class_attributes_value->argc )
                        php_error_docref(NULL, E_ERROR, "Middleware attribute must have atleast 1 argument");

                    if(! zend_string_equals_cstr(class_attributes_value->args[0].name, "dest", sizeof("dest") - 1) )
                        php_error_docref(NULL, E_ERROR, "Middleware attribute first argument must be class string");

                    if( Z_TYPE(class_attributes_value->args[0].value) != IS_STRING )
                        php_error_docref(NULL, E_ERROR, "Middleware attribute first argument must be class string");
     
                    dest = class_attributes_value->args[0].value;

                    for( int i = 1; i < class_attributes_value->argc; ++i )
                    {
                        zend_hash_update_ind(argument,class_attributes_value->args[i].name, &class_attributes_value->args[i].value);
                    }
                }

                if( Z_TYPE(dest) == IS_STRING )
                {
                    add_next_index_zval(&middleware, &dest);

                    zval _argument;
                    ZVAL_ARR(&_argument, argument);
                    add_next_index_zval(&middleware, &_argument);

                    add_next_index_zval(middleware_globals, &middleware);
                } // TODO: [[else]] throw a error/exception

            }
            ZEND_HASH_FOREACH_END();
        }

        /* Method attributes */
        if( target_method->common.attributes )
        {
            ZEND_HASH_FOREACH_PTR(target_method->common.attributes, zend_attribute *attribute)
            {
                zval middleware;
                array_init(&middleware);

                zval dest;
                HashTable *argument;
                ALLOC_HASHTABLE(argument);
                zend_hash_init(argument, 0, NULL, ZVAL_PTR_DTOR, 0);


                if( zend_string_equals_cstr(attribute->name, "Middleware", sizeof("Middleware") - 1) )
                {
                    if(! attribute->argc )
                        php_error_docref(NULL, E_ERROR, "Middleware attribute must have atleast 1 argument");

                    if(! zend_string_equals_cstr(attribute->args[0].name, "dest", sizeof("dest") - 1) )
                        php_error_docref(NULL, E_ERROR, "Middleware attribute first argument must be class string");

                    if( Z_TYPE(attribute->args[0].value) != IS_STRING )
                        php_error_docref(NULL, E_ERROR, "Middleware attribute first argument must be class string");
     
                    dest = attribute->args[0].value;

                    for( int i = 1; i < attribute->argc; ++i )
                    {
                        zend_hash_update_ind(argument, attribute->args[i].name, &attribute->args[i].value);
                    }
                }

                if( Z_TYPE(dest) == IS_STRING )
                {
                    add_next_index_zval(&middleware, &dest);

                    zval _argument;
                    ZVAL_ARR(&_argument, argument);
                    add_next_index_zval(&middleware, &_argument);

                    add_next_index_zval(middleware_globals, &middleware);
                } // TODO: [[else]] throw a error/exception

            } ZEND_HASH_FOREACH_END();
        }

        RETURN_ZVAL(middleware_globals, 1, 0);
    }   

    onec_string* action;
    onec_string_init(action);

    onec_string_append(action, 3, class_src, "::", method_src);

    zval* middleware_target;

    if( (middleware_target = zend_hash_str_find(Z_ARR_P(middlewares), action->val, action->len)) )
    {
        ZEND_HASH_FOREACH_VAL_IND(Z_ARR_P(middleware_target), zval* value)
        {
            add_next_index_zval(middleware_globals, value);
        }
        ZEND_HASH_FOREACH_END();
    }

    RETURN_ZVAL(middleware_globals, 1, 0);
}

ZEND_MINIT_FUNCTION(Zpheur_Schemes_Http_Middleware)
{
    zend_class_entry ce;

    INIT_NS_CLASS_ENTRY(ce, "Zpheur\\Schemes\\Http", "Middleware", zpheur_schemes_http_middleware_class_method);
    zpheur_schemes_http_middleware_class_entry = zend_register_internal_class(&ce);
    zpheur_schemes_http_middleware_class_entry->ce_flags |= ZEND_ACC_NO_DYNAMIC_PROPERTIES;
    // zpheur_schemes_http_middleware_class_entry->ce_flags |= ZEND_ACC_ALLOW_DYNAMIC_PROPERTIES;
    // zpheur_schemes_http_middleware_class_entry->parent = zpheur_actions_http_defaultaction_class_entry;

    zend_declare_property_null(zpheur_schemes_http_middleware_class_entry, "globals", sizeof("globals") - 1, ZEND_ACC_PUBLIC);
    zend_declare_property_null(zpheur_schemes_http_middleware_class_entry, "middlewares", sizeof("middlewares") - 1, ZEND_ACC_PUBLIC);
    zend_declare_property_null(zpheur_schemes_http_middleware_class_entry, "middleware_filepath", sizeof("middleware_filepath") - 1, ZEND_ACC_PUBLIC);

    return SUCCESS;
}