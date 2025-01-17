
#ifndef H_ZPHEUR_CACHES_SIMPLECACHE
#define H_ZPHEUR_CACHES_SIMPLECACHE

#include "ext/standard/info.h"
#include "ext/standard/php_standard.h"

#include "zend_API.h"
#include "zend_interfaces.h"
#include "zend_exceptions.h"


zend_class_entry* zpheur_caches_simplecache_class_entry;

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(SimpleCache_get_arginfo, 0, 2, _IS_BOOL, 0)
    ZEND_ARG_TYPE_INFO(0, key, IS_STRING, 0)
    ZEND_ARG_TYPE_INFO(0, default, IS_MIXED, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(SimpleCache_set_arginfo, 0, 3, _IS_BOOL, 0)
    ZEND_ARG_TYPE_INFO(0, key, IS_STRING, 0)
    ZEND_ARG_TYPE_INFO(0, value, IS_MIXED, 0)
    ZEND_ARG_TYPE_INFO(0, ttl, IS_LONG, 1)
ZEND_END_ARG_INFO()


PHP_METHOD(SimpleCache, get);
PHP_METHOD(SimpleCache, set);


static const zend_function_entry zpheur_caches_simplecache_class_method[] = {
    PHP_ME(SimpleCache, get, SimpleCache_get_arginfo, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
    PHP_ME(SimpleCache, set, SimpleCache_set_arginfo, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
	ZEND_FE_END
};

#endif