#include <php.h>
#include <ext/standard/info.h>
#include <php_zpheur.h>

#include <string.h>
#include <zpheur.h>
#include <include/runtime.h>
#include "ParameterBag_arginfo.h"
#include "InputBag_arginfo.h"


ZEND_MINIT_FUNCTION(Zpheur_Schemes_Http_Foundation_InputBag)
{
    zend_class_entry ce;

    INIT_NS_CLASS_ENTRY(ce, "Zpheur\\Schemes\\Http\\Foundation", "InputBag", zpheur_schemes_http_foundation_inputbag_class_method);
    zpheur_schemes_http_foundation_inputbag_class_entry = zend_register_internal_class_ex(&ce, zpheur_schemes_http_foundation_parameterbag_class_entry);

    return SUCCESS;
}
