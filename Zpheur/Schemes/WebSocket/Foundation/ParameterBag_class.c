#include <php.h>
#include <ext/standard/info.h>
#include <php_zpheur.h>

#include <string.h>
#include <zpheur.h>
#include <include/runtime.h>
#include "ParameterBag_arginfo.h"


void free_parameter_bag_object(zend_object* object)
{
    parameter_bag_object* instance = ZPHEUR_GET_OBJECT(parameter_bag_object, object);

    zend_object_std_dtor(&instance->std);
    if( instance->common )
    {
    	if( instance->common->parameters != NULL )
    	{
    		zend_hash_destroy(instance->common->parameters);
    		FREE_HASHTABLE(instance->common->parameters);
    	}
        efree(instance->common);       
    }
}

zend_object* create_parameter_bag_object( zend_class_entry* ce )
{
    parameter_bag_object* object = 
        ecalloc(1, sizeof(parameter_bag_object) + zend_object_properties_size(ce));

    zend_object_std_init(&object->std, ce);
    object_properties_init(&object->std, ce);

    memcpy(&parameter_bag_object_handlers, zend_get_std_object_handlers(), sizeof(parameter_bag_object_handlers));
    parameter_bag_object_handlers.offset = XtOffsetOf(parameter_bag_object, std);
    parameter_bag_object_handlers.free_obj = free_parameter_bag_object;
    object->std.handlers = &parameter_bag_object_handlers;
    object->common = ecalloc(1, sizeof(parameter_bag_common_t));
    object->common->parameters = NULL;

    return &object->std;
}

PHP_METHOD(ParameterBag, __construct)
{
	HashTable* parameters = NULL;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ARRAY_HT(parameters);
	ZEND_PARSE_PARAMETERS_END();
		
    parameter_bag_object* instance = 
        ZPHEUR_ZVAL_GET_OBJECT(parameter_bag_object, getThis());

	// ZEND_HASH_FOREACH_KEY_VAL_IND(parameters, zend_ulong index, zend_string* key, zval* value)
	// {
	// 	// TODO if filter key as number throw error
	// 	zend_hash_update(instance->common->parameters, key, value);
	// }
	// ZEND_HASH_FOREACH_END();

    ALLOC_HASHTABLE(instance->common->parameters);
    zend_hash_init(instance->common->parameters, 0, NULL, ZVAL_PTR_DTOR, 0);
    zend_hash_copy(instance->common->parameters, parameters, zval_add_ref);
}

PHP_METHOD(ParameterBag, set)
{
	char*   name_src = NULL;
	size_t  name_len = 0;

	zval*   value;

	ZEND_PARSE_PARAMETERS_START(2, 2)
		Z_PARAM_STRING(name_src, name_len)
		Z_PARAM_ZVAL_OR_NULL(value)
	ZEND_PARSE_PARAMETERS_END();

    parameter_bag_object* instance = 
        ZPHEUR_ZVAL_GET_OBJECT(parameter_bag_object, getThis());

	zval target_value;
	ZVAL_COPY(&target_value, value);

	zend_hash_str_update(instance->common->parameters, name_src, name_len, &target_value);
}

PHP_METHOD(ParameterBag, get)
{
	char*   field_src = NULL;
	size_t  field_len = 0;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_STRING(field_src, field_len)
	ZEND_PARSE_PARAMETERS_END();

	zval* value;
    parameter_bag_object* instance = 
        ZPHEUR_ZVAL_GET_OBJECT(parameter_bag_object, getThis());

	if( (value = zend_hash_str_find(instance->common->parameters, field_src, field_len)) )
		RETURN_ZVAL(value, 1, 0);

	RETURN_NULL();
}

PHP_METHOD(ParameterBag, has)
{
	char*   field_src = NULL;
	size_t  field_len = 0;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_STRING(field_src, field_len)
	ZEND_PARSE_PARAMETERS_END();

    parameter_bag_object* instance = 
        ZPHEUR_ZVAL_GET_OBJECT(parameter_bag_object, getThis());

	if( zend_hash_str_find(instance->common->parameters, field_src, field_len) )
		RETURN_TRUE;

	RETURN_FALSE;
}

PHP_METHOD(ParameterBag, count)
{
	ZEND_PARSE_PARAMETERS_NONE();

    parameter_bag_object* instance = 
        ZPHEUR_ZVAL_GET_OBJECT(parameter_bag_object, getThis());

	RETURN_LONG(zend_hash_num_elements(
		instance->common->parameters
	));
}


PHP_METHOD(ParameterBag, all)
{
	ZEND_PARSE_PARAMETERS_NONE();

    parameter_bag_object* instance = 
        ZPHEUR_ZVAL_GET_OBJECT(parameter_bag_object, getThis());

    // HashTable* copy;
    // ALLOC_HASHTABLE(copy);
    // zend_hash_init(copy, 0, NULL, ZVAL_PTR_DTOR, 0);
    // zend_hash_copy(copy, instance->common->parameters, zval_add_ref);
    instance->common->parameters->gc.refcount++;
	RETURN_ARR(instance->common->parameters);
	// RETURN_ARR(copy);
}

ZEND_MINIT_FUNCTION(Zpheur_Schemes_WebSocket_Foundation_ParameterBag)
{
    zend_class_entry ce;

    INIT_NS_CLASS_ENTRY(ce, "Zpheur\\Schemes\\WebSocket\\Foundation", "ParameterBag", zpheur_schemes_websocket_foundation_parameterbag_class_method);
    zpheur_schemes_websocket_foundation_parameterbag_class_entry = zend_register_internal_class(&ce);
    zpheur_schemes_websocket_foundation_parameterbag_class_entry->ce_flags |= ZEND_ACC_NO_DYNAMIC_PROPERTIES;
    zpheur_schemes_websocket_foundation_parameterbag_class_entry->create_object = create_parameter_bag_object;

    return SUCCESS;
}
