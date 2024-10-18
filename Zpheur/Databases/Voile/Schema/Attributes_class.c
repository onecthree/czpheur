#include <php.h>
#include <php_zpheur.h>
#include <ext/standard/info.h>

#include "Attributes_arginfo.h"


ZEND_MINIT_FUNCTION(Zpheur_Databases_Voile_Schema_Collection)
{
    zend_class_entry ce_collection;
    INIT_NS_CLASS_ENTRY(ce_collection, "Zpheur\\Databases\\Voile\\Schema", "Collection", NULL);
    zpheur_databases_voile_model_schema_abstractiontype_collection_class_entry = zend_register_internal_class(&ce_collection);
    zpheur_databases_voile_model_schema_abstractiontype_collection_class_entry->ce_flags |= ZEND_ACC_NO_DYNAMIC_PROPERTIES;

    return SUCCESS;
}

ZEND_MINIT_FUNCTION(Zpheur_Databases_Voile_Schema_Field)
{
    zend_class_entry ce_field;
    INIT_NS_CLASS_ENTRY(ce_field, "Zpheur\\Databases\\Voile\\Schema", "Field", NULL);
    zpheur_databases_voile_model_schema_abstractiontype_field_class_entry = zend_register_internal_class(&ce_field);
    zpheur_databases_voile_model_schema_abstractiontype_field_class_entry->ce_flags |= ZEND_ACC_NO_DYNAMIC_PROPERTIES;

    return SUCCESS;
}