#include <php.h>
#include "ext/standard/info.h"
#include "php_zpheur.h"

#include <string.h>
#include <zpheur.h>
#include <include/runtime.h>
#include "Filesystem/ItemTagHandler_arginfo.h"
#include "Filesystem_arginfo.h"


PHP_METHOD(Filesystem, __construct)
{
    // char*        name_src = NULL;
    // size_t       name_len = 0;
    // zend_ulong   ttl = 0;
    // char*        path_src = NULL;
    // size_t       path_len = 0;
    zval*           name;
    zval*           ttl;
    zval*           path;

    ZEND_PARSE_PARAMETERS_START(3, 3)
        Z_PARAM_ZVAL(name)
        Z_PARAM_ZVAL(ttl)
        Z_PARAM_ZVAL(path)
    ZEND_PARSE_PARAMETERS_END();    

    zend_update_property(Z_OBJCE_P(getThis()), Z_OBJ_P(getThis()), "name", sizeof("name") - 1, name);
    zend_update_property(Z_OBJCE_P(getThis()), Z_OBJ_P(getThis()), "ttl", sizeof("ttl") - 1, ttl);
    zend_update_property(Z_OBJCE_P(getThis()), Z_OBJ_P(getThis()), "path", sizeof("path") - 1, path);
}

PHP_METHOD(Filesystem, datetimeObject)
{
    zval* datetime;

    ZEND_PARSE_PARAMETERS_START(1, 1)
        Z_PARAM_ZVAL(datetime)
    ZEND_PARSE_PARAMETERS_END();

    if( zend_string_equals_cstr(Z_OBJCE_P(datetime)->name, "DateTime", sizeof("DateTime") - 1) )
    {
        zend_this_update_property("datetime_object", datetime);
    }
    else
    {
        zend_error(E_ERROR, "argument must be DateTime::class object");
    }

    RETURN_ZVAL(getThis(), 1, 0);
}


PHP_METHOD(Filesystem, getItem)
{
    char*       name_src = NULL;
    size_t      name_len = 0;

    ZEND_PARSE_PARAMETERS_START(1, 1)
        Z_PARAM_STRING(name_src, name_len)
    ZEND_PARSE_PARAMETERS_END();

    zval* fs_name = 
        zend_read_property(
            Z_OBJCE_P(getThis()), Z_OBJ_P(getThis()),
            "name", sizeof("name") - 1, 0, NULL);
    zval* fs_ttl = 
        zend_read_property(
            Z_OBJCE_P(getThis()), Z_OBJ_P(getThis()),
            "ttl", sizeof("ttl") - 1, 0, NULL);
    zval* fs_path = 
        zend_read_property(
            Z_OBJCE_P(getThis()), Z_OBJ_P(getThis()),
            "path", sizeof("path") - 1, 0, NULL);
    zval* fs_datetime = 
        zend_read_property(
            Z_OBJCE_P(getThis()), Z_OBJ_P(getThis()),
            "datetime_object", sizeof("datetime_object") - 1, 0, NULL);

    zend_object* item_tag_aware =
        php_class_init("Zpheur\\Caches\\Adapter\\Filesystem\\ItemTagHandler");

    zval fs_filename;
    ZVAL_STRING(&fs_filename, name_src);

    zval* params_construct = safe_emalloc(4, sizeof(zval), 0);
    ZVAL_ZVAL(&params_construct[0], fs_name, 1, 0);
    ZVAL_ZVAL(&params_construct[1], &fs_filename, 1, 0);
    ZVAL_ZVAL(&params_construct[2], fs_ttl, 1, 0);
    ZVAL_ZVAL(&params_construct[3], fs_path, 1, 0);

    php_class_call_method(item_tag_aware, "__construct", sizeof("__construct") - 1, 4, params_construct, 0);

    zval* datetime_params = safe_emalloc(1, 0, 0);
    ZVAL_ZVAL(&datetime_params[0], fs_datetime, 1, 0);
    php_class_call_method(item_tag_aware, "datetime_object", sizeof("datetime_object") - 1, 1, datetime_params, 0);

    RETURN_OBJ(item_tag_aware);
}

