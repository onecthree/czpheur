
#ifndef H_ZPHEUR_SCHEMES_WEBSOCKET_FOUNDATION_HEADERBAG
#define H_ZPHEUR_SCHEMES_WEBSOCKET_FOUNDATION_HEADERBAG

#include "ext/standard/info.h"
#include "ext/standard/php_standard.h"

#include "zend_API.h"
#include "zend_interfaces.h"
#include "zend_exceptions.h"


zend_class_entry* zpheur_schemes_websocket_foundation_headerbag_class_entry;

ZEND_BEGIN_ARG_INFO_EX(HeaderBag___construct_arginfo, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(HeaderBag_get_arginfo, 0, 1, IS_STRING|IS_NULL, 0)
    ZEND_ARG_TYPE_INFO(0, name, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(HeaderBag_has_arginfo, 0, 1, IS_STRING|IS_NULL, 0)
    ZEND_ARG_TYPE_INFO(0, name, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(HeaderBag_all_arginfo, 0, 0, IS_ARRAY, 0)
ZEND_END_ARG_INFO()


PHP_METHOD(HeaderBag, __construct);  
PHP_METHOD(HeaderBag, get);
PHP_METHOD(HeaderBag, has);
PHP_METHOD(HeaderBag, all);


static const zend_function_entry zpheur_schemes_websocket_foundation_headerbag_class_method[] = {
    PHP_ME(HeaderBag, __construct, HeaderBag___construct_arginfo, ZEND_ACC_PUBLIC | ZEND_ACC_CTOR)
    PHP_ME(HeaderBag, get, HeaderBag_get_arginfo, ZEND_ACC_PUBLIC)
    PHP_ME(HeaderBag, has, HeaderBag_has_arginfo, ZEND_ACC_PUBLIC)
    PHP_ME(HeaderBag, all, HeaderBag_all_arginfo, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};

#endif