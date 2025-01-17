#include <php.h>
#include <ext/standard/info.h>
#include <Zend/zend_types.h>
#include <php_zpheur.h>

#include <ctype.h>
#include <string.h>
#include <zpheur.h>
#include <include/onecstr.h>
#include <include/runtime.h>
#include <Zpheur/Consoles/Input/InputArgument_arginfo.h>
#include <Zpheur/Actions/Reflection/ActionResolver_arginfo.h>
#include <Zpheur/Actions/Reflection/ArgumentResolver_arginfo.h>
#include "Application_arginfo.h"


zend_object_handlers application_object_handlers;

typedef struct _application_common_t
{
    zend_object* input_argument; // don't need release
    zend_object* action_resolver; // don't need release
    zend_object* argument_resolver; // don't need release
    onec_string* base_namespace;
} application_common_t;

typedef struct _application_object
{
    application_common_t* common;
    zend_object std;
} application_object;

void free_application_object(zend_object *object)
{
    application_object* instance = ZPHEUR_GET_OBJECT(application_object, object);

    zend_object_std_dtor(&instance->std);
    if( instance->common ) {
    	if( instance->common->base_namespace ) {
    		onec_string_release(instance->common->base_namespace);
    	}
        efree(instance->common);       
    }
}

zend_object* create_application_object( zend_class_entry* ce )
{
    application_object* object = 
        ecalloc(1, sizeof(application_object) + zend_object_properties_size(ce));

    zend_object_std_init(&object->std, ce);
    object_properties_init(&object->std, ce);

    memcpy(&application_object_handlers, zend_get_std_object_handlers(), sizeof(application_object_handlers));
    application_object_handlers.offset = XtOffsetOf(application_object, std);
    application_object_handlers.free_obj = free_application_object;
    object->std.handlers = &application_object_handlers;

    object->common = emalloc(sizeof(application_common_t));
    object->common->input_argument = NULL;
    object->common->action_resolver = NULL;
    object->common->argument_resolver = NULL;
    object->common->base_namespace = NULL;

    return &object->std;
}

PHP_METHOD(Application, __construct)
{
	zval*	input_argument;
	zval*	action_resolver;
	zval*	argument_resolver;

	ZEND_PARSE_PARAMETERS_START(3, 3)
		Z_PARAM_OBJECT_OF_CLASS(input_argument, zpheur_consoles_input_inputargument_class_entry);
		Z_PARAM_OBJECT_OF_CLASS(action_resolver, zpheur_actions_reflection_actionresolver_class_entry);
		Z_PARAM_OBJECT_OF_CLASS(argument_resolver, zpheur_actions_reflection_argumentresolver_class_entry);
	ZEND_PARSE_PARAMETERS_END();

    application_object* instance = 
        ZPHEUR_ZVAL_GET_OBJECT(application_object, getThis());

    instance->common->input_argument = Z_OBJ_P(input_argument);
    instance->common->action_resolver = Z_OBJ_P(action_resolver);
    instance->common->argument_resolver = Z_OBJ_P(argument_resolver);
}

