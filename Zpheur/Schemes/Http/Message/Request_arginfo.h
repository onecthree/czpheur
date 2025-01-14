
#ifndef H_ZPHEUR_SCHEMES_HTTP_FOUNDATION_REQUEST
#define H_ZPHEUR_SCHEMES_HTTP_FOUNDATION_REQUEST

#include "ext/standard/info.h"
#include "ext/standard/php_standard.h"

#include "zend_API.h"
#include "zend_interfaces.h"
#include "zend_exceptions.h"


zend_class_entry* zpheur_schemes_http_message_request_class_entry;

ZEND_BEGIN_ARG_INFO_EX(Request___construct_arginfo, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(Request_setBodyParams_arginfo, 0, 3, IS_VOID, 0)
    ZEND_ARG_TYPE_INFO(0, request, IS_ARRAY | IS_NULL, 0)
    ZEND_ARG_TYPE_INFO(0, query, IS_ARRAY | IS_NULL, 0)
    ZEND_ARG_TYPE_INFO(0, cookies, IS_ARRAY | IS_NULL, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(Request_setFilesParams_arginfo, 0, 1, IS_VOID, 0)
    ZEND_ARG_TYPE_INFO(0, files, IS_ARRAY | IS_NULL, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(Request_setHeaderParams_arginfo, 0, 2, IS_VOID, 0)
    ZEND_ARG_TYPE_INFO(0, server, IS_ARRAY | IS_NULL, 0)
    ZEND_ARG_TYPE_INFO(0, header, IS_ARRAY | IS_NULL, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(Request_setAttributeParams_arginfo, 0, 1, IS_VOID, 0)
    ZEND_ARG_TYPE_INFO(0, attribute, IS_ARRAY | IS_NULL, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(Request___get_arginfo, 0, 1, IS_MIXED, 0)
    ZEND_ARG_TYPE_INFO(0, property, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(Request_parseHeader_arginfo, 0, 1, IS_OBJECT, 0)
    ZEND_ARG_TYPE_INFO(0, server, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(Request_parseQuery_arginfo, 0, 2, IS_ARRAY, 0)
    ZEND_ARG_TYPE_INFO(0, query, IS_ARRAY, 0)
    ZEND_ARG_TYPE_INFO(0, uri, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(Request___service_arginfo, 0, 0, IS_OBJECT, 0)
ZEND_END_ARG_INFO()


PHP_METHOD(Request, __construct);
PHP_METHOD(Request, setBodyParams);
PHP_METHOD(Request, setFilesParams);
PHP_METHOD(Request, setHeaderParams);
PHP_METHOD(Request, setAttributeParams);
PHP_METHOD(Request, __get);
PHP_METHOD(Request, parseHeader);
PHP_METHOD(Request, parseQuery);
PHP_METHOD(Request, __service);


static const zend_function_entry zpheur_schemes_http_foundation_request_class_method[] = {
    PHP_ME(Request, __construct, Request___construct_arginfo, ZEND_ACC_PUBLIC | ZEND_ACC_CTOR)
    PHP_ME(Request, setBodyParams, Request_setBodyParams_arginfo, ZEND_ACC_PUBLIC )
    PHP_ME(Request, setFilesParams, Request_setFilesParams_arginfo, ZEND_ACC_PUBLIC )
    PHP_ME(Request, setHeaderParams, Request_setHeaderParams_arginfo, ZEND_ACC_PUBLIC )
    PHP_ME(Request, setAttributeParams, Request_setAttributeParams_arginfo, ZEND_ACC_PUBLIC )
    PHP_ME(Request, __get, Request___get_arginfo, ZEND_ACC_PUBLIC )
    PHP_ME(Request, parseHeader, Request_parseHeader_arginfo, ZEND_ACC_PUBLIC )
    PHP_ME(Request, parseQuery, Request_parseQuery_arginfo, ZEND_ACC_PUBLIC )
    PHP_ME(Request, __service, Request___service_arginfo, ZEND_ACC_PUBLIC )
	ZEND_FE_END
};

#endif