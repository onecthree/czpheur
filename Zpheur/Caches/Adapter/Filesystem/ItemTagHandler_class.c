#include <php.h>
#include <ext/standard/info.h>
#include <ext/date/php_date.h>
#include <php_zpheur.h>

#include <string.h>
#include <zpheur.h>
#include <include/runtime.h>
#include "ItemTagHandler_arginfo.h"


PHP_METHOD(ItemTagAware, __construct)
{
    zval*         name;
    zval*         filename;
    zval*         ttl;
    zval*         path;
    zend_object*  datetime_object;

    ZEND_PARSE_PARAMETERS_START(4, 4)
    	Z_PARAM_ZVAL(name)
        Z_PARAM_ZVAL(filename)
        Z_PARAM_ZVAL(ttl)
        Z_PARAM_ZVAL(path)
    ZEND_PARSE_PARAMETERS_END();    

    zend_update_property(
    	Z_OBJCE_P(getThis()), Z_OBJ_P(getThis()), "name", sizeof("name") - 1, name);
    zend_update_property(
        Z_OBJCE_P(getThis()), Z_OBJ_P(getThis()), "filename", sizeof("filename") - 1, filename);
    zend_update_property(
        Z_OBJCE_P(getThis()), Z_OBJ_P(getThis()), "ttl", sizeof("ttl") - 1, ttl);
    zend_update_property(
        Z_OBJCE_P(getThis()), Z_OBJ_P(getThis()), "path", sizeof("path") - 1, path);

    char* const str_name = Z_STRVAL_P(name);
    char* const str_path = Z_STRVAL_P(path);
    char* const str_filename = Z_STRVAL_P(filename);

    size_t fullpath_len =
        snprintf(NULL, 0, "%s/%s/%s", str_path, str_name, str_filename);
    char* fullpath_src = (char*)emalloc(fullpath_len + 1);
    php_sprintf(fullpath_src, "%s/%s/%s", str_path, str_name, str_filename);
    fullpath_src[fullpath_len] = '\0';

    size_t expires_len =
        snprintf(NULL, 0, "%s/%s/expires_%s", str_path, str_name, str_filename);
    char* expires_src = (char*)emalloc(expires_len + 1);
    php_sprintf(expires_src, "%s/%s/expires_%s", str_path, str_name, str_filename);
    expires_src[expires_len] = '\0';

    itemtagaware_full_literal_path* flp = (itemtagaware_full_literal_path*)emalloc(sizeof(itemtagaware_full_literal_path));
    flp->fullpath_src = fullpath_src;
    flp->expires_src = expires_src;

    zval ptr_flp_src;
    ZVAL_PTR(&ptr_flp_src, (void*)flp);

    zend_update_property(
        Z_OBJCE_P(getThis()), Z_OBJ_P(getThis()), 
        "fileinfo", sizeof("fileinfo") - 1, &ptr_flp_src
    );
}

PHP_METHOD(ItemTagAware, datetimeObject)
{
    zend_object*         zo_datetime;

    ZEND_PARSE_PARAMETERS_START(1, 1)
        Z_PARAM_OBJ(zo_datetime)
    ZEND_PARSE_PARAMETERS_END();

    if( zend_string_equals_cstr(zo_datetime->ce->name, "DateTime", sizeof("DateTime") - 1) )
    {
        zval zv_datetime;
        ZVAL_OBJ(&zv_datetime, zo_datetime);
        zend_update_property(
            Z_OBJCE_P(getThis()), Z_OBJ_P(getThis()),
            "datetime_object", sizeof("datetime_object") - 1, 
            &zv_datetime
        );
    }
    else
    {
        zend_error(E_ERROR, "argument must be DateTime::class object");
    }

    RETURN_ZVAL(getThis(), 1, 0);
}

PHP_METHOD(ItemTagAware, expiresAfter)
{
	zval*        expires;

    ZEND_PARSE_PARAMETERS_START(1, 1)
    	Z_PARAM_ZVAL(expires)
    ZEND_PARSE_PARAMETERS_END();   

    zend_update_property(
    	Z_OBJCE_P(getThis()), Z_OBJ_P(getThis()), "expires", sizeof("expires") - 1, expires);
}

