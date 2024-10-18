#include <php.h>
#include <ext/standard/info.h>
#include <php_zpheur.h>

#include <string.h>
#include <zpheur.h>
#include <include/runtime.h>
#include "HeaderBag_arginfo.h"


PHP_METHOD(HeaderBag, __construct)
{
	zval* headers;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ZVAL(headers);
	ZEND_PARSE_PARAMETERS_END();

	zend_this_update_property("headers", headers);
}

PHP_METHOD(HeaderBag, get)
{
	char*   field_src;
	size_t  field_len;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_STRING(field_src, field_len)
	ZEND_PARSE_PARAMETERS_END();

	zval* value;
	zval* headers = zend_this_read_property("headers");

	if( (value = zend_hash_str_find(Z_ARR_P(headers), field_src, field_len)) )
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

	zval* headers = zend_this_read_property("headers");

	if( zend_hash_str_find(Z_ARR_P(headers), field_src, field_len) )
		RETURN_TRUE;

	RETURN_FALSE;
}

PHP_METHOD(HeaderBag, all)
{
	ZEND_PARSE_PARAMETERS_NONE();

	zval* headers = zend_this_read_property("headers");

	RETURN_ZVAL(headers, 1, 0);
}

ZEND_MINIT_FUNCTION(Zpheur_Schemes_Http_Foundation_HeaderBag)
{
    zend_class_entry ce;

    INIT_NS_CLASS_ENTRY(ce, "Zpheur\\Schemes\\Http\\Foundation", "HeaderBag", zpheur_schemes_http_foundation_headerbag_class_method);
    zpheur_schemes_http_foundation_headerbag_class_entry = zend_register_internal_class(&ce);

    zend_declare_property_null(zpheur_schemes_http_foundation_headerbag_class_entry, "headers", sizeof("headers") - 1, ZEND_ACC_PUBLIC);

    return SUCCESS;
}