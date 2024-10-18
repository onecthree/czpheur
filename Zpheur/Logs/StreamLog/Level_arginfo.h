
#ifndef H_ZPHEUR_LOGS_STREAMLOG_LEVEL
#define H_ZPHEUR_LOGS_STREAMLOG_LEVEL

#include "ext/standard/info.h"
#include "ext/standard/php_standard.h"

#include "zend_API.h"
#include "zend_interfaces.h"
#include "zend_exceptions.h"


zend_class_entry* zpheur_logs_streamlog_level_class_entry;

ZEND_BEGIN_ARG_INFO_EX(Level___construct_arginfo, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, name, IS_STRING, 0)
ZEND_END_ARG_INFO()

PHP_METHOD(Level, __construct);

static const zend_function_entry zpheur_logs_streamlog_level_class_method[] = {
	PHP_ME(Level, __construct, Level___construct_arginfo, ZEND_ACC_PUBLIC | ZEND_ACC_CTOR)
	ZEND_FE_END
};

#endif