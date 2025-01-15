
#ifndef H_ZPHEUR_DATATRANSFORMS_DOTENV_DOTENV
#define H_ZPHEUR_DATATRANSFORMS_DOTENV_DOTENV

#include "ext/standard/info.h"
#include "ext/standard/php_standard.h"

#include "zend_API.h"
#include "zend_interfaces.h"
#include "zend_exceptions.h"


zend_class_entry* zpheur_datatransforms_dotenv_dotenv_class_entry;

ZEND_BEGIN_ARG_INFO_EX(Dotenv___construct_arginfo, 0, 0, 1)
    ZEND_ARG_TYPE_INFO(0, envPath, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(Dotenv_serialize_arginfo, 1, 0, IS_OBJECT, 0)
    ZEND_ARG_TYPE_INFO(0, path, IS_STRING, 0)
ZEND_END_ARG_INFO()

// ERROR: stub argument given return as IS_OBJECT | IS_NULL make segfault
// FIX: given return with IS_MIXED should have fix the issue
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(Dotenv_unserialize_arginfo, 2, 2, IS_MIXED, 0)
    ZEND_ARG_TYPE_INFO(0, data, IS_ARRAY | IS_TRUE, 0)
    ZEND_ARG_TYPE_INFO(0, callback, IS_CALLABLE, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(Dotenv_unsafeParse_arginfo, 0, 2, IS_OBJECT, 0)
    ZEND_ARG_TYPE_INFO(0, save_comment, _IS_BOOL, 0)
    ZEND_ARG_TYPE_INFO(0, type_cast, _IS_BOOL, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(Dotenv_safeParse_arginfo, 0, 2, IS_OBJECT, 0)
    ZEND_ARG_TYPE_INFO(0, save_comment, _IS_BOOL, 0)
    ZEND_ARG_TYPE_INFO(0, type_cast, _IS_BOOL, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(Dotenv_set_arginfo, 0, 2, _IS_BOOL, 0)
    ZEND_ARG_TYPE_INFO(0, key, IS_STRING, 0)
    ZEND_ARG_TYPE_INFO(0, value, IS_STRING | IS_LONG | IS_DOUBLE | IS_NULL, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(Dotenv_save_arginfo, 0, 0, IS_VOID, 0)
ZEND_END_ARG_INFO();


PHP_METHOD(Dotenv, __construct);
PHP_METHOD(Dotenv, serialize);
PHP_METHOD(Dotenv, unserialize);
PHP_METHOD(Dotenv, unsafeParse);
PHP_METHOD(Dotenv, safeParse);
PHP_METHOD(Dotenv, set);
PHP_METHOD(Dotenv, save);


static const zend_function_entry zpheur_datatransforms_dotenv_dotenv_class_method[] = {
    PHP_ME(Dotenv, __construct, Dotenv___construct_arginfo, ZEND_ACC_PUBLIC | ZEND_ACC_CTOR)
    PHP_ME(Dotenv, serialize, Dotenv_serialize_arginfo, ZEND_ACC_PUBLIC)
    PHP_ME(Dotenv, unserialize, Dotenv_unserialize_arginfo, ZEND_ACC_PUBLIC)
    PHP_ME(Dotenv, unsafeParse, Dotenv_unsafeParse_arginfo, ZEND_ACC_PUBLIC)
    PHP_ME(Dotenv, safeParse, Dotenv_safeParse_arginfo, ZEND_ACC_PUBLIC)
    PHP_ME(Dotenv, set, Dotenv_set_arginfo, ZEND_ACC_PUBLIC)
    PHP_ME(Dotenv, save, Dotenv_save_arginfo, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};

#endif