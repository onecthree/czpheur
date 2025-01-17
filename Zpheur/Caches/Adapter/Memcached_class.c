#include <php.h>
#include <ext/standard/info.h>
#include <php_zpheur.h>

#include <string.h>
#include <zpheur.h>
#include <include/runtime.h>
#include <include/onecstr.h>
#include "Memcached_arginfo.h"


zend_object_handlers cache_memcached_object_handlers;

typedef struct _cache_memcached_common_t
{
    void* TODO;
} cache_memcached_common_t;

typedef struct _cache_memcached_object
{
    cache_memcached_common_t* common;
    zend_object std;
} cache_memcached_object;

void free_cache_memcached_object(zend_object* object)
{
    cache_memcached_object* instance = ZPHEUR_GET_OBJECT(cache_memcached_object, object);

    zend_object_std_dtor(&instance->std);
    if( instance->common )
    {
        // TODO
        efree(instance->common);       
    }
}

zend_object* create_cache_memcached_object( zend_class_entry* ce )
{
    cache_memcached_object* object = 
        ecalloc(1, sizeof(cache_memcached_object) + zend_object_properties_size(ce));

    zend_object_std_init(&object->std, ce);
    object_properties_init(&object->std, ce);

    memcpy(&cache_memcached_object_handlers, zend_get_std_object_handlers(), sizeof(cache_memcached_object_handlers));
    cache_memcached_object_handlers.offset = XtOffsetOf(cache_memcached_object, std);
    cache_memcached_object_handlers.free_obj = free_cache_memcached_object;
    object->std.handlers = &cache_memcached_object_handlers;
    object->common = ecalloc(1, sizeof(cache_memcached_common_t));

    return &object->std;
}

PHP_METHOD(Cache_Memcached, __construct)
{
    char* prefix_src = NULL;
    size_t prefix_len = 0;
    zval* servers = NULL;
    zval* options = NULL;

    ZEND_PARSE_PARAMETERS_START(3, 3)
        Z_PARAM_STRING(prefix_src, prefix_len)
        Z_PARAM_ARRAY(servers)
        Z_PARAM_ARRAY(options)
    ZEND_PARSE_PARAMETERS_END();   

    zend_object* memcached_obj = php_class_init("Memcached", sizeof("Memcached") - 1);

    /* {{{ construct */
    zval* params___construct = safe_emalloc(1, sizeof(zval), 0);

    zend_string* prefix = zend_string_init(prefix_src, prefix_len, 0);
    zval _prefix; ZVAL_STR(&_prefix, prefix);
    params___construct[0] = _prefix;

    php_class_call_method(memcached_obj, "__construct", sizeof("__construct") - 1, 1, params___construct, 0);
    /* construct }}} */

    /* {{{ addServers */
    zval* params_addServers = safe_emalloc(1, sizeof(zval), 0);
    zval _servers; ZVAL_ARR(&_servers, Z_ARR_P(servers));
    params_addServers[0] = _servers;

    php_class_call_method(memcached_obj, "addServers", sizeof("addServers") - 1, 1, params_addServers, 0);
    /* addServers }}} */

    /* {{{ setOptions */
    zval* params_setOptions = safe_emalloc(1, sizeof(zval), 0);
    zval _options; ZVAL_ARR(&_options, Z_ARR_P(options));
    params_setOptions[0] = _options;

    php_class_call_method(memcached_obj, "setOptions", sizeof("setOptions") - 1, 1, params_setOptions, 0);
    /* setOptions }}} */

    zval memcached;
    ZVAL_OBJ(&memcached, memcached_obj);
    zend_this_update_property("instance", &memcached);
}

PHP_METHOD(Cache_Memcached, get)
{
    char* key_src = NULL;
    size_t key_len = 0;
    zval* default_value = NULL;

    ZEND_PARSE_PARAMETERS_START(1, 2)
        Z_PARAM_STRING(key_src, key_len)
        Z_PARAM_OPTIONAL
        Z_PARAM_ZVAL_OR_NULL(default_value)
    ZEND_PARSE_PARAMETERS_END();

    zval* instance = zend_this_read_property("instance");
    zend_object* memcached_obj = Z_OBJ_P(instance);

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

    zval retval;
    php_class_call_method_stacked(memcached_obj, "get", sizeof("get") - 1, 2, params_get, &retval, 0);

    zend_string_release(key_str);
    efree(params_get);
    /* get }}} */

    if( Z_TYPE(retval) == IS_FALSE ) {
        if(! default_value ) {
            RETURN_NULL();
        }
        RETURN_ZVAL(default_value, 0, 0);
    }

    RETURN_ZVAL(&retval, 0, 0);
}

PHP_METHOD(Cache_Memcached, set)
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

    zval* instance = zend_this_read_property("instance");
    zend_object* memcached_obj = Z_OBJ_P(instance);

    /* {{{ set */
    zval* params_set = safe_emalloc(3, sizeof(zval), 0);
    zend_string* key_str = zend_string_init(key_src, key_len, 0);
    zval key; ZVAL_STR(&key, key_str);
    params_set[0] = key;
    params_set[1] = *value;
    zval _ttl; ZVAL_LONG(&_ttl, ttl);
    params_set[2] = _ttl;

    zval retval;
    php_class_call_method_stacked(memcached_obj, "set", sizeof("set") - 1, 3, params_set, &retval, 0);

    zend_string_release(key_str);
    efree(params_set);
    /* set }}} */
    
    RETURN_ZVAL(&retval, 0, 0);
}

#define _Memcached "Memcached"
static zend_type zpheur_caches_adapter_memcached_prop_instance_type_lists( void )
{
    zend_string* memcached_class = 
        zend_string_init_interned(_Memcached, sizeof(_Memcached) - 1, 0);
    zend_alloc_ce_cache(memcached_class);

    zend_type_list* type_list = pemalloc(ZEND_TYPE_LIST_SIZE(1), 1);
    type_list->num_types = 1;
    type_list->types[0] = (zend_type)ZEND_TYPE_INIT_CLASS(memcached_class, 0, 0);

    return (zend_type)ZEND_TYPE_INIT_UNION(type_list, 0);
}

ZEND_MINIT_FUNCTION(Zpheur_Caches_Cache_Adapter_Memcached)
{
    zend_class_entry ce;

    INIT_NS_CLASS_ENTRY(ce, "Zpheur\\Caches\\Adapter", "Memcached", zpheur_caches_cache_memcached_class_method);
    zpheur_caches_adapter_memcached_class_entry = zend_register_internal_class(&ce);
    zpheur_caches_adapter_memcached_class_entry->ce_flags |= ZEND_ACC_NO_DYNAMIC_PROPERTIES;
    zpheur_caches_adapter_memcached_class_entry->create_object = create_cache_memcached_object;

    zval default_value;
    ZVAL_UNDEF(&default_value);
    zend_declare_typed_property(zpheur_caches_adapter_memcached_class_entry,
        zend_string_init("instance", sizeof("instance") - 1, 0)
        , &default_value, ZEND_ACC_PUBLIC, NULL,
        zpheur_caches_adapter_memcached_prop_instance_type_lists()
    );

    return SUCCESS;
}