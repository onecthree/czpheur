#include <php.h>
#include <ext/standard/info.h>
#include <php_zpheur.h>

#include <string.h>
#include <zpheur.h>
#include <include/runtime.h>
#include <include/onecstr.h>
#include "InvalidArgumentException_arginfo.h"


ZEND_MINIT_FUNCTION(Zpheur_Actions_Middleware_InvalidArgumentException)
{
    zend_class_entry ce;

    INIT_NS_CLASS_ENTRY(ce, "Zpheur\\Actions\\Middleware", "InvalidArgumentException", zpheur_caches_middleware_invalidargumentexception_class_method);
    zpheur_caches_middleware_invalidargumentexception_class_entry = zend_register_internal_class_ex(&ce, zend_ce_exception);
    zpheur_caches_middleware_invalidargumentexception_class_entry->ce_flags |= ZEND_ACC_FINAL;
    zpheur_caches_middleware_invalidargumentexception_class_entry->ce_flags |= ZEND_ACC_NO_DYNAMIC_PROPERTIES;
    
    zend_class_implements(zpheur_caches_middleware_invalidargumentexception_class_entry, 1, zend_ce_throwable);

    return SUCCESS;
}