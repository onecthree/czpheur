
#ifndef H_ZPHEUR_ACTIONS_REFLECTION_ACTIONRESOLVER
#define H_ZPHEUR_ACTIONS_REFLECTION_ACTIONRESOLVER

#include "ext/standard/info.h"
#include "ext/standard/php_standard.h"

#include "zend_API.h"
#include "zend_interfaces.h"
#include "zend_exceptions.h"


zend_class_entry* zpheur_actions_reflection_actionresolver_class_entry;

ZEND_BEGIN_ARG_INFO_EX(ActionResolver___construct_arginfo, 0, 0, 1)
    ZEND_ARG_TYPE_INFO(0, middleware, IS_OBJECT, 0)
ZEND_END_ARG_INFO() 

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(ActionResolver_withDefaultAction_arginfo, 1, 2, IS_MIXED, 0)
    ZEND_ARG_TYPE_INFO(0, class, IS_STRING, 0)
    ZEND_ARG_TYPE_INFO(0, method, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(ActionResolver_bindIfDefaultAction_arginfo, 0, 1, IS_ARRAY, 0)
    ZEND_ARG_TYPE_INFO(0, dispatch, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(ActionResolver_staticMiddlewareCall_arginfo, 1, 1, IS_MIXED, 0)
    ZEND_ARG_TYPE_INFO(0, use_static_call, IS_TRUE, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(ActionResolver_process_arginfo, 0, 2, IS_ARRAY, 0)
    ZEND_ARG_TYPE_INFO(0, class, IS_STRING, 0)
    ZEND_ARG_TYPE_INFO(0, method, IS_STRING, 0)
ZEND_END_ARG_INFO()


PHP_METHOD(ActionResolver, __construct);
PHP_METHOD(ActionResolver, withDefaultAction);
PHP_METHOD(ActionResolver, bindIfDefaultAction);
PHP_METHOD(ActionResolver, staticMiddlewareCall);
PHP_METHOD(ActionResolver, process);


static const zend_function_entry zpheur_actions_reflection_actionresolver_class_method[] = {
	PHP_ME(ActionResolver, __construct, ActionResolver___construct_arginfo, ZEND_ACC_PUBLIC | ZEND_ACC_CTOR)
    PHP_ME(ActionResolver, withDefaultAction, ActionResolver_withDefaultAction_arginfo, ZEND_ACC_PUBLIC)
    PHP_ME(ActionResolver, bindIfDefaultAction, ActionResolver_bindIfDefaultAction_arginfo, ZEND_ACC_PUBLIC)
    PHP_ME(ActionResolver, staticMiddlewareCall, ActionResolver_staticMiddlewareCall_arginfo, ZEND_ACC_PUBLIC)
    PHP_ME(ActionResolver, process, ActionResolver_process_arginfo, ZEND_ACC_PUBLIC)
    ZEND_FE_END
};

#endif