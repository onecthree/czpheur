#include <php.h>
#include <ext/standard/info.h>
#include<Zend/zend_types.h>
#include <php_zpheur.h>

#include <ctype.h>
#include <string.h>
#include <zpheur.h>
#include <include/onecstr.h>
#include <include/runtime.h>
#include "Application_arginfo.h"


PHP_METHOD(Application, __construct)
{
	zval*	input_argument;
	zval*	action_resolver;
	zval*	argument_resolver;

	ZEND_PARSE_PARAMETERS_START(3, 3)
		Z_PARAM_ZVAL(input_argument)
		Z_PARAM_ZVAL(action_resolver)
		Z_PARAM_ZVAL(argument_resolver)
	ZEND_PARSE_PARAMETERS_END();

	zend_this_update_property("input_argument", input_argument);
	zend_this_update_property("action_resolver", action_resolver);
	zend_this_update_property("argument_resolver", argument_resolver);
}

PHP_METHOD(Application, withBaseNamespace)
{
	char*   base_namespace_src;
	size_t  base_namespace_len;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_STRING(base_namespace_src, base_namespace_len)
	ZEND_PARSE_PARAMETERS_END();

	zval base_namespace;
	ZVAL_STRINGL(&base_namespace, base_namespace_src, base_namespace_len);
	zend_this_update_property("base_namespace", &base_namespace);

	RETURN_ZVAL(getThis(), 1, 0);
}

// Define parser needle's
#define     TOKEN_SYMBOL_WHITESPACE		' '
#define     TOKEN_SYMBOL_TABSPACE		'\t'
#define     TOKEN_SYMBOL_BREAKLINE		'\n'
#define		TOKEN_ALPHA_LOWER_START		'a'
#define     TOKEN_ALPHA_LOWER_END		'z'
#define		TOKEN_ALPHA_UPPER_START		'A'
#define     TOKEN_ALPHA_UPPER_END		'Z'
#define     TOKEN_DIGIT_START			'0'
#define     TOKEN_DIGIT_END				'9'
#define     TOKEN_SYMBOL_UNDERSCORE		'_'
#define     TOKEN_SYMBOL_NULL			'\0'

#define     TOKEN_SYMBOL_HASHMARK		'#'
#define     TOKEN_SYMBOL_NEWLINE		'\n'

#define     TOKEN_SYMBOL_SINGLE_QUOTE	'\''
#define     TOKEN_SYMBOL_DOUBLE_QUOTE	'"'
#define     TOKEN_SYMBOL_COLON 			':'
#define     TOKEN_SYMBOL_SLASH			'/'
#define     TOKEN_SYMBOL_BACKSLASH		'\\'
#define     TOKEN_SYMBOL_EOF			EOF

#define     OPERATOR_SYMBOL_EQUAL		'='

#define CONSOLE_STATE_INIT 				0
#define CONSOLE_STATE_BEGIN_CLASS		1
#define CONSOLE_STATE_BEGIN_METHOD		2
#define CONSOLE_STATE_CLASS    			4
#define CONSOLE_STATE_METHOD    		5

