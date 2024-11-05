#include <php.h>
#include <ext/standard/info.h>
#include <php_zpheur.h>
#include "Zend/zend_attributes.h"
#include "ext/spl/spl_iterators.h"
#include <Zend/zend_interfaces.h>

#include <string.h>
#include <zpheur.h>
#include <include/runtime.h>
#include "ParameterBag_arginfo.h"
#include "Kernel_arginfo.h"


PHP_METHOD(HttpKernel, __construct)
{
	zval*	dispatcher;
	zval*  	action_resolver;
	zval*  	argument_resolver;

	ZEND_PARSE_PARAMETERS_START(3, 3)
		Z_PARAM_ZVAL(dispatcher)
		Z_PARAM_ZVAL(action_resolver)
		Z_PARAM_ZVAL(argument_resolver)
	ZEND_PARSE_PARAMETERS_END();

	zend_this_update_property("dispatcher", dispatcher);
	zend_this_update_property("action_resolver", action_resolver);
	zend_this_update_property("argument_resolver", argument_resolver);
}

PHP_METHOD(HttpKernel, handle)
{
	zval*   request_method;
	zval*   request_uri;

	ZEND_PARSE_PARAMETERS_START(2, 2)
		Z_PARAM_ZVAL(request_method)
		Z_PARAM_ZVAL(request_uri)
	ZEND_PARSE_PARAMETERS_END();

	zval* dispatcher 		= zend_this_read_property("dispatcher");
	zval* action_resolver 	= zend_this_read_property("action_resolver");
	zval* argument_resolver = zend_this_read_property("argument_resolver");
	zval* return_action;

	zval* params_dispatch = safe_emalloc(2, sizeof(zval), 0);
	ZVAL_ZVAL(&params_dispatch[0], request_method, 1, 0);
	ZVAL_ZVAL(&params_dispatch[1], request_uri, 1, 0);

	zval* dispatched =
		php_class_call_method(Z_OBJ_P(dispatcher), "dispatch", sizeof("dispatch") - 1, 2, params_dispatch, 0);

	zval* params_bindIfDefaultAction = safe_emalloc(1, sizeof(zval), 0);
	ZVAL_ZVAL(&params_bindIfDefaultAction[0], dispatched, 1, 0);
	
	zval* dispatched_bind =
		php_class_call_method(Z_OBJ_P(action_resolver), "bindIfDefaultAction",  sizeof("bindIfDefaultAction") - 1, 1, params_bindIfDefaultAction, 0);


	if( EG(exception) )
	{
		zend_error(E_ERROR, "failed to function call");	
	}

	// RETURN_NULL();
	// A resolver for middleware list check [block step]
	zval* params_process = safe_emalloc(1, sizeof(zval), 0);
	ZVAL_ZVAL(&params_process[0], dispatched_bind, 1, 0);
	zval* middleware_lists = 
		php_class_call_method(Z_OBJ_P(action_resolver), "process", sizeof("process") - 1, 1, params_process, 0);

	// RETURN_NULL();

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
	    			char* 	property_name_src = zpi_property->name->val + 3;
	    			size_t  property_name_len = zpi_property->name->len - 3;

	    			if( zend_string_equals_cstr(prop_name, property_name_src, property_name_len) )
	    			{
		    			zend_error(
							E_ERROR,
							"Cannot assign to protected property '%s::$%s'",
							middleware_class_name->val,
							property_name_src
						);
	    			}
	    		}

	    		if( (zpi_property->flags & ZEND_ACC_PRIVATE) )
	    		{
	    			char* 	property_name_src = zpi_property->name->val + (middleware_class_name->len + 2);
	    			size_t  property_name_len = zpi_property->name->len - (middleware_class_name->len + 2);

	    			if( zend_string_equals_cstr(prop_name, property_name_src, property_name_len) )
	    			{
		    			zend_error(
							E_ERROR,
							"Cannot assign to private property '%s::$%s'",
							middleware_class_name->val,
							property_name_src
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
			zval* params_getTargetParameter = (zval*)safe_emalloc(2, sizeof(zval), 0);
			ZVAL_STRINGL(&params_getTargetParameter[0], middleware_class_name->val, middleware_class_name->len);
			ZVAL_STRINGL(&params_getTargetParameter[1], "__construct", sizeof("__construct") - 1);

			zval* middleware_params_src = php_class_call_method(Z_OBJ_P(argument_resolver), "getTargetParameter", sizeof("getTargetParameter") - 1, 2, params_getTargetParameter, 0);

			if( Z_TYPE_P(middleware_params_src) != IS_NULL )
			{
				zend_ulong 	middleware_params_len = zend_hash_num_elements(Z_ARR_P(middleware_params_src));
				zval* 		middleware_params_resolve = (zval*)safe_emalloc(middleware_params_len, sizeof(zval), 0);

				for( int i = 0; i < middleware_params_len; i += 1)
				{
					ZVAL_ZVAL(&middleware_params_resolve[i], zend_hash_index_find(Z_ARR_P(middleware_params_src), i), 0, 0);
					ZVAL_MAKE_REF(&middleware_params_resolve[i]);
				}

				// Construct no return value
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

			// It should return zval ptr
			return_action = php_class_call_method(middleware_class, "__invoke", sizeof("__invoke") - 1, middleware_params_len, middleware_params_resolve, 0);

			// Clean-up
			zval_ptr_dtor(middleware_params_resolve);

			#define ZPHEUR_MIDDLEWARE 	"Zpheur\\Schemes\\Http\\Middleware\\Next"
			#define APP_RESPONSE 		"Zpheur\\Schemes\\Http\\Message\\Response"

			if( Z_TYPE_P(return_action) != IS_OBJECT )
				zend_error(E_ERROR, "%s::__invoke() must return " ZPHEUR_MIDDLEWARE, middleware_class_name->val);

			if( Z_OBJCE_P(return_action)->parent && zend_string_equals_cstr(Z_OBJCE_P(return_action)->parent->name, APP_RESPONSE, sizeof(APP_RESPONSE) - 1) )
				goto early_disturb;

			if( !zend_string_equals_cstr(Z_OBJCE_P(return_action)->name, ZPHEUR_MIDDLEWARE, sizeof(ZPHEUR_MIDDLEWARE) - 1) )
				zend_error(E_ERROR, "%s::__invoke() must return " ZPHEUR_MIDDLEWARE, middleware_class_name->val);
		}
	}
	ZEND_HASH_FOREACH_END();

	zval_ptr_dtor(middleware_lists);

	zval*   		action_class 	= zend_hash_str_find(Z_ARR_P(dispatched_bind), "class", sizeof("class") - 1);
	zval*   		action_method 	= zend_hash_str_find(Z_ARR_P(dispatched_bind), "method", sizeof("method") - 1);

	zend_string* 	_action_class 	= zval_get_string(action_class);
	zend_object*	class_action 	= php_class_init(_action_class->val, _action_class->len);

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

			// Construct no return value
			php_class_call_constructor(class_action, method_action_param_len, middleware_params_resolve);	
		}
		else
			zval_ptr_dtor(method_action_param_src);
		zval_ptr_dtor(params_getTargetParameter);
	}

	if( EG(exception) )
    { 
        goto early_disturb;
    }

	/* Call action method */
	{
		/* Set segments from uri to container */
		zval* segments = zend_hash_str_find(Z_ARR_P(dispatched), "segments", sizeof("segments") - 1);

		zval* params_withTargetSegments = (zval*)safe_emalloc(1, sizeof(zval), 0);
		ZVAL_ZVAL(&params_withTargetSegments[0], segments, 1, 0);

		/* Call segment resolver */
		php_class_call_method(Z_OBJ_P(argument_resolver), "withTargetSegments", sizeof("withTargetSegments") - 1, 1, params_withTargetSegments, 0);

		zval* params_getTargetParameter = (zval*)safe_emalloc(2, sizeof(zval), 0);
		ZVAL_ZVAL(&params_getTargetParameter[0], action_class, 1, 0);
		ZVAL_ZVAL(&params_getTargetParameter[1], action_method, 1, 0);


		zval* method_action_param_src = php_class_call_method(Z_OBJ_P(argument_resolver), "getTargetParameter", sizeof("getTargetParameter") - 1, 2, params_getTargetParameter, 0);

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
		return_action =
			php_class_call_method(class_action, action_call->val, action_call->len, method_action_param_len, middleware_params_resolve, 0);
	}

	early_disturb:	
		  RETURN_ZVAL(return_action, 1, 0);
}

