
#ifndef H_ZPHEUR_SCHEMES_HTTP_ROUTING_DISPATCHER
#define H_ZPHEUR_SCHEMES_HTTP_ROUTING_DISPATCHER

#include "ext/standard/info.h"
#include "ext/standard/php_standard.h"

#include "zend_API.h"
#include "zend_interfaces.h"
#include "zend_exceptions.h"


zend_class_entry* zpheur_schemes_http_routing_dispatcher_class_entry;

ZEND_BEGIN_ARG_INFO_EX(RoutingDispatcher___construct_arginfo, 0, 0, 1)
    ZEND_ARG_INFO(0, route_source)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(RoutingDispatcher_dispatch_arginfo, 0, 2, IS_ARRAY, 0)
    ZEND_ARG_TYPE_INFO(0, method, IS_STRING, 0)
    ZEND_ARG_TYPE_INFO(0, uri, IS_STRING, 0)
ZEND_END_ARG_INFO()


PHP_METHOD(RoutingDispatcher, __construct);
PHP_METHOD(RoutingDispatcher, dispatch);


static const zend_function_entry zpheur_schemes_http_routing_dispatcher_class_method[] = {
    PHP_ME(RoutingDispatcher, __construct, RoutingDispatcher___construct_arginfo, ZEND_ACC_PUBLIC | ZEND_ACC_CTOR)
    PHP_ME(RoutingDispatcher, dispatch, RoutingDispatcher_dispatch_arginfo, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};

#endif