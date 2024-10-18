
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

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(Http_DefaultAction_view_arginfo, 0, 1, IS_VOID, 0)
    ZEND_ARG_TYPE_INFO(0, template, IS_STRING, 0)
    ZEND_ARG_TYPE_INFO(0, data, IS_MIXED, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(Http_DefaultAction_exitCode_arginfo, 0, 1, IS_LONG, 0)
    ZEND_ARG_TYPE_INFO(0, code, IS_LONG, 0)
ZEND_END_ARG_INFO()


PHP_METHOD(Http_DefaultAction, __construct);
PHP_METHOD(Http_DefaultAction, view);
PHP_METHOD(Http_DefaultAction, exitCode);


static const zend_function_entry zpheur_actions_defaultaction_class_method[] = {
	PHP_ME(Http_DefaultAction, __construct, Http_DefaultAction___construct_arginfo, ZEND_ACC_PUBLIC | ZEND_ACC_CTOR)
	PHP_ME(Http_DefaultAction, view, Http_DefaultAction_view_arginfo, ZEND_ACC_PUBLIC)
	PHP_ME(Http_DefaultAction, exitCode, Http_DefaultAction_exitCode_arginfo, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};

#endif