#pragma once

#ifndef H_ZPHEUR_CONSOLES_RUNTIME_APPLICATION
#define H_ZPHEUR_CONSOLES_RUNTIME_APPLICATION

#include "ext/standard/info.h"
#include "ext/standard/php_standard.h"

#include "zend_API.h"
#include "zend_interfaces.h"
#include "zend_exceptions.h"

#include <include/onecstr.h>


static int console_class_method_parser( char* base_namespace, zend_string* target_action_class, onec_string** target_class, onec_string** target_method );
static int console_subns_class_method_parser( char* base_namespace, zend_string* target_action_class, onec_string** target_class );

zend_class_entry* zpheur_consoles_runtime_application_class_entry;

ZEND_BEGIN_ARG_INFO_EX(Application___construct_arginfo, 0, 0, 3)
    ZEND_ARG_TYPE_INFO(0, inputArgument, IS_OBJECT, 0)
    ZEND_ARG_TYPE_INFO(0, actionResolver, IS_OBJECT, 0)
    ZEND_ARG_TYPE_INFO(0, argumentResolver, IS_OBJECT, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(Application_withBaseNamespace_arginfo, 0, 1, IS_MIXED, 0)
    ZEND_ARG_TYPE_INFO(0, baseNamespace, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(Application_run_arginfo, 0, 0, IS_MIXED, 0)
ZEND_END_ARG_INFO()


PHP_METHOD(Application, __construct);
PHP_METHOD(Application, withBaseNamespace);
PHP_METHOD(Application, run);


static const zend_function_entry zpheur_consoles_runtime_application_class_method[] = {
    PHP_ME(Application, __construct, Application___construct_arginfo, ZEND_ACC_PUBLIC | ZEND_ACC_CTOR)
    PHP_ME(Application, withBaseNamespace, Application_withBaseNamespace_arginfo, ZEND_ACC_PUBLIC)
    PHP_ME(Application, run, Application_run_arginfo, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};

#endif