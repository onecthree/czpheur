#include <php.h>
#include <ext/standard/info.h>
#include <php_zpheur.h>
#include <Zend/zend_attributes.h>
#include <ext/spl/spl_iterators.h>
#include <Zend/zend_interfaces.h>

#include <string.h>
#include <include/runtime.h>
#include <include/regexp.h>
#include "Model_arginfo.h"

#define SQLT_CHAR 0
#define SQLT_VARCHAR 1
#define SQLT_INT 2

typedef struct
{
    char *name;
    uint8_t sql_type;
    bool is_primary;
    bool is_unique;

} column_schema;

typedef struct
{
    zval    model_ns;
    zval    query;
    zval    table;
    zval    pdo;
} phase;

PHP_METHOD(PhaseModel, __construct)
{
    char *model_ns = NULL;
    size_t model_ns_len = 0;

    ZEND_PARSE_PARAMETERS_START(0, 1)
        Z_PARAM_OPTIONAL
        Z_PARAM_STRING(model_ns, model_ns_len)
    ZEND_PARSE_PARAMETERS_END();

    if( !model_ns_len ) // is zero
    {
        // Fetch class name contexts
        zend_class_entry *class_model_scope = zend_get_called_scope(execute_data);
        model_ns = class_model_scope->name->val;
    }

    int skip = 0;
    HashTable *prop_obj = zend_std_get_properties( Z_OBJ_P(getThis()) );
    ZEND_HASH_FOREACH_STR_KEY(prop_obj, zend_string *key)
    {
        if( skip < 1 )
        {
            skip = 1;
            continue;
        }

        zend_unset_property(Z_OBJCE_P(getThis()), Z_OBJ_P(getThis()), key->val, key->len);
    } ZEND_HASH_FOREACH_END();

    phase *phase_info = emalloc(sizeof(phase));

    zend_object *o_model_ns_class = php_class_init(model_ns);
    HashTable *ht_cm_attributes = o_model_ns_class->ce->attributes;

    zval zv_model_ns;
    ZVAL_STRING(&zv_model_ns, model_ns);
    phase_info->model_ns = zv_model_ns;

    // fetch attribute class model
    zval *z_attribute = o_model_ns_class->ce->attributes->arPacked;
    zend_attribute *za_attr = z_attribute->value.ptr;

    // iterate all attribute scope, and check class specific for \Collection
    for( int indAttrOf = 0; indAttrOf < za_attr->argc; ++indAttrOf )
    {
        if(
            zend_string_equals_cstr(
                    za_attr[indAttrOf].name,
                    "Zpheur\\Databases\\Phase\\Schema\\Table",
                    sizeof("Zpheur\\Databases\\Phase\\Schema\\Table") - 1
            )
        )
        {
            phase_info->table = za_attr[indAttrOf].args[0].value;
        }
    }

    zval zv_query;
    ZVAL_STRING(&zv_query, "");

    phase_info->query = zv_query;

    zval *zv_config =
        zend_hash_find_ind(
                &EG(symbol_table),
                zend_string_init("phaseDbState", sizeof("phaseDbState") - 1, 0)
        );

    zval *z_db_host = 
        zend_read_property(Z_OBJCE_P(zv_config), Z_OBJ_P(zv_config), 
            "database_host", sizeof("database_host") - 1, false, NULL);
    zval *z_db_port = 
        zend_read_property(Z_OBJCE_P(zv_config), Z_OBJ_P(zv_config), 
            "database_port", sizeof("database_port") - 1, false, NULL);
    zval *z_db_username = 
        zend_read_property(Z_OBJCE_P(zv_config), Z_OBJ_P(zv_config), 
            "database_username", sizeof("database_username") - 1, false, NULL);
    zval *z_db_password = 
        zend_read_property(Z_OBJCE_P(zv_config), Z_OBJ_P(zv_config), 
            "database_password", sizeof("database_password") - 1, false, NULL);
    zval *z_db_name = 
        zend_read_property(Z_OBJCE_P(zv_config), Z_OBJ_P(zv_config), 
            "database_name", sizeof("database_name") - 1, false, NULL);
    zval *z_set_conn =
        zend_read_property(Z_OBJCE_P(zv_config), Z_OBJ_P(zv_config), 
            "database_set_conn", sizeof("database_set_conn") - 1, false, NULL);

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

    zval zv_pdo;
    ZVAL_OBJ(&zv_pdo, z_pdo);

    phase_info->query = zv_pdo;

    zval zv_phase_info;
    ZVAL_PTR(&zv_phase_info, (void*)phase_info);

    zend_update_property(NULL, Z_OBJ_P(getThis()), "phase", sizeof("phase") - 1, &zv_phase_info);
}

PHP_METHOD(PhaseModel, migration)
{
    ZEND_PARSE_PARAMETERS_NONE();

    zend_class_entry *model_ns = zend_get_called_scope(execute_data);

    zend_object *PhaseModel_scope = php_class_init("Zpheur\\Databases\\Phase\\Model");

    zval *PhaseModel___construct_params = safe_emalloc(1, sizeof(zval), 0);
    ZVAL_STRING(&PhaseModel___construct_params[0], ZSTR_VAL(model_ns->name));

    php_class_call_method(
        PhaseModel_scope, "__construct", sizeof("__construct") - 1, 1, PhaseModel___construct_params, true);
}

PHP_METHOD(PhaseModel, all)
{
    ZEND_PARSE_PARAMETERS_NONE();
}

ZEND_MINIT_FUNCTION(Zpheur_Databases_Phase_Model)
{
    zend_class_entry ce;

    INIT_NS_CLASS_ENTRY(ce, "Zpheur\\Databases\\Phase", "Model", zpheur_databases_phase_model_class_method);
    zpheur_databases_phase_model_class_entry = zend_register_internal_class(&ce);
    zpheur_databases_phase_model_class_entry->ce_flags |= ZEND_ACC_ALLOW_DYNAMIC_PROPERTIES;

    zend_declare_property_null(zpheur_databases_phase_model_class_entry, "phase", sizeof("phase") - 1, ZEND_ACC_PROTECTED);

    return SUCCESS;
}