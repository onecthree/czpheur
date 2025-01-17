
#ifndef H_ZPHEUR_CACHES_CACHE
#define H_ZPHEUR_CACHES_CACHE

#include "ext/standard/info.h"
#include "ext/standard/php_standard.h"

#include "zend_API.h"
#include "zend_interfaces.h"
#include "zend_exceptions.h"


zend_class_entry* zpheur_caches_adapter_table_class_entry;

ZEND_BEGIN_ARG_INFO_EX(Cache_Table___construct_arginfo, 0, 0, 0)
ZEND_END_ARG_INFO()

// ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(Cache_Table___service_arginfo, 0, 0, IS_OBJECT, 0)
// ZEND_END_ARG_INFO()


PHP_METHOD(Cache_Table, __construct);
// PHP_METHOD(Cache_Table, __service);


static const zend_function_entry zpheur_caches_cache_table_class_method[] = {
    PHP_ME(Cache_Table, __construct, Cache_Table___construct_arginfo, ZEND_ACC_PUBLIC | ZEND_ACC_CTOR)
	ZEND_FE_END
};

#endif