PHP_METHOD(Application, withBaseNamespace)
{
	char* base_namespace_src;
	size_t base_namespace_len;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_STRING(base_namespace_src, base_namespace_len)
	ZEND_PARSE_PARAMETERS_END();

    application_object* instance = 
        ZPHEUR_ZVAL_GET_OBJECT(application_object, getThis());

   	if( instance->common->base_namespace != NULL ) {
   		onec_string_release(instance->common->base_namespace);
   	}

   	instance->common->base_namespace = onec_string_initd(base_namespace_src, base_namespace_len);

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

static int console_class_method_parser( char* base_namespace, zend_string* target_class_zs, onec_stringlc* target_class, onec_stringlc* target_method )
{
	onec_string_appendlc(*target_class, 1, base_namespace);
	int state = CONSOLE_STATE_INIT;
	char input;

	for( int index = 0; index < target_class_zs->len; index += 1 ) {
		input = target_class_zs->val[index];
		switch( state ) {
			case CONSOLE_STATE_INIT:
				switch( input ) {
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
				onec_string_putlc(*target_class, '\\');
				onec_string_putlc(*target_class, toupper(input));
			break;
			case CONSOLE_STATE_BEGIN_METHOD:
				state = CONSOLE_STATE_METHOD;
				onec_string_appendlc(*target_class, 1, "Action");
				onec_string_putlc(*target_method, input);
			break;
			case CONSOLE_STATE_CLASS:
				switch( input ) {
					case TOKEN_SYMBOL_SLASH:
					case TOKEN_SYMBOL_COLON:
						state = CONSOLE_STATE_INIT;
						index -= 1;
					break;
					case TOKEN_ALPHA_LOWER_START ... TOKEN_ALPHA_LOWER_END:
					case TOKEN_ALPHA_UPPER_START ... TOKEN_ALPHA_UPPER_END:
					case TOKEN_DIGIT_START ... TOKEN_DIGIT_END:
					case TOKEN_SYMBOL_UNDERSCORE:
						onec_string_putlc(*target_class, input);
					break;
					default:
						goto parse_error;
					break;
				}
			break;
			case CONSOLE_STATE_METHOD:
				switch( input ) {
					case TOKEN_ALPHA_LOWER_START ... TOKEN_ALPHA_LOWER_END:
					case TOKEN_ALPHA_UPPER_START ... TOKEN_ALPHA_UPPER_END:
					case TOKEN_DIGIT_START ... TOKEN_DIGIT_END:
					case TOKEN_SYMBOL_UNDERSCORE:
						onec_string_putlc(*target_method, input);
					break;
					default:
						goto parse_error;
					break;
				}
			break;
		}
	}

	if(! (*target_method).len ) {
		onec_string_appendlc(*target_class, 1, "Action");
	}

	return 1;

	parse_error:
	return 0;
}

static int console_subns_class_method_parser( char* base_namespace, zend_string* target_class_zs, onec_stringlc* target_class )
{
	onec_string_appendlc(*target_class, 1, base_namespace);
	int state = CONSOLE_STATE_INIT;
	char input;

	for( int index = 0; index < target_class_zs->len; index += 1 ) {
		input = target_class_zs->val[index];
		switch( state ) {
			case CONSOLE_STATE_INIT:
				switch( input ) {
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
				onec_string_putlc(*target_class, '\\');
				onec_string_putlc(*target_class, toupper(input));
			break;
			case CONSOLE_STATE_BEGIN_METHOD:
				state = CONSOLE_STATE_METHOD;
				onec_string_appendlc(*target_class, 1, "\\");
				onec_string_putlc(*target_class, toupper(input));
			break;
			case CONSOLE_STATE_CLASS: // make
				switch( input ) {
					case TOKEN_SYMBOL_SLASH:
					case TOKEN_SYMBOL_COLON:
						state = CONSOLE_STATE_INIT;
						index -= 1;
					break;
					case TOKEN_ALPHA_LOWER_START ... TOKEN_ALPHA_LOWER_END:
					case TOKEN_ALPHA_UPPER_START ... TOKEN_ALPHA_UPPER_END:
					case TOKEN_DIGIT_START ... TOKEN_DIGIT_END:
					case TOKEN_SYMBOL_UNDERSCORE:
						onec_string_putlc(*target_class, input);
					break;
					default:
						goto parse_error;
					break;
				}
			break;
			case CONSOLE_STATE_METHOD: // route
				switch( input ) {
					case TOKEN_ALPHA_LOWER_START ... TOKEN_ALPHA_LOWER_END:
					case TOKEN_ALPHA_UPPER_START ... TOKEN_ALPHA_UPPER_END:
					case TOKEN_DIGIT_START ... TOKEN_DIGIT_END:
					case TOKEN_SYMBOL_UNDERSCORE:
						onec_string_putlc(*target_class, input);
					break;
					default:
						goto parse_error;
					break;
				}
			break;
		}
	}

	onec_string_appendlc(*target_class, 1, "Action");

	return 1;

	parse_error:
	return 0;
}

/* {{{ application_class_method_dispatcher */
static HashTable* application_class_method_dispatcher( char* base_namespace, zval* input_argument, zend_object* action_resolver )
{
	/* move index 0 to 1 */
	zval* target_action = zend_hash_index_find(Z_ARR_P(input_argument), 1); // After "bin/console"

    HashTable* dispatch; // don't free'd
    ALLOC_HASHTABLE(dispatch);
    zend_hash_init(dispatch, 0, NULL, ZVAL_PTR_DTOR, 0);

    // zval target_class_empty;
    // ZVAL_STRINGL(&target_class_empty, "", sizeof("") - 1);
    // zend_hash_str_add(dispatch, "class", sizeof("class") - 1, &target_class_empty);

    // zval target_method_empty;
    // ZVAL_STRINGL(&target_method_empty, "", sizeof("") - 1);
    // zend_hash_str_add(dispatch, "method", sizeof("method") - 1, &target_method_empty);

    do {
		if(! target_action ) {
        	break;
		}

		zend_string* target_action_zs = Z_STR_P(target_action);
		int parse_success;
		int second_parse = 0;
		char input;

		onec_stringlc target_class;
		onec_stringlc target_method;

		onec_string_initlc(target_class);
		onec_string_initlc(target_method);

		char* target_class_src = NULL;
		zend_object* target_class_init = NULL;

		__ext_second_parse:
		if(! second_parse ) {
			parse_success = console_class_method_parser(
				base_namespace, target_action_zs,
				&target_class, &target_method
			);
		} else {
			onec_string_resetlc(target_class);
			parse_success = console_subns_class_method_parser(
				base_namespace, target_action_zs, &target_class
			);
		}

		if( !parse_success || !target_class.len ) {
    		goto local_cleanup;
		}

		onec_string_trimlc(target_class);
		target_class_init = // Only check class exists, clean up soon
			php_class_init_silent(target_class.val, target_class.len);

		if(! target_class_init ) {
			if( second_parse ) {
				goto local_cleanup;
			}

        	second_parse = 1;
        	goto __ext_second_parse;
		}

		onec_string_trimlc(target_class);
		onec_string_trimlc(target_method);
		zval class_name, method_name;

		ZEND_HASH_FOREACH_PTR(&target_class_init->ce->function_table, zend_function* value) 
        {
        	// Is method are public modifier
           	if(! (value->common.fn_flags & ZEND_ACC_PUBLIC) ) {
           		continue;
           	}

        	if( !second_parse && zend_string_equals_cstr(
			value->common.function_name, target_method.val, target_method.len) ) {
				ZVAL_STRINGL(&method_name, target_method.val, target_method.len);	
				goto __ext_target_found;
			} else {
				if( zend_string_equals_cstr(value->common.function_name, "__invoke", sizeof("__invoke") - 1) ) {
					ZVAL_STRINGL(&method_name, "__invoke", sizeof("__invoke") - 1);	
					goto __ext_target_found;
	            }
			}
        } ZEND_HASH_FOREACH_END(); 

    	goto local_cleanup;

    	__ext_target_found:
		ZVAL_STRINGL(&class_name, target_class.val, target_class.len);
		zend_hash_str_update(dispatch, "class", sizeof("class") - 1, &class_name);
		zend_hash_str_update(dispatch, "method", sizeof("method") - 1, &method_name);

		zend_object_release(target_class_init); // clean up

		// zval* return_dispatch = emalloc(sizeof(zval));
		// ZVAL_ARR(return_dispatch, dispatch);
		// return return_dispatch;
		return dispatch;

		local_cleanup:
		if( target_class_init ) {
			zend_object_release(target_class_init); // clean up
		}
	} while(0);

	zval* params = (zval*)safe_emalloc(1, sizeof(zval), 0);
	// zval* copy = emalloc(sizeof(zval)); 
	// ZVAL_ARR(copy, dispatch);
	zval copy; ZVAL_ARR(&copy, dispatch);
	params[0] = copy;

	zval default_dispatch;
	php_class_call_method_stacked(action_resolver, "bindIfDefaultAction", sizeof("bindIfDefaultAction") - 1,
		1, params, &default_dispatch, 0);
	efree(params);

	zend_hash_clean(dispatch);
	zend_hash_copy(dispatch, Z_ARR(default_dispatch), zval_add_ref);

	zend_hash_destroy(Z_ARR(default_dispatch));
	FREE_HASHTABLE(Z_ARR(default_dispatch));

	return dispatch;
} /* application_class_method_dispatcher }}} */

/* {{{ run */
PHP_METHOD(Application, run)
{
	ZEND_PARSE_PARAMETERS_NONE();

	// Parse class-method from argument given [given declaration step]
    application_object* instance = 
        ZPHEUR_ZVAL_GET_OBJECT(application_object, getThis());

	onec_string* base_namespace = instance->common->base_namespace;
	zend_object* input_argument = instance->common->input_argument;
	zend_object* action_resolver = instance->common->action_resolver;
	zend_object* argument_resolver = instance->common->argument_resolver;
	// zval* return_action;

	// Literally check if method was found or not [block step]
	/* use prop instead method */
	zval input_argument_value;
	php_class_call_method_stacked(input_argument, "listAllValues", sizeof("listAllValues") - 1,
		0, NULL, &input_argument_value, 0);

	char* cs_base_namespace = onec_string_get(base_namespace);
	// zval* dispatched =
	HashTable* dispatched =
		application_class_method_dispatcher(cs_base_namespace, &input_argument_value, action_resolver);

	zend_hash_destroy(Z_ARR(input_argument_value));
	FREE_HASHTABLE(Z_ARR(input_argument_value));
	free(cs_base_namespace);

	if(! EG(exception) ) {
		// A resolver for middleware list check [block step]
		zval* params_process = safe_emalloc(2, sizeof(zval), 0);
		zval zv_table; ZVAL_ARR(&zv_table, dispatched);
		params_process[0] = zv_table;
		zval zv_true; ZVAL_TRUE(&zv_true);
		params_process[1] = zv_true;

		zval before_middlewares;
		php_class_call_method_stacked(action_resolver, "beforeMiddlewaresResolve",
			sizeof("beforeMiddlewaresResolve") - 1, 2, params_process, &before_middlewares, 0);
		efree(params_process);

		// Iteration for middleware list arg parse [block step]
		ZEND_HASH_FOREACH_VAL(Z_ARR(before_middlewares), zval* middleware)
		{		
			zval* middleware_cname = zend_hash_index_find(Z_ARR_P(middleware), 0);
			zval* middleware_props = zend_hash_index_find(Z_ARR_P(middleware), 1);

			zend_string* middleware_class_name = zval_get_string(middleware_cname);
			zend_object* middleware_class = php_class_init(middleware_class_name->val, middleware_class_name->len);

			/* Set property value {{{ */
			ZEND_HASH_FOREACH_STR_KEY_VAL(Z_ARR_P(middleware_props), zend_string* prop_name, zval* prop_value)
			{
				ZEND_HASH_FOREACH_PTR(&middleware_class->ce->properties_info, zend_property_info* zpi_property )
		    	{	
		    		bool is_property = zend_string_equals(zpi_property->name, prop_name);

		    		if( Z_PHP_ACC_PUBLIC(zpi_property->flags) && is_property ) {
						zend_update_property(middleware_class->ce, middleware_class, prop_name->val, prop_name->len, prop_value);
						goto skip_error;
		    		}

		    		if( property_name_compare_offset_x(zpi_property->name, prop_name) ) {
			    		if( Z_PHP_ACC_PROTECTED(zpi_property->flags) ) {
			    			zend_error(
								E_ERROR,
								"Cannot assign to protected property '%s::$%s'",
								middleware_class_name->val,
								zpi_property->name->val
							);
			    		}

						if( Z_PHP_ACC_PRIVATE(zpi_property->flags) ) {
			    			zend_error(
								E_ERROR,
								"Cannot assign to private property '%s::$%s'",
								middleware_class_name->val,
								zpi_property->name->val
							);
			    		}
			    	}
		    	}
		    	ZEND_HASH_FOREACH_END();

				zend_error(
					E_ERROR,
					"Cannot assign to undefined property '%s::$%s'",
					middleware_class_name->val,
					prop_name->val
				);

				skip_error: {}
			}
			ZEND_HASH_FOREACH_END();
			/* }}} */

			/* Call middleware constructor */
			{
				zval* params_resolve = (zval*)safe_emalloc(2, sizeof(zval), 0);
				zval _middleware_class_name; ZVAL_STR(&_middleware_class_name, middleware_class_name);
				params_resolve[0] = _middleware_class_name;
				zend_string* _zs_construct = zend_string_init("__construct", sizeof("__construct") - 1, 0);
				zval _construct; ZVAL_STR(&_construct, _zs_construct);
				params_resolve[1] = _construct;

				zval middleware_params_src;
				php_class_call_method_stacked(argument_resolver,
					"resolve", sizeof("resolve") - 1, 2, params_resolve, &middleware_params_src, 0);

				if( Z_TYPE(middleware_params_src) != IS_NULL ) {
					zend_ulong 	middleware_params_len = zend_hash_num_elements(Z_ARR(middleware_params_src));
					zval* 		middleware_params_resolve = (zval*)safe_emalloc(middleware_params_len, sizeof(zval), 0);

					for( int i = 0; i < middleware_params_len; i += 1) {
						middleware_params_resolve[i] = *(zend_hash_index_find(Z_ARR(middleware_params_src), i));
					}

					php_class_call_constructor(middleware_class, middleware_params_len, middleware_params_resolve);
					zend_hash_graceful_reverse_destroy(Z_ARR(middleware_params_src));
					FREE_HASHTABLE(Z_ARR(middleware_params_src));
					efree(middleware_params_resolve);
				} // when construct not found; if may can inherit from the parent (?)

				zend_string_release(_zs_construct);
				efree(params_resolve);
			}

			/* Call middleware method */
			zval local_return_action;
			do {
				zval* params_resolve = (zval*)safe_emalloc(2, sizeof(zval), 0);

				zval _middleware_class_name; ZVAL_STR(&_middleware_class_name, middleware_class_name);
				params_resolve[0] = _middleware_class_name;
				zend_string* process_str = zend_string_init("process", sizeof("process") - 1, 0);
				zval process_param; ZVAL_STR(&process_param, process_str);
				params_resolve[1] = process_param;

				zval middleware_params_src;
				php_class_call_method_stacked(argument_resolver, "resolve", sizeof("resolve") - 1, 2,
					params_resolve, &middleware_params_src, 0);

				if( Z_TYPE(middleware_params_src) != IS_NULL ) {
					zend_ulong middleware_params_len = zend_hash_num_elements(Z_ARR(middleware_params_src));
					zval* middleware_params_resolve = (zval*)safe_emalloc(middleware_params_len, sizeof(zval), 0);

					for( int i = 0; i < middleware_params_len; i += 1) {
						middleware_params_resolve[i] = *(zend_hash_index_find(Z_ARR(middleware_params_src), i));
					}

					// It should return zval ptr
					php_class_call_method_stacked(middleware_class, "process", sizeof("process") - 1,
					middleware_params_len, middleware_params_resolve, &local_return_action, 0);

					zend_hash_graceful_reverse_destroy(Z_ARR(middleware_params_src));
					FREE_HASHTABLE(Z_ARR(middleware_params_src));
					efree(middleware_params_resolve);
				} else {
					php_error_docref(NULL,
		        		E_CORE_ERROR, "Call to undefined method %s::process()",
		        		middleware_class_name->val);
				}

				zend_string_release(process_str);
				efree(params_resolve);
			} while(0);

			switch( (1 << Z_TYPE(local_return_action)) )
			{
				case MAY_BE_NULL: 
					// efree(local_return_action);
				break; // Ignore null return or void
				default:
					if(! EXPECTED(EG(exception)) ) { // throw exception
						php_error_docref(NULL, E_ERROR,
							"%s::process() return type must be integer or void",
							middleware_class_name->val
						);	
						break;
					}
				case MAY_BE_LONG:
					// efree(local_return_action);
					zend_object_release(middleware_class);
					zend_string_release(middleware_class_name);

					zend_hash_destroy(Z_ARR(before_middlewares));
					FREE_HASHTABLE(Z_ARR(before_middlewares));
					goto early_disturb;
				break;
			}

			zend_object_release(middleware_class);
			zend_string_release(middleware_class_name);
		}
		ZEND_HASH_FOREACH_END(); // Before middleware

		zend_hash_destroy(Z_ARR(before_middlewares));
		FREE_HASHTABLE(Z_ARR(before_middlewares));

		zval* action_class = 
			zend_hash_str_find(dispatched, "class", sizeof("class") - 1);
		zval* action_method = 
			zend_hash_str_find(dispatched, "method", sizeof("method") - 1);

		zend_object* class_action = 
			php_class_init(Z_STR_P(action_class)->val, Z_STR_P(action_class)->len);


		/* Call action constructor */
		do {
			zval* params_resolve = (zval*)safe_emalloc(2, sizeof(zval), 0);

			params_resolve[0] = *action_class;
			zend_string* _construct = zend_string_init("__construct", sizeof("__construct") - 1, 0);
			zval __zv_construct; ZVAL_STR(&__zv_construct, _construct);
			params_resolve[1] = __zv_construct;

			zval method_action_param_src;
			php_class_call_method_stacked(argument_resolver, "resolve", sizeof("resolve") - 1, 2,
				params_resolve, &method_action_param_src, 0);

			if( Z_TYPE(method_action_param_src) != IS_NULL ) {
				zend_ulong 	method_action_param_len = zend_hash_num_elements(Z_ARR(method_action_param_src));
				zval* middleware_params_resolve = (zval*)safe_emalloc(method_action_param_len, sizeof(zval), 0);

				for( int i = 0; i < method_action_param_len; i += 1) {
					middleware_params_resolve[i] = *(zend_hash_index_find(Z_ARR(method_action_param_src), i));
				}

				// Construct no return value
				php_class_call_constructor(class_action, method_action_param_len, middleware_params_resolve);	

				zend_hash_graceful_reverse_destroy(Z_ARR(method_action_param_src));
				FREE_HASHTABLE(Z_ARR(method_action_param_src));
				efree(middleware_params_resolve);
			} // when construct not found; if may can inherit from the parent (?)
		
			zend_string_release(_construct);
			efree(params_resolve);
		} while(0); // Action constructor

		if( EG(exception) ) { 
	        goto early_disturb;
	    }

		/* Call action method */
		zval local_return_action;
		do {
			zval* params_resolve = (zval*)safe_emalloc(2, sizeof(zval), 0);
			params_resolve[0] = *action_class;
			params_resolve[1] = *action_method;

			zval method_action_param_src;
			php_class_call_method_stacked(argument_resolver, "resolve", sizeof("resolve") - 1, 2,
				params_resolve, &method_action_param_src, 0);
			efree(params_resolve);

			// it should return zval ptr
			zend_string* action_call = zval_get_string(action_method);

			zend_ulong method_action_param_len = zend_hash_num_elements(Z_ARR(method_action_param_src));
			zval* middleware_params_resolve = (zval*)safe_emalloc(method_action_param_len, sizeof(zval), 0);

			if( Z_TYPE(method_action_param_src) != IS_NULL || method_action_param_len > 0 ) {
				for( int i = 0; i < method_action_param_len; i++ ) {
					middleware_params_resolve[i] = 
					*(zend_hash_index_find(Z_ARR(method_action_param_src), i));
				}

				// local_return_action =
				php_class_call_method_stacked(class_action, action_call->val, action_call->len,
					method_action_param_len, middleware_params_resolve, &local_return_action, 0);

				zend_hash_destroy(Z_ARR(method_action_param_src));
				FREE_HASHTABLE(Z_ARR(method_action_param_src));
			} else {
				php_error_docref(NULL,
	        		E_CORE_ERROR, "Call to undefined method %s::%s()",
	        		Z_STR_P(action_class)->val, Z_STR_P(action_method)->val);
			}

			efree(middleware_params_resolve);
			zend_string_release(action_call);

			switch( (1 << Z_TYPE(local_return_action)) ) {
				case MAY_BE_LONG:
				    zend_register_long_constant("EXIT_CODE", sizeof("EXIT_CODE") - 1, 
				        zval_get_long(&local_return_action), CONST_CS | CONST_PERSISTENT, 51);
				break;
				default:
					if( EXPECTED(EG(exception)) ) {
						zend_object_release(class_action);
						// efree(local_return_action);

						goto early_disturb;
						break;
					}

					php_error_docref(NULL, E_ERROR,
						"%s::%s() return type must be integer",
						Z_STR_P(action_class)->val, Z_STR_P(action_method)->val
					);	
				break;
			}
		} while(0); // Action method

		zend_object_release(class_action);
		// efree(local_return_action);
	} // If throw except

	early_disturb:

	zend_hash_destroy(dispatched);
	FREE_HASHTABLE(dispatched);
} /* run }}} */

/* {{{ terminate */
PHP_METHOD(Application, terminate)
{
	char* class_src = NULL;
	size_t class_len = 0;
	char* method_src = NULL;
	size_t method_len = 0;

	ZEND_PARSE_PARAMETERS_START(2, 2)
		Z_PARAM_STRING(class_src, class_len)
		Z_PARAM_STRING(method_src, method_len)
	ZEND_PARSE_PARAMETERS_END();

	// Exception cleared
	if( EG(exception) ) {
		zend_clear_exception();
	}

    application_object* instance = 
        ZPHEUR_ZVAL_GET_OBJECT(application_object, getThis());
    zend_object* action_resolver = instance->common->action_resolver;
    zend_object* argument_resolver = instance->common->argument_resolver;

	zend_string* class_name = zend_string_init(class_src, class_len, 0);
	zend_string* method_name = zend_string_init(method_src, method_len, 0);

	zend_object* class_action = 
		php_class_init(class_src, class_len);

	zval zv_class_name;
	zval zv_method_name;
	ZVAL_STR(&zv_class_name, class_name);
	ZVAL_STR(&zv_method_name, method_name);

	/* Call action constructor */
	do {
		zval* params_resolve = (zval*)safe_emalloc(2, sizeof(zval), 0);
		params_resolve[0] = zv_class_name;
		zend_string* _construct = zend_string_init("__construct", sizeof("__construct") - 1, 0);
		zval __zv_construct; ZVAL_STR(&__zv_construct, _construct);
		params_resolve[1] = __zv_construct;

		zval method_action_param_src;
		php_class_call_method_stacked(argument_resolver, "resolve", sizeof("resolve") - 1, 2,
			params_resolve, &method_action_param_src, 0);

		if( Z_TYPE(method_action_param_src) != IS_NULL ) {
			zend_ulong 	method_action_param_len = zend_hash_num_elements(Z_ARR(method_action_param_src));
			zval* middleware_params_resolve = (zval*)safe_emalloc(method_action_param_len, sizeof(zval), 0);

			for( int i = 0; i < method_action_param_len; i += 1) {
				middleware_params_resolve[i] = *(zend_hash_index_find(Z_ARR(method_action_param_src), i));
			}

			// Construct no return value
			php_class_call_constructor(class_action, method_action_param_len, middleware_params_resolve);	

			zend_hash_graceful_reverse_destroy(Z_ARR(method_action_param_src));
			FREE_HASHTABLE(Z_ARR(method_action_param_src));
			efree(middleware_params_resolve);
		} // when construct not found; if may can inherit from the parent (?)
	
		zend_string_release(_construct);
		efree(params_resolve);
	} while(0); // Action constructor

	if( EG(exception) ) { 
    	php_error_docref(NULL, E_ERROR,
    		"UncaughtError: Exception already thrown");
    }

	/* Call action method */
	zval local_return_action;
	do {
		zval* params_resolve = (zval*)safe_emalloc(2, sizeof(zval), 0);
		params_resolve[0] = zv_class_name;
		params_resolve[1] = zv_method_name;

		// zval* method_action_param_src =
		zval method_action_param_src;
		php_class_call_method_stacked(argument_resolver, "resolve", sizeof("resolve") - 1, 2,
			params_resolve, &method_action_param_src, 0);
		efree(params_resolve);

		zend_ulong method_action_param_len = zend_hash_num_elements(Z_ARR(method_action_param_src));
		zval* middleware_params_resolve = (zval*)safe_emalloc(method_action_param_len, sizeof(zval), 0);

		if( Z_TYPE(method_action_param_src) != IS_NULL || method_action_param_len > 0 ) {
			for( int i = 0; i < method_action_param_len; i++ ) {
				middleware_params_resolve[i] = 
				*(zend_hash_index_find(Z_ARR(method_action_param_src), i));
			}

			php_class_call_method_stacked(class_action, method_src, method_len,
				method_action_param_len, middleware_params_resolve, &local_return_action, 0);

			zend_hash_destroy(Z_ARR(method_action_param_src));
			FREE_HASHTABLE(Z_ARR(method_action_param_src));
		} else {
			php_error_docref(NULL,
        		E_CORE_ERROR, "Call to undefined method %s::%s()",
        		class_src, method_src);
		}

		efree(middleware_params_resolve);

		switch( (1 << Z_TYPE(local_return_action)) ) {
			case MAY_BE_LONG:
			    zend_register_long_constant("EXIT_CODE", sizeof("EXIT_CODE") - 1, 
			        zval_get_long(&local_return_action), CONST_CS | CONST_PERSISTENT, 51);
			break;
			default:
				if( EG(exception) ) { // throw exception
					if( Z_TYPE(local_return_action) == IS_OBJECT ) {
						zend_object_release(Z_OBJ(local_return_action));
					}
					// efree(local_return_action);
					zend_object_release(class_action);
					goto early_disturb;
				}
				php_error_docref(NULL, E_ERROR,
					"%s::%s() return type must be integer",
					class_src, method_src
				);	
			break;
		}
	} while(0); // Action method

	zend_object_release(class_action);	
	zend_string_release(class_name);
	zend_string_release(method_name);

	if( EG(exception) ) {
    	// php_error_docref(NULL, E_ERROR,
    	// 	"UncaughtError: Exception already thrown");
    	zend_clear_exception();
	}

	early_disturb: {
		// TODO exit
	}
} /* terminate }}} */

ZEND_MINIT_FUNCTION(Zpheur_Consoles_Runtime_Application)
{	
    zend_class_entry ce;

    INIT_NS_CLASS_ENTRY(ce, "Zpheur\\Consoles\\Runtime", "Application", zpheur_consoles_runtime_application_class_method);
    zpheur_consoles_runtime_application_class_entry = zend_register_internal_class(&ce);
    zpheur_consoles_runtime_application_class_entry->ce_flags |= ZEND_ACC_ALLOW_DYNAMIC_PROPERTIES;
    zpheur_consoles_runtime_application_class_entry->create_object = create_application_object;

    return SUCCESS;
}	