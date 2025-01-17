
#ifndef H_ZPHEUR_SCHEMES_HTTP_MESSAGE_REQUESTINTERFACE
#define H_ZPHEUR_SCHEMES_HTTP_MESSAGE_REQUESTINTERFACE

#include "ext/standard/info.h"
#include "ext/standard/php_standard.h"

#include "zend_API.h"
#include "zend_interfaces.h"
#include "zend_exceptions.h"


zend_class_entry* zpheur_schemes_http_message_request_requestinterface_class_entry;

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(RequestInterface_send_arginfo, 0, 1, IS_OBJECT, 0)
    ZEND_ARG_TYPE_INFO(0, buffer, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(RequestInterface_json_arginfo, 0, 1, IS_OBJECT, 0)
    ZEND_ARG_TYPE_INFO(0, data, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

PHP_METHOD(RequestInterface, send);
PHP_METHOD(RequestInterface, json);

static const zend_function_entry zpheur_schemes_http_message_request_requestinterface_class_method[] = {
    PHP_ME(RequestInterface, send, RequestInterface_send_arginfo, ZEND_ACC_PUBLIC)
    PHP_ME(RequestInterface, json, RequestInterface_json_arginfo, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};

#endif