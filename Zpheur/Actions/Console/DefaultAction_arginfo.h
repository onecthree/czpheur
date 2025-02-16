
#ifndef H_ZPHEUR_ACTIONS_CONSOLE_DEFAULTACTION
#define H_ZPHEUR_ACTIONS_CONSOLE_DEFAULTACTION

#include "ext/standard/info.h"
#include "ext/standard/php_standard.h"

#include "zend_API.h"
#include "zend_interfaces.h"
#include "zend_exceptions.h"


zend_class_entry* zpheur_actions_console_defaultaction_class_entry;

ZEND_BEGIN_ARG_INFO_EX(Console_DefaultAction___construct_arginfo, 0, 0, 0)
ZEND_END_ARG_INFO()


PHP_METHOD(Console_DefaultAction, __construct);


static const zend_function_entry zpheur_actions_console_defaultaction_class_method[] = {
	PHP_ME(Console_DefaultAction, __construct, Console_DefaultAction___construct_arginfo, ZEND_ACC_PUBLIC | ZEND_ACC_CTOR)
	ZEND_FE_END
};

#endif