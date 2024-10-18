#include <php.h>
#include <ext/standard/info.h>
#include <php_zpheur.h>

#include <string.h>
#include <zpheur.h>
#include <include/runtime.h>
#include "ParameterBag_arginfo.h"


PHP_METHOD(ParameterBag, __construct)
{
	zval* parameters = NULL;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ZVAL(parameters);
	ZEND_PARSE_PARAMETERS_END();
	
	HashTable *new_parameters;
    ALLOC_HASHTABLE(new_parameters);
	zend_hash_init(new_parameters, 0, NULL, ZVAL_PTR_DTOR, 0);

	ZEND_HASH_FOREACH_KEY_VAL_IND(Z_ARR_P(parameters), zend_ulong index, zend_string* key, zval* value)
	{
		if( key )
			zend_hash_update(new_parameters, key, value);
		else
			zend_hash_index_update(new_parameters, index, value);
	}
	ZEND_HASH_FOREACH_END();

	zval target_new_parameters;
	ZVAL_ARR(&target_new_parameters, new_parameters);
	zend_this_update_property("parameters", &target_new_parameters);
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

	zval* parameters = zend_this_read_property("parameters");

	zval target_value;
	ZVAL_COPY(&target_value, value);

	zend_hash_str_update(Z_ARR_P(parameters), name_src, name_len, &target_value);
}

PHP_METHOD(ParameterBag, get)
{
	char*   field_src = NULL;
	size_t  field_len = 0;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_STRING(field_src, field_len)
	ZEND_PARSE_PARAMETERS_END();

	zval* value;
	zval* parameters = zend_this_read_property("parameters");

	if( (value = zend_hash_str_find(Z_ARR_P(parameters), field_src, field_len)) )
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

	zval* parameters = zend_this_read_property("parameters");

	if( zend_hash_str_find(Z_ARR_P(parameters), field_src, field_len) )
		RETURN_TRUE;

	RETURN_FALSE;
}

PHP_METHOD(ParameterBag, count)
{
	ZEND_PARSE_PARAMETERS_NONE();

	zval* parameters = zend_this_read_property("parameters");

	RETURN_LONG(zend_hash_num_elements(Z_ARR_P(parameters)));
}


PHP_METHOD(ParameterBag, all)
{
	ZEND_PARSE_PARAMETERS_NONE();

	zval* parameters = zend_this_read_property("parameters");

	RETURN_ZVAL(parameters, 1, 0);
}

ZEND_MINIT_FUNCTION(Zpheur_Schemes_Http_Foundation_ParameterBag)
{
    zend_class_entry ce;

    INIT_NS_CLASS_ENTRY(ce, "Zpheur\\Schemes\\Http\\Foundation", "ParameterBag", zpheur_schemes_http_foundation_parameterbag_class_method);
    zpheur_schemes_http_foundation_parameterbag_class_entry = zend_register_internal_class(&ce);
    zpheur_schemes_http_foundation_parameterbag_class_entry->ce_flags |= ZEND_ACC_NO_DYNAMIC_PROPERTIES;

    zend_declare_property_null(zpheur_schemes_http_foundation_parameterbag_class_entry, "parameters", sizeof("parameters") - 1, ZEND_ACC_PUBLIC);

    return SUCCESS;
}
