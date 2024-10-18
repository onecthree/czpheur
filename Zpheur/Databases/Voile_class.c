#include <php.h>
#include <ext/standard/info.h>
#include <php_zpheur.h>
#include <Zend/zend_exceptions.h>

#include <string.h>
#include <include/runtime.h>
#include "Voile_arginfo.h"


PHP_METHOD(Voile, __construct)
{
    // zval *z_mongodb_manager;
    zval *z_configuration;

    ZEND_PARSE_PARAMETERS_START(1, 1) // 2, 2
        // Z_PARAM_ZVAL(z_mongodb_manager)
        Z_PARAM_ARRAY(z_configuration)
    ZEND_PARSE_PARAMETERS_END();


    /* Get URI connection */
    zval *db_uri = 
        zend_hash_index_find(
            Z_ARRVAL_P(z_configuration),
            CONST_VOILE_DB_URI
        );

    /* Get database name */
    zval *db_name = 
        zend_hash_index_find(
            Z_ARRVAL_P(z_configuration),
            CONST_VOILE_DB_NAME
        );

    /* Update self-properties */
    zend_update_property(Z_OBJCE_P(getThis()), Z_OBJ_P(getThis()), "database_uri", sizeof("database_uri") - 1, db_uri);
    zend_update_property(Z_OBJCE_P(getThis()), Z_OBJ_P(getThis()), "database_name", sizeof("database_name") - 1, db_name);

    /* Test connection check */
    /* Check if connection was accessible */
    zend_object *zo_manager =
            php_class_init("MongoDB\\Driver\\Manager");
    php_class_call_method(zo_manager, "__construct", sizeof("__construct") - 1, 1, db_uri, false);

    zval *zv_param_command = safe_emalloc(1, sizeof(zval), 0);
    HashTable *ht_param;
    ALLOC_HASHTABLE(ht_param);
    zend_hash_init(ht_param, 0, NULL, ZVAL_PTR_DTOR, 0);

    zval _ping;
    ZVAL_LONG(&_ping, 1);
    zend_hash_str_update(ht_param, "ping", sizeof("ping") - 1, &_ping);
    ZVAL_ARR(&zv_param_command[0], ht_param);

    zend_object *z_command = php_class_init("MongoDB\\Driver\\Command");
    php_class_call_method(z_command, "__construct", sizeof("__construct") - 1, 1, zv_param_command,false);

    zval *zv_param_exec_command = safe_emalloc(2, sizeof(zval), 0);
    ZVAL_STRING(&zv_param_exec_command[0], "admin");
    ZVAL_OBJ(&zv_param_exec_command[1], z_command);

    php_class_call_method(zo_manager, "executeCommand", sizeof("executeCommand") - 1, 2, zv_param_exec_command, false);

    /* If not, throw error */
    if( EG(exception) )
    { 
        // An exception was thrown earlier in the code
        zend_object* exception_obj = EG(exception);
        zval* exception_msg = zend_read_property(zend_ce_exception, exception_obj, "message", sizeof("message") - 1, 1, NULL);

        // Clear the exception
        zend_clear_exception();

        // zend_throw_error(zend_ce_exception, "%s", Z_STRVAL_P(exception_msg));
        zend_error(E_ERROR, "%s", Z_STRVAL_P(exception_msg));
    }
    /* End test check */
}

ZEND_MINIT_FUNCTION(Zpheur_Databases_Voile)
{
    zend_class_entry ce;

    INIT_NS_CLASS_ENTRY(ce, "Zpheur\\Databases", "Voile", zpheur_databases_voile_class_method);
    zpheur_databases_voile_class_entry = zend_register_internal_class(&ce);
    zpheur_databases_voile_class_entry->ce_flags |= ZEND_ACC_NO_DYNAMIC_PROPERTIES;

    zend_declare_property_null(zpheur_databases_voile_class_entry, "database_name", sizeof("database_name") - 1, ZEND_ACC_PUBLIC);
    zend_declare_property_null(zpheur_databases_voile_class_entry, "database_uri", sizeof("database_uri") - 1, ZEND_ACC_PUBLIC);

    zend_declare_class_constant_long(
        zpheur_databases_voile_class_entry,
        "DB_URI",
        sizeof("DB_URI") - 1,
        CONST_VOILE_DB_URI
    );

    zend_declare_class_constant_long(
        zpheur_databases_voile_class_entry,
        "DB_NAME",
        sizeof("DB_NAME") - 1,
        CONST_VOILE_DB_NAME
    );

    return SUCCESS;
}