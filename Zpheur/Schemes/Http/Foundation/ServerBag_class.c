#include <php.h>
#include <ext/standard/info.h>
#include <php_zpheur.h>

#include <string.h>
#include <zpheur.h>
#include <include/runtime.h>
#include "HeaderBag_arginfo.h"
#include "ServerBag_arginfo.h"


ZEND_MINIT_FUNCTION(Zpheur_Schemes_Http_Foundation_ServerBag)
{
    zend_class_entry ce;

    INIT_NS_CLASS_ENTRY(ce, "Zpheur\\Schemes\\Http\\Foundation", "ServerBag", zpheur_schemes_http_foundation_serverbag_class_method);
    zpheur_schemes_http_foundation_serverbag_class_entry = zend_register_internal_class_ex(&ce, zpheur_schemes_http_foundation_headerbag_class_entry);

    return SUCCESS;
}
