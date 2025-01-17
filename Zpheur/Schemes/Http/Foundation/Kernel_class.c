#include <php.h>
#include <ext/standard/info.h>
#include <php_zpheur.h>
#include "Zend/zend_attributes.h"
#include "ext/spl/spl_iterators.h"
#include <Zend/zend_interfaces.h>

#include <string.h>
#include <zpheur.h>
#include <include/runtime.h>
#include <Zpheur/Schemes/Http/Message/Response_arginfo.h>
#include <Zpheur/Actions/Reflection/ArgumentResolver_arginfo.h>
#include <Zpheur/Schemes/Http/Message/Response/ResponseInterface_arginfo.h>
#include "ParameterBag_arginfo.h"
#include "Kernel_arginfo.h"

#define L_HTTP_RESPONSE_SRC "Zpheur\\Schemes\\Http\\Message\\Response"
#define L_HTTP_RESPONSE_LEN sizeof("Zpheur\\Schemes\\Http\\Message\\Response") - 1
#define L_HTTP_RESPONSEINTERFACE_SRC "Zpheur\\Schemes\\Http\\Message\\Response\\ResponseInterface"
#define L_HTTP_RESPONSEINTERFACE_LEN sizeof("Zpheur\\Schemes\\Http\\Message\\Response\\ResponseInterface") - 1

zend_object_handlers kernel_object_handlers;

typedef struct _kernel_common_t
{
    zend_object* dispatcher; // don't need release
    zend_object* action_resolver; // don't need release
    zend_object* argument_resolver; // don't need release
} kernel_common_t;

typedef struct _kernel_object
{
    kernel_common_t* common;
    zend_object std;
} kernel_object;

void free_kernel_object(zend_object *object)
{
    kernel_object* instance = ZPHEUR_GET_OBJECT(kernel_object, object);

    zend_object_std_dtor(&instance->std);
    if( instance->common )
    {
        efree(instance->common);       
    }
}

zend_object* create_kernel_object( zend_class_entry* ce )
{
    kernel_object* object = 
        ecalloc(1, sizeof(kernel_object) + zend_object_properties_size(ce));

    zend_object_std_init(&object->std, ce);
    object_properties_init(&object->std, ce);

    memcpy(&kernel_object_handlers, zend_get_std_object_handlers(), sizeof(kernel_object_handlers));
    kernel_object_handlers.offset = XtOffsetOf(kernel_object, std);
    kernel_object_handlers.free_obj = free_kernel_object;
    object->std.handlers = &kernel_object_handlers;

    object->common = emalloc(sizeof(kernel_common_t));
    object->common->dispatcher = NULL;
    object->common->action_resolver = NULL;
    object->common->argument_resolver = NULL;

    return &object->std;
}

PHP_METHOD(HttpKernel, __construct)
{
	zval*	dispatcher;
	zval*  	action_resolver;
	zval*  	argument_resolver;
	zval*  	c_middleware;

	ZEND_PARSE_PARAMETERS_START(3, 3)
		Z_PARAM_ZVAL(dispatcher)
		Z_PARAM_ZVAL(action_resolver)
		Z_PARAM_ZVAL(argument_resolver)
	ZEND_PARSE_PARAMETERS_END();

    kernel_object* instance = 
        ZPHEUR_ZVAL_GET_OBJECT(kernel_object, getThis());
    instance->common->dispatcher = Z_OBJ_P(dispatcher);
    instance->common->action_resolver = Z_OBJ_P(action_resolver);
    instance->common->argument_resolver = Z_OBJ_P(argument_resolver);
}

/**
 * While return a stringable buffer from latest action,
 * it keep to always free'd copied \App\Service\Http\Message\Response.
 * In another case it is actually can return the copied itself
 * as object, but remain a memory leak.
 * TODO need alternativly to return object as memory safe
 */
