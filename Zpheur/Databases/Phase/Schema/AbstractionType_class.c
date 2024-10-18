#include <php.h>
#include <php_zpheur.h>
#include <ext/standard/info.h>

#include "AbstractionType_arginfo.h"


ZEND_MINIT_FUNCTION(Zpheur_Databases_Phase_Schema_AbstractionType_AInt)
{
    zend_class_entry ce_aint;
    INIT_NS_CLASS_ENTRY(ce_aint, "Zpheur\\Databases\\Phase\\Schema\\AbstractionType", "AInt", NULL);
    zpheur_databases_phase_model_schema_abstractiontype_aint_class_entry = zend_register_internal_class(&ce_aint);
    zpheur_databases_phase_model_schema_abstractiontype_aint_class_entry->ce_flags |= ZEND_ACC_NO_DYNAMIC_PROPERTIES;

    return SUCCESS;
}

ZEND_MINIT_FUNCTION(Zpheur_Databases_Phase_Schema_AbstractionType_AVarChar)
{
    zend_class_entry ce_avarchar;
    INIT_NS_CLASS_ENTRY(ce_avarchar, "Zpheur\\Databases\\Phase\\Schema\\AbstractionType", "AVarChar", NULL);
    zpheur_databases_phase_model_schema_abstractiontype_aint_class_entry = zend_register_internal_class(&ce_avarchar);
    zpheur_databases_phase_model_schema_abstractiontype_aint_class_entry->ce_flags |= ZEND_ACC_NO_DYNAMIC_PROPERTIES;

    return SUCCESS;
}