
#ifndef H_ZPHEUR_ACTIONS_HTTP_DEFAULTACTION
#define H_ZPHEUR_ACTIONS_HTTP_DEFAULTACTION

#include "ext/standard/info.h"
#include "ext/standard/php_standard.h"

#include "zend_API.h"
#include "zend_interfaces.h"
#include "zend_exceptions.h"


zend_class_entry* zpheur_actions_http_defaultaction_class_entry;

ZEND_BEGIN_ARG_INFO_EX(Http_DefaultAction___construct_arginfo, 0, 0, 0)
ZEND_END_ARG_INFO()

PHP_METHOD(Http_DefaultAction, __construct);

static const zend_function_entry zpheur_http_actions_defaultaction_class_method[] = {
	PHP_ME(Http_DefaultAction, __construct, Http_DefaultAction___construct_arginfo, ZEND_ACC_PUBLIC | ZEND_ACC_CTOR)
	ZEND_FE_END
};

#endif