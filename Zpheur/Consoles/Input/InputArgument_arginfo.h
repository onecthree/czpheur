
#ifndef H_ZPHEUR_CONSOLES_INPUT_INPUTARGUMENT
#define H_ZPHEUR_CONSOLES_INPUT_INPUTARGUMENT

#include "ext/standard/info.h"
#include "ext/standard/php_standard.h"

#include "zend_API.h"
#include "zend_interfaces.h"
#include "zend_exceptions.h"


zend_class_entry* zpheur_consoles_input_inputargument_class_entry;

ZEND_BEGIN_ARG_INFO_EX(InputArgument___construct_arginfo, 0, 0, 2)
    ZEND_ARG_TYPE_INFO(0, length, IS_LONG, 0)
    ZEND_ARG_TYPE_INFO(0, value, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(InputArgument_getCount_arginfo, 0, 0, IS_OBJECT, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(InputArgument_getValue_arginfo, 0, 1, IS_STRING, 0)
    ZEND_ARG_TYPE_INFO(0, index, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(InputArgument_shiftArgument_arginfo, 0, 0, IS_VOID, 0)
ZEND_END_ARG_INFO()


PHP_METHOD(InputArgument, __construct);
PHP_METHOD(InputArgument, getCount);
PHP_METHOD(InputArgument, getValue);
PHP_METHOD(InputArgument, shiftArgument);


static const zend_function_entry zpheur_consoles_input_inputargument_class_method[] = {
    PHP_ME(InputArgument, __construct, InputArgument___construct_arginfo, ZEND_ACC_PUBLIC | ZEND_ACC_CTOR)
    PHP_ME(InputArgument, getCount, InputArgument_getCount_arginfo, ZEND_ACC_PUBLIC)
    PHP_ME(InputArgument, getValue, InputArgument_getValue_arginfo, ZEND_ACC_PUBLIC)
    PHP_ME(InputArgument, shiftArgument, InputArgument_shiftArgument_arginfo, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};

#endif