PHP_METHOD(HttpKernel, terminate)
{
	zval*	 error_exception = NULL;

	char*    class_name_src = NULL;
	size_t   class_name_len = 0;

	char*    class_method_src = NULL;
	size_t   class_method_len = 0;

	ZEND_PARSE_PARAMETERS_START(3, 3)
		Z_PARAM_ZVAL(error_exception)
		Z_PARAM_STRING(class_name_src, class_name_len)
		Z_PARAM_STRING(class_method_src, class_method_len)
	ZEND_PARSE_PARAMETERS_END();

	/* ErrorException thrown from action space */
	// zval*	request = zend_target_read_property_ex(error_exception, "request");
	// zval*	response = zend_target_read_property_ex(error_exception, "response");

	zval* 			argument_resolver = zend_this_read_property("argument_resolver");
	zend_object*	class_action = php_class_init(class_name_src, class_name_len);
	zval* 			return_action;

	/* Call action constructor */
	{
		zval* params_getTargetParameter = (zval*)safe_emalloc(2, sizeof(zval), 0);
		ZVAL_STRINGL(&params_getTargetParameter[0], class_name_src, class_name_len);
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

			// Construct no return value
			php_class_call_constructor(class_action, method_action_param_len, middleware_params_resolve);	
		}
		else
			zval_ptr_dtor(method_action_param_src);
		zval_ptr_dtor(params_getTargetParameter);
	}

	if( EG(exception) )
    { 
        goto early_disturb;
    }

	/* Call action method */
	{
		/* Set segments from uri to container */
		// zval* segments = zend_hash_str_find(Z_ARR_P(dispatched), "segments", sizeof("segments") - 1);
		zval segments;
		array_init(&segments);

		zval* params_withTargetSegments = (zval*)safe_emalloc(1, sizeof(zval), 0);
		ZVAL_ZVAL(&params_withTargetSegments[0], &segments, 1, 0);

		/* Call segment resolver */
		php_class_call_method(Z_OBJ_P(argument_resolver), "withTargetSegments", sizeof("withTargetSegments") - 1, 1, params_withTargetSegments, 0);

		zval* params_getTargetParameter = (zval*)safe_emalloc(2, sizeof(zval), 0);
		ZVAL_STRINGL(&params_getTargetParameter[0], class_name_src, class_name_len);
		ZVAL_STRINGL(&params_getTargetParameter[1], class_method_src, class_method_len);


		zval* method_action_param_src = php_class_call_method(Z_OBJ_P(argument_resolver), "getTargetParameter", sizeof("getTargetParameter") - 1, 2, params_getTargetParameter, 0);

		zend_ulong 		method_action_param_len = zend_hash_num_elements(Z_ARR_P(method_action_param_src));
		zval* 			middleware_params_resolve = (zval*)safe_emalloc(method_action_param_len, sizeof(zval), 0);

		for( int i = 0; i < method_action_param_len; i += 1)
		{
			ZVAL_ZVAL(&middleware_params_resolve[i], zend_hash_index_find(Z_ARR_P(method_action_param_src), i), 0, 0);
			ZVAL_MAKE_REF(&middleware_params_resolve[i]);
		}

		// clean-up
		zval_ptr_dtor(params_getTargetParameter);

		// it should return zval ptr
		return_action =
			php_class_call_method(class_action, class_method_src, class_method_len, method_action_param_len, middleware_params_resolve, 0);
	}

	early_disturb:	
		  RETURN_ZVAL(return_action, 1, 0);
}

ZEND_MINIT_FUNCTION(Zpheur_Schemes_Http_Foundation_Kernel)
{
    zend_class_entry ce;

    INIT_NS_CLASS_ENTRY(ce, "Zpheur\\Schemes\\Http\\Foundation", "Kernel", zpheur_schemes_http_foundation_kernel_class_method);
    zpheur_schemes_http_foundation_kernel_class_entry = zend_register_internal_class(&ce);
    zpheur_schemes_http_foundation_kernel_class_entry->ce_flags |= ZEND_ACC_ALLOW_DYNAMIC_PROPERTIES;

    zend_declare_property_null(zpheur_schemes_http_foundation_kernel_class_entry, "dispatcher", sizeof("dispatcher") - 1, ZEND_ACC_PUBLIC);
    zend_declare_property_null(zpheur_schemes_http_foundation_kernel_class_entry, "action_resolver", sizeof("action_resolver") - 1, ZEND_ACC_PUBLIC);
    zend_declare_property_null(zpheur_schemes_http_foundation_kernel_class_entry, "argument_resolver", sizeof("argument_resolver") - 1, ZEND_ACC_PUBLIC);

    return SUCCESS;
}
