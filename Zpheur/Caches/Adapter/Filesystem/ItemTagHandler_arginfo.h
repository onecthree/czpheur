// #pragma once

#ifndef ZPHEUR_CACHES_ADAPTER_FILESYSTEM_ITEMTAGHANDLER
#define ZPHEUR_CACHES_ADAPTER_FILESYSTEM_ITEMTAGHANDLER

#include "ext/standard/info.h"
#include "ext/standard/php_standard.h"

#include "zend_API.h"
#include "zend_interfaces.h"
#include "zend_exceptions.h"


typedef struct
{
    char* fullpath_src;
    char* expires_src;
} itemtagaware_full_literal_path;


zend_class_entry* zpheur_caches_filesystem_itemtaghandler_class_entry;

ZEND_BEGIN_ARG_INFO_EX(ItemTagAware___construct_arginfo, 0, 0, 4)
    ZEND_ARG_TYPE_INFO(0, name, IS_STRING, 0)
    ZEND_ARG_TYPE_INFO(0, filename, IS_STRING, 0)
    ZEND_ARG_TYPE_INFO(0, ttl, IS_STRING, 0)
    ZEND_ARG_TYPE_INFO(0, path, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(ItemTagAware_datetimeObject_arginfo, 0, 1, IS_VOID, 0)
    ZEND_ARG_TYPE_INFO(0, datetime, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(ItemTagAware_expiresAfter_arginfo, 0, 1, IS_VOID, 0)
    ZEND_ARG_TYPE_INFO(0, time, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(ItemTagAware_isHit_arginfo, 0, 0, _IS_BOOL, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(ItemTagAware_set_arginfo, 0, 1, IS_VOID, 0)
    ZEND_ARG_TYPE_INFO(0, item, IS_MIXED, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(ItemTagAware_get_arginfo, 1, 0, IS_MIXED, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(ItemTagAware_delete_arginfo, 0, 0, _IS_BOOL, 0)
ZEND_END_ARG_INFO()


PHP_METHOD(ItemTagAware, __construct);
PHP_METHOD(ItemTagAware, datetimeObject);
PHP_METHOD(ItemTagAware, expiresAfter);
PHP_METHOD(ItemTagAware, isHit);
PHP_METHOD(ItemTagAware, set);
PHP_METHOD(ItemTagAware, get);
PHP_METHOD(ItemTagAware, delete);


static const zend_function_entry zpheur_caches_filesystem_itemtaghandler_class_method[] = {
    PHP_ME(ItemTagAware, __construct, ItemTagAware___construct_arginfo, ZEND_ACC_PUBLIC | ZEND_ACC_CTOR )
    PHP_ME(ItemTagAware, datetimeObject, ItemTagAware_datetimeObject_arginfo, ZEND_ACC_PUBLIC )
    PHP_ME(ItemTagAware, expiresAfter, ItemTagAware_expiresAfter_arginfo, ZEND_ACC_PUBLIC )
    PHP_ME(ItemTagAware, isHit, ItemTagAware_isHit_arginfo, ZEND_ACC_PUBLIC )
    PHP_ME(ItemTagAware, set, ItemTagAware_set_arginfo, ZEND_ACC_PUBLIC )
    PHP_ME(ItemTagAware, get, ItemTagAware_get_arginfo, ZEND_ACC_PUBLIC )
    PHP_ME(ItemTagAware, delete, ItemTagAware_delete_arginfo, ZEND_ACC_PUBLIC )
    ZEND_FE_END
};

#endif