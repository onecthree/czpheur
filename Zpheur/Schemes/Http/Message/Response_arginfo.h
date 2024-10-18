
#ifndef H_ZPHEUR_SCHEMES_HTTP_MESSAGE_RESPONSE
#define H_ZPHEUR_SCHEMES_HTTP_MESSAGE_RESPONSE

#include "ext/standard/info.h"
#include "ext/standard/php_standard.h"

#include "zend_API.h"
#include "zend_interfaces.h"
#include "zend_exceptions.h"


void static _set_mime_type( zend_ulong utype );

zend_class_entry* zpheur_schemes_http_message_response_class_entry;

ZEND_BEGIN_ARG_INFO_EX(Response___construct_arginfo, 0, 1, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(Response_send_arginfo, 1, 1, IS_STRING, 0)
    ZEND_ARG_TYPE_INFO(0, response, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(Response_statusCode_arginfo, 1, 1, IS_OBJECT, 0)
    ZEND_ARG_TYPE_INFO(0, statusCode, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(Response_redirect_arginfo, 1, 1, IS_OBJECT, 0)
    ZEND_ARG_TYPE_INFO(0, dest, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(Response_set_arginfo, 1, 2, IS_OBJECT, 0)
    ZEND_ARG_TYPE_INFO(0, type, IS_ARRAY|IS_STRING, 0)
    ZEND_ARG_TYPE_INFO(0, header, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(Response___toString_arginfo, 0, 0, IS_STRING, 0)
ZEND_END_ARG_INFO()


PHP_METHOD(Response, __construct);
PHP_METHOD(Response, send);
PHP_METHOD(Response, statusCode);
PHP_METHOD(Response, redirect);
PHP_METHOD(Response, set);
PHP_METHOD(Response, __toString);


static const zend_function_entry zpheur_schemes_http_message_response_class_method[] = {
    PHP_ME(Response, __construct, Response___construct_arginfo, ZEND_ACC_PUBLIC)
    PHP_ME(Response, send, Response_send_arginfo, ZEND_ACC_PUBLIC)
    PHP_ME(Response, statusCode, Response_statusCode_arginfo, ZEND_ACC_PUBLIC)
    PHP_ME(Response, redirect, Response_redirect_arginfo, ZEND_ACC_PUBLIC)
    PHP_ME(Response, set, Response_set_arginfo, ZEND_ACC_PUBLIC)
    PHP_ME(Response, __toString, Response___toString_arginfo, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};

#endif