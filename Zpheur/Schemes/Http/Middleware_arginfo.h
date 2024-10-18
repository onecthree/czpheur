
#ifndef H_ZPHEUR_SCHEMES_HTTP_MIDDLEWARE
#define H_ZPHEUR_SCHEMES_HTTP_MIDDLEWARE

#include "ext/standard/info.h"
#include "ext/standard/php_standard.h"

#include "zend_API.h"
#include "zend_interfaces.h"
#include "zend_exceptions.h"


zend_class_entry* zpheur_schemes_http_middleware_class_entry;

ZEND_BEGIN_ARG_INFO_EX(Middleware___construct_arginfo, 0, 0, 1)
    ZEND_ARG_TYPE_INFO(0, middleware_cache_path, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(Middleware_setGlobalLists_arginfo, 1, 1, IS_MIXED, 0)
    ZEND_ARG_TYPE_INFO(0, globals, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(Middleware_withActionCall_arginfo, 1, 3, IS_ARRAY, 0)
    ZEND_ARG_TYPE_INFO(0, class, IS_STRING, 0)
    ZEND_ARG_TYPE_INFO(0, method, IS_STRING, 0)
    ZEND_ARG_TYPE_INFO(0, static_call, IS_TRUE, 0)
ZEND_END_ARG_INFO()


PHP_METHOD(Middleware, __construct);
PHP_METHOD(Middleware, setGlobalLists);
PHP_METHOD(Middleware, withActionCall);


static const zend_function_entry zpheur_schemes_http_middleware_class_method[] = {
    PHP_ME(Middleware, __construct, Middleware___construct_arginfo, ZEND_ACC_PUBLIC | ZEND_ACC_CTOR)
    PHP_ME(Middleware, setGlobalLists, Middleware_setGlobalLists_arginfo, ZEND_ACC_PUBLIC)
    PHP_ME(Middleware, withActionCall, Middleware_withActionCall_arginfo, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};

#endif