#include "php.h"
#include "ext/standard/info.h"
#include "php_zpheur.h"

#include <string.h>
#include <include/runtime.h>
#include <zpheur.h>
#include "ArgumentResolver_arginfo.h"
#include <Zpheur/Dependencies/ServiceLocator/Container_arginfo.h>

zend_object_handlers argument_resolver_object_handlers;

typedef struct _argument_resolver_common_t
{
    zend_object* container; // don't need release
} argument_resolver_common_t;

typedef struct _argument_resolver_object
{
    argument_resolver_common_t* common;
    zend_object std;
} argument_resolver_object;

void free_argument_resolver_object(zend_object *object)
{
    argument_resolver_object* instance = ZPHEUR_GET_OBJECT(argument_resolver_object, object);

    zend_object_std_dtor(&instance->std);
    if( instance->common )
    {
        efree(instance->common);       
    }
}

zend_object* create_argument_resolver_object( zend_class_entry* ce )
{
    argument_resolver_object* object = 
        ecalloc(1, sizeof(argument_resolver_object) + zend_object_properties_size(ce));

    zend_object_std_init(&object->std, ce);
    object_properties_init(&object->std, ce);

    memcpy(&argument_resolver_object_handlers, zend_get_std_object_handlers(), sizeof(argument_resolver_object_handlers));
    argument_resolver_object_handlers.offset = XtOffsetOf(argument_resolver_object, std);
    argument_resolver_object_handlers.free_obj = free_argument_resolver_object;
    object->std.handlers = &argument_resolver_object_handlers;

    object->common = emalloc(sizeof(argument_resolver_common_t));
    object->common->container = NULL;

    return &object->std;
}

PHP_METHOD(ArgumentResolver, __construct)
{
    zval* container = NULL;
    zend_uchar type;
    zend_string* class_name;

    ZEND_PARSE_PARAMETERS_START(1, 1)
        Z_PARAM_OBJECT_OF_CLASS(container, zpheur_dependencies_servicelocator_container_class_entry)
    ZEND_PARSE_PARAMETERS_END();

    argument_resolver_object* instance = 
        ZPHEUR_ZVAL_GET_OBJECT(argument_resolver_object, getThis());
    instance->common->container = Z_OBJ_P(container);
}

