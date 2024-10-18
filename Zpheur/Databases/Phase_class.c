#include <php.h>
#include <ext/standard/info.h>
#include <php_zpheur.h>
#include <Zend/zend_exceptions.h>

#include <string.h>
#include <include/runtime.h>
#include "Phase_arginfo.h"


PHP_METHOD(Phase, __construct)
{
    zval *z_array_config;

    ZEND_PARSE_PARAMETERS_START(1, 1)
        Z_PARAM_ARRAY(z_array_config)
    ZEND_PARSE_PARAMETERS_END();

    zval *z_db_host = 
        zend_hash_index_find(Z_ARR_P(z_array_config), CONST_PHASE_DB_HOST);
    zval *z_db_port = 
        zend_hash_index_find(Z_ARR_P(z_array_config), CONST_PHASE_DB_PORT);
    zval *z_db_username = 
        zend_hash_index_find(Z_ARR_P(z_array_config), CONST_PHASE_DB_USERNAME);
    zval *z_db_password = 
        zend_hash_index_find(Z_ARR_P(z_array_config), CONST_PHASE_DB_PASSWORD);
    zval *z_db_name = 
        zend_hash_index_find(Z_ARR_P(z_array_config), CONST_PHASE_DB_NAME);

    zval *z_db_driver =
        zend_hash_index_find(Z_ARR_P(z_array_config), CONST_PHASE_DB_DRIVER);

    // Declare new PDO class to check connection info was valid
    zend_object *z_pdo = php_class_init("PDO");
    zval *z_params_pdo = safe_emalloc(3, sizeof(zval), 0);

    int conn_info_len = snprintf(NULL, 0, "mysql:host=%s;port=%ld;dbname=%s", 
        Z_STRVAL_P(z_db_host),
        zval_get_long(z_db_port),
        Z_STRVAL_P(z_db_name)
    );

    char conn_info[conn_info_len];
    php_sprintf(conn_info, "mysql:host=%s;port=%ld;dbname=%s", 
        Z_STRVAL_P(z_db_host),
        zval_get_long(z_db_port),
        Z_STRVAL_P(z_db_name)
    );

    ZVAL_STRING(&z_params_pdo[0], conn_info);
    ZVAL_STRING(&z_params_pdo[1], Z_STRVAL_P(z_db_username));
    ZVAL_STRING(&z_params_pdo[2], Z_STRVAL_P(z_db_password));

    php_class_call_method(z_pdo, "__construct", sizeof("__construct") - 1, 3, z_params_pdo, false);
    

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

    zend_update_property(
        Z_OBJCE_P(getThis()), Z_OBJ_P(getThis()),
        "database_host", sizeof("database_host") - 1,
        z_db_host
    );

    zend_update_property(
        Z_OBJCE_P(getThis()), Z_OBJ_P(getThis()),
        "database_port", sizeof("database_port") - 1,
        z_db_port
    );

    zend_update_property(
        Z_OBJCE_P(getThis()), Z_OBJ_P(getThis()),
        "database_name", sizeof("database_name") - 1,
        z_db_name
    );

    zend_update_property(
        Z_OBJCE_P(getThis()), Z_OBJ_P(getThis()),
        "database_set_conn", sizeof("database_set_conn") - 1,
        &z_params_pdo[0]
    );

    zend_update_property(
        Z_OBJCE_P(getThis()), Z_OBJ_P(getThis()),
        "database_username", sizeof("database_username") - 1,
        &z_params_pdo[1]
    );

    zend_update_property(
        Z_OBJCE_P(getThis()), Z_OBJ_P(getThis()),
        "database_password", sizeof("database_password") - 1,
        &z_params_pdo[2]
    );
}

ZEND_MINIT_FUNCTION(Zpheur_Databases_Phase)
{
    zend_class_entry ce;

    INIT_NS_CLASS_ENTRY(ce, "Zpheur\\Databases", "Phase", zpheur_databases_phase_class_method);
    zpheur_databases_phase_class_entry = zend_register_internal_class(&ce);
    zpheur_databases_phase_class_entry->ce_flags |= ZEND_ACC_NO_DYNAMIC_PROPERTIES;

    zend_declare_property_null(zpheur_databases_phase_class_entry, "database_host", sizeof("database_host") - 1, ZEND_ACC_PUBLIC);
    zend_declare_property_null(zpheur_databases_phase_class_entry, "database_port", sizeof("database_port") - 1, ZEND_ACC_PUBLIC);
    zend_declare_property_null(zpheur_databases_phase_class_entry, "database_name", sizeof("database_name") - 1, ZEND_ACC_PUBLIC);
    zend_declare_property_null(zpheur_databases_phase_class_entry, "database_set_conn", sizeof("database_set_conn") - 1, ZEND_ACC_PUBLIC);
    zend_declare_property_null(zpheur_databases_phase_class_entry, "database_username", sizeof("database_username") - 1, ZEND_ACC_PUBLIC);
    zend_declare_property_null(zpheur_databases_phase_class_entry, "database_password", sizeof("database_password") - 1, ZEND_ACC_PUBLIC);

    zend_declare_class_constant_long(
        zpheur_databases_phase_class_entry,
        "DB_HOST",
        sizeof("DB_HOST") - 1,
        CONST_PHASE_DB_HOST
    );

    zend_declare_class_constant_long(
        zpheur_databases_phase_class_entry,
        "DB_PORT",
        sizeof("DB_PORT") - 1,
        CONST_PHASE_DB_PORT
    );

    zend_declare_class_constant_long(
        zpheur_databases_phase_class_entry,
        "DB_USERNAME",
        sizeof("DB_USERNAME") - 1,
        CONST_PHASE_DB_USERNAME
    );

    zend_declare_class_constant_long(
        zpheur_databases_phase_class_entry,
        "DB_PASSWORD",
        sizeof("DB_PASSWORD") - 1,
        CONST_PHASE_DB_PASSWORD
    );

    zend_declare_class_constant_long(
        zpheur_databases_phase_class_entry,
        "DB_NAME",
        sizeof("DB_NAME") - 1,
        CONST_PHASE_DB_NAME
    );

    zend_declare_class_constant_long(
        zpheur_databases_phase_class_entry,
        "DB_DRIVER",
        sizeof("DB_DRIVER") - 1,
        CONST_PHASE_DB_DRIVER
    );

    // Driver adapter for various SQL version
    zend_declare_class_constant_long(
        zpheur_databases_phase_class_entry,
        "SQL_DRIVER_MYSQL",
        sizeof("SQL_DRIVER_MYSQL") - 1,
        CONST_PHASE_DRIVER_MYSQL
    );

    return SUCCESS;
}