static int console_class_method_parser(
	char* base_namespace,
	zend_string* target_class_zs,
	onec_string** target_class,
	onec_string** target_method
)
{
	onec_string_append(*target_class, 1, base_namespace);

	int 	state = CONSOLE_STATE_INIT;
	char 	input;

	for( int index = 0; index < target_class_zs->len; index += 1 )
	{
		input = target_class_zs->val[index];

		switch( state )
		{
			case CONSOLE_STATE_INIT:
				switch( input )
				{
					case TOKEN_ALPHA_LOWER_START ... TOKEN_ALPHA_LOWER_END:
					case TOKEN_SYMBOL_UNDERSCORE:
						index -= 1;
					case TOKEN_SYMBOL_SLASH:
						state = CONSOLE_STATE_BEGIN_CLASS;
					break;
					case TOKEN_SYMBOL_COLON:
						state = CONSOLE_STATE_BEGIN_METHOD;
					break;
					default:
						goto parse_error;
					break;
				}
			break;
			case CONSOLE_STATE_BEGIN_CLASS:
				state = CONSOLE_STATE_CLASS;
				onec_string_put(*target_class, '\\');
				onec_string_put(*target_class, toupper(input));
			break;
			case CONSOLE_STATE_BEGIN_METHOD:
				state = CONSOLE_STATE_METHOD;
				onec_string_append(*target_class, 1, "Action");
				onec_string_put(*target_method, input);
			break;
			case CONSOLE_STATE_CLASS:
				switch( input )
				{
					case TOKEN_SYMBOL_SLASH:
					case TOKEN_SYMBOL_COLON:
						state = CONSOLE_STATE_INIT;
						index -= 1;
					break;
					case TOKEN_ALPHA_LOWER_START ... TOKEN_ALPHA_LOWER_END:
					case TOKEN_ALPHA_UPPER_START ... TOKEN_ALPHA_UPPER_END:
					case TOKEN_DIGIT_START ... TOKEN_DIGIT_END:
					case TOKEN_SYMBOL_UNDERSCORE:
						onec_string_put(*target_class, input);
					break;
					default:
						goto parse_error;
					break;
				}
			break;
			case CONSOLE_STATE_METHOD:
				switch( input )
				{
					case TOKEN_ALPHA_LOWER_START ... TOKEN_ALPHA_LOWER_END:
					case TOKEN_ALPHA_UPPER_START ... TOKEN_ALPHA_UPPER_END:
					case TOKEN_DIGIT_START ... TOKEN_DIGIT_END:
					case TOKEN_SYMBOL_UNDERSCORE:
						onec_string_put(*target_method, input);
					break;
					default:
						goto parse_error;
					break;
				}
			break;
		}
	}

	if(! (*target_method)->len )
		onec_string_append(*target_class, 1, "Action");

	return 1;

	parse_error:
	return 0;
}

static int console_subns_class_method_parser(
	char* base_namespace,
	zend_string* target_class_zs,
	onec_string** target_class
)
{
	onec_string_append(*target_class, 1, base_namespace);

	int 	state = CONSOLE_STATE_INIT;
	char 	input;

	for( int index = 0; index < target_class_zs->len; index += 1 )
	{
		input = target_class_zs->val[index];

		switch( state )
		{
			case CONSOLE_STATE_INIT:
				switch( input )
				{
					case TOKEN_ALPHA_LOWER_START ... TOKEN_ALPHA_LOWER_END:
					case TOKEN_SYMBOL_UNDERSCORE:
						index -= 1;
					case TOKEN_SYMBOL_SLASH:
						state = CONSOLE_STATE_BEGIN_CLASS;
					break;
					case TOKEN_SYMBOL_COLON:
						state = CONSOLE_STATE_BEGIN_METHOD;
					break;
					default:
						goto parse_error;
					break;
				}
			break;
			case CONSOLE_STATE_BEGIN_CLASS:
				state = CONSOLE_STATE_CLASS;
				onec_string_put(*target_class, '\\');
				onec_string_put(*target_class, toupper(input));
			break;
			case CONSOLE_STATE_BEGIN_METHOD:
				state = CONSOLE_STATE_METHOD;
				onec_string_append(*target_class, 1, "\\");
				onec_string_put(*target_class, toupper(input));
			break;
			case CONSOLE_STATE_CLASS: // make
				switch( input )
				{
					case TOKEN_SYMBOL_SLASH:
					case TOKEN_SYMBOL_COLON:
						state = CONSOLE_STATE_INIT;
						index -= 1;
					break;
					case TOKEN_ALPHA_LOWER_START ... TOKEN_ALPHA_LOWER_END:
					case TOKEN_ALPHA_UPPER_START ... TOKEN_ALPHA_UPPER_END:
					case TOKEN_DIGIT_START ... TOKEN_DIGIT_END:
					case TOKEN_SYMBOL_UNDERSCORE:
						onec_string_put(*target_class, input);
					break;
					default:
						goto parse_error;
					break;
				}
			break;
			case CONSOLE_STATE_METHOD: // route
				switch( input )
				{
					case TOKEN_ALPHA_LOWER_START ... TOKEN_ALPHA_LOWER_END:
					case TOKEN_ALPHA_UPPER_START ... TOKEN_ALPHA_UPPER_END:
					case TOKEN_DIGIT_START ... TOKEN_DIGIT_END:
					case TOKEN_SYMBOL_UNDERSCORE:
						onec_string_put(*target_class, input);
					break;
					default:
						goto parse_error;
					break;
				}
			break;
		}
	}

	onec_string_append(*target_class, 1, "Action");

	return 1;

	parse_error:
	return 0;
}

