#include <php.h>
#include <ext/standard/info.h>
#include <php_zpheur.h>

#include <zpheur.h>
#include <include/onecstr.h>
#include <include/runtime.h>
#include <zpheur.h>
#include "InputArgument_arginfo.h"


zend_object_handlers input_argument_object_handlers;

typedef struct _input_argument_common_t
{
	size_t count;
    HashTable* value; // Need release
} input_argument_common_t;

typedef struct _input_argument_object
{
    input_argument_common_t* common;
    zend_object std;
} input_argument_object;

void free_input_argument_object(zend_object *object)
{
    input_argument_object* instance = ZPHEUR_GET_OBJECT(input_argument_object, object);

    zend_object_std_dtor(&instance->std);
    if( instance->common )
    {
    	if( EXPECTED(instance->common->value) )
    	{
    		zend_hash_destroy(instance->common->value);
    		FREE_HASHTABLE(instance->common->value);
    	}

        efree(instance->common);       
    }
}

zend_object* create_input_argument_object( zend_class_entry* ce )
{
    input_argument_object* object = 
        ecalloc(1, sizeof(input_argument_object) + zend_object_properties_size(ce));

    zend_object_std_init(&object->std, ce);
    object_properties_init(&object->std, ce);

    memcpy(&input_argument_object_handlers, zend_get_std_object_handlers(), sizeof(input_argument_object_handlers));
    input_argument_object_handlers.offset = XtOffsetOf(input_argument_object, std);
    input_argument_object_handlers.free_obj = free_input_argument_object;
    object->std.handlers = &input_argument_object_handlers;

    object->common = emalloc(sizeof(input_argument_common_t));
    object->common->count = 3;
    object->common->value = NULL;

    return &object->std;
}

PHP_METHOD(InputArgument, __construct)
{
	zend_long count = 0;
	HashTable* value = NULL;
	bool shifted = false;

	ZEND_PARSE_PARAMETERS_START(2, 3)
		Z_PARAM_LONG(count)
		Z_PARAM_ARRAY_HT(value)
		Z_PARAM_OPTIONAL
		Z_PARAM_BOOL(shifted)
	ZEND_PARSE_PARAMETERS_END();

	int value_count = zend_hash_num_elements(value);

	if( value_count < 1 )
		php_error_docref(NULL, E_ERROR, "parameter ($count) #1 must be bigger than zero");

	if( value_count != count )
		php_error_docref(NULL, E_ERROR, "Argument count mismatch, %d given instead of %ld", value_count, count);

	if(! EXPECTED(HT_IS_PACKED(value)) )
		php_error_docref(NULL, E_ERROR, "parameter ($value) #2 must by type of linear array");

    input_argument_object* instance = 
        ZPHEUR_ZVAL_GET_OBJECT(input_argument_object, getThis());

    instance->common->count = shifted ? --count : count;

  	HashTable* table;
  	ALLOC_HASHTABLE(table);
  	zend_hash_init(table, value_count, NULL, ZVAL_PTR_DTOR, 0);

  	int i = 0;
    ZEND_HASH_FOREACH_VAL(value, zval* zv_value) {
    	if( !shifted || i++ > 0 )
    	{
    		zval copy;
    		ZVAL_COPY(&copy, zv_value);
    		zend_hash_next_index_insert(table, &copy);
    	}
    }
    ZEND_HASH_FOREACH_END();

    if( instance->common->value )
    {
    	zend_hash_destroy(instance->common->value);
    	FREE_HASHTABLE(instance->common->value);
    }
    instance->common->value = table;
}

PHP_METHOD(InputArgument, getCount)
{
	ZEND_PARSE_PARAMETERS_NONE();

    input_argument_object* instance = 
        ZPHEUR_ZVAL_GET_OBJECT(input_argument_object, getThis());

    zval copy;
    ZVAL_LONG(&copy, instance->common->count);

    RETURN_ZVAL(&copy, 1, 0);
}

PHP_METHOD(InputArgument, getValue)
{
	zend_long index = 0;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_LONG(index);
	ZEND_PARSE_PARAMETERS_END();

	if( index < 0 )
		php_error_docref(NULL, E_ERROR, "parameter ($index) #1 atleast start from zero");

    input_argument_object* instance = 
        ZPHEUR_ZVAL_GET_OBJECT(input_argument_object, getThis());

    zval* argument =
    	zend_hash_index_find(instance->common->value, index);

    if( !argument || !((1 << Z_TYPE_P(argument)) & MAY_BE_STRING) )
    	RETURN_NULL();

    Z_ADDREF_P(argument);

    RETURN_ZVAL(argument, 1, 0);
}

PHP_METHOD(InputArgument, listAllValues)
{
	ZEND_PARSE_PARAMETERS_NONE();

    input_argument_object* instance = 
        ZPHEUR_ZVAL_GET_OBJECT(input_argument_object, getThis());

    zval copy;
    array_init(&copy);
    zend_hash_copy(Z_ARR(copy), instance->common->value, zval_add_ref);

    RETURN_ZVAL(&copy, 0, 0);
}

ZEND_MINIT_FUNCTION(Zpheur_Consoles_Input_InputArgument)
{
    zend_class_entry ce;

    INIT_NS_CLASS_ENTRY(ce, "Zpheur\\Consoles\\Input", "InputArgument", zpheur_consoles_input_inputargument_class_method);
    zpheur_consoles_input_inputargument_class_entry = zend_register_internal_class(&ce);
    zpheur_consoles_input_inputargument_class_entry->ce_flags |= ZEND_ACC_ALLOW_DYNAMIC_PROPERTIES;
    zpheur_consoles_input_inputargument_class_entry->create_object = create_input_argument_object;

    return SUCCESS;
}