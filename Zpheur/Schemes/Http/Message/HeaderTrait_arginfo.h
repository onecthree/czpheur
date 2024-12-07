
#ifndef H_ZPHEUR_SCHEMES_HTTP_MESSAGE_HEADERTRAIT
#define H_ZPHEUR_SCHEMES_HTTP_MESSAGE_HEADERTRAIT

#include "ext/standard/info.h"
#include "ext/standard/php_standard.h"

#include "zend_API.h"
#include "zend_interfaces.h"
#include "zend_exceptions.h"


void local_set_header( char* key_src, size_t key_len, char* value_src, size_t value_len );

zend_class_entry* zpheur_schemes_http_message_headertrait_class_entry;

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(HeaderTrait_set_arginfo, 0, 2, IS_VOID, 0)
    ZEND_ARG_TYPE_INFO(0, header, IS_LONG, 0)
    ZEND_ARG_TYPE_INFO(0, value, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(HeaderTrait_redirect_arginfo, 0, 1, IS_VOID, 0)
    ZEND_ARG_TYPE_INFO(0, dest, IS_STRING, 0)
    ZEND_ARG_TYPE_INFO(0, permanent, IS_LONG|IS_TRUE, 0)
ZEND_END_ARG_INFO()


PHP_METHOD(HeaderTrait, set);
PHP_METHOD(HeaderTrait, redirect);


static const zend_function_entry zpheur_schemes_http_message_headertrait_class_method[] = {
    PHP_ME(HeaderTrait, set, HeaderTrait_set_arginfo, ZEND_ACC_PUBLIC)
    PHP_ME(HeaderTrait, redirect, HeaderTrait_redirect_arginfo, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};

#endif