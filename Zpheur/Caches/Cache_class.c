#include <php.h>
#include <ext/standard/info.h>
#include <php_zpheur.h>

#include <string.h>
#include <zpheur.h>
#include <include/runtime.h>
#include <include/onecstr.h>
#include "Cache_arginfo.h"


zend_object_handlers cache_object_handlers;

typedef struct _cache_common_t
{
    void* TODO;
} cache_common_t;

typedef struct _cache_object
{
    cache_common_t* common;
    zend_object std;
} cache_object;

void free_cache_object(zend_object* object)
{
    cache_object* instance = ZPHEUR_GET_OBJECT(cache_object, object);

    zend_object_std_dtor(&instance->std);
    if( instance->common )
    {
        // TODO
        efree(instance->common);       
    }
}

zend_object* create_cache_object( zend_class_entry* ce )
{
    cache_object* object = 
        ecalloc(1, sizeof(cache_object) + zend_object_properties_size(ce));

    zend_object_std_init(&object->std, ce);
    object_properties_init(&object->std, ce);

    memcpy(&cache_object_handlers, zend_get_std_object_handlers(), sizeof(cache_object_handlers));
    cache_object_handlers.offset = XtOffsetOf(cache_object, std);
    cache_object_handlers.free_obj = free_cache_object;
    object->std.handlers = &cache_object_handlers;
    object->common = ecalloc(1, sizeof(cache_common_t));

    return &object->std;
}

PHP_METHOD(Cache, __construct)
{
    // TODO
}

PHP_METHOD(Cache, __service)
{
    // TODO
}

ZEND_MINIT_FUNCTION(Zpheur_Caches_Cache)
{
    zend_class_entry ce;

    INIT_NS_CLASS_ENTRY(ce, "Zpheur\\Caches", "Cache", zpheur_caches_cache_class_method);
    zpheur_caches_cache_class_entry = zend_register_internal_class(&ce);
    zpheur_caches_cache_class_entry->ce_flags |= ZEND_ACC_NO_DYNAMIC_PROPERTIES;
    zpheur_caches_cache_class_entry->create_object = create_cache_object;

    return SUCCESS;
}