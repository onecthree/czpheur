
#ifndef H_ZPHEUR_LOGS_STREAMLOG
#define H_ZPHEUR_LOGS_STREAMLOG

#include "ext/standard/info.h"
#include "ext/standard/php_standard.h"

#include "zend_API.h"
#include "zend_interfaces.h"
#include "zend_exceptions.h"


zend_class_entry* zpheur_logs_streamlog_class_entry;

ZEND_BEGIN_ARG_INFO_EX(StreamLog___construct_arginfo, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, name, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(StreamLog_pushHandler_arginfo, 0, 1, IS_OBJECT, 0)
	ZEND_ARG_TYPE_INFO(0, stream_handler, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(StreamLog_info_arginfo, 0, 1, IS_OBJECT, 0)
    ZEND_ARG_TYPE_INFO(0, message, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(StreamLog_warning_arginfo, 0, 1, IS_OBJECT, 0)
    ZEND_ARG_TYPE_INFO(0, message, IS_STRING, 0)
ZEND_END_ARG_INFO()


PHP_METHOD(StreamLog, __construct);
PHP_METHOD(StreamLog, pushHandler);
PHP_METHOD(StreamLog, info);
PHP_METHOD(StreamLog, warning);

static const zend_function_entry zpheur_logs_streamlog_class_method[] = {
	PHP_ME(StreamLog, __construct, StreamLog___construct_arginfo, ZEND_ACC_PUBLIC | ZEND_ACC_CTOR)
    PHP_ME(StreamLog, pushHandler, StreamLog_pushHandler_arginfo, ZEND_ACC_PUBLIC )
    PHP_ME(StreamLog, info, StreamLog_info_arginfo, ZEND_ACC_PUBLIC )
    PHP_ME(StreamLog, warning, StreamLog_warning_arginfo, ZEND_ACC_PUBLIC )
	ZEND_FE_END
};

#endif