
#ifndef H_ZPHEUR_SCHEMES_HTTP_ROUTING_ROUTE
#define H_ZPHEUR_SCHEMES_HTTP_ROUTING_ROUTE

#include "ext/standard/info.h"
#include "ext/standard/php_standard.h"

#include "zend_API.h"
#include "zend_interfaces.h"
#include "zend_exceptions.h"


#define LOCALVERB_GET       716984
#define LOCALVERB_POST      80798384
#define LOCALVERB_PUT       808584
#define LOCALVERB_PATCH     8065846772
#define LOCALVERB_DELETE    686976698469
#define LOCALVERB_HEAD      72696568
#define LOCALVERB_OPTIONS   79808473797883
#define LOCALVERB_WEBSOCKET 876966837967756984

zend_class_entry* zpheur_schemes_http_routing_route_class_entry;

ZEND_BEGIN_ARG_INFO_EX(Route___construct_arginfo, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(Route_add_arginfo, 1, 3, IS_MIXED, 0)
    ZEND_ARG_TYPE_INFO(0, method, IS_LONG, 0)
    ZEND_ARG_TYPE_INFO(0, uri, IS_STRING, 0)
    ZEND_ARG_TYPE_INFO(0, action, IS_MIXED, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(Route_setCacheFilepath_arginfo, 1, 2, IS_VOID, 0)
    ZEND_ARG_TYPE_INFO(0, route, IS_STRING, 0)
    ZEND_ARG_TYPE_INFO(0, middleware, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(Route_middleware_arginfo, 0, 1, IS_MIXED, 0)
    ZEND_ARG_TYPE_INFO(0, middlewareList, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(Route_clearRouteCaches_arginfo, 1, 0, IS_VOID, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(Route_clearMiddlewareCaches_arginfo, 1, 0, IS_VOID, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(Route_save_arginfo, 0, 0, IS_VOID, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(Route_tryVerbFromString_arginfo, 0, 1, IS_LONG | IS_NULL, 0)
    ZEND_ARG_TYPE_INFO(0, http_verb, IS_STRING, 0)
ZEND_END_ARG_INFO()


PHP_METHOD(Route, __construct);
PHP_METHOD(Route, setCacheFilepath);
PHP_METHOD(Route, middleware);
PHP_METHOD(Route, add);
PHP_METHOD(Route, save);
PHP_METHOD(Route, tryVerbFromString);


static const zend_function_entry zpheur_schemes_http_routing_route_class_method[] = {
    PHP_ME(Route, __construct, Route___construct_arginfo, ZEND_ACC_PUBLIC | ZEND_ACC_CTOR)
    PHP_ME(Route, setCacheFilepath, Route_setCacheFilepath_arginfo, ZEND_ACC_PUBLIC)
    PHP_ME(Route, middleware, Route_middleware_arginfo, ZEND_ACC_PUBLIC)
    PHP_ME(Route, add, Route_add_arginfo, ZEND_ACC_PUBLIC)
    PHP_ME(Route, save, Route_save_arginfo, ZEND_ACC_PUBLIC)
    PHP_ME(Route, tryVerbFromString, Route_tryVerbFromString_arginfo, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
	ZEND_FE_END
};

#endif