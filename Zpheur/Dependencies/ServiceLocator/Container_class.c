#include <php.h>
#include <ext/standard/info.h>
#include <php_zpheur.h>

#include <string.h>
#include <zpheur.h>
#include <include/runtime.h>
#include "Container_arginfo.h"


void placeholder( char const *src, HashTable *ht_tmp )
{
    int src_len = 0;
    char c;

    bool CHAR_SKIP = false;

    int buffer_size = 2;
    int buffer_len = 0;
    char *buffer = malloc(buffer_size);

    char left[1024];
    char right[1024];

    int SIDE_MODE = LEFT;

    while( true )
    {
        c = src[src_len];

        if( c == '\\' )
        {
            ++src_len;
            CHAR_SKIP = true;
            continue;
        }

        if( CHAR_SKIP == false && SIDE_MODE == LEFT && c == '=' )
        {
            buffer[buffer_len] = '\0';
            strncpy(left, buffer, sizeof left);
            memset(buffer, 0, sizeof(buffer));
            buffer_size = 2;
            buffer_len = 0;
            SIDE_MODE = RIGHT;
            ++src_len;
            continue;
        }

        if( CHAR_SKIP == false && SIDE_MODE == RIGHT && (c == ' ' || c == '\0') )
        {
            buffer[buffer_len] = '\0';
            strncpy(right, buffer, sizeof right);

            zval z_right;
            ZVAL_STRING(&z_right, right);
            zend_hash_str_update(ht_tmp, left, strlen(left), &z_right);
            
            memset(buffer, 0, sizeof(buffer));
            buffer_size = 2;
            buffer_len = 0;
            SIDE_MODE = LEFT;
            ++src_len;
            continue;
        }

        if( buffer_len >= buffer_size )
        {
            buffer_size = buffer_size * 2;
            buffer = realloc(buffer, buffer_size);
            
        }

        buffer[buffer_len] = c;

        ++buffer_len;
        ++src_len;
        CHAR_SKIP = false;

        if( c == '\0' )
        {
            break;
        }
    }
}

PHP_METHOD(Container, __construct)
{
    ZEND_PARSE_PARAMETERS_NONE();

    // zpheur_globals.CONTAINER = (*getThis());


    zval classes_container;
    zval scalars_container;

    array_init(&classes_container);
    array_init(&scalars_container);

    zend_this_update_property("classes", &classes_container);
    zend_this_update_property("scalars", &scalars_container);
}

PHP_METHOD(Container, get)
{
    char*   name_src = NULL;
    size_t  name_len = 0;

    ZEND_PARSE_PARAMETERS_START(1, 1)
        Z_PARAM_STRING(name_src, name_len)
    ZEND_PARSE_PARAMETERS_END();

    zval* value;

    zval* class_container = zend_this_read_property("classes");
    if(! (value = zend_hash_str_find(Z_ARR_P(class_container), name_src, name_len)) )
    {
        zval* scalar_container = zend_this_read_property("scalars");        
        if(! (value = zend_hash_str_find(Z_ARR_P(scalar_container), name_src, name_len)) )
        {

            zend_object* target_class = php_class_init(name_src);

            php_class_call_method(target_class, "__construct", sizeof("__construct") - 1, 0, NULL, 1);

            value = php_class_call_method(target_class, "servicePin", sizeof("servicePin") - 1, 0, NULL, 0);

            zend_hash_str_update(Z_ARR_P(class_container), name_src, name_len, value);
        }
    }

    ZVAL_MAKE_REF(value);
    
    RETURN_ZVAL(value, 0, 0);
}