static HashTable* application_class_method_dispatcher( char* base_namespace, zval** input_argument, zval** action_resolver )
{
	/* move index 0 to 1 */
	zval* target_action = zend_hash_index_find(Z_ARR_P((*input_argument)), 1);

    HashTable *dispatch; // don't free'd
    ALLOC_HASHTABLE(dispatch);
    zend_hash_init(dispatch, 0, NULL, ZVAL_PTR_DTOR, 0);

    zval target_class_empty;
    ZVAL_STRINGL(&target_class_empty, "", sizeof("") - 1);
    zend_hash_str_add(dispatch, "class", sizeof("class") - 1, &target_class_empty);

    zval target_method_empty;
    ZVAL_STRINGL(&target_method_empty, "", sizeof("") - 1);
    zend_hash_str_add(dispatch, "method", sizeof("method") - 1, &target_method_empty);

    
    do {
		if(! target_action )
        	break;

		zend_string* target_action_zs = zval_get_string(target_action);

		int    parse_success;
		int    second_parse = 0;
		char   input;

		onec_string *target_class, *target_method;

		onec_string_init(target_class);
		onec_string_init(target_method);

		char* 	  	 target_class_src = NULL;
		zend_object* target_class_init;

		__ext_second_parse:
		if(! second_parse )
			parse_success = console_class_method_parser(
				base_namespace, target_action_zs,
				&target_class, &target_method
			);
		else
			parse_success = console_subns_class_method_parser(
				base_namespace, target_action_zs, &target_class
			);

		if( !parse_success || !target_class->len )
    		goto local_cleanup;

		target_class_src = onec_string_get(target_class);
		target_class_init = php_class_init_ex(target_class_src);

		if(! target_class_init )
		{
			if( second_parse )
				goto local_cleanup;

        	onec_string_reset(target_class);
        	onec_string_reset(target_method);
        	second_parse = 1;
        	goto __ext_second_parse;
		}

		zval class_name, method_name;

		ZEND_HASH_FOREACH_PTR(&target_class_init->ce->function_table, zend_function* value) 
        {
        	// Is method are public modifier
           	if(! (value->common.fn_flags & ZEND_ACC_PUBLIC) )
           		continue;

        	if( !second_parse && 
        		zend_string_equals_cstr(
    			value->common.function_name, target_method->val, target_method->len
        		)
        	)
        	{
				ZVAL_STRINGL(&method_name, target_method->val, target_method->len);	
				goto __ext_target_found;
			} else
			if( zend_string_equals_cstr(
				value->common.function_name, "__invoke", sizeof("__invoke") - 1)
			)
			{
				ZVAL_STRINGL(&method_name, "__invoke", sizeof("__invoke") - 1);	
				goto __ext_target_found;
            }

        } ZEND_HASH_FOREACH_END(); 

    	goto local_cleanup;

    	__ext_target_found:
		ZVAL_STRINGL(&class_name, target_class->val, target_class->len);
		zend_hash_str_update(dispatch, "class", sizeof("class") - 1, &class_name);
		zend_hash_str_update(dispatch, "method", sizeof("method") - 1, &method_name);

		if( target_class_src )
			free(target_class_src);

		onec_string_release(target_class);
		onec_string_release(target_method);
		zend_string_release(target_action_zs);

		return dispatch;

		local_cleanup:

		if( target_class_src )
			free(target_class_src);

		onec_string_release(target_class);
		onec_string_release(target_method);
		zend_string_release(target_action_zs);
	} while(0);

	zval* params = (zval*)safe_emalloc(1, sizeof(zval), 0);
	ZVAL_ARR(&params[0], dispatch); // passed-by-ref not need free'd

	zval* default_dispatch =
		php_class_call_method(Z_OBJ_P((*action_resolver)), "bindIfDefaultAction", sizeof("bindIfDefaultAction") - 1, 1, params, 0);

	return Z_ARR_P(default_dispatch);
}

