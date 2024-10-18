
#ifndef H_ZPHEUR_CACHES_ADAPTER_FILESYSTEM
#define H_ZPHEUR_CACHES_ADAPTER_FILESYSTEM

#include "ext/standard/info.h"
#include "ext/standard/php_standard.h"

#include "zend_API.h"
#include "zend_interfaces.h"
#include "zend_exceptions.h"


zend_class_entry* zpheur_caches_filesystem_class_entry;

ZEND_BEGIN_ARG_INFO_EX(Filesystem___construct_arginfo, 0, 0, 3)
    ZEND_ARG_TYPE_INFO(0, name, IS_STRING, 0)
    ZEND_ARG_TYPE_INFO(0, ttl, IS_LONG, 0)
    ZEND_ARG_TYPE_INFO(0, path, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(Filesystem_datetimeObject_arginfo, 0, 0, IS_VOID, 0)
    // ZEND_ARG_TYPE_INFO(0, datetime, IS_MIXED, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(Filesystem_getItem_arginfo, 0, 1, IS_OBJECT, 0)
    ZEND_ARG_TYPE_INFO(0, name, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(Filesystem_save_arginfo, 0, 1, IS_VOID, 0)
    ZEND_ARG_TYPE_INFO(0, item, IS_OBJECT, 0)
ZEND_END_ARG_INFO()


PHP_METHOD(Filesystem, __construct);
PHP_METHOD(Filesystem, datetimeObject);
PHP_METHOD(Filesystem, getItem);
PHP_METHOD(Filesystem, save);


static const zend_function_entry zpheur_caches_filesystem_class_method[] = {
    PHP_ME(Filesystem, __construct, Filesystem___construct_arginfo, ZEND_ACC_PUBLIC | ZEND_ACC_CTOR )
    PHP_ME(Filesystem, datetimeObject, Filesystem_datetimeObject_arginfo, ZEND_ACC_PUBLIC )
    PHP_ME(Filesystem, getItem, Filesystem_getItem_arginfo, ZEND_ACC_PUBLIC )
    PHP_ME(Filesystem, save, Filesystem_save_arginfo, ZEND_ACC_PUBLIC )
	ZEND_FE_END
};

#endif