
#ifndef H_ZPHEUR_CACHES_SIMPLECACHE_CACHEINTERFACE
#define H_ZPHEUR_CACHES_SIMPLECACHE_CACHEINTERFACE

#include "ext/standard/info.h"
#include "ext/standard/php_standard.h"

#include "zend_API.h"
#include "zend_interfaces.h"
#include "zend_exceptions.h"


zend_class_entry* zpheur_caches_simplecache_cacheinterface_class_entry;

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(SimpleCache_CacheInterface_init_arginfo, 0, 1, IS_VOID, 0)
    ZEND_ARG_TYPE_INFO(0, adapter_target, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(SimpleCache_CacheInterface_get_arginfo, 0, 2, IS_MIXED, 0)
    ZEND_ARG_TYPE_INFO(0, key, IS_STRING, 0)
    ZEND_ARG_TYPE_INFO(0, default, IS_MIXED, 1)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(SimpleCache_CacheInterface_set_arginfo, 0, 3, _IS_BOOL, 0)
    ZEND_ARG_TYPE_INFO(0, key, IS_STRING, 0)
    ZEND_ARG_TYPE_INFO(0, value, IS_MIXED, 0)
    ZEND_ARG_TYPE_INFO(0, ttl, IS_LONG, 1)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(SimpleCache_CacheInterface_delete_arginfo, 0, 1, _IS_BOOL, 0)
    ZEND_ARG_TYPE_INFO(0, key, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(SimpleCache_CacheInterface_clear_arginfo, 0, 0, _IS_BOOL, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(SimpleCache_CacheInterface_getMultiple_arginfo, 0, 2, IS_ITERABLE, 0)
    ZEND_ARG_TYPE_INFO(0, key, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(SimpleCache_CacheInterface_setMultiple_arginfo, 0, 2, _IS_BOOL, 0)
    ZEND_ARG_TYPE_INFO(0, key, IS_STRING, 0)
    ZEND_ARG_TYPE_INFO(0, ttl, IS_LONG, 1)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(SimpleCache_CacheInterface_deleteMultiple_arginfo, 0, 1, _IS_BOOL, 0)
    ZEND_ARG_TYPE_INFO(0, key, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(SimpleCache_CacheInterface_has_arginfo, 0, 1, _IS_BOOL, 0)
    ZEND_ARG_TYPE_INFO(0, key, IS_STRING, 0)
ZEND_END_ARG_INFO()

PHP_METHOD(SimpleCache_CacheInterface, init);
PHP_METHOD(SimpleCache_CacheInterface, get);
PHP_METHOD(SimpleCache_CacheInterface, set);
PHP_METHOD(SimpleCache_CacheInterface, delete);
PHP_METHOD(SimpleCache_CacheInterface, clear);
PHP_METHOD(SimpleCache_CacheInterface, getMultiple);
PHP_METHOD(SimpleCache_CacheInterface, setMultiple);
PHP_METHOD(SimpleCache_CacheInterface, deleteMultiple);
PHP_METHOD(SimpleCache_CacheInterface, has);


static const zend_function_entry zpheur_caches_simplecache_cacheinterface_class_method[] = {
    PHP_ME(SimpleCache_CacheInterface, init, SimpleCache_CacheInterface_init_arginfo, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
    PHP_ME(SimpleCache_CacheInterface, get, SimpleCache_CacheInterface_get_arginfo, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
    PHP_ME(SimpleCache_CacheInterface, set, SimpleCache_CacheInterface_set_arginfo, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
    PHP_ME(SimpleCache_CacheInterface, delete, SimpleCache_CacheInterface_delete_arginfo, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
    PHP_ME(SimpleCache_CacheInterface, clear, SimpleCache_CacheInterface_clear_arginfo, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
    PHP_ME(SimpleCache_CacheInterface, getMultiple, SimpleCache_CacheInterface_getMultiple_arginfo, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
    PHP_ME(SimpleCache_CacheInterface, setMultiple, SimpleCache_CacheInterface_setMultiple_arginfo, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
    PHP_ME(SimpleCache_CacheInterface, deleteMultiple, SimpleCache_CacheInterface_deleteMultiple_arginfo, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
    PHP_ME(SimpleCache_CacheInterface, has, SimpleCache_CacheInterface_has_arginfo, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
	ZEND_FE_END
};

#endif