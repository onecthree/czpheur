#include <php.h>
#include <ext/standard/info.h>
#include <php_zpheur.h>

#include "Level_arginfo.h"


PHP_METHOD(Level, __construct)
{
	ZEND_PARSE_PARAMETERS_NONE();
}

ZEND_MINIT_FUNCTION(Zpheur_Logs_StreamLog_Level)
{
    zend_class_entry ce;
    INIT_NS_CLASS_ENTRY(ce, "Zpheur\\Logs\\StreamLog", "Level", zpheur_logs_streamlog_level_class_method);
    zpheur_logs_streamlog_level_class_entry = zend_register_internal_class(&ce);
    zpheur_logs_streamlog_level_class_entry->ce_flags |= ZEND_ACC_NO_DYNAMIC_PROPERTIES;

    zend_declare_class_constant_long(
        zpheur_logs_streamlog_level_class_entry,
        "INFO", sizeof("INFO") - 1,
        1
    );

    zend_declare_class_constant_long(
        zpheur_logs_streamlog_level_class_entry,
        "WARNING", sizeof("WARNING") - 1,
        2
    );

    return SUCCESS;
}