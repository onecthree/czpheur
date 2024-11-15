#include <php.h>
#include <ext/standard/info.h>
#include <php_zpheur.h>
#include "DefaultAction_arginfo.h"


PHP_METHOD(Console_DefaultAction, __construct)
{
    ZEND_PARSE_PARAMETERS_NONE();
}

PHP_METHOD(Console_DefaultAction, exitCode)
{
    zend_long exit_code = 0;

    ZEND_PARSE_PARAMETERS_START(1, 1)
        Z_PARAM_LONG(exit_code)
    ZEND_PARSE_PARAMETERS_END();

    zend_register_long_constant("EXIT_CODE", sizeof("EXIT_CODE") -1, exit_code, CONST_CS | CONST_PERSISTENT, 51);

    RETURN_LONG(exit_code);
}

ZEND_MINIT_FUNCTION(Zpheur_Actions_Console_DefaultAction)
{
    zend_class_entry ce;
    INIT_NS_CLASS_ENTRY(ce, "Zpheur\\Actions\\Console", "DefaultAction", zpheur_actions_console_defaultaction_class_method);
    zpheur_actions_console_defaultaction_class_entry = zend_register_internal_class(&ce);
    zpheur_actions_console_defaultaction_class_entry->ce_flags |= ZEND_ACC_NO_DYNAMIC_PROPERTIES;

    return SUCCESS;
}