/* {{{ handle */
PHP_METHOD(HttpKernel, handle)
{
	zval*   request_method;
	zval*   request_uri;

	ZEND_PARSE_PARAMETERS_START(2, 2)
		Z_PARAM_ZVAL(request_method)
		Z_PARAM_ZVAL(request_uri)
	ZEND_PARSE_PARAMETERS_END();

    kernel_object* instance = 
        ZPHEUR_ZVAL_GET_OBJECT(kernel_object, getThis());
    zend_object* dispatcher = instance->common->dispatcher;
    zend_object* action_resolver = instance->common->action_resolver;
    zend_object* argument_resolver = instance->common->argument_resolver;
	zval return_action;
	bool return_set = false;

	zval* params_dispatch = safe_emalloc(2, sizeof(zval), 0);
	params_dispatch[0] = *request_method;
	params_dispatch[1] = *request_uri;

	zval dispatched;
	php_class_call_method_stacked(dispatcher, "dispatch", sizeof("dispatch") - 1, 2,
		params_dispatch, &dispatched, 0);
	efree(params_dispatch);

	zval* params_bindIfDefaultAction = safe_emalloc(1, sizeof(zval), 0);
	params_bindIfDefaultAction[0] = dispatched;
	
	zval dispatched_bind;
	php_class_call_method_stacked(action_resolver, "bindIfDefaultAction",  sizeof("bindIfDefaultAction") - 1, 1,
		params_bindIfDefaultAction, &dispatched_bind, 0);
	efree(params_bindIfDefaultAction);

	if( EG(exception) ) {
		zend_error(E_ERROR, "failed to function call");	
	}

	// A resolver for middleware list check [block step]
	zval* params_process = safe_emalloc(1, sizeof(zval), 0);
	params_process[0] = dispatched_bind;

	zval before_middlewares;
	php_class_call_method_stacked(action_resolver, "beforeMiddlewaresResolve", sizeof("beforeMiddlewaresResolve") - 1,
		1, params_process, &before_middlewares, 0);
	efree(params_process);

	// Iteration for middleware list arg parse [block step]
	ZEND_HASH_FOREACH_VAL(Z_ARR(before_middlewares), zval* middleware)
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
			php_class_call_method_stacked(argument_resolver, "resolve", sizeof("resolve") - 1,
				2, params_resolve, &middleware_params_src, 0);

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

				php_class_call_method_stacked(middleware_class, "process", sizeof("process") - 1,
					middleware_params_len, middleware_params_resolve, &return_action, 0);
				return_set = true;

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

		zend_object* object;
		switch( (1 << Z_TYPE(return_action)) ) {
			case MAY_BE_NULL: break; // Ignore null return or void
			case MAY_BE_OBJECT:
				object = Z_OBJ(return_action);
				if( zend_string_equals_cstr(object->ce->name, L_HTTP_RESPONSE_SRC, L_HTTP_RESPONSE_LEN) ||
					(object->ce->parent && zend_string_equals_cstr(object->ce->parent->name,
						L_HTTP_RESPONSE_SRC, L_HTTP_RESPONSE_LEN)) ) {
					zend_object_release(middleware_class);
					zend_string_release(middleware_class_name);

					zend_hash_destroy(Z_ARR(before_middlewares));
					FREE_HASHTABLE(Z_ARR(before_middlewares));

					goto early_after_middleware;
				}
			default:
				php_error_docref(NULL, E_ERROR,
					"%s::process() return type must be instance of %s or void",
					middleware_class_name->val, L_HTTP_RESPONSE_SRC
				);	
			break;
		}

		zend_object_release(middleware_class);
		zend_string_release(middleware_class_name);
	}
	ZEND_HASH_FOREACH_END(); // Before middleware

	zend_hash_destroy(Z_ARR(before_middlewares));
	FREE_HASHTABLE(Z_ARR(before_middlewares));

	zval* action_class = 
		zend_hash_str_find(Z_ARR(dispatched_bind), "class", sizeof("class") - 1);
	zval* action_method = 
		zend_hash_str_find(Z_ARR(dispatched_bind), "method", sizeof("method") - 1);

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
	do {
		/* Set segments from uri to container */
		zval* params_addSegments = (zval*)safe_emalloc(1, sizeof(zval), 0);
		params_addSegments[0] = dispatched;

		/* Call segment resolver */
		zval ret_target_segment;
		php_class_call_method_stacked(argument_resolver, "addSegmentsToServicesContainer",
			sizeof("addSegmentsToServicesContainer") - 1, 1, params_addSegments, &ret_target_segment, 0);
		efree(params_addSegments);

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

			php_class_call_method_stacked(class_action, action_call->val, action_call->len,
				method_action_param_len, middleware_params_resolve, &return_action, 0);
			return_set = true;

			zend_hash_destroy(Z_ARR(method_action_param_src));
			FREE_HASHTABLE(Z_ARR(method_action_param_src));
		} else {
			php_error_docref(NULL,
        		E_CORE_ERROR, "Call to undefined method %s::%s()",
        		Z_STR_P(action_class)->val, Z_STR_P(action_method)->val);
		}

		efree(middleware_params_resolve);
		zend_string_release(action_call);

		zend_object* object;
		switch( (1 << Z_TYPE(return_action)) ) {
			case MAY_BE_OBJECT:
				object = Z_OBJ(return_action);
				if(! zend_class_implements_interface(object->ce, 
				zpheur_schemes_http_message_response_responseinterface_class_entry) ) {
					php_error_docref(NULL, E_ERROR,
						"%s must implement the interface " L_HTTP_RESPONSEINTERFACE_SRC,
						object->ce->name->val
					);
				}

				if( !zend_string_equals_cstr(object->ce->name, L_HTTP_RESPONSE_SRC, L_HTTP_RESPONSE_LEN) &&
				!(object->ce->parent && zend_string_equals_cstr(object->ce->parent->name, L_HTTP_RESPONSE_SRC,
				L_HTTP_RESPONSE_LEN)) ){
					php_error_docref(NULL, E_ERROR,
						"%s::%s() return type must be instance of %s",
						Z_STR_P(action_class)->val, Z_STR_P(action_method)->val, L_HTTP_RESPONSE_SRC
					);	
					break;
				}
			break;
			default:
				if( EG(exception) ) {
					if( Z_TYPE(return_action) == IS_OBJECT )
						zend_object_release(Z_OBJ(return_action));
					return_set = false;
					zend_object_release(class_action);
					goto early_disturb;
				}
			break;
		}
	} while(0); // Action method

	zend_object_release(class_action);

	if( EG(exception) ) { 
        goto early_disturb;
    }

    early_after_middleware: {
		zval after_middlewares;
		php_class_call_method_stacked(action_resolver, "getAfterRequestMiddlewares",
			sizeof("getAfterRequestMiddlewares") - 1, 0, NULL, &after_middlewares, 0);

		// Iteration for middleware list arg parse [block step]
		ZEND_HASH_FOREACH_VAL(Z_ARR(after_middlewares), zval* middleware)
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

		    		if( Z_PHP_ACC_PUBLIC(zpi_property->flags) && is_property ) {
						zend_update_property(middleware_class->ce, middleware_class, prop_name->val, prop_name->len, prop_value);
						goto skip_error_2;
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

				skip_error_2: {}
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
				php_class_call_method_stacked(argument_resolver, "resolve", sizeof("resolve") - 1,
					2, params_resolve, &middleware_params_src, 0);

				if( Z_TYPE(middleware_params_src) != IS_NULL ) {
					zend_ulong 	middleware_params_len = zend_hash_num_elements(Z_ARR(middleware_params_src));
					zval* 		middleware_params_resolve = (zval*)safe_emalloc(middleware_params_len, sizeof(zval), 0);

					for( int i = 0; i < middleware_params_len; i += 1) {
						middleware_params_resolve[i] = *(zend_hash_index_find(Z_ARR(middleware_params_src), i));
					}

					php_class_call_constructor(middleware_class, middleware_params_len, middleware_params_resolve);
					efree(middleware_params_resolve);

					zend_hash_graceful_reverse_destroy(Z_ARR(middleware_params_src));
					FREE_HASHTABLE(Z_ARR(middleware_params_src));
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
				efree(params_resolve);
			
				if( Z_TYPE(middleware_params_src) != IS_NULL ) {
					zend_ulong 	middleware_params_len = zend_hash_num_elements(Z_ARR(middleware_params_src));
					zval* middleware_params_resolve = (zval*)safe_emalloc(middleware_params_len, sizeof(zval), 0);

					for( int i = 0; i < middleware_params_len; i += 1) {
						middleware_params_resolve[i] = *(zend_hash_index_find(Z_ARR(middleware_params_src), i));
					}

					// It should return zval ptr
					php_class_call_method_stacked(middleware_class, "process", sizeof("process") - 1,
						middleware_params_len, middleware_params_resolve, &local_return_action, 0);

					zend_hash_destroy(Z_ARR(middleware_params_src));
					FREE_HASHTABLE(Z_ARR(middleware_params_src));
					efree(middleware_params_resolve);
				} else {
					php_error_docref(NULL,
	            		E_CORE_ERROR, "Call to undefined method %s::process()",
	            		middleware_class_name->val);
				}

				zend_string_release(process_str);
			} while(0);

			zend_object_release(middleware_class);
			zend_string_release(middleware_class_name);

			if(! ((1 << Z_TYPE(local_return_action)) & MAY_BE_NULL) ) {
				php_error_docref(NULL,
	        		E_ERROR, "%s::process() expects a non return value (void), %s returned",
	        		middleware_class_name->val, ZTYPE_TO_STR(Z_TYPE(local_return_action)));
			}
		}
		ZEND_HASH_FOREACH_END();

		zend_hash_graceful_reverse_destroy(Z_ARR(after_middlewares));
		FREE_HASHTABLE(Z_ARR(after_middlewares));
	}

	early_disturb:
	zend_hash_destroy(Z_ARR(dispatched_bind));
	FREE_HASHTABLE(Z_ARR(dispatched_bind));
	zend_hash_destroy(Z_ARR(dispatched));
	FREE_HASHTABLE(Z_ARR(dispatched));

	if( return_set ) {
		RETURN_ZVAL(&return_action, 0, 0);
	}
	RETURN_STRINGL("hehe", sizeof("hehe") - 1);
} /* handle }}} */

