#include <php.h>
#include <ext/standard/info.h>
#include <php_zpheur.h>

#include <string.h>
#include <include/runtime.h>
#include <include/onecstr.h>
#include <zpheur.h>
#include <Zpheur/Actions/Middleware_arginfo.h>
#include "ActionResolver_arginfo.h"

zend_object_handlers action_resolver_object_handlers;

typedef struct _action_resolver_common_t
{
	zend_object* middleware; // doesn't need release
	onec_string* default_action_class_name;
	onec_string* default_action_method_name;
} action_resolver_common_t;

typedef struct _action_resolver_object
{
    action_resolver_common_t* common;
    zend_object std;
} action_resolver_object;

void free_action_resolver_object(zend_object* object)
{
    action_resolver_object* instance = ZPHEUR_GET_OBJECT(action_resolver_object, object);

    zend_object_std_dtor(&instance->std);
    if( instance->common )
    {
    	if( instance->common->default_action_class_name )
    		onec_string_release(instance->common->default_action_class_name);

    	if( instance->common->default_action_method_name )
    		onec_string_release(instance->common->default_action_method_name);

        efree(instance->common);       
    }
}

zend_object* create_action_resolver_object( zend_class_entry* ce )
{
    action_resolver_object* object = 
        ecalloc(1, sizeof(action_resolver_object) + zend_object_properties_size(ce));

    zend_object_std_init(&object->std, ce);
    object_properties_init(&object->std, ce);

    memcpy(&action_resolver_object_handlers, zend_get_std_object_handlers(), sizeof(action_resolver_object_handlers));
    action_resolver_object_handlers.offset = XtOffsetOf(action_resolver_object, std);
    action_resolver_object_handlers.free_obj = free_action_resolver_object;
    object->std.handlers = &action_resolver_object_handlers;

    object->common = ecalloc(1, sizeof(action_resolver_common_t));
    object->common->middleware = NULL;
    object->common->default_action_class_name = NULL;
    object->common->default_action_method_name = NULL;

    return &object->std;
}

PHP_METHOD(ActionResolver, __construct)
{
	zval* middleware = NULL;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_OBJECT_OF_CLASS(middleware, zpheur_actions_middleware_class_entry)
	ZEND_PARSE_PARAMETERS_END();

    action_resolver_object* instance = 
        ZPHEUR_ZVAL_GET_OBJECT(action_resolver_object, getThis());

   	instance->common->middleware = Z_OBJ_P(middleware);
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

    action_resolver_object* instance = 
        ZPHEUR_ZVAL_GET_OBJECT(action_resolver_object, getThis());

    instance->common->default_action_class_name = onec_string_initd(class_src, class_len);
    instance->common->default_action_method_name = onec_string_initd(method_src, method_len);

	RETURN_ZVAL(getThis(), 1, 0);
}

PHP_METHOD(ActionResolver, getAfterRequestMiddlewares)
{
	ZEND_PARSE_PARAMETERS_NONE();

    action_resolver_object* instance = 
        ZPHEUR_ZVAL_GET_OBJECT(action_resolver_object, getThis());

    zval* params_listAllMiddlewares = safe_emalloc(1, sizeof(zval), 0);
    zend_string* middleware_type = zend_string_init("after", sizeof("after") - 1, 0);
    zval zv_middleware_type; ZVAL_STR(&zv_middleware_type, middleware_type);
    params_listAllMiddlewares[0] = zv_middleware_type;

    zval* after_middlewares = php_class_call_method(
    	instance->common->middleware,
    	"listAllMiddlewares", sizeof("listAllMiddlewares") - 1,
    	1, params_listAllMiddlewares, 0);

    zend_string_release(middleware_type);
    efree(params_listAllMiddlewares);

    HashTable* return_table;
    ALLOC_HASHTABLE(return_table);
    zend_hash_init(return_table, 0, NULL, ZVAL_PTR_DTOR, 0);

    zend_hash_copy(return_table, Z_ARR_P(after_middlewares), zval_add_ref);

    zend_hash_destroy(Z_ARR_P(after_middlewares));
    FREE_HASHTABLE(Z_ARR_P(after_middlewares));
    efree(after_middlewares);

    RETURN_ARR(return_table);
}