PHP_METHOD(ArgumentResolver, resolve)
{
	char*  	class_src = NULL;
	size_t 	class_len = 0;

	char*  	method_src = NULL;
	size_t 	method_len = 0;

	ZEND_PARSE_PARAMETERS_START(2, 2)
		Z_PARAM_STRING(class_src, class_len)
		Z_PARAM_STRING(method_src, method_len)
	ZEND_PARSE_PARAMETERS_END();

    argument_resolver_object* instance = 
        ZPHEUR_ZVAL_GET_OBJECT(argument_resolver_object, getThis());

	zend_object* target_class =
        php_class_init_silent(class_src, class_len);
	zend_function* target_method = NULL;

    HashTable* class_argument;
    ALLOC_HASHTABLE(class_argument);
    zend_hash_init(class_argument, 0, NULL, ZVAL_PTR_DTOR, 0);

	ZEND_HASH_FOREACH_PTR(&target_class->ce->function_table, zend_function* value) 
	{
		int is_public = (value->common.fn_flags & ZEND_ACC_PUBLIC);
		int name_equalied = zend_string_equals_cstr(value->common.function_name, method_src, method_len);

        if( is_public && name_equalied )
        {
            target_method = value;
            break;
        }	
    } ZEND_HASH_FOREACH_END();


    if( target_method == NULL )
    {
        // clean up
        zend_hash_destroy(class_argument);
        FREE_HASHTABLE(class_argument);
        zend_object_release(target_class);
        RETURN_NULL();
    }

    zend_arg_info*	args_src = target_method->common.arg_info;
    int args_len = target_method->common.num_args;

    for( int arg_index = 0; arg_index < args_len; ++arg_index )
    {
        zend_arg_info* argument = &args_src[arg_index];
        zend_string* arg_type = zend_type_to_string(argument->type);
        // zend_string* arg_name = argument->name;

        // if(! arg_type )
        if(! arg_type )
            php_error_docref(NULL, E_ERROR, "argument must have type hint");

        // if( zend_string_equals_cstr(arg_type, "int", sizeof("int") - 1) ||
        //     zend_string_equals_cstr(arg_type, "string", sizeof("string") - 1) ||
        //     zend_string_equals_cstr(arg_type, "array", sizeof("array") - 1)
        // )
        zval copy;
        if( ZEND_TYPE_FULL_MASK(argument->type) &
            (MAY_BE_LONG | MAY_BE_STRING | MAY_BE_ARRAY) )
        {
            ZVAL_STR(&copy, argument->name);
            zend_hash_next_index_insert(class_argument, &copy);
        }
        else
        {
            ZVAL_STR(&copy, arg_type);
            zend_hash_next_index_insert(class_argument, &copy);
        }

        // When clean'in up; may pontential to corrupt HashTable table contains
        // zend_string_release(arg_type);
        // zend_string_release(arg_name);
    }

    zend_object_release(target_class);

    if( args_len )
    {
        zval* params_getService = (zval*)safe_emalloc(1, sizeof(zval), 0);
        zval zv_class_argument; ZVAL_ARR(&zv_class_argument, class_argument);
        params_getService[0] = zv_class_argument;

        zval* filled_argument =
            php_class_call_method(instance->common->container, 
                "getService", sizeof("getService") - 1, 1, params_getService, 0);
        efree(params_getService);

        zend_hash_destroy(class_argument);
        FREE_HASHTABLE(class_argument);

        /**
        // A re-hash table for allowing free'd from method call
        HashTable* filled_argument_ref;
        ALLOC_HASHTABLE(filled_argument_ref);
        zend_hash_init(filled_argument_ref, 0, NULL, ZVAL_PTR_DTOR, 0);
        zend_hash_copy(filled_argument_ref, Z_ARR_P(filled_argument), zval_add_ref);

        zend_hash_destroy(Z_ARR_P(filled_argument));
        FREE_HASHTABLE(Z_ARR_P(filled_argument));
        efree(filled_argument);

        RETURN_ARR(filled_argument_ref);
        **/

        // Use convetion instead re-hash table and free zval ptr
        zval copy; ZVAL_COPY(&copy, filled_argument);
        efree(filled_argument);
        RETURN_ARR(Z_ARR(copy));
    }

    RETURN_ARR(class_argument);
}

PHP_METHOD(ArgumentResolver, addSegmentsToServicesContainer)
{
    HashTable* dispatch;
    ZEND_PARSE_PARAMETERS_START(1, 1)
        Z_PARAM_ARRAY_HT(dispatch);
    ZEND_PARSE_PARAMETERS_END();

    zval* segments = zend_hash_str_find(dispatch, "segments", sizeof("segments") - 1);
    zend_long segments_exists = zend_hash_num_elements(Z_ARR_P(segments));
    if( segments_exists > 0 )
    {
        argument_resolver_object* instance = 
            ZPHEUR_ZVAL_GET_OBJECT(argument_resolver_object, getThis());
        zval* params_addServiceFromArray = safe_emalloc(1, sizeof(zval), 0);
        params_addServiceFromArray[0] = *segments;
        
        zval* return_from = php_class_call_method(instance->common->container, 
            "setScalarFromArray", sizeof("setScalarFromArray") - 1, 1, params_addServiceFromArray, 0);

        efree(params_addServiceFromArray);
        efree(return_from);
    }
}

PHP_METHOD(ArgumentResolver, __destruct)
{
    zend_this_unset_property("container");
}

ZEND_MINIT_FUNCTION(Zpheur_Actions_Reflection_ArgumentResolver)
{   
    zend_class_entry ce;

    INIT_NS_CLASS_ENTRY(ce, "Zpheur\\Actions\\Reflection", "ArgumentResolver", zpheur_actions_reflection_argumentresolver_class_method);
    zpheur_actions_reflection_argumentresolver_class_entry = zend_register_internal_class(&ce);
    zpheur_actions_reflection_argumentresolver_class_entry->ce_flags |= ZEND_ACC_NO_DYNAMIC_PROPERTIES;
    zpheur_actions_reflection_argumentresolver_class_entry->create_object = create_argument_resolver_object;

    return SUCCESS;
}