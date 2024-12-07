
#ifndef H_ZPHEUR_SCHEMES_HTTP_MIDDLEWARE
#define H_ZPHEUR_SCHEMES_HTTP_MIDDLEWARE

#include "ext/standard/info.h"
#include "ext/standard/php_standard.h"

#include "zend_API.h"
#include "zend_interfaces.h"
#include "zend_exceptions.h"


zend_class_entry* zpheur_actions_middleware_class_entry;

ZEND_BEGIN_ARG_INFO_EX(Middleware___construct_arginfo, 0, 0, 1)
    ZEND_ARG_TYPE_INFO(0, middleware_cache_path, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(Middleware_setBeforeRequest_arginfo, 1, 1, IS_MIXED, 0)
    ZEND_ARG_TYPE_INFO(0, middlewares, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(Middleware_setAfterRequest_arginfo, 1, 1, IS_MIXED, 0)
    ZEND_ARG_TYPE_INFO(0, middlewares, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(Middleware_listAllMiddlewares_arginfo, 2, 2, IS_ARRAY, 0)
    ZEND_ARG_TYPE_INFO(0, middlewareTypes, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(Middleware_resolve_arginfo, 2, 2, IS_ARRAY, 0)
    ZEND_ARG_TYPE_INFO(0, class, IS_STRING, 0)
    ZEND_ARG_TYPE_INFO(0, method, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(Middleware_scanAndResolve_arginfo, 2, 2, IS_ARRAY, 0)
    ZEND_ARG_TYPE_INFO(0, class, IS_STRING, 0)
    ZEND_ARG_TYPE_INFO(0, method, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(Middleware___destruct_arginfo, 0, 0, 0)
ZEND_END_ARG_INFO()


PHP_METHOD(Middleware, __construct);
PHP_METHOD(Middleware, setBeforeRequest);
PHP_METHOD(Middleware, setAfterRequest);
PHP_METHOD(Middleware, listAllMiddlewares);
PHP_METHOD(Middleware, resolve);
PHP_METHOD(Middleware, scanAndResolve);
PHP_METHOD(Middleware, __destruct);


static const zend_function_entry zpheur_actions_middleware_class_method[] = {
    PHP_ME(Middleware, __construct, Middleware___construct_arginfo, ZEND_ACC_PUBLIC | ZEND_ACC_CTOR)
    PHP_ME(Middleware, setBeforeRequest, Middleware_setBeforeRequest_arginfo, ZEND_ACC_PUBLIC)
    PHP_ME(Middleware, setAfterRequest, Middleware_setAfterRequest_arginfo, ZEND_ACC_PUBLIC)
    PHP_ME(Middleware, listAllMiddlewares, Middleware_listAllMiddlewares_arginfo, ZEND_ACC_PUBLIC)
    PHP_ME(Middleware, resolve, Middleware_resolve_arginfo, ZEND_ACC_PUBLIC)
    PHP_ME(Middleware, scanAndResolve, Middleware_scanAndResolve_arginfo, ZEND_ACC_PUBLIC)
    PHP_ME(Middleware, __destruct, Middleware___destruct_arginfo, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};

#endif