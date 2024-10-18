
#ifndef H_ZPHEUR_SCHEMES_HTTP_ROUTING_DISPATCHER_DISPATCHITEM
#define H_ZPHEUR_SCHEMES_HTTP_ROUTING_DISPATCHER_DISPATCHITEM

#include "ext/standard/info.h"
#include "ext/standard/php_standard.h"

#include "zend_API.h"
#include "zend_interfaces.h"
#include "zend_exceptions.h"


zend_class_entry* zpheur_schemes_http_routing_dispatcher_dispatchitem_class_entry;

ZEND_BEGIN_ARG_INFO_EX(DispatchItem___construct_arginfo, 0, 0, 0)
ZEND_END_ARG_INFO()

PHP_METHOD(DispatchItem, __construct);

static const zend_function_entry zpheur_schemes_http_routing_dispatcher_dispatchitem_class_method[] = {
    PHP_ME(DispatchItem, __construct, DispatchItem___construct_arginfo, ZEND_ACC_PUBLIC | ZEND_ACC_CTOR)
	ZEND_FE_END
};

#endif