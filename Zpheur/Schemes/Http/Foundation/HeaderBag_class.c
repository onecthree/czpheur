#include <php.h>
#include <ext/standard/info.h>
#include <php_zpheur.h>

#include <string.h>
#include <zpheur.h>
#include <include/runtime.h>
#include "HeaderBag_arginfo.h"


zend_object_handlers header_bag_object_handlers;

typedef struct _header_bag_common_t
{
    HashTable* headers;
} header_bag_common_t;

typedef struct _header_bag_object
{
    header_bag_common_t* common;
    zend_object std;
} header_bag_object;

void free_header_bag_object(zend_object* object)
{
    header_bag_object* instance = ZPHEUR_GET_OBJECT(header_bag_object, object);

    zend_object_std_dtor(&instance->std);
    if( instance->common ) {
    	if( instance->common->headers != NULL ) {
    		zend_hash_destroy(instance->common->headers);
    		FREE_HASHTABLE(instance->common->headers);
    	}
        efree(instance->common);       
    }
}

zend_object* create_header_bag_object( zend_class_entry* ce )
{
    header_bag_object* object = 
        ecalloc(1, sizeof(header_bag_object) + zend_object_properties_size(ce));

    zend_object_std_init(&object->std, ce);
    object_properties_init(&object->std, ce);

    memcpy(&header_bag_object_handlers, zend_get_std_object_handlers(), sizeof(header_bag_object_handlers));
    header_bag_object_handlers.offset = XtOffsetOf(header_bag_object, std);
    header_bag_object_handlers.free_obj = free_header_bag_object;
    object->std.handlers = &header_bag_object_handlers;
    object->common = ecalloc(1, sizeof(header_bag_common_t));
    object->common->headers = NULL;

    return &object->std;
}

PHP_METHOD(HeaderBag, __construct)
{
	HashTable* headers = NULL;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ARRAY_HT(headers);
	ZEND_PARSE_PARAMETERS_END();

    header_bag_object* instance = 
        ZPHEUR_ZVAL_GET_OBJECT(header_bag_object, getThis());

    ALLOC_HASHTABLE(instance->common->headers);
    zend_hash_init(instance->common->headers, 0, NULL, ZVAL_PTR_DTOR, 0);
    zend_hash_copy(instance->common->headers, headers, zval_add_ref);
}

PHP_METHOD(HeaderBag, get)
{
	char*   field_src = NULL;
	size_t  field_len = 0;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_STRING(field_src, field_len)
	ZEND_PARSE_PARAMETERS_END();

	zval* value;
    header_bag_object* instance = 
        ZPHEUR_ZVAL_GET_OBJECT(header_bag_object, getThis());

	if( (value = zend_hash_str_find(instance->common->headers, field_src, field_len)) )
		RETURN_ZVAL(value, 1, 0);

	RETURN_NULL();
}

PHP_METHOD(HeaderBag, has)
{
	char*   field_src = NULL;
	size_t  field_len = 0;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_STRING(field_src, field_len)
	ZEND_PARSE_PARAMETERS_END();

    header_bag_object* instance = 
        ZPHEUR_ZVAL_GET_OBJECT(header_bag_object, getThis());

	if( zend_hash_str_find(instance->common->headers, field_src, field_len) )
		RETURN_TRUE;

	RETURN_FALSE;
}

PHP_METHOD(HeaderBag, count)
{
	ZEND_PARSE_PARAMETERS_NONE();

    header_bag_object* instance = 
        ZPHEUR_ZVAL_GET_OBJECT(header_bag_object, getThis());

	RETURN_LONG(zend_hash_num_elements(
		instance->common->headers
	));
}

PHP_METHOD(HeaderBag, all)
{
	ZEND_PARSE_PARAMETERS_NONE();

    header_bag_object* instance = 
        ZPHEUR_ZVAL_GET_OBJECT(header_bag_object, getThis());

    instance->common->headers->gc.refcount++;
	RETURN_ARR(instance->common->headers);
}

ZEND_MINIT_FUNCTION(Zpheur_Schemes_Http_Foundation_HeaderBag)
{
    zend_class_entry ce;

    INIT_NS_CLASS_ENTRY(ce, "Zpheur\\Schemes\\Http\\Foundation", "HeaderBag", zpheur_schemes_http_foundation_headerbag_class_method);
    zpheur_schemes_http_foundation_headerbag_class_entry = zend_register_internal_class(&ce);
    zpheur_schemes_http_foundation_headerbag_class_entry->create_object = create_header_bag_object;
    zpheur_schemes_http_foundation_headerbag_class_entry->ce_flags |= ZEND_ACC_NO_DYNAMIC_PROPERTIES;

    return SUCCESS;
}