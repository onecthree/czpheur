
#ifndef H_ZPHEUR_DATABASES_PHASE
#define H_ZPHEUR_DATABASES_PHASE

#include "ext/standard/info.h"
#include "ext/standard/php_standard.h"

#include "zend_API.h"
#include "zend_interfaces.h"
#include "zend_exceptions.h"

#define CONST_PHASE_DB_HOST 0
#define CONST_PHASE_DB_PORT 1
#define CONST_PHASE_DB_USERNAME 2
#define CONST_PHASE_DB_PASSWORD 3
#define CONST_PHASE_DB_NAME 4
#define CONST_PHASE_DB_DRIVER 5

#define CONST_PHASE_DRIVER_MYSQL 1
#define CONST_PHASE_DRIVER_SQLITE 2


zend_class_entry* zpheur_databases_phase_class_entry;

ZEND_BEGIN_ARG_INFO_EX(Phase___construct_arginfo, 0, 0, 2)
    ZEND_ARG_TYPE_INFO(0, uri_connection, IS_OBJECT, 0)
    ZEND_ARG_TYPE_INFO(0, configuration, IS_MIXED, 0)
ZEND_END_ARG_INFO()

PHP_METHOD(Phase, __construct);

static const zend_function_entry zpheur_databases_phase_class_method[] = {
    PHP_ME(Phase, __construct, Phase___construct_arginfo, ZEND_ACC_PUBLIC | ZEND_ACC_CTOR)
	ZEND_FE_END
};

// Define class entry
static void zpheur_databases_phase__init_class_entry()
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
}

#endif