PHP_METHOD(ItemTagAware, isHit)
{
	ZEND_PARSE_PARAMETERS_NONE();

    zval* fileinfo = 
    zend_read_property(
        Z_OBJCE_P(getThis()), Z_OBJ_P(getThis()),
        "fileinfo", sizeof("fileinfo") - 1, 0, NULL);

    itemtagaware_full_literal_path* flp = (itemtagaware_full_literal_path*)fileinfo->value.ptr;


    php_stream* expires_file = php_stream_open_wrapper_ex(flp->expires_src, "r", 0, NULL, NULL);
    php_stream* fullpath_file = php_stream_open_wrapper_ex(flp->fullpath_src, "r", 0, NULL, NULL);

    if( !expires_file || !fullpath_file )
    {
        RETURN_BOOL(false);
    }

    char*   expires_buffer = (char*)malloc(4);
    size_t  expires_size   = 4;
    size_t  expires_index  = 0;
  
    char ch;
    while( (ch = php_stream_getc(expires_file)) != EOF )
    {
        if( expires_index >= expires_size )
        {
            expires_size *= 2;
            char* _expires_buffer = (char*)realloc(expires_buffer, expires_size);
            if(! _expires_buffer )
            {
                zend_error(E_ERROR, "unexpected_error: error while realloc some variable");
            }

            expires_buffer = _expires_buffer;
        }

        expires_buffer[expires_index++] = ch;
    }
    expires_buffer[expires_index] = '\0';

    zval *it_datetime = 
        zend_read_property(
            Z_OBJCE_P(getThis()), Z_OBJ_P(getThis()),
            "datetime_object", sizeof("datetime_object") - 1, 0, NULL);
    zval *base_cmp =
        php_class_call_method(Z_OBJ_P(it_datetime), "getTimestamp", sizeof("getTimestamp") - 1, 0, NULL, 0);

    zend_ulong target_cmp = 0;
    int i = 0;
    while( expires_buffer[i] != '\0' )
    {
        target_cmp *= 10;
        target_cmp += expires_buffer[i++] - '0';
    }

    if( zval_get_long(base_cmp) > target_cmp ) // expired
    {
        RETURN_FALSE;
    }

    char*   fullpath_buffer = (char*)malloc(4);
    size_t  fullpath_size   = 4;
    size_t  fullpath_index  = 0;
    while( (ch = php_stream_getc(fullpath_file)) != EOF )
    {
        if( fullpath_index >= fullpath_size )
        {
            fullpath_size *= 2;
            char* _fullpath_buffer = (char*)realloc(fullpath_buffer, fullpath_size);
            if(! _fullpath_buffer )
            {
                zend_error(E_ERROR, "unexpected_error: error while realloc some variable");
            }

            fullpath_buffer = _fullpath_buffer;
        }

        fullpath_buffer[fullpath_index++] = ch;
    }
    fullpath_buffer[fullpath_index] = '\0';

    zval get_item;
    ZVAL_STRING(&get_item, fullpath_buffer);

    zend_update_property(
        Z_OBJCE_P(getThis()), Z_OBJ_P(getThis()), "item", sizeof("item") - 1, &get_item);

    php_stream_free(
        expires_file,
        PHP_STREAM_FREE_KEEP_RSRC
        | (expires_file->is_persistent ?
           PHP_STREAM_FREE_CLOSE_PERSISTENT : PHP_STREAM_FREE_CLOSE));
    php_stream_free(
        fullpath_file,
        PHP_STREAM_FREE_KEEP_RSRC
        | (fullpath_file->is_persistent ?
           PHP_STREAM_FREE_CLOSE_PERSISTENT : PHP_STREAM_FREE_CLOSE));

    free(expires_buffer);
    free(fullpath_buffer);

    RETURN_TRUE;
}

PHP_METHOD(ItemTagAware, set)
{
    zval*     item;

    ZEND_PARSE_PARAMETERS_START(1, 1)
        Z_PARAM_ZVAL(item)
    ZEND_PARSE_PARAMETERS_END();

    zend_update_property(
        Z_OBJCE_P(getThis()), Z_OBJ_P(getThis()),
        "item", sizeof("item") - 1, item
    );
}

