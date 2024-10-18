#include <php.h>
#include <php_zpheur.h>
#include <ext/standard/info.h>

#include "AbstractionType_arginfo.h"


ZEND_MINIT_FUNCTION(Zpheur_Databases_Voile_Schema_AbstractionType_AObjectId)
{
    zend_class_entry ce_aobjectid;
    INIT_NS_CLASS_ENTRY(ce_aobjectid, "Zpheur\\Databases\\Voile\\Schema\\AbstractionType", "AObjectId", NULL);
    zpheur_databases_voile_model_schema_abstractiontype_aobjectid_class_entry = zend_register_internal_class(&ce_aobjectid);
    zpheur_databases_voile_model_schema_abstractiontype_aobjectid_class_entry->ce_flags |= ZEND_ACC_NO_DYNAMIC_PROPERTIES;

    return SUCCESS;
}

ZEND_MINIT_FUNCTION(Zpheur_Databases_Voile_Schema_AbstractionType_AString)
{
    zend_class_entry ce_astring;
    INIT_NS_CLASS_ENTRY(ce_astring, "Zpheur\\Databases\\Voile\\Schema\\AbstractionType", "AString", NULL);
    zpheur_databases_voile_model_schema_abstractiontype_astring_class_entry = zend_register_internal_class(&ce_astring);
    zpheur_databases_voile_model_schema_abstractiontype_astring_class_entry->ce_flags |= ZEND_ACC_NO_DYNAMIC_PROPERTIES;

    return SUCCESS;
}

ZEND_MINIT_FUNCTION(Zpheur_Databases_Voile_Schema_AbstractionType_ANumber)
{
    zend_class_entry ce_anumber;
    INIT_NS_CLASS_ENTRY(ce_anumber, "Zpheur\\Databases\\Voile\\Schema\\AbstractionType", "ANumber", NULL);
    zpheur_databases_voile_model_schema_abstractiontype_anumber_class_entry = zend_register_internal_class(&ce_anumber);
    zpheur_databases_voile_model_schema_abstractiontype_anumber_class_entry->ce_flags |= ZEND_ACC_NO_DYNAMIC_PROPERTIES;

    return SUCCESS;
}

ZEND_MINIT_FUNCTION(Zpheur_Databases_Voile_Schema_AbstractionType_ABoolean)
{
    zend_class_entry ce_aboolean;
    INIT_NS_CLASS_ENTRY(ce_aboolean, "Zpheur\\Databases\\Voile\\Schema\\AbstractionType", "ABoolean", NULL);
    zpheur_databases_voile_model_schema_abstractiontype_aboolean_class_entry = zend_register_internal_class(&ce_aboolean);
    zpheur_databases_voile_model_schema_abstractiontype_aboolean_class_entry->ce_flags |= ZEND_ACC_NO_DYNAMIC_PROPERTIES;
    
    return SUCCESS;
}

ZEND_MINIT_FUNCTION(Zpheur_Databases_Voile_Schema_AbstractionType_ADateTime)
{    
    zend_class_entry ce_adatetime;
    INIT_NS_CLASS_ENTRY(ce_adatetime, "Zpheur\\Databases\\Voile\\Schema\\AbstractionType", "ADateTime", NULL);
    zpheur_databases_voile_model_schema_abstractiontype_adatetime_class_entry = zend_register_internal_class(&ce_adatetime);
    zpheur_databases_voile_model_schema_abstractiontype_adatetime_class_entry->ce_flags |= ZEND_ACC_NO_DYNAMIC_PROPERTIES;

    return SUCCESS;
}

ZEND_MINIT_FUNCTION(Zpheur_Databases_Voile_Schema_AbstractionType_AArray)
{
    zend_class_entry ce_aarray;
    INIT_NS_CLASS_ENTRY(ce_aarray, "Zpheur\\Databases\\Voile\\Schema\\AbstractionType", "AArray", NULL);
    zpheur_databases_voile_model_schema_abstractiontype_aarray_class_entry = zend_register_internal_class(&ce_aarray);
    zpheur_databases_voile_model_schema_abstractiontype_aarray_class_entry->ce_flags |= ZEND_ACC_NO_DYNAMIC_PROPERTIES;

    return SUCCESS;
}

ZEND_MINIT_FUNCTION(Zpheur_Databases_Voile_Schema_AbstractionType_AObject)
{
    zend_class_entry ce_aobject;
    INIT_NS_CLASS_ENTRY(ce_aobject, "Zpheur\\Databases\\Voile\\Schema\\AbstractionType", "AObject", NULL);
    zpheur_databases_voile_model_schema_abstractiontype_aobject_class_entry = zend_register_internal_class(&ce_aobject);
    zpheur_databases_voile_model_schema_abstractiontype_aobject_class_entry->ce_flags |= ZEND_ACC_NO_DYNAMIC_PROPERTIES;

    return SUCCESS;
}

ZEND_MINIT_FUNCTION(Zpheur_Databases_Voile_Schema_AbstractionType_ADecimal128)
{
	zend_class_entry ce_adecimal128;
    INIT_NS_CLASS_ENTRY(ce_adecimal128, "Zpheur\\Databases\\Voile\\Schema\\AbstractionType", "ADecimal128", NULL);
    zpheur_databases_voile_model_schema_abstractiontype_adecimal128_class_entry = zend_register_internal_class(&ce_adecimal128);
    zpheur_databases_voile_model_schema_abstractiontype_adecimal128_class_entry->ce_flags |= ZEND_ACC_NO_DYNAMIC_PROPERTIES;

    return SUCCESS;
}