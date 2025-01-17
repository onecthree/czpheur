#include <php.h>
#include <ext/standard/info.h>
#include <php_zpheur.h>

#include <string.h>
#include <zpheur.h>
#include <include/runtime.h>
#include <include/onecstr.h>
#include "CacheInterface_arginfo.h"


zend_object_handlers simplecache_cacheinterface_object_handlers;

typedef struct _simplecache_cacheinterface_common_t
{
 // TODO
    void* TODO;
} simplecache_cacheinterface_common_t;

typedef struct _simplecache_cacheinterface_object
{
    simplecache_cacheinterface_common_t* common;
    zend_object std;
} simplecache_cacheinterface_object;

void free_simplecache_cacheinterface_object(zend_object* object)
{
    simplecache_cacheinterface_object* instance = ZPHEUR_GET_OBJECT(simplecache_cacheinterface_object, object);

    zend_object_std_dtor(&instance->std);
    if( instance->common )
    {
        // TODO
        efree(instance->common);       
    }
}

zend_object* create_simplecache_cacheinterface_object( zend_class_entry* ce )
{
    simplecache_cacheinterface_object* object = 
        ecalloc(1, sizeof(simplecache_cacheinterface_object) + zend_object_properties_size(ce));

    zend_object_std_init(&object->std, ce);
    object_properties_init(&object->std, ce);

    memcpy(&simplecache_cacheinterface_object_handlers, zend_get_std_object_handlers(), sizeof(simplecache_cacheinterface_object_handlers));
    simplecache_cacheinterface_object_handlers.offset = XtOffsetOf(simplecache_cacheinterface_object, std);
    simplecache_cacheinterface_object_handlers.free_obj = free_simplecache_cacheinterface_object;
    object->std.handlers = &simplecache_cacheinterface_object_handlers;
    object->common = ecalloc(1, sizeof(simplecache_cacheinterface_common_t));

    return &object->std;
}

PHP_METHOD(SimpleCache_CacheInterface, init)
{
    // Interface method (empty body)
}

PHP_METHOD(SimpleCache_CacheInterface, get)
{
    // Interface method (empty body)
}

PHP_METHOD(SimpleCache_CacheInterface, set)
{
    // Interface method (empty body)
}

PHP_METHOD(SimpleCache_CacheInterface, delete)
{
    // Interface method (empty body)
}

PHP_METHOD(SimpleCache_CacheInterface, clear)
{
    // Interface method (empty body)
}

PHP_METHOD(SimpleCache_CacheInterface, getMultiple)
{
    // Interface method (empty body)
}

PHP_METHOD(SimpleCache_CacheInterface, setMultiple)
{
    // Interface method (empty body)
}

PHP_METHOD(SimpleCache_CacheInterface, deleteMultiple)
{
    // Interface method (empty body)
}

PHP_METHOD(SimpleCache_CacheInterface, has)
{
    // Interface method (empty body)
}

ZEND_MINIT_FUNCTION(Zpheur_Caches_SimpleCache_CacheInterface)
{
    zend_class_entry ce;

    INIT_NS_CLASS_ENTRY(ce, "Zpheur\\Caches\\SimpleCache", "CacheInterface", zpheur_caches_simplecache_cacheinterface_class_method);
    zpheur_caches_simplecache_cacheinterface_class_entry = zend_register_internal_class(&ce);
    zpheur_caches_simplecache_cacheinterface_class_entry->ce_flags |= ZEND_ACC_INTERFACE;
    zpheur_caches_simplecache_cacheinterface_class_entry->ce_flags |= ZEND_ACC_NO_DYNAMIC_PROPERTIES;
    zpheur_caches_simplecache_cacheinterface_class_entry->create_object = create_simplecache_cacheinterface_object;

    return SUCCESS;
}