PHP_METHOD(ActionResolver, beforeMiddlewaresResolve)
{
	HashTable* dispatch;
	bool is_static = false;

	ZEND_PARSE_PARAMETERS_START(1, 2);
		Z_PARAM_ARRAY_HT(dispatch);
		Z_PARAM_OPTIONAL
		Z_PARAM_BOOL(is_static)
	ZEND_PARSE_PARAMETERS_END();

    action_resolver_object* instance = 
        ZPHEUR_ZVAL_GET_OBJECT(action_resolver_object, getThis());

	zval* params_resolve = safe_emalloc(2, sizeof(zval), 0);
	params_resolve[0] = *zend_hash_str_find(dispatch, "class", sizeof("class") - 1);
	params_resolve[1] = *zend_hash_str_find(dispatch, "method", sizeof("method") - 1);

	zval* resolved_middlewares;
	if(! is_static )
	{
		resolved_middlewares = 
			php_class_call_method(instance->common->middleware,
				"resolve", sizeof("resolve") - 1, 2, params_resolve, 0);
	}
	else
	{
		resolved_middlewares = 
			php_class_call_method(instance->common->middleware, 
				"scanAndResolve", sizeof("scanAndResolve") - 1, 2, params_resolve, 0);
	}

    efree(params_resolve);

    HashTable* return_table;
    ALLOC_HASHTABLE(return_table);
    zend_hash_init(return_table, 0, NULL, ZVAL_PTR_DTOR, 0);

    zend_hash_copy(return_table, Z_ARR_P(resolved_middlewares), zval_add_ref);

    zend_hash_destroy(Z_ARR_P(resolved_middlewares));
    FREE_HASHTABLE(Z_ARR_P(resolved_middlewares));
    efree(resolved_middlewares);

    RETURN_ARR(return_table);
}

PHP_METHOD(ActionResolver, bindIfDefaultAction)
{
	HashTable* dispatch;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ARRAY_HT(dispatch)
	ZEND_PARSE_PARAMETERS_END();

    action_resolver_object* instance = 
        ZPHEUR_ZVAL_GET_OBJECT(action_resolver_object, getThis());

	// zval* target_class = zend_hash_str_find(dispatch, "class", sizeof("class") - 1);
	// zval* target_method = zend_hash_str_find(dispatch, "method", sizeof("method") - 1);
	zval* target_order = zend_hash_str_find(dispatch, "order", sizeof("order") - 1);

	bool action_resolve = !EXPECTED(target_order) || zval_get_long(target_order) < 0;

	// if( action_resolve && target_class && target_method )
	if( action_resolve )
	{
		onec_string* _default_class = instance->common->default_action_class_name;
		zend_string* default_class = zend_string_init(
			_default_class->val, _default_class->len, 0);

		onec_string* _default_method = instance->common->default_action_method_name;
		zend_string* default_method = zend_string_init(
			_default_method->val, _default_method->len, 0);

		zval zv_default_class;
		ZVAL_STR(&zv_default_class, default_class);

		zval zv_default_method;
		ZVAL_STR(&zv_default_method, default_method);

		Z_ADDREF_P(&zv_default_class);
		Z_ADDREF_P(&zv_default_method);

		zend_hash_str_update(dispatch, "class", sizeof("class") - 1, &zv_default_class);
		zend_hash_str_update(dispatch, "method", sizeof("method") - 1, &zv_default_method);

		zend_string_release(default_class);
		zend_string_release(default_method);
	}

	zval return_table;
	array_init(&return_table);

	zend_hash_copy(Z_ARR(return_table), dispatch, zval_add_ref);
	RETURN_ZVAL(&return_table, 1, 0);
}

PHP_METHOD(ActionResolver, __destruct)
{
	ZEND_PARSE_PARAMETERS_NONE();
}

ZEND_MINIT_FUNCTION(Zpheur_Actions_Reflection_ActionResolver)
{
    zend_class_entry ce;

    INIT_NS_CLASS_ENTRY(ce, "Zpheur\\Actions\\Reflection", "ActionResolver", zpheur_actions_reflection_actionresolver_class_method);
    zpheur_actions_reflection_actionresolver_class_entry = zend_register_internal_class(&ce);
    zpheur_actions_reflection_actionresolver_class_entry->ce_flags |= ZEND_ACC_ALLOW_DYNAMIC_PROPERTIES;
    zpheur_actions_reflection_actionresolver_class_entry->create_object = create_action_resolver_object;

    zend_declare_property_null(zpheur_actions_reflection_actionresolver_class_entry, "defaultActionClass", sizeof("defaultActionClass") - 1, ZEND_ACC_PUBLIC);
    zend_declare_property_null(zpheur_actions_reflection_actionresolver_class_entry, "defaultActionMethod", sizeof("defaultActionMethod") - 1, ZEND_ACC_PUBLIC);
    zend_declare_property_long(zpheur_actions_reflection_actionresolver_class_entry, "staticMiddlewareCall", sizeof("staticMiddlewareCall") - 1, 0, ZEND_ACC_PUBLIC);

    return SUCCESS;
}