PHP_METHOD(ItemTagAware, get)
{
	ZEND_PARSE_PARAMETERS_NONE();

    RETURN_ZVAL(
        zend_read_property(
            Z_OBJCE_P(getThis()), Z_OBJ_P(getThis()),
            "item", sizeof("item") - 1, 0, NULL
        )
    , 0, 1);
}

PHP_METHOD(ItemTagAware, delete)
{

    ZEND_PARSE_PARAMETERS_NONE();

    php_stream_wrapper *wrapper;
    zval *zcontext = NULL;
    php_stream_context *context = NULL;

    zval* fileinfo = 
        zend_read_property(
            Z_OBJCE_P(getThis()), Z_OBJ_P(getThis()),
            "fileinfo", sizeof("fileinfo") - 1, 0, NULL);

    itemtagaware_full_literal_path* flp = (itemtagaware_full_literal_path*)fileinfo->value.ptr;

    context = php_stream_context_from_zval(zcontext, 0);
    wrapper = php_stream_locate_url_wrapper(flp->fullpath_src, NULL, 0);

    if (!wrapper || !wrapper->wops) {
        php_error_docref(NULL, E_WARNING, "Unable to locate stream wrapper");
        RETURN_FALSE;
    }
    if (!wrapper->wops->unlink) {
        php_error_docref(NULL, E_WARNING, "%s does not allow unlinking", wrapper->wops->label ? wrapper->wops->label : "Wrapper");
        RETURN_FALSE;
    }
    wrapper->wops->unlink(wrapper, flp->fullpath_src, REPORT_ERRORS, context);

    context = php_stream_context_from_zval(zcontext, 0);
    wrapper = php_stream_locate_url_wrapper(flp->expires_src, NULL, 0);
    if (!wrapper || !wrapper->wops) {
        php_error_docref(NULL, E_WARNING, "Unable to locate stream wrapper");
        RETURN_FALSE;
    }
    if (!wrapper->wops->unlink) {
        php_error_docref(NULL, E_WARNING, "%s does not allow unlinking", wrapper->wops->label ? wrapper->wops->label : "Wrapper");
        RETURN_FALSE;
    }
    wrapper->wops->unlink(wrapper, flp->expires_src, REPORT_ERRORS, context);

    RETURN_TRUE;
}

ZEND_MINIT_FUNCTION(Zpheur_Caches_Adapter_Filesystem_ItemTagHandler)
{    
    zend_class_entry ce;

    INIT_NS_CLASS_ENTRY(ce, "Zpheur\\Caches\\Adapter\\Filesystem", "ItemTagHandler", zpheur_caches_filesystem_itemtaghandler_class_method);
    zpheur_caches_filesystem_itemtaghandler_class_entry = zend_register_internal_class(&ce);
    zpheur_caches_filesystem_itemtaghandler_class_entry->ce_flags |= ZEND_ACC_NO_DYNAMIC_PROPERTIES;

    zend_declare_property_null(zpheur_caches_filesystem_itemtaghandler_class_entry, "expires", sizeof("expires") - 1, ZEND_ACC_PROTECTED);
    zend_declare_property_null(zpheur_caches_filesystem_itemtaghandler_class_entry, "ttl", sizeof("ttl") - 1, ZEND_ACC_PROTECTED);
    zend_declare_property_null(zpheur_caches_filesystem_itemtaghandler_class_entry, "filename", sizeof("filename") - 1, ZEND_ACC_PROTECTED);
    zend_declare_property_null(zpheur_caches_filesystem_itemtaghandler_class_entry, "name", sizeof("name") - 1, ZEND_ACC_PROTECTED);
    zend_declare_property_null(zpheur_caches_filesystem_itemtaghandler_class_entry, "path", sizeof("path") - 1, ZEND_ACC_PROTECTED);
    zend_declare_property_null(zpheur_caches_filesystem_itemtaghandler_class_entry, "item", sizeof("item") - 1, ZEND_ACC_PROTECTED);
    zend_declare_property_null(zpheur_caches_filesystem_itemtaghandler_class_entry, "datetime_object", sizeof("datetime_object") - 1, ZEND_ACC_PROTECTED);
    zend_declare_property_null(zpheur_caches_filesystem_itemtaghandler_class_entry, "fileinfo", sizeof("fileinfo") - 1, ZEND_ACC_PRIVATE);

    return SUCCESS;
}