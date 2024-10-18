#include <php.h>
#include <ext/standard/info.h>
#include <php_zpheur.h>

#include <string.h>
#include <zpheur.h>
#include <include/runtime.h>
#include "Mime_arginfo.h"


PHP_METHOD(Mime, __construct)
{
    ZEND_PARSE_PARAMETERS_NONE();
}

ZEND_MINIT_FUNCTION(Zpheur_Files_Mime)
{
    zend_class_entry ce;

    INIT_NS_CLASS_ENTRY(ce, "Zpheur\\Files", "Mime", zpheur_files_mime_class_method);
    zpheur_files_mime_class_entry = zend_register_internal_class(&ce);
    zpheur_files_mime_class_entry->ce_flags |= ZEND_ACC_ALLOW_DYNAMIC_PROPERTIES;

    // zend_declare_class_constant_long(zpheur_files_mime_class_entry, "JSON", sizeof("JSON") - 1, 1);
    // zend_declare_class_constant_long(zpheur_files_mime_class_entry, "TEXT", sizeof("TEXT") - 1, 2);

    zend_declare_class_constant_stringl(zpheur_files_mime_class_entry, "JSON", sizeof("JSON") - 1, "application/json", sizeof("application/json") - 1);
    zend_declare_class_constant_stringl(zpheur_files_mime_class_entry, "TEXT", sizeof("TEXT") - 1, "text/html", sizeof("text/html") - 1);

    return SUCCESS;
}