#include <php.h>
#include <ext/standard/info.h>
#include <php_zpheur.h>

#include <string.h>
#include <zpheur.h>
#include <include/runtime.h>
#include <include/onecstr.h>
#include "SimpleCache_arginfo.h"
#include <Zpheur/Caches/SimpleCache/CacheInterface_arginfo.h>
#include <Zpheur/Caches/Adapter/Memcached_arginfo.h>
#include <Zpheur/Caches/Adapter/Table_arginfo.h>


zend_object_handlers simplecache_object_handlers;

typedef struct _simplecache_common_t
{
    void* TODO;
} simplecache_common_t;

typedef struct _simplecache_object
{
    simplecache_common_t* common;
    zend_object std;
} simplecache_object;

void free_simplecache_object(zend_object* object)
{
    simplecache_object* instance = ZPHEUR_GET_OBJECT(simplecache_object, object);

    zend_object_std_dtor(&instance->std);
    if( instance->common )
    {
        // TODO
        efree(instance->common);       
    }
}

zend_object* create_simplecache_object( zend_class_entry* ce )
{
    simplecache_object* object = 
        ecalloc(1, sizeof(simplecache_object) + zend_object_properties_size(ce));

    zend_object_std_init(&object->std, ce);
    object_properties_init(&object->std, ce);

    memcpy(&simplecache_object_handlers, zend_get_std_object_handlers(), sizeof(simplecache_object_handlers));
    simplecache_object_handlers.offset = XtOffsetOf(simplecache_object, std);
    simplecache_object_handlers.free_obj = free_simplecache_object;
    object->std.handlers = &simplecache_object_handlers;
    object->common = ecalloc(1, sizeof(simplecache_common_t));

    return &object->std;
}

PHP_METHOD(SimpleCache, get)
{
    char* key_src = NULL;
    size_t key_len = 0;
    zval* default_value = NULL;

    ZEND_PARSE_PARAMETERS_START(1, 2)
        Z_PARAM_STRING(key_src, key_len)
        Z_PARAM_OPTIONAL
        Z_PARAM_ZVAL_OR_NULL(default_value)
    ZEND_PARSE_PARAMETERS_END();

    zend_class_entry* ce = zend_get_called_scope(execute_data);
    zval* instance = zend_static_read_property(ce, "instance");
    zend_object* instance_obj = Z_OBJ_P(instance);

    /* {{{ get */
    zval* params_get = safe_emalloc(2, sizeof(zval), 0);
    zend_string* key_str = zend_string_init(key_src, key_len, 0);
    zval key; ZVAL_STR(&key, key_str);
    params_get[0] = key;

    if(! default_value ) {
        zval _default_value; ZVAL_NULL(&_default_value);
        params_get[1] = _default_value;
    } else {
        params_get[1] = *default_value;   
    }

    // zval* retval = 
    zval retval;
    php_class_call_method_stacked(instance_obj, "get", sizeof("get") - 1, 2, params_get, &retval, 0);

    zend_string_release(key_str);
    efree(params_get);
    /* get }}} */    

    if( Z_TYPE(retval) == IS_NULL ) {
        RETURN_NULL();
    }

    RETURN_ZVAL(&retval, 0, 0);
}

PHP_METHOD(SimpleCache, set)
{
    char* key_src = NULL;
    size_t key_len = 0;
    zval* value = NULL;
    zend_long ttl = 0;

    ZEND_PARSE_PARAMETERS_START(2, 3)
        Z_PARAM_STRING(key_src, key_len)
        Z_PARAM_ZVAL(value)
        Z_PARAM_OPTIONAL
        Z_PARAM_LONG(ttl)
    ZEND_PARSE_PARAMETERS_END();

    zend_class_entry* ce = zend_get_called_scope(execute_data);
    zval* instance = zend_static_read_property(ce, "instance");
    zend_object* instance_obj = Z_OBJ_P(instance);

    /* {{{ set */
    zval* params_set = safe_emalloc(3, sizeof(zval), 0);
    zend_string* key_str = zend_string_init(key_src, key_len, 0);
    zval key; ZVAL_STR(&key, key_str);
    params_set[0] = key;
    params_set[1] = *value;
    zval _ttl; ZVAL_LONG(&_ttl, ttl);
    params_set[2] = _ttl;

    // zval* retval = 
    zval retval;
    php_class_call_method_stacked(instance_obj, "set", sizeof("set") - 1, 3, params_set, &retval, 0);

    zend_string_release(key_str);
    efree(params_set);
    /* set }}} */
    
    RETURN_ZVAL(&retval, 0, 0);
}

#define _Cache_Memcached "Zpheur\\Caches\\Adapter\\Memcached"
#define _Cache_Table "Zpheur\\Caches\\Adapter\\Table"
static zend_type zpheur_caches_simplecache_prop_instance_type_lists( void )
{
    zend_string* memcached_class = 
        zend_string_init_interned(_Cache_Memcached, sizeof(_Cache_Memcached) - 1, 0);
    zend_alloc_ce_cache(memcached_class);
    zend_string* table_class = 
        zend_string_init_interned(_Cache_Table, sizeof(_Cache_Table) - 1, 0);
    zend_alloc_ce_cache(table_class);

    zend_type_list* type_list = pemalloc(ZEND_TYPE_LIST_SIZE(2), 1);
    type_list->num_types = 2;
    type_list->types[0] = (zend_type)ZEND_TYPE_INIT_CLASS(memcached_class, 0, 0);
    type_list->types[1] = (zend_type)ZEND_TYPE_INIT_CLASS(table_class, 0, 0);

    return (zend_type)ZEND_TYPE_INIT_UNION(type_list, 0);
}

ZEND_MINIT_FUNCTION(Zpheur_Caches_SimpleCache)
{
    zend_class_entry ce;

    INIT_NS_CLASS_ENTRY(ce, "Zpheur\\Caches", "SimpleCache", zpheur_caches_simplecache_class_method);
    zpheur_caches_simplecache_class_entry = zend_register_internal_class(&ce);
    zpheur_caches_simplecache_class_entry->ce_flags |= ZEND_ACC_NO_DYNAMIC_PROPERTIES;
    zpheur_caches_simplecache_class_entry->create_object = create_simplecache_object;
    zend_class_implements_interface(zpheur_caches_simplecache_class_entry, 
        zpheur_caches_simplecache_cacheinterface_class_entry
    );

    zval default_value;
    ZVAL_UNDEF(&default_value);
    zend_declare_typed_property(zpheur_caches_simplecache_class_entry,
        zend_string_init("instance", sizeof("instance") - 1, 0)
        , &default_value, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC, NULL,
        zpheur_caches_simplecache_prop_instance_type_lists()
    );

    return SUCCESS;
}