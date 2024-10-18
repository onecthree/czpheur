
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

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(Dotenv_parse_arginfo, 2, 2, IS_OBJECT, 0)
    ZEND_ARG_TYPE_INFO(0, saveComment, _IS_BOOL, 0)
    ZEND_ARG_TYPE_INFO(0, typeCast, _IS_BOOL, 0)
ZEND_END_ARG_INFO()


PHP_METHOD(Dotenv, __construct);
PHP_METHOD(Dotenv, serialize);
PHP_METHOD(Dotenv, unserialize);
PHP_METHOD(Dotenv, parse);


static const zend_function_entry zpheur_datatransforms_dotenv_dotenv_class_method[] = {
    PHP_ME(Dotenv, __construct, Dotenv___construct_arginfo, ZEND_ACC_PUBLIC | ZEND_ACC_CTOR)
    PHP_ME(Dotenv, serialize, Dotenv_serialize_arginfo, ZEND_ACC_PUBLIC)
    PHP_ME(Dotenv, unserialize, Dotenv_unserialize_arginfo, ZEND_ACC_PUBLIC)
    PHP_ME(Dotenv, parse, Dotenv_parse_arginfo, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};

#endif