PHP_METHOD(Application, run)
{
	ZEND_PARSE_PARAMETERS_NONE();

	// Parse class-method from argument given [given declaration step]
	zval* base_namespace = zend_this_read_property("base_namespace");
	zval* input_argument = zend_this_read_property("input_argument");
	zval* action_resolver = zend_this_read_property("action_resolver");
	zval* argument_resolver = zend_this_read_property("argument_resolver");

	// Literally check if method was found or not [block step]
	/* use prop instead method */
	// zval* input_argument_value = php_class_call_method(Z_OBJ_P(input_argument), "getValue", sizeof("getValue") - 1, 0, NULL, 0);
	zval* input_argument_value = zend_read_property(Z_OBJCE_P(input_argument), Z_OBJ_P(input_argument), "value", sizeof("value") - 1, 0, NULL);
	HashTable* dispatched = application_class_method_dispatcher(Z_STRVAL_P(base_namespace), &input_argument_value, &action_resolver);

	if(! EG(exception) )
	{
		// A resolver for middleware list check [block step]
		zval* params_process = safe_emalloc(1, sizeof(zval), 0);
		ZVAL_ARR(&params_process[0], dispatched);
		zval* middleware_lists = 
			php_class_call_method(Z_OBJ_P(action_resolver), "process", sizeof("process") - 1, 1, params_process, 0);

		// Iteration for middleware list arg parse [block step]
		ZEND_HASH_FOREACH_VAL(Z_ARR_P(middleware_lists), zval* middleware)
		{
			zval* 			middleware_cname = zend_hash_index_find(Z_ARR_P(middleware), 0);
			zval* 			middleware_props = zend_hash_index_find(Z_ARR_P(middleware), 1);

			zend_string* 	middleware_class_name = zval_get_string(middleware_cname);
			zend_object* 	middleware_class = php_class_init(middleware_class_name->val, middleware_class_name->len);

			/* Set property value {{{ */
			ZEND_HASH_FOREACH_STR_KEY_VAL(Z_ARR_P(middleware_props), zend_string* prop_name, zval* prop_value)
			{
				ZEND_HASH_FOREACH_PTR(&middleware_class->ce->properties_info, zend_property_info* zpi_property )
		    	{	
		    		bool is_property = zend_string_equals(zpi_property->name, prop_name);

		    		if( (zpi_property->flags & ZEND_ACC_PUBLIC) && is_property )
		    		{
						zend_update_property(middleware_class->ce, middleware_class, prop_name->val, prop_name->len, prop_value);
						goto skip_error;
		    		}

		    		if( (zpi_property->flags & ZEND_ACC_PROTECTED) )
		    		{
		    			char property_name_src[zpi_property->name->len - 2];
		    			zend_ulong property_name_len = 0;
		    			zend_ulong target_len = 3;

		    			while( target_len < zpi_property->name->len )
		    			{
		    				property_name_src[property_name_len] = zpi_property->name->val[target_len];
		    				target_len += 1; property_name_len += 1;
		    			}
		    			property_name_src[property_name_len] = '\0';

		    			if( zend_string_equals_cstr(prop_name, property_name_src, property_name_len) )
		    			{
			    			zend_error(
								E_ERROR,
								"Cannot assign to protected property '%s::$%s'",
								middleware_class->ce->name->val,
								property_name_src
							);
		    			}
		    		}

		    		if( (zpi_property->flags & ZEND_ACC_PRIVATE) )
		    		{
		    			char property_name_src[zpi_property->name->len - (middleware_class->ce->name->len + 1)];
		    			zend_ulong property_name_len = 0;
		    			zend_ulong target_len = (middleware_class->ce->name->len + 2);

		    			while( target_len < zpi_property->name->len )
		    			{
		    				property_name_src[property_name_len] = zpi_property->name->val[target_len];
		    				target_len += 1; property_name_len += 1;
		    			}
		    			property_name_src[property_name_len] = '\0';

		    			if( zend_string_equals_cstr(prop_name, property_name_src, property_name_len) )
		    			{
			    			zend_error(
								E_ERROR,
								"Cannot assign to private property '%s::$%s'",
								middleware_class->ce->name->val,
								property_name_src
							);
						}
		    		}
		    	}
		    	ZEND_HASH_FOREACH_END();

				zend_error(
					E_ERROR,
					"Cannot assign to undefined property '%s::$%s'",
					middleware_class->ce->name->val,
					prop_name->val
				);

				skip_error: {}
			}
			ZEND_HASH_FOREACH_END();
			/* }}} */

			/* Call middleware constructor */
			{
				zval* params_getTargetParameter = (zval*)safe_emalloc(2, sizeof(zval), 0);
				ZVAL_STRINGL(&params_getTargetParameter[0], middleware_class_name->val, middleware_class_name->len);
				ZVAL_STRINGL(&params_getTargetParameter[1], "__construct", sizeof("__construct") - 1);

				zval* 		middleware_params_src = php_class_call_method(Z_OBJ_P(argument_resolver), "getTargetParameter", sizeof("getTargetParameter") - 1, 2, params_getTargetParameter, 0);

				zend_ulong 	middleware_params_len = zend_hash_num_elements(Z_ARR_P(middleware_params_src));
				zval* 		middleware_params_resolve = (zval*)safe_emalloc(middleware_params_len, sizeof(zval), 0);

				if( Z_TYPE_P(middleware_params_src) != IS_NULL )
				{
					zend_ulong 	middleware_params_len = zend_hash_num_elements(Z_ARR_P(middleware_params_src));
					zval* 		middleware_params_resolve = (zval*)safe_emalloc(middleware_params_len, sizeof(zval), 0);

					for( int i = 0; i < middleware_params_len; i += 1)
					{
						ZVAL_ZVAL(&middleware_params_resolve[i], zend_hash_index_find(Z_ARR_P(middleware_params_src), i), 0, 0);
						ZVAL_MAKE_REF(&middleware_params_resolve[i]);
					}

					php_class_call_constructor(middleware_class, middleware_params_len, middleware_params_resolve);
				}
				else
					zval_ptr_dtor(middleware_params_src);
				zval_ptr_dtor(params_getTargetParameter);
			}

			/* Call middleware method */
			{
				zval* params_getTargetParameter = (zval*)safe_emalloc(2, sizeof(zval), 0);
				ZVAL_STRINGL(&params_getTargetParameter[0], middleware_class_name->val, middleware_class_name->len);
				ZVAL_STRINGL(&params_getTargetParameter[1], "__invoke", sizeof("__invoke") - 1);

				zval* 		middleware_params_src = php_class_call_method(Z_OBJ_P(argument_resolver), "getTargetParameter", sizeof("getTargetParameter") - 1, 2, params_getTargetParameter, 0);
				zend_ulong 	middleware_params_len = zend_hash_num_elements(Z_ARR_P(middleware_params_src));
				zval* 		middleware_params_resolve = (zval*)safe_emalloc(middleware_params_len, sizeof(zval), 0);

				zval_ptr_dtor(params_getTargetParameter);

				for( int i = 0; i < middleware_params_len; i += 1)
				{
					ZVAL_ZVAL(&middleware_params_resolve[i], zend_hash_index_find(Z_ARR_P(middleware_params_src), i), 0, 0);
					ZVAL_MAKE_REF(&middleware_params_resolve[i]);
				}

				// it should return zval ptr
				php_class_call_method(middleware_class, "__invoke", sizeof("__invoke") - 1, middleware_params_len, middleware_params_resolve, 0);
			}
		}
		ZEND_HASH_FOREACH_END();

		zval_ptr_dtor(middleware_lists);

		zval*   		action_class = zend_hash_str_find(dispatched, "class", sizeof("class") - 1);
		zval*   		action_method = zend_hash_str_find(dispatched, "method", sizeof("method") - 1);

		zend_string*    _action_class = zval_get_string(action_class);
		zend_object*	class_action = php_class_init(_action_class->val, _action_class->len);

		/* Call action constructor */
		{
			zval* params_getTargetParameter = (zval*)safe_emalloc(2, sizeof(zval), 0);
			ZVAL_ZVAL(&params_getTargetParameter[0], action_class, 1, 0);
			ZVAL_STRINGL(&params_getTargetParameter[1], "__construct", sizeof("__construct") - 1);

			zval* method_action_param_src = php_class_call_method(Z_OBJ_P(argument_resolver), "getTargetParameter", sizeof("getTargetParameter") - 1, 2, params_getTargetParameter, 0);

			if( Z_TYPE_P(method_action_param_src) != IS_NULL )
			{
				zend_ulong 	method_action_param_len = zend_hash_num_elements(Z_ARR_P(method_action_param_src));
				zval* 		middleware_params_resolve = (zval*)safe_emalloc(method_action_param_len, sizeof(zval), 0);

				for( int i = 0; i < method_action_param_len; i += 1)
				{
					ZVAL_ZVAL(&middleware_params_resolve[i], zend_hash_index_find(Z_ARR_P(method_action_param_src), i), 0, 0);
					ZVAL_MAKE_REF(&middleware_params_resolve[i]);
				}

				// it should return zval ptr
				php_class_call_constructor(class_action, method_action_param_len, middleware_params_resolve);	
			}
			else
				zval_ptr_dtor(method_action_param_src);
			zval_ptr_dtor(params_getTargetParameter);
		}

		/* Call action method */
		{
			zval* params_getTargetParameter = (zval*)safe_emalloc(2, sizeof(zval), 0);
			ZVAL_ZVAL(&params_getTargetParameter[0], action_class, 1, 0);
			ZVAL_ZVAL(&params_getTargetParameter[1], action_method, 1, 0);

			zval* 			method_action_param_src = php_class_call_method(Z_OBJ_P(argument_resolver), "getTargetParameter", sizeof("getTargetParameter") - 1, 2, params_getTargetParameter, 0);
			zend_ulong 		method_action_param_len = zend_hash_num_elements(Z_ARR_P(method_action_param_src));
			zval* 			middleware_params_resolve = (zval*)safe_emalloc(method_action_param_len, sizeof(zval), 0);

			for( int i = 0; i < method_action_param_len; i += 1)
			{
				ZVAL_ZVAL(&middleware_params_resolve[i], zend_hash_index_find(Z_ARR_P(method_action_param_src), i), 0, 0);
				ZVAL_MAKE_REF(&middleware_params_resolve[i]);
			}

			// clean-up
			zval_ptr_dtor(action_class);
			zval_ptr_dtor(action_method);
			zval_ptr_dtor(params_getTargetParameter);

			// it should return zval ptr
			zend_string* action_call = zval_get_string(action_method);
			php_class_call_method(class_action, action_call->val, action_call->len, method_action_param_len, middleware_params_resolve, 0);
		}

		/* Call action destructor */
		{
	        php_class_call_method(class_action, "__destruct", sizeof("__destruct") - 1, 0, NULL, 1);
		}
	}
}


