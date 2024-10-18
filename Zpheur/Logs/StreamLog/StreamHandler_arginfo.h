
#ifndef H_ZPHEUR_LOGS_STREAMLOG_STREAMHANDLER
#define H_ZPHEUR_LOGS_STREAMLOG_STREAMHANDLER

#include "ext/standard/info.h"
#include "ext/standard/php_standard.h"

#include "zend_API.h"
#include "zend_interfaces.h"
#include "zend_exceptions.h"


zend_class_entry* zpheur_logs_streamlog_streamhandler_class_entry;

ZEND_BEGIN_ARG_INFO_EX(StreamHandler___construct_arginfo, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, name, IS_STRING, 0)
    ZEND_ARG_TYPE_INFO(0, level, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(StreamHandler_datetimeObject_arginfo, 0, 1, IS_OBJECT, 0)
    ZEND_ARG_TYPE_INFO(0, datetime, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(StreamHandler_datetimeFormat_arginfo, 0, 1, IS_OBJECT, 0)
    ZEND_ARG_TYPE_INFO(0, format, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(StreamHandler_setDelimiter_arginfo, 0, 1, IS_OBJECT, 0)
    ZEND_ARG_TYPE_INFO(0, delimiter, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(StreamHandler_setLogFormat_arginfo, 0, 1, IS_OBJECT, 0)
    ZEND_ARG_TYPE_INFO(0, formatter_len, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(StreamHandler_log_arginfo, 0, 1, IS_OBJECT, 0)
    ZEND_ARG_TYPE_INFO(0, log, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(StreamHandler_getLevel_arginfo, 0, 0, IS_LONG, 0)
ZEND_END_ARG_INFO()


PHP_METHOD(StreamHandler, __construct);
PHP_METHOD(StreamHandler, datetimeObject);
PHP_METHOD(StreamHandler, datetimeFormat);
PHP_METHOD(StreamHandler, setDelimiter);
PHP_METHOD(StreamHandler, setLogFormat);
PHP_METHOD(StreamHandler, log);
PHP_METHOD(StreamHandler, getLevel);

static const zend_function_entry zpheur_logs_streamlog_streamhandler_class_method[] = {
	PHP_ME(StreamHandler, __construct, StreamHandler___construct_arginfo, ZEND_ACC_PUBLIC | ZEND_ACC_CTOR)
    PHP_ME(StreamHandler, datetimeObject, StreamHandler_datetimeObject_arginfo, ZEND_ACC_PUBLIC )
    PHP_ME(StreamHandler, datetimeFormat, StreamHandler_datetimeFormat_arginfo, ZEND_ACC_PUBLIC )
    PHP_ME(StreamHandler, setDelimiter, StreamHandler_setDelimiter_arginfo, ZEND_ACC_PUBLIC )
    PHP_ME(StreamHandler, setLogFormat, StreamHandler_setLogFormat_arginfo, ZEND_ACC_PUBLIC )
    PHP_ME(StreamHandler, log, StreamHandler_log_arginfo, ZEND_ACC_PUBLIC )
    PHP_ME(StreamHandler, getLevel, StreamHandler_getLevel_arginfo, ZEND_ACC_PUBLIC )
	ZEND_FE_END
};

#endif