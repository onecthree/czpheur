#include <php.h>
#include <ext/standard/info.h>
#include <php_zpheur.h>

#include <string.h>
#include <zpheur.h>
#include <include/runtime.h>
#include <include/onecstr.h>
#include "Table_arginfo.h"


zend_object_handlers cache_table_object_handlers;

typedef struct _cache_table_common_t
{
    void* TODO;
} cache_table_common_t;

typedef struct _cache_table_object
{
    cache_table_common_t* common;
    zend_object std;
} cache_table_object;

void free_cache_table_object(zend_object* object)
{
    cache_table_object* instance = ZPHEUR_GET_OBJECT(cache_table_object, object);

    zend_object_std_dtor(&instance->std);
    if( instance->common )
    {
        // TODO
        efree(instance->common);       
    }
}

zend_object* create_cache_table_object( zend_class_entry* ce )
{
    cache_table_object* object = 
        ecalloc(1, sizeof(cache_table_object) + zend_object_properties_size(ce));

    zend_object_std_init(&object->std, ce);
    object_properties_init(&object->std, ce);

    memcpy(&cache_table_object_handlers, zend_get_std_object_handlers(), sizeof(cache_table_object_handlers));
    cache_table_object_handlers.offset = XtOffsetOf(cache_table_object, std);
    cache_table_object_handlers.free_obj = free_cache_table_object;
    object->std.handlers = &cache_table_object_handlers;
    object->common = ecalloc(1, sizeof(cache_table_common_t));

    return &object->std;
}

PHP_METHOD(Cache_Table, __construct)
{
    // TODO
}

ZEND_MINIT_FUNCTION(Zpheur_Caches_Cache_Adapter_Table)
{
    zend_class_entry ce;

    INIT_NS_CLASS_ENTRY(ce, "Zpheur\\Caches\\Adapter", "Table", zpheur_caches_cache_table_class_method);
    zpheur_caches_adapter_table_class_entry = zend_register_internal_class(&ce);
    zpheur_caches_adapter_table_class_entry->ce_flags |= ZEND_ACC_NO_DYNAMIC_PROPERTIES;
    zpheur_caches_adapter_table_class_entry->create_object = create_cache_table_object;

    return SUCCESS;
}