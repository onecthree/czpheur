
#ifndef H_ZPHEUR_ACTIONS_REFLECTION_ARGUMENTRESOLVER
#define H_ZPHEUR_ACTIONS_REFLECTION_ARGUMENTRESOLVER

#include "ext/standard/info.h"
#include "ext/standard/php_standard.h"

#include "zend_API.h"
#include "zend_interfaces.h"
#include "zend_exceptions.h"


zend_class_entry* zpheur_actions_reflection_argumentresolver_class_entry;

ZEND_BEGIN_ARG_INFO_EX(ArgumentResolver___construct_arginfo, 0, 0, 1)
    ZEND_ARG_TYPE_INFO(0, container, IS_OBJECT, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(ArgumentResolver_getTargetParameter_arginfo, 0, 2, IS_ARRAY|IS_NULL, 0)
    ZEND_ARG_TYPE_INFO(0, class, IS_STRING, 0)
    ZEND_ARG_TYPE_INFO(0, method, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(ArgumentResolver_withTargetSegments_arginfo, 0, 1, IS_VOID, 0)
    ZEND_ARG_TYPE_INFO(0, segments, IS_ARRAY, 0)
ZEND_END_ARG_INFO()


PHP_METHOD(ArgumentResolver, __construct);
PHP_METHOD(ArgumentResolver, getTargetParameter);
PHP_METHOD(ArgumentResolver, withTargetSegments);


static const zend_function_entry zpheur_actions_reflection_argumentresolver_class_method[] = {
    PHP_ME(ArgumentResolver, __construct, ArgumentResolver___construct_arginfo, ZEND_ACC_PUBLIC | ZEND_ACC_CTOR)
    PHP_ME(ArgumentResolver, getTargetParameter, ArgumentResolver_getTargetParameter_arginfo, ZEND_ACC_PUBLIC)
    PHP_ME(ArgumentResolver, withTargetSegments, ArgumentResolver_withTargetSegments_arginfo, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};

#endif