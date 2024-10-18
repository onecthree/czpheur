#include <php.h>
#include <php_zpheur.h>
#include <ext/standard/info.h>

#include "Attributes_arginfo.h"


ZEND_MINIT_FUNCTION(Zpheur_Databases_Phase_Schema_Table)
{
    zend_class_entry ce_collection;
    INIT_NS_CLASS_ENTRY(ce_collection, "Zpheur\\Databases\\Phase\\Schema", "Table", NULL);
    zpheur_databases_phase_model_schema_abstractiontype_table_class_entry = zend_register_internal_class(&ce_collection);
    zpheur_databases_phase_model_schema_abstractiontype_table_class_entry->ce_flags |= ZEND_ACC_NO_DYNAMIC_PROPERTIES;

    return SUCCESS;
}

ZEND_MINIT_FUNCTION(Zpheur_Databases_Phase_Schema_Column)
{
    zend_class_entry ce_field;
    INIT_NS_CLASS_ENTRY(ce_field, "Zpheur\\Databases\\Phase\\Schema", "Column", NULL);
    zpheur_databases_phase_model_schema_abstractiontype_column_class_entry = zend_register_internal_class(&ce_field);
    zpheur_databases_phase_model_schema_abstractiontype_column_class_entry->ce_flags |= ZEND_ACC_NO_DYNAMIC_PROPERTIES;

    return SUCCESS;
}