
#ifndef H_ZPHEUR_SCHEMES_HTTP_MIDDLEWARE_NEXT
#define H_ZPHEUR_SCHEMES_HTTP_MIDDLEWARE_NEXT

#include "ext/standard/info.h"
#include "ext/standard/php_standard.h"

#include "zend_API.h"
#include "zend_interfaces.h"
#include "zend_exceptions.h"


zend_class_entry* zpheur_schemes_http_middleware_next_class_entry;

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(Next___invoke_arginfo, 1, 0, IS_MIXED, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(Next_servicePin_arginfo, 1, 0, IS_MIXED, 0)
ZEND_END_ARG_INFO()


PHP_METHOD(Next, __invoke);
PHP_METHOD(Next, servicePin);


static const zend_function_entry zpheur_schemes_http_middleware_next_class_method[] = {
    PHP_ME(Next, __invoke, Next___invoke_arginfo, ZEND_ACC_PUBLIC)
    PHP_ME(Next, servicePin, Next_servicePin_arginfo, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};

#endif