ZEND_MINIT_FUNCTION(Zpheur_Consoles_Runtime_Application)
{	
    zend_class_entry ce;

    INIT_NS_CLASS_ENTRY(ce, "Zpheur\\Consoles\\Runtime", "Application", zpheur_consoles_runtime_application_class_method);
    zpheur_consoles_runtime_application_class_entry = zend_register_internal_class(&ce);
    zpheur_consoles_runtime_application_class_entry->ce_flags |= ZEND_ACC_ALLOW_DYNAMIC_PROPERTIES;

    zend_declare_property_null(zpheur_consoles_runtime_application_class_entry, "input_argument", sizeof("input_argument") - 1, ZEND_ACC_PUBLIC);
    zend_declare_property_null(zpheur_consoles_runtime_application_class_entry, "action_resolver", sizeof("action_resolver") - 1, ZEND_ACC_PUBLIC);
    zend_declare_property_null(zpheur_consoles_runtime_application_class_entry, "argument_resolver", sizeof("argument_resolver") - 1, ZEND_ACC_PUBLIC);
    zend_declare_property_stringl(zpheur_consoles_runtime_application_class_entry, "base_namespace", sizeof("base_namespace") - 1, "", sizeof("") - 1, ZEND_ACC_PUBLIC);

    return SUCCESS;
}	