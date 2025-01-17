
#ifndef H_ZPHEUR_CACHES_CACHE
#define H_ZPHEUR_CACHES_CACHE

#include "ext/standard/info.h"
#include "ext/standard/php_standard.h"

#include "zend_API.h"
#include "zend_interfaces.h"
#include "zend_exceptions.h"


zend_class_entry* zpheur_caches_adapter_memcached_class_entry;

ZEND_BEGIN_ARG_INFO_EX(Cache_Memcached___construct_arginfo, 0, 0, 3)
	ZEND_ARG_TYPE_INFO(0, prefix, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, servers, IS_ARRAY, 0)
	ZEND_ARG_TYPE_INFO(0, options, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(Cache_Memcached_get_arginfo, 0, 2, IS_MIXED, 0)
    ZEND_ARG_TYPE_INFO(0, key, IS_STRING, 0)
    ZEND_ARG_TYPE_INFO(0, default, IS_MIXED, 1)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(Cache_Memcached_set_arginfo, 0, 3, _IS_BOOL, 0)
    ZEND_ARG_TYPE_INFO(0, key, IS_STRING, 0)
    ZEND_ARG_TYPE_INFO(0, value, IS_MIXED, 0)
    ZEND_ARG_TYPE_INFO(0, ttl, IS_LONG, 1)
ZEND_END_ARG_INFO()

// ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(Cache_Memcached_delete_arginfo, 0, 1, _IS_BOOL, 0)
//     ZEND_ARG_TYPE_INFO(0, key, IS_STRING, 0)
// ZEND_END_ARG_INFO()

// ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(Cache_Memcached_clear_arginfo, 0, 0, _IS_BOOL, 0)
// ZEND_END_ARG_INFO()

// ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(Cache_Memcached_getMultiple_arginfo, 0, 2, IS_ITERABLE, 0)
//     ZEND_ARG_TYPE_INFO(0, key, IS_STRING, 0)
// ZEND_END_ARG_INFO()

// ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(Cache_Memcached_setMultiple_arginfo, 0, 2, _IS_BOOL, 0)
//     ZEND_ARG_TYPE_INFO(0, key, IS_STRING, 0)
//     ZEND_ARG_TYPE_INFO(0, ttl, IS_LONG, 1)
// ZEND_END_ARG_INFO()

// ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(Cache_Memcached_deleteMultiple_arginfo, 0, 1, _IS_BOOL, 0)
//     ZEND_ARG_TYPE_INFO(0, key, IS_STRING, 0)
// ZEND_END_ARG_INFO()

// ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(Cache_Memcached_has_arginfo, 0, 1, _IS_BOOL, 0)
//     ZEND_ARG_TYPE_INFO(0, key, IS_STRING, 0)
// ZEND_END_ARG_INFO()


PHP_METHOD(Cache_Memcached, __construct);
PHP_METHOD(Cache_Memcached, get);
PHP_METHOD(Cache_Memcached, set);


static const zend_function_entry zpheur_caches_cache_memcached_class_method[] = {
    PHP_ME(Cache_Memcached, __construct, Cache_Memcached___construct_arginfo, ZEND_ACC_PUBLIC | ZEND_ACC_CTOR)
    PHP_ME(Cache_Memcached, get, Cache_Memcached_get_arginfo, ZEND_ACC_PUBLIC)
    PHP_ME(Cache_Memcached, set, Cache_Memcached_set_arginfo, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};

#endif