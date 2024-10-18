
#ifndef H_ZPHEUR_DATABASES_VOILE_COLLECTION
#define H_ZPHEUR_DATABASES_VOILE_COLLECTION

#include "php/20220829/ext/standard/info.h"
#include "php/20220829/ext/standard/php_standard.h"

#include "php/20220829/Zend/zend_API.h"
#include "php/20220829/Zend/zend_interfaces.h"
#include "php/20220829/Zend/zend_exceptions.h"


typedef int (*c_spl_iterator_apply_func_t)(zend_object_iterator *iter, void *puser, bool once);

zend_class_entry* zpheur_databases_voile_collection_class_entry;

ZEND_BEGIN_ARG_INFO_EX(Collection___construct_arginfo, 0, 0, 2)
    ZEND_ARG_TYPE_INFO(0, array_items, IS_ARRAY, 0)
    ZEND_ARG_TYPE_INFO(0, cursor, IS_OBJECT, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(Collection_count_arginfo, 0, 0, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(Collection_current_arginfo, 0, 0, IS_MIXED, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(Collection_key_arginfo, 0, 0, IS_MIXED, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(Collection_next_arginfo, 0, 0, IS_VOID, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(Collection_rewind_arginfo, 0, 0, IS_VOID, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(Collection_valid_arginfo, 0, 0, _IS_BOOL, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(Collection_map_arginfo, 0, 1, IS_ARRAY, 0)
    ZEND_ARG_TYPE_INFO(0, map, IS_CALLABLE, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(Collection_filter_arginfo, 0, 1, IS_ARRAY, 0)
    ZEND_ARG_TYPE_INFO(0, filter, IS_CALLABLE, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(Collection___debugInfo_arginfo, 0, 0, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(Collection___get_arginfo, 0, 1, IS_MIXED, 0)
    ZEND_ARG_TYPE_INFO(0, name, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(Collection___invoke_arginfo, 0, 0, IS_MIXED, 0)
ZEND_END_ARG_INFO()

PHP_METHOD(Collection, __construct);

PHP_METHOD(Collection, count);
PHP_METHOD(Collection, current);
PHP_METHOD(Collection, key);
PHP_METHOD(Collection, next);
PHP_METHOD(Collection, rewind);
PHP_METHOD(Collection, valid);
PHP_METHOD(Collection, map);
PHP_METHOD(Collection, filter);
PHP_METHOD(Collection, __debugInfo);
PHP_METHOD(Collection, __get);
PHP_METHOD(Collection, __invoke);


static const zend_function_entry zpheur_databases_voile_model_collection_class_method[] = {
    PHP_ME(Collection, __construct, Collection___construct_arginfo, ZEND_ACC_PUBLIC | ZEND_ACC_CTOR)
    PHP_ME(Collection, count, Collection_count_arginfo, ZEND_ACC_PUBLIC)
    PHP_ME(Collection, current, Collection_current_arginfo, ZEND_ACC_PUBLIC)
    PHP_ME(Collection, key, Collection_key_arginfo, ZEND_ACC_PUBLIC)
    PHP_ME(Collection, next, Collection_next_arginfo, ZEND_ACC_PUBLIC)
    PHP_ME(Collection, rewind, Collection_rewind_arginfo, ZEND_ACC_PUBLIC)
    PHP_ME(Collection, valid, Collection_valid_arginfo, ZEND_ACC_PUBLIC)
    PHP_ME(Collection, map, Collection_map_arginfo, ZEND_ACC_PUBLIC)
    PHP_ME(Collection, filter, Collection_filter_arginfo, ZEND_ACC_PUBLIC)
    PHP_ME(Collection, __debugInfo, Collection___debugInfo_arginfo, ZEND_ACC_PUBLIC)
    PHP_ME(Collection, __get, Collection___get_arginfo, ZEND_ACC_PUBLIC)
    PHP_ME(Collection, __invoke, Collection___invoke_arginfo, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};

#endif
