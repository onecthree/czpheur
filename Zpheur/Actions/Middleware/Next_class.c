#include <php.h>
#include <ext/standard/info.h>
#include <php_zpheur.h>

#include <string.h>
#include <zpheur.h>
#include <include/runtime.h>
#include "Next_arginfo.h"


PHP_METHOD(Next, __invoke)
{   
    ZEND_PARSE_PARAMETERS_NONE();

    RETURN_ZVAL(getThis(), 1, 0);
}   

PHP_METHOD(Next, servicePin)
{
    ZEND_PARSE_PARAMETERS_NONE();
    
    RETURN_ZVAL(getThis(), 1, 0);
}

ZEND_MINIT_FUNCTION(Zpheur_Actions_Middleware_Next)
{
    zend_class_entry ce;

    INIT_NS_CLASS_ENTRY(ce, "Zpheur\\Actions\\Middleware", "Next", zpheur_actions_middleware_next_class_method);
    zpheur_actions_middleware_next_class_entry = zend_register_internal_class(&ce);
    zpheur_actions_middleware_next_class_entry->ce_flags |= ZEND_ACC_NO_DYNAMIC_PROPERTIES;

    return SUCCESS;
}