PHP_METHOD(Filesystem, save)
{
    zval*       ita_object;

    ZEND_PARSE_PARAMETERS_START(1, 1)
        Z_PARAM_OBJECT_OF_CLASS_OR_NULL(ita_object, zpheur_caches_filesystem_itemtaghandler_class_entry)
    ZEND_PARSE_PARAMETERS_END();

    if(! ita_object )
    {
        zend_error(E_ERROR, "must be object");
    }

    zval* fs_ttl = 
        zend_read_property(
            Z_OBJCE_P(getThis()), Z_OBJ_P(getThis()),
            "ttl", sizeof("ttl") - 1, 0, NULL);

    zval* fs_datetime = 
        zend_read_property(
            Z_OBJCE_P(getThis()), Z_OBJ_P(getThis()),
            "datetime_object", sizeof("datetime_object") - 1, 0, NULL);

    zval* ita_fileinfo =
        zend_read_property(
            Z_OBJCE_P(ita_object), Z_OBJ_P(ita_object),
            "fileinfo", sizeof("fileinfo") - 1, 0, NULL);

    zval* ita_item =
        zend_read_property(
            Z_OBJCE_P(ita_object), Z_OBJ_P(ita_object),
            "item", sizeof("item") - 1, 0, NULL);
    zval* ita_expires =
        zend_read_property(
            Z_OBJCE_P(ita_object), Z_OBJ_P(ita_object),
            "expires", sizeof("expires") - 1, 0, NULL);

    itemtagaware_full_literal_path* flp = (itemtagaware_full_literal_path*)ita_fileinfo->value.ptr;

    php_stream*  
        fullpath_cache = php_stream_open_wrapper_ex(flp->fullpath_src, "w", 0 | REPORT_ERRORS, NULL, NULL);
        php_stream_write_string(fullpath_cache, Z_STRVAL_P(ita_item));
        php_stream_free(
            fullpath_cache,
            PHP_STREAM_FREE_KEEP_RSRC
            | (fullpath_cache->is_persistent ?
               PHP_STREAM_FREE_CLOSE_PERSISTENT : PHP_STREAM_FREE_CLOSE));

    zval* curr_timestamp =
        php_class_call_method(Z_OBJ_P(fs_datetime), "getTimestamp", sizeof("getTimestamp") - 1, 0, NULL, 0);

    zend_ulong target_exp = zval_get_long(curr_timestamp);
    target_exp += zval_get_long(Z_TYPE_P(ita_expires) == IS_LONG ? ita_expires : fs_ttl);

    size_t total_timestamp_len =
        snprintf(NULL, 0, "%ld", target_exp);
    char  total_timestamp_src[total_timestamp_len + 1];
    php_sprintf(total_timestamp_src, "%ld", target_exp);

    php_stream*  
        expires_cache = php_stream_open_wrapper_ex(flp->expires_src, "w", 0 | REPORT_ERRORS, NULL, NULL);
        php_stream_write_string(expires_cache, total_timestamp_src);
        php_stream_free(
            expires_cache,
            PHP_STREAM_FREE_KEEP_RSRC
            | (expires_cache->is_persistent ?
               PHP_STREAM_FREE_CLOSE_PERSISTENT : PHP_STREAM_FREE_CLOSE));
}

ZEND_MINIT_FUNCTION(Zpheur_Caches_Adapter_Filesystem)
{
    zend_class_entry ce;

    INIT_NS_CLASS_ENTRY(ce, "Zpheur\\Caches\\Adapter", "Filesystem", zpheur_caches_filesystem_class_method);
    zpheur_caches_filesystem_class_entry = zend_register_internal_class(&ce);
    // zpheur_caches_filesystem_class_entry->ce_flags |= ZEND_ACC_NO_DYNAMIC_PROPERTIES;

    zend_declare_property_null(zpheur_caches_filesystem_class_entry, "datetime_object", sizeof("datetime_object") - 1, ZEND_ACC_PROTECTED);
    zend_declare_property_null(zpheur_caches_filesystem_class_entry, "name", sizeof("name") - 1, ZEND_ACC_PROTECTED);
    zend_declare_property_null(zpheur_caches_filesystem_class_entry, "ttl", sizeof("ttl") - 1, ZEND_ACC_PROTECTED);
    zend_declare_property_null(zpheur_caches_filesystem_class_entry, "path", sizeof("path") - 1, ZEND_ACC_PROTECTED);

    return SUCCESS;
}