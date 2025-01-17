
#ifndef H_ZPHEUR_SCHEMES_HTTP_MESSAGE_RESPONSEINTERFACE
#define H_ZPHEUR_SCHEMES_HTTP_MESSAGE_RESPONSEINTERFACE

#include "ext/standard/info.h"
#include "ext/standard/php_standard.h"

#include "zend_API.h"
#include "zend_interfaces.h"
#include "zend_exceptions.h"


zend_class_entry* zpheur_schemes_http_message_response_responseinterface_class_entry;

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(ResponseInterface_send_arginfo, 0, 1, IS_OBJECT, 0)
    ZEND_ARG_TYPE_INFO(0, buffer, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(ResponseInterface_json_arginfo, 0, 1, IS_OBJECT, 0)
    ZEND_ARG_TYPE_INFO(0, data, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

PHP_METHOD(ResponseInterface, send);
PHP_METHOD(ResponseInterface, json);

static const zend_function_entry zpheur_schemes_http_message_response_responseinterface_class_method[] = {
    PHP_ME(ResponseInterface, send, ResponseInterface_send_arginfo, ZEND_ACC_PUBLIC)
    PHP_ME(ResponseInterface, json, ResponseInterface_json_arginfo, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};

#endif