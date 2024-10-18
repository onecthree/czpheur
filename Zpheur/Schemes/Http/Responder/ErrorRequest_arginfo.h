
#ifndef H_ZPHEUR_SCHEMES_HTTP_RESPONDER_ERRORREQUEST
#define H_ZPHEUR_SCHEMES_HTTP_RESPONDER_ERRORREQUEST

#include "ext/standard/info.h"
#include "ext/standard/php_standard.h"

#include "zend_API.h"
#include "zend_interfaces.h"
#include "zend_exceptions.h"


zend_class_entry* zpheur_schemes_http_responder_errorrequest_class_entry;

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(ErrorRequest_foo_arginfo, 0, 0, IS_VOID, 0)
ZEND_END_ARG_INFO()


PHP_METHOD(ErrorRequest, foo);


static const zend_function_entry zpheur_schemes_http_responder_errorrequest_class_method[] = {
    PHP_ME(ErrorRequest, foo, ErrorRequest_foo_arginfo, ZEND_ACC_PUBLIC )
	ZEND_FE_END
};

#endif