PHP_METHOD(Container, getOf)
{
    zval*   names;

    ZEND_PARSE_PARAMETERS_START(1, 1)
        Z_PARAM_ZVAL(names)
    ZEND_PARSE_PARAMETERS_END();

    zval arguments;
    array_init(&arguments);

    zval* class_container = zend_this_read_property("classes");
    zval* scalar_container = zend_this_read_property("scalars");  
        
    ZEND_HASH_FOREACH_STR_KEY_VAL(Z_ARR_P(names), zend_string* _name, zval* _val)
    {
        zend_string* name_zs = zval_get_string(_val);

        char*   name_src = name_zs->val;
        size_t  name_len = name_zs->len;
        zval*   argument_value;
        zval    arguments_ret;


        if( zend_string_equals_cstr(name_zs, CONTAINER_CLASS_NAME_SRC, CONTAINER_CLASS_NAME_LEN) )
        {   
            ZVAL_ZVAL(&arguments_ret, getThis(), 0, 0);
            ZVAL_MAKE_REF(&arguments_ret);
        }
        else
        {
            // Is not - classes target
            if(! (argument_value = zend_hash_str_find(Z_ARR_P(class_container), name_src, name_len)) )
            {      
                // Is not - scalars target
                if(! (argument_value = zend_hash_str_find(Z_ARR_P(scalar_container), name_src, name_len)) )
                {
                    zend_object* target_class = php_class_init_ex(name_src);

                    if( target_class )
                    {
                        php_class_call_method(target_class, "__construct", sizeof("__construct") - 1, 0, NULL, 1);

                        argument_value = php_class_call_method(target_class, "servicePin", sizeof("servicePin") - 1, 0, NULL, 0);

                        zend_hash_str_update(Z_ARR_P(class_container), name_src, name_len, argument_value);   

                        // ZVAL_ZVAL(&arguments_ret, argument_value, 1, 0);
                        // ZVAL_ZVAL(&arguments_ret, argument_value, 0, 0);
                        ZVAL_MAKE_REF(argument_value);
                        ZVAL_ZVAL(&arguments_ret, argument_value, 0, 0);
                        ZVAL_MAKE_REF(&arguments_ret);
                    }
                    else
                    {
                        ZVAL_NULL(&arguments_ret);   
                    }
                }
                else // scalars target
                {
                    ZVAL_MAKE_REF(argument_value);
                    ZVAL_ZVAL(&arguments_ret, argument_value, 0, 0);
                    ZVAL_MAKE_REF(&arguments_ret);
                }
            }
            else // classes target
            {
                ZVAL_MAKE_REF(argument_value);
                ZVAL_ZVAL(&arguments_ret, argument_value, 0, 0);
                ZVAL_MAKE_REF(&arguments_ret);
            }
        }

        add_next_index_zval(&arguments, &arguments_ret);
    }
    ZEND_HASH_FOREACH_END();

    RETURN_ZVAL(&arguments, 0, 0);
}

PHP_METHOD(Container, set)
{
    char*       name_src;
    size_t      name_len;
    zval*       value;
    bool        is_scalars = false;

    ZEND_PARSE_PARAMETERS_START(2, 3)
        Z_PARAM_STRING(name_src, name_len)
        Z_PARAM_ZVAL(value)
        Z_PARAM_OPTIONAL
        Z_PARAM_BOOL(is_scalars)
    ZEND_PARSE_PARAMETERS_END();

    zval* container = zend_this_read_property(is_scalars ? "scalars" : "classes");

    zval _value;
    ZVAL_COPY(&_value, value);
    zend_hash_str_update(Z_ARR_P(container), name_src, name_len, &_value);   

    RETURN_TRUE;
}

PHP_METHOD(Container, has)
{
    char*   name_src = NULL;
    size_t  name_len = 0;

    ZEND_PARSE_PARAMETERS_START(1, 1)
        Z_PARAM_STRING(name_src, name_len)
    ZEND_PARSE_PARAMETERS_END();

    zval* value;

    zval* class_container = zend_this_read_property("classes");
    if( (value = zend_hash_str_find(Z_ARR_P(class_container), name_src, name_len)) )
        RETURN_TRUE;

    zval* scalar_container = zend_this_read_property("scalars");        
    if( (value = zend_hash_str_find(Z_ARR_P(scalar_container), name_src, name_len)) )
        RETURN_TRUE;


    RETURN_FALSE;
}

ZEND_MINIT_FUNCTION(Zpheur_Dependencies_ServiceLocator_Container)
{
    zend_class_entry ce;

    INIT_NS_CLASS_ENTRY(ce, "Zpheur\\Dependencies\\ServiceLocator", "Container", zpheur_dependencies_servicelocator_container_class_method);
    zpheur_dependencies_servicelocator_container_class_entry = zend_register_internal_class(&ce);
    zpheur_dependencies_servicelocator_container_class_entry->ce_flags |= ZEND_ACC_NO_DYNAMIC_PROPERTIES;

    zend_declare_property_null(zpheur_dependencies_servicelocator_container_class_entry, "classes", sizeof("classes") - 1, ZEND_ACC_PUBLIC);
    zend_declare_property_null(zpheur_dependencies_servicelocator_container_class_entry, "scalars", sizeof("scalars") - 1, ZEND_ACC_PUBLIC);

    return SUCCESS;
}