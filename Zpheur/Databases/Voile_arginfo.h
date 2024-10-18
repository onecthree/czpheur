
#ifndef H_ZPHEUR_DATABASES_VOILE
#define H_ZPHEUR_DATABASES_VOILE

#include "ext/standard/info.h"
#include "ext/standard/php_standard.h"

#include "zend_API.h"
#include "zend_interfaces.h"
#include "zend_exceptions.h"

#define CONST_VOILE_DB_URI 0
#define CONST_VOILE_DB_NAME 1


zend_class_entry* zpheur_databases_voile_class_entry;

ZEND_BEGIN_ARG_INFO_EX(Voile___construct_arginfo, 0, 0, 2)
    ZEND_ARG_TYPE_INFO(0, uri_connection, IS_OBJECT, 0)
    ZEND_ARG_TYPE_INFO(0, configuration, IS_MIXED, 0)
ZEND_END_ARG_INFO()

PHP_METHOD(Voile, __construct);

static const zend_function_entry zpheur_databases_voile_class_method[] = {
    PHP_ME(Voile, __construct, Voile___construct_arginfo, ZEND_ACC_PUBLIC | ZEND_ACC_CTOR)
	ZEND_FE_END
};

#endif