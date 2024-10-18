#include <php.h>
#include <ext/standard/info.h>
#include <php_zpheur.h>

#include <zpheur.h>
#include <include/runtime.h>
#include <zpheur.h>
#include "InputArgument_arginfo.h"


PHP_METHOD(InputArgument, __construct)
{
	zend_ulong 		count = 0;
	zval*      		value = NULL;

	ZEND_PARSE_PARAMETERS_START(2, 2)
		Z_PARAM_LONG(count)
		Z_PARAM_ARRAY(value)
	ZEND_PARSE_PARAMETERS_END();

	int value_count = zend_hash_num_elements(Z_ARR_P(value));

	if(! value_count )
		php_error_docref(NULL, E_ERROR, "Minimum 1 argument required");

	if( value_count != count )
		php_error_docref(NULL, E_ERROR, "Argument count mismatch, %d given instead of %ld", value_count, count);

	zval _count;
	ZVAL_LONG(&_count, count);
	zend_this_update_property("count", &_count);

	zend_this_update_property("value", value);
}

PHP_METHOD(InputArgument, getCount)
{
	ZEND_PARSE_PARAMETERS_NONE();

	zval* 		length = zend_this_read_property("length");
	zend_ulong	length_src = zval_get_long(length) - 1;

	zval length_return;
	ZVAL_LONG(&length_return, length_src);

	RETURN_ZVAL(&length_return, 0, 1);
}

PHP_METHOD(InputArgument, getValue)
{
	zend_ulong index = 0;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_LONG(index);
	ZEND_PARSE_PARAMETERS_END();

	zval* value = zend_this_read_property("value");
	zval* item_of_value = zend_hash_index_find(Z_ARR_P(value), index);

	if( item_of_value == NULL )
		RETURN_NULL();

	RETURN_ZVAL(item_of_value, 1, 0);
}

PHP_METHOD(InputArgument, shiftArgument)
{
	ZEND_PARSE_PARAMETERS_NONE();

	zval* count = zend_this_read_property("count");
	zval* value = zend_this_read_property("value");

	zend_ulong decrement_count = zval_get_long(count) - 1;
	ARRAY_SHIFT(value);

	zval new_count;
	ZVAL_LONG(&new_count, decrement_count);
	zend_this_update_property("count", &new_count);
	// zval* 
	// ZEND_HASH_FOREACH_NUM_KEY_VAL(Z_ARR_P(value), zend_ulong index, zval* item)
	// {

	// }
	// ZEND_HASH_FOREACH_END();
}

ZEND_MINIT_FUNCTION(Zpheur_Consoles_Input_InputArgument)
{
    zend_class_entry ce;

    INIT_NS_CLASS_ENTRY(ce, "Zpheur\\Consoles\\Input", "InputArgument", zpheur_consoles_input_inputargument_class_method);
    zpheur_consoles_input_inputargument_class_entry = zend_register_internal_class(&ce);
    zpheur_consoles_input_inputargument_class_entry->ce_flags |= ZEND_ACC_ALLOW_DYNAMIC_PROPERTIES;

    zend_declare_property_null(zpheur_consoles_input_inputargument_class_entry, "count", sizeof("count") - 1, ZEND_ACC_PUBLIC);
    zend_declare_property_null(zpheur_consoles_input_inputargument_class_entry, "value", sizeof("value") - 1, ZEND_ACC_PUBLIC);

    return SUCCESS;
}