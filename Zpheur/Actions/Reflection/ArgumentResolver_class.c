#include "php.h"
#include "ext/standard/info.h"
#include "php_zpheur.h"

#include <string.h>
#include <include/runtime.h>
#include <zpheur.h>
#include "ArgumentResolver_arginfo.h"


PHP_METHOD(ArgumentResolver, __construct)
{
    zval*   container;

    ZEND_PARSE_PARAMETERS_START(1, 1)
        Z_PARAM_ZVAL(container);
    ZEND_PARSE_PARAMETERS_END();

    zend_this_update_property("container", container);
}


PHP_METHOD(ArgumentResolver, getTargetParameter)
{
	char*  	class_src = NULL;
	size_t 	class_len = 0;

	char*  	method_src = NULL;
	size_t 	method_len = 0;

	ZEND_PARSE_PARAMETERS_START(2, 2)
		Z_PARAM_STRING(class_src, class_len)
		Z_PARAM_STRING(method_src, method_len)
	ZEND_PARSE_PARAMETERS_END();

	zend_object* 	target_class = php_class_init(class_src, class_len);
	zend_function* 	target_method;
	bool            target_method_found = false;

    zval class_argument;
    array_init(&class_argument);

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
        RETURN_NULL();

    zend_arg_info*	args_src = target_method->common.arg_info;
    int 			args_len = target_method->common.num_args;

    for( int arg_index = 0; arg_index < args_len; ++arg_index )
    {
        zend_arg_info*		argument = &args_src[arg_index];
        zend_string*		arg_type = zend_type_to_string(argument->type);
        zend_string* 		arg_name = argument->name;

        if(! arg_type )
        {
            php_error_docref(NULL, E_ERROR, "Argument must has a type hint");
        }

        if( zend_string_equals_cstr(arg_type, "int", sizeof("int") - 1) ||
            zend_string_equals_cstr(arg_type, "string", sizeof("string") - 1) ||
            zend_string_equals_cstr(arg_type, "array", sizeof("array") - 1)
            // zend_string_equals_cstr(arg_type, "object", sizeof("object") - 1)
        )
        {
            zval _arg_name;
            ZVAL_STR(&_arg_name, arg_name);
            add_next_index_zval(&class_argument, &_arg_name);
        }
        else
        {
            zval _arg_type;
            ZVAL_STR(&_arg_type, arg_type);
            add_next_index_zval(&class_argument, &_arg_type);
        }
    }

    if( args_len )
    {
        zval* container = zend_this_read_property("container");

        zval *params = (zval*)safe_emalloc(1, sizeof(zval), 0);
        ZVAL_ZVAL(&params[0], &class_argument, 1, 0);

        zval* filled_argument =
            php_class_call_method(Z_OBJ_P(container), "getOf", sizeof("getOf") - 1, 1, params, 0);

        // RETURN_ZVAL(filled_argument, 1, 0);
        RETURN_ZVAL(filled_argument, 0, 0);
    }

    RETURN_ARR(Z_ARR_P(&class_argument));
}

PHP_METHOD(ArgumentResolver, withTargetSegments)
{
    zval* segments;
        
    ZEND_PARSE_PARAMETERS_START(1, 1)
        Z_PARAM_ARRAY(segments)
    ZEND_PARSE_PARAMETERS_END();

    zval* container = zend_this_read_property("container");    

    ZEND_HASH_FOREACH_STR_KEY_VAL(Z_ARR_P(segments), zend_string* name, zval* value)
    {
        zval* params_set = (zval*)safe_emalloc(3, sizeof(zval), 0);
        ZVAL_STR(&params_set[0], name);
        ZVAL_ZVAL(&params_set[1], value, 1, 0);
        ZVAL_TRUE(&params_set[2]);

        php_class_call_method(Z_OBJ_P(container), "set", sizeof("set") - 1, 3, params_set, 0);
    }
    ZEND_HASH_FOREACH_END();
}

ZEND_MINIT_FUNCTION(Zpheur_Actions_Reflection_ArgumentResolver)
{   
    zend_class_entry ce;

    INIT_NS_CLASS_ENTRY(ce, "Zpheur\\Actions\\Reflection", "ArgumentResolver", zpheur_actions_reflection_argumentresolver_class_method);
    zpheur_actions_reflection_argumentresolver_class_entry = zend_register_internal_class(&ce);
    zpheur_actions_reflection_argumentresolver_class_entry->ce_flags |= ZEND_ACC_ALLOW_DYNAMIC_PROPERTIES;

    return SUCCESS;
}