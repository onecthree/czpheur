
#ifndef H_ZPHEUR_SCHEMES_WEBSOCKET_FOUNDATION_PARAMETERBAG
#define H_ZPHEUR_SCHEMES_WEBSOCKET_FOUNDATION_PARAMETERBAG

#include "ext/standard/info.h"
#include "ext/standard/php_standard.h"

#include "zend_API.h"
#include "zend_interfaces.h"
#include "zend_exceptions.h"


zend_class_entry* zpheur_schemes_websocket_foundation_parameterbag_class_entry;

zend_object_handlers parameter_bag_object_handlers;

typedef struct _parameter_bag_common_t
{
    HashTable* parameters;
} parameter_bag_common_t;

typedef struct _parameter_bag_object
{
    parameter_bag_common_t* common;
    zend_object std;
} parameter_bag_object;

ZEND_BEGIN_ARG_INFO_EX(ParameterBag___construct_arginfo, 0, 0, 1)
    ZEND_ARG_INFO(0, bag)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(ParameterBag_get_arginfo, 0, 1, IS_STRING|IS_NULL, 0)
    ZEND_ARG_TYPE_INFO(0, field, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(ParameterBag_set_arginfo, 0, 2, IS_VOID, 0)
    ZEND_ARG_TYPE_INFO(0, name, IS_STRING, 0)
    ZEND_ARG_TYPE_INFO(0, value, IS_MIXED, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(ParameterBag_has_arginfo, 0, 1, _IS_BOOL, 0)
    ZEND_ARG_TYPE_INFO(0, field, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(ParameterBag_count_arginfo, 0, 0, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(ParameterBag_all_arginfo, 0, 0, IS_LONG, 0)
ZEND_END_ARG_INFO()


PHP_METHOD(ParameterBag, __construct);
PHP_METHOD(ParameterBag, get);
PHP_METHOD(ParameterBag, set);
PHP_METHOD(ParameterBag, has);
PHP_METHOD(ParameterBag, count);
PHP_METHOD(ParameterBag, all);


static const zend_function_entry zpheur_schemes_websocket_foundation_parameterbag_class_method[] = {
    PHP_ME(ParameterBag, __construct, ParameterBag___construct_arginfo, ZEND_ACC_PUBLIC | ZEND_ACC_CTOR)
    PHP_ME(ParameterBag, get, ParameterBag_get_arginfo, ZEND_ACC_PUBLIC)
    PHP_ME(ParameterBag, set, ParameterBag_set_arginfo, ZEND_ACC_PUBLIC)
    PHP_ME(ParameterBag, has, ParameterBag_has_arginfo, ZEND_ACC_PUBLIC)
    PHP_ME(ParameterBag, count, ParameterBag_count_arginfo, ZEND_ACC_PUBLIC)
    PHP_ME(ParameterBag, all, ParameterBag_all_arginfo, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};

#endif