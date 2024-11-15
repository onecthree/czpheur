#include <php.h>
#include <ext/standard/info.h>
#include <php_zpheur.h>

#include <string.h>
#include <include/runtime.h>
#include <zpheur.h>
#include "ActionResolver_arginfo.h"


PHP_METHOD(ActionResolver, __construct)
{
	zend_uchar type;
	zend_string* class_name;
	zval* middleware = NULL;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ZVAL(middleware)
	ZEND_PARSE_PARAMETERS_END();

	type = Z_TYPE_P(middleware);
	if( !middleware || type != IS_OBJECT )
	{
		php_error_docref(NULL, E_ERROR,
			"Argument #1 ($middleware) must be of type object, %s given",
			ZTYPE_TO_STR(type)
		);
	}

	class_name = Z_OBJ_P(middleware)->ce->name;
	if(! zend_string_equals_literal(class_name, "Zpheur\\Actions\\Middleware") )
	{
		php_error_docref(NULL, E_ERROR,
			"Argument #1 ($middleware) must be instance of Zpheur\\Actions\\Middleware, %s given",
			zstr_cstr(class_name)
		);
	}

	zend_this_update_property("middleware", middleware);
}

PHP_METHOD(ActionResolver, withDefaultAction)
{
	char*  	class_src = NULL;
	size_t  class_len = 0;

	char*   method_src = NULL;
	size_t  method_len = 0;

	ZEND_PARSE_PARAMETERS_START(2, 2)
		Z_PARAM_STRING(class_src, class_len)
		Z_PARAM_STRING(method_src, method_len)
	ZEND_PARSE_PARAMETERS_END();	

	zval target_class;
	ZVAL_STRINGL(&target_class, class_src, class_len);
	zend_this_update_property("defaultActionClass", &target_class);

	zval target_method;
	ZVAL_STRINGL(&target_method, method_src, method_len);
	zend_this_update_property("defaultActionMethod", &target_method);

	RETURN_ZVAL(getThis(), 1, 0);
}

PHP_METHOD(ActionResolver, staticMiddlewareCall)
{
	bool    use_static_call = false;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_BOOL(use_static_call)
	ZEND_PARSE_PARAMETERS_END();

	if( use_static_call )
	{
		zval static_value;
		ZVAL_TRUE(&static_value);
		zend_this_update_property("staticMiddlewareCall", &static_value);
	}

	RETURN_ZVAL(getThis(), 1, 0);
}

PHP_METHOD(ActionResolver, bindIfDefaultAction)
{
	zval*  	dispatch;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ZVAL(dispatch)
	ZEND_PARSE_PARAMETERS_END();

	zval*	target_class = zend_hash_str_find(Z_ARR_P(dispatch), "class", sizeof("class") - 1);
	zval*	target_method = zend_hash_str_find(Z_ARR_P(dispatch), "method", sizeof("method") - 1);
	zval*   target_order = zend_hash_str_find(Z_ARR_P(dispatch), "order", sizeof("order") - 1);

	bool    action_resolve = !target_order || zval_get_long(target_order) < 0;

	if( action_resolve && target_class && target_method )
	{
		zval* default_class = zend_this_read_property("defaultActionClass");
		zend_hash_str_update(Z_ARR_P(dispatch), "class", sizeof("class") - 1, default_class);

		zval* default_method = zend_this_read_property("defaultActionMethod");
		zend_hash_str_update(Z_ARR_P(dispatch), "method", sizeof("method") - 1, default_method);
	}

	RETURN_ZVAL(dispatch, 1, 0);
}

PHP_METHOD(ActionResolver, process)
{
	zval*	dispatch;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ZVAL(dispatch)
	ZEND_PARSE_PARAMETERS_END();

	zval*   target_class = zend_hash_str_find(Z_ARR_P(dispatch), "class", sizeof("class") - 1);
	zval*   target_method = zend_hash_str_find(Z_ARR_P(dispatch), "method", sizeof("method") - 1);

	if( !target_class )
		php_error_docref(NULL, E_ERROR, "Target class must be string");

	if( !target_method )
		php_error_docref(NULL, E_ERROR, "Target method must be string");

	zval* static_call = zend_this_read_property("staticMiddlewareCall");
	zval* middleware = zend_this_read_property("middleware");
	zval* middleware_lists;

	/**
	 * Static call used for method target indivinduals instead classes target
	 * and run under runtime request instead cached array
	 * which is which, are used for console/cli run
	 * 
	 */
	if( zval_get_long(static_call) )
	{
		zval* params_withActionCall = (zval*)safe_emalloc(3, sizeof(zval), 0);

		ZVAL_ZVAL(&params_withActionCall[0], target_class, 1, 0);
		ZVAL_ZVAL(&params_withActionCall[1], target_method, 1, 0);
		ZVAL_TRUE(&params_withActionCall[2]);

		middleware_lists =
			php_class_call_method(Z_OBJ_P(middleware), "withActionCall", sizeof("withActionCall") - 1, 3, params_withActionCall, 0);
	}
	else
	{
		zval* params_withActionCall = (zval*)safe_emalloc(2, sizeof(zval), 0);

		ZVAL_ZVAL(&params_withActionCall[0], target_class, 1, 0);
		ZVAL_ZVAL(&params_withActionCall[1], target_method, 1, 0);

		middleware_lists =
			php_class_call_method(Z_OBJ_P(middleware), "withActionCall", sizeof("withActionCall") - 1, 2, params_withActionCall, 0);
	}

	RETURN_ZVAL(middleware_lists, 1, 0);
}

ZEND_MINIT_FUNCTION(Zpheur_Actions_Reflection_ActionResolver)
{
    zend_class_entry ce;

    INIT_NS_CLASS_ENTRY(ce, "Zpheur\\Actions\\Reflection", "ActionResolver", zpheur_actions_reflection_actionresolver_class_method);
    zpheur_actions_reflection_actionresolver_class_entry = zend_register_internal_class(&ce);
    zpheur_actions_reflection_actionresolver_class_entry->ce_flags |= ZEND_ACC_ALLOW_DYNAMIC_PROPERTIES;

    zend_declare_property_null(zpheur_actions_reflection_actionresolver_class_entry, "middleware", sizeof("middleware") - 1, ZEND_ACC_PUBLIC);
    zend_declare_property_null(zpheur_actions_reflection_actionresolver_class_entry, "defaultActionClass", sizeof("defaultActionClass") - 1, ZEND_ACC_PUBLIC);
    zend_declare_property_null(zpheur_actions_reflection_actionresolver_class_entry, "defaultActionMethod", sizeof("defaultActionMethod") - 1, ZEND_ACC_PUBLIC);
    zend_declare_property_long(zpheur_actions_reflection_actionresolver_class_entry, "staticMiddlewareCall", sizeof("staticMiddlewareCall") - 1, 0, ZEND_ACC_PUBLIC);

    return SUCCESS;
}