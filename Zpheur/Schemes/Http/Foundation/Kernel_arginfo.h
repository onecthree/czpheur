
#ifndef H_ZPHEUR_SCHEMES_HTTP_FOUNDATION_KERNEL
#define H_ZPHEUR_SCHEMES_HTTP_FOUNDATION_KERNEL

#include "ext/standard/info.h"
#include "ext/standard/php_standard.h"

#include "zend_API.h"
#include "zend_interfaces.h"
#include "zend_exceptions.h"


zend_class_entry* zpheur_schemes_http_foundation_kernel_class_entry;

ZEND_BEGIN_ARG_INFO_EX(HttpKernel___construct_arginfo, 0, 0, 2)
    ZEND_ARG_TYPE_INFO(0, dispatcher, IS_OBJECT, 0)
    ZEND_ARG_TYPE_INFO(0, resolver, IS_OBJECT, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(HttpKernel_handle_arginfo, 0, 2, IS_MIXED, 0)
    // ZEND_ARG_TYPE_INFO(0, request, IS_OBJECT, 0)
    ZEND_ARG_TYPE_INFO(0, request_method, IS_STRING, 0)
    ZEND_ARG_TYPE_INFO(0, request_uri, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(HttpKernel_terminate_arginfo, 0, 2, IS_MIXED, 0)
    ZEND_ARG_TYPE_INFO(0, class_name, IS_STRING, 0)
    ZEND_ARG_TYPE_INFO(0, method_name, IS_STRING, 0)
ZEND_END_ARG_INFO()


PHP_METHOD(HttpKernel, __construct);
PHP_METHOD(HttpKernel, handle);
PHP_METHOD(HttpKernel, terminate);


static const zend_function_entry zpheur_schemes_http_foundation_kernel_class_method[] = {
    PHP_ME(HttpKernel, __construct, HttpKernel___construct_arginfo, ZEND_ACC_PUBLIC | ZEND_ACC_CTOR)
    PHP_ME(HttpKernel, handle, HttpKernel_handle_arginfo, ZEND_ACC_PUBLIC)
    PHP_ME(HttpKernel, terminate, HttpKernel_terminate_arginfo, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};

#endif