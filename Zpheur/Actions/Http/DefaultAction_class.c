#include <php.h>
#include <ext/standard/info.h>
#include <php_zpheur.h>
#include "DefaultAction_arginfo.h"


PHP_METHOD(Http_DefaultAction, __construct)
{
    ZEND_PARSE_PARAMETERS_NONE();
}

ZEND_MINIT_FUNCTION(Zpheur_Actions_Http_DefaultAction)
{
    zend_class_entry ce;
    INIT_NS_CLASS_ENTRY(ce, "Zpheur\\Actions\\Http", "DefaultAction", zpheur_http_actions_defaultaction_class_method);
    zpheur_actions_http_defaultaction_class_entry = zend_register_internal_class(&ce);
    zpheur_actions_http_defaultaction_class_entry->ce_flags |= ZEND_ACC_NO_DYNAMIC_PROPERTIES;

    return SUCCESS;
}