/* {{{ terminate */
PHP_METHOD(HttpKernel, terminate)
{
	char* class_src = NULL;
	size_t class_len = 0;
	char* method_src = NULL;
	size_t method_len = 0;

	ZEND_PARSE_PARAMETERS_START(2, 2)
		Z_PARAM_STRING(class_src, class_len)
		Z_PARAM_STRING(method_src, method_len)
	ZEND_PARSE_PARAMETERS_END();

	if( EG(exception) ) {
		zend_clear_exception();
	}

    kernel_object* instance = 
        ZPHEUR_ZVAL_GET_OBJECT(kernel_object, getThis());
    zend_object* action_resolver = instance->common->action_resolver;
    zend_object* argument_resolver = instance->common->argument_resolver;
	zval return_action;

	zend_string* class_name = zend_string_init(class_src, class_len, 0);
	zend_string* method_name = zend_string_init(method_src, method_len, 0);
	zend_object* class_action = php_class_init(class_src, class_len);

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
		zend_clear_exception();
	}

	/* Call action method */
	do {
		zval* params_resolve = (zval*)safe_emalloc(2, sizeof(zval), 0);
		params_resolve[0] = zv_class_name;
		params_resolve[1] = zv_method_name;

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
				method_action_param_len, middleware_params_resolve, &return_action, 0);

			zend_hash_destroy(Z_ARR(method_action_param_src));
			FREE_HASHTABLE(Z_ARR(method_action_param_src));
		} else {
			php_error_docref(NULL,
        		E_CORE_ERROR, "Call to undefined method %s::%s()",
        		class_src, method_src);
		}

		efree(middleware_params_resolve);

		zend_object* object;
		switch( (1 << Z_TYPE(return_action)) )
		{
			case MAY_BE_OBJECT:
				object = Z_OBJ(return_action);

				if(! zend_class_implements_interface(object->ce, 
				zpheur_schemes_http_message_response_responseinterface_class_entry) ) {
					php_error_docref(NULL, E_ERROR,
						"%s must implement the interface " L_HTTP_RESPONSEINTERFACE_SRC,
						object->ce->name->val
					);
				}
				
				if( zend_string_equals_cstr(object->ce->name, L_HTTP_RESPONSE_SRC, L_HTTP_RESPONSE_LEN) ||
				(object->ce->parent && zend_string_equals_cstr(object->ce->parent->name,
				L_HTTP_RESPONSE_SRC, L_HTTP_RESPONSE_LEN)) ) {
					break;
				}
			default:
				php_error_docref(NULL, E_ERROR,
					"%s::%s() return type must be instance of %s",
					class_src, method_src, L_HTTP_RESPONSE_SRC
				);	
			break;
		}
	} while(0); // Action method

	if( EG(exception) ) {
    	zend_clear_exception();
	}

	zend_object_release(class_action);	
	zend_string_release(class_name);
	zend_string_release(method_name);

	zval after_middlewares;
	php_class_call_method_stacked(action_resolver, "getAfterRequestMiddlewares",
		sizeof("getAfterRequestMiddlewares") - 1, 0, NULL, &after_middlewares, 0);

	// Iteration for middleware list arg parse [block step]
	ZEND_HASH_FOREACH_VAL(Z_ARR(after_middlewares), zval* middleware)
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
					zend_update_property(middleware_class->ce, middleware_class,
						prop_name->val, prop_name->len, prop_value);
					goto skip_error_2;
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

					if( Z_PHP_ACC_PRIVATE(zpi_property->flags) )
		    		{
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

			skip_error_2: {}
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
			php_class_call_method_stacked(argument_resolver, "resolve", sizeof("resolve") - 1, 2,
				params_resolve, &middleware_params_src, 0);

			if( Z_TYPE(middleware_params_src) != IS_NULL ) {
				zend_ulong middleware_params_len = zend_hash_num_elements(Z_ARR(middleware_params_src));
				zval* middleware_params_resolve = (zval*)safe_emalloc(middleware_params_len, sizeof(zval), 0);

				for( int i = 0; i < middleware_params_len; i += 1) {
					middleware_params_resolve[i] = *(zend_hash_index_find(Z_ARR(middleware_params_src), i));
				}

				php_class_call_constructor(middleware_class, middleware_params_len, middleware_params_resolve);
				efree(middleware_params_resolve);

				zend_hash_graceful_reverse_destroy(Z_ARR(middleware_params_src));
				FREE_HASHTABLE(Z_ARR(middleware_params_src));
			} // when construct not found; if may can inherit from the parent (?)

			zend_string_release(_zs_construct);
			efree(params_resolve);
		} // Call action constructor (middleware)

		if( EG(exception) ) {
	    	zend_clear_exception();
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
			efree(params_resolve);
		
			if( Z_TYPE(middleware_params_src) != IS_NULL ) {
				zend_ulong 	middleware_params_len = zend_hash_num_elements(Z_ARR(middleware_params_src));
				zval* middleware_params_resolve = (zval*)safe_emalloc(middleware_params_len, sizeof(zval), 0);

				for( int i = 0; i < middleware_params_len; i += 1) {
					middleware_params_resolve[i] = *(zend_hash_index_find(Z_ARR(middleware_params_src), i));
				}

				// It should return zval ptr
				php_class_call_method_stacked(middleware_class, "process", sizeof("process") - 1,
					middleware_params_len, middleware_params_resolve, &local_return_action, 0);

				zend_hash_destroy(Z_ARR(middleware_params_src));
				FREE_HASHTABLE(Z_ARR(middleware_params_src));
				efree(middleware_params_resolve);
			} else {
				php_error_docref(NULL,
            		E_CORE_ERROR, "Call to undefined method %s::process()",
            		middleware_class_name->val);
			}

			zend_string_release(process_str);
		} while(0); // Call action method (middleware)

		if( EG(exception) ) {
	    	zend_clear_exception();
		}

		zend_object_release(middleware_class);
		zend_string_release(middleware_class_name);

		if(! ((1 << Z_TYPE(local_return_action)) & MAY_BE_NULL) ) {
			php_error_docref(NULL,
        		E_ERROR, "%s::process() expects a non return value (void), %s returned",
        		middleware_class_name->val, ZTYPE_TO_STR(Z_TYPE(local_return_action))
        	);
		}
	}
	ZEND_HASH_FOREACH_END();
	
	zend_hash_graceful_reverse_destroy(Z_ARR(after_middlewares));
	FREE_HASHTABLE(Z_ARR(after_middlewares));

	RETURN_ZVAL(&return_action, 0, 0);
} /* terminate }}} */

ZEND_MINIT_FUNCTION(Zpheur_Schemes_Http_Foundation_Kernel)
{
    zend_class_entry ce;

    INIT_NS_CLASS_ENTRY(ce, "Zpheur\\Schemes\\Http\\Foundation", "Kernel", zpheur_schemes_http_foundation_kernel_class_method);
    zpheur_schemes_http_foundation_kernel_class_entry = zend_register_internal_class(&ce);
    zpheur_schemes_http_foundation_kernel_class_entry->ce_flags |= ZEND_ACC_ALLOW_DYNAMIC_PROPERTIES;
    zpheur_schemes_http_foundation_kernel_class_entry->create_object = create_kernel_object;

    return SUCCESS;
}
