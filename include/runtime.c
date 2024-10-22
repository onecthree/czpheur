#include "php.h"
#include "php_main.h"
#include "ext/standard/info.h"
#include "php_zpheur.h"
#include <zpheur.h>
#include "runtime.h"

#include "ext/spl/php_spl.h"
#include "ext/spl/spl_functions.h"
// #include "ext/spl/spl_engine.h"
#include "ext/spl/spl_array.h"
#include "ext/spl/spl_directory.h"
#include "ext/spl/spl_iterators.h"
#include "ext/spl/spl_exceptions.h"
#include "ext/spl/spl_observer.h"
#include "ext/spl/spl_dllist.h"
#include "ext/spl/spl_fixedarray.h"
#include "ext/spl/spl_heap.h"
#include "zend_exceptions.h"
#include "zend_interfaces.h"

zval *php_call_enum_method( zend_class_entry *enum_ce, const char *function_name, unsigned int param_counts, zval *params )
{
    zval fun_str;
    ZVAL_STRING(&fun_str, function_name);
    zend_fcall_info fci;
    zend_fcall_info_cache fci_cache;

    zend_string* function_name_str = zend_string_init(function_name, strlen(function_name), 0);
    zend_function* func = zend_hash_find_ptr(&enum_ce->function_table, function_name_str);

    if (func == NULL) {
        zend_string_release(function_name_str);
        return NULL; // Function not found
    }

    zval *retval = emalloc(sizeof(zval));

    fci.size = sizeof(fci);
    fci.function_name = fun_str;
    fci.retval = &*retval;
    fci.param_count = param_counts;
    fci.params = params;
    fci.object = NULL;
    // fci.no_separation = 0;
    fci.named_params = NULL;

    // fci_cache.initialized = 1;
    fci_cache.called_scope = NULL;
    fci_cache.calling_scope = NULL;
    fci_cache.function_handler = func;

    // zend_call_function(&fci, &fci_cache);

    if ( zend_call_function(&fci, &fci_cache) == SUCCESS )
    {
        zend_string_release(function_name_str);
        return retval;
    }

    return NULL; // Error calling the function
}

zval *php_call_function( const char *function_name, unsigned int param_counts, zval *params )
{
    zval fun_str;
    ZVAL_STRING(&fun_str, function_name);
    zend_fcall_info fci;
    zend_fcall_info_cache fci_cache;

    zend_string* function_name_str = zend_string_init(function_name, strlen(function_name), 0);
    zend_function* func = zend_hash_find_ptr(EG(function_table), function_name_str);

    if (func == NULL) {
        zend_string_release(function_name_str);
        return NULL; // Function not found
    }

    zval *retval = emalloc(sizeof(zval));

    fci.size = sizeof(fci);
    fci.function_name = fun_str;
    fci.retval = &*retval;
    fci.param_count = param_counts;
    fci.params = params;
    fci.object = NULL;
    // fci.no_separation = 0;
    fci.named_params = NULL;

    // fci_cache.initialized = 1;
    fci_cache.called_scope = NULL;
    fci_cache.calling_scope = NULL;
    fci_cache.function_handler = func;

    // zend_call_function(&fci, &fci_cache);

    if ( zend_call_function(&fci, &fci_cache) == SUCCESS )
    {
        zend_string_release(function_name_str);
        return retval;
    }
    return NULL; // Error calling the function
}

zval *php_call_static_method_2( zend_object *obj, const char *function_name, unsigned int param_counts, zval *params )
{
    zval fun_str;
    ZVAL_STRING(&fun_str, function_name);
    zend_fcall_info fci;
    zend_fcall_info_cache fci_cache;

    zend_string* function_name_str = zend_string_init(function_name, strlen(function_name), 0);
    zend_function* func;

    ZEND_HASH_FOREACH_PTR(&obj->ce->function_table, zend_function *funcs)
    {
        if (funcs->common.fn_flags & ZEND_ACC_PUBLIC) {
            if( strcmp(funcs->common.function_name->val, function_name) == 0 )
            {
                func = funcs;
            }
        }
    }
    ZEND_HASH_FOREACH_END();

    // return NULL;

    if (func == NULL) {
        zend_string_release(function_name_str);
        php_printf("ga ada");
        return NULL; // Function not found
    }

    zval *retval = emalloc(sizeof(zval));

    fci.size = sizeof(fci);
    fci.function_name = fun_str;
    // fci.retval = &(*retval);
    fci.param_count = param_counts;
    fci.params = params;
    fci.object = NULL;
    // fci.no_separation = 0;
    fci.named_params = NULL;

    // fci_cache.initialized = 1;
    fci_cache.called_scope = NULL;
    fci_cache.calling_scope = NULL;
    fci_cache.function_handler = func;
    fci_cache.object = obj;

    zend_call_function(&fci, &fci_cache);

    // if ( zend_call_function(&fci, &fci_cache) == SUCCESS )
    // {
    //     // zend_string_release(function_name_str);
    //     return retval;
    // }
    return NULL; // Error calling the function
}

//int php_execute_file( char const *file_path, int flag_mode )
//{
//    zend_string *zs_file_path = zend_string_init(file_path, strlen(file_path), 0);
//    zval dummy;
//    zend_file_handle file_handle;
//    zend_op_array *new_op_array;
//    zval result;
//    int ret;
//
////    zend_stream_init_filename_ex(&file_handle, class_file);
////    ret = php_stream_open_for_zend_ex(&file_handle, USE_PATH|STREAM_OPEN_FOR_INCLUDE);
//    file_handle.type = ZEND_HANDLE_FILENAME;
//    file_handle.filename = zs_file_path;
//
//    zend_string *opened_path;
//    if (!file_handle.opened_path) {
//        file_handle.opened_path = zend_string_copy(zs_file_path);
//    }
//
//    opened_path = zend_string_copy(file_handle.opened_path);
//
//    ZVAL_NULL(&dummy);
//    if (zend_hash_add(&EG(included_files), opened_path, &dummy)) {
//        switch( flag_mode )
//        {
//            case REQUIRE:
//                new_op_array = zend_compile_file(&file_handle, ZEND_REQUIRE);
//            break;
//            case REQUIRE_ONCE:
//                new_op_array = zend_compile_file(&file_handle, ZEND_REQUIRE_ONCE);
//            break;
//            case INCLUDE:
//                new_op_array = zend_compile_file(&file_handle, ZEND_INCLUDE);
//            break;
//            case INCLUDE_ONCE:
//                new_op_array = zend_compile_file(&file_handle, ZEND_INCLUDE_ONCE);
//            break;
//        }
//    } else {
//        new_op_array = NULL;
//    }
//
//    zend_string_release_ex(opened_path, 0);
//    if (new_op_array) {
//        uint32_t orig_jit_trace_num = EG(jit_trace_num);
//
//        ZVAL_UNDEF(&result);
//        zend_execute(new_op_array, &result);
//        EG(jit_trace_num) = orig_jit_trace_num;
//
//        destroy_op_array(new_op_array);
//        efree(new_op_array);
//        if (!EG(exception)) {
//            zval_ptr_dtor(&result);
//        }
//
//        zend_destroy_file_handle(&file_handle);
//        zend_string_release(zs_file_path);
//
//        return 0;
//    }
//
//    zend_destroy_file_handle(&file_handle);
//    zend_string_release(zs_file_path);
//
//    return 0;
//}

void php_execute_file( char const *filepath, int flag_mode, zend_op_array* t_op_array )
{
    zend_string *file_path = zend_string_init(filepath, strlen(filepath), 0);

    zend_file_handle file_handle;
    file_handle.type = ZEND_HANDLE_FILENAME;
    file_handle.filename = file_path;
    file_handle.opened_path = NULL;

    zend_op_array* op_array = t_op_array;

    switch( flag_mode )
    {
        case 0: op_array = zend_compile_file(&file_handle, ZEND_REQUIRE); break;
        case 1: op_array = zend_compile_file(&file_handle, ZEND_REQUIRE_ONCE); break;
        case 2: op_array = zend_compile_file(&file_handle, ZEND_INCLUDE); break;
        case 3: op_array = zend_compile_file(&file_handle, ZEND_INCLUDE_ONCE); break;
    }

    if( op_array != NULL )
    {
        zend_execute(op_array, NULL);
        destroy_op_array(op_array);
        efree(op_array);
    }

    zend_destroy_file_handle(&file_handle);
    zend_string_release(file_path);
}

zval* php_call_closure( zval *z_closure, zend_object *o_closure, unsigned int param_counts, zval *params )
{
    zend_fcall_info fci;
    zend_fcall_info_cache fci_cache;
    zval *retval = emalloc(sizeof(zval));

    fci.function_name = *z_closure;
    fci.param_count = param_counts;
    fci.params = params;
    fci.retval = &(*retval);
    fci.size = sizeof(fci);
    fci.object = o_closure;
    fci.named_params = NULL;

    fci_cache.function_handler = NULL;
    fci_cache.called_scope = NULL;
    fci_cache.calling_scope = NULL;
    fci_cache.object = NULL;

    if( zend_call_function(&fci, &fci_cache) == SUCCESS )
    {
        return retval;
    }
    return NULL;
}

inline static zend_class_entry *php_class_exists( zend_string *class_name )
{
    zend_class_entry* class_entry;
    return zend_lookup_class(class_name);
}

zend_class_entry *php_enum_init( char const* class_name )
{
    zend_class_entry *ce;
    zend_string *zstr_class_name;
    zstr_class_name = zend_string_init(class_name, strlen(class_name), 0);

    // Get the class entry
    ce = zend_lookup_class(zstr_class_name);
    
    // Check if the class exists
    if( ce == NULL )
    {
        php_error_docref(
            NULL,
            E_ERROR, 
            "Enum \"%s\" not found",
            class_name
        );
    }

    return ce;
}

void php_call_static_method(
        zend_object *obj,
        const char *method_name, 
        size_t method_name_length, 
        int num_params, 
        zval* params,
        zval* retval
    ) 
{
    zval function_name;
    ZVAL_STRINGL(&function_name, method_name, method_name_length);

    zval zv_obj;
    ZVAL_OBJ(&zv_obj, obj);

    call_user_function(NULL, &zv_obj, &function_name, retval, num_params, params);
    // zend_call_known_function(NULL, obj, obj->ce, retval, num_params, params, NULL);

    // zval_ptr_dtor(&obj);
    // zval_ptr_dtor(&function_name);
}

zend_object *php_class_init_self_ex( char const* class_name )
{
    zend_class_entry *ce;
    zend_string *zstr_class_name;
    zstr_class_name = zend_string_init(class_name, strlen(class_name), 0);

    // Get the class entry
    ce = zend_lookup_class(zstr_class_name);
    
    // Check if the class exists
    if( ce == NULL )
        return NULL;

    ce->parent = NULL;
    ce->destructor = NULL;

    zval obj;
    object_init_ex(&obj, ce);
    zend_object *object = Z_OBJ_P(&obj);

    return object;
}

zend_object *php_class_init_ex( char const* class_name )
{
    zend_class_entry *ce;
    zend_string *zstr_class_name;
    zstr_class_name = zend_string_init(class_name, strlen(class_name), 0);

    // Get the class entry
    ce = zend_lookup_class(zstr_class_name);
    
    // Check if the class exists
    if( ce == NULL )
        return NULL;

    ce->destructor = NULL;

    zval obj;
    object_init_ex(&obj, ce);
    // zend_object *object = emalloc(sizeof(zend_object));
    zend_object *object = Z_OBJ_P(&obj);

    return object;
}

zend_object *php_class_init( char const* class_name )
{
    zend_class_entry *ce;
    zend_string *zstr_class_name;
    zstr_class_name = zend_string_init(class_name, strlen(class_name), 0);

    // Get the class entry
    ce = zend_lookup_class(zstr_class_name);
    
    // Check if the class exists
    if( ce == NULL )
    {
        php_error_docref(
            NULL,
            E_ERROR, 
            "Class \"%s\" not found",
            class_name
        );
    }

    ce->destructor = NULL;

    zval obj;
    object_init_ex(&obj, ce);
    // zend_object *object = emalloc(sizeof(zend_object));
    zend_object *object = Z_OBJ_P(&obj);

    return object;
}

void php_class_call_constructor(
        zend_object*    zend_object_class, 
        unsigned int    param_counts, 
        zval*           params
    )
{
    zend_string*    method          =  zend_string_init("__construct", sizeof("__construct") - 1, 0);
    zval*           return_value    =  (zval*)emalloc(sizeof(zval));
    zend_result     result          =  zend_call_method_if_exists(
        zend_object_class, method, return_value, param_counts, params
    );

    zend_string_release(method);

    if( result == FAILURE )
        goto return_void;

    if( EG(exception) )
    { 
        zend_object* error_exception = EG(exception);

        if( zend_hash_num_elements(&error_exception->ce->properties_info) )
        {
            zval* exception_msg = zend_target_read_property_ex(zend_ce_exception, error_exception, "message");

            if( Z_TYPE_P(exception_msg) != IS_STRING )
                zend_bailout();
        }
        else
        {
            zend_bailout();
        }
    }
    
    return_void: { }
}

zval* php_class_call_method(
        zend_object*    zend_object_class, 
        char*           method_src,
        zend_ulong      method_len,
        unsigned int    param_counts, 
        zval*           params, 
        bool            silent
    )
{
    zend_string*    method          =  zend_string_init(method_src, method_len, 0);
    zval*           return_value    =  (zval*)emalloc(sizeof(zval));
    zend_result     result          =  zend_call_method_if_exists(
        zend_object_class, method, return_value, param_counts, params
    );

    zend_string_release(method);

    if( result != SUCCESS && !silent )
        php_error(E_ERROR, "method is not exists %s", method_src);

    // TODO: need improve error and exception throw to php-land
    do
    {
        if( EG(exception) )
        { 
            zend_object* error_exception = EG(exception);

            if( zend_hash_num_elements(&error_exception->ce->properties_info) )
            {
                zval* exception_msg = zend_target_read_property_ex(zend_ce_exception, error_exception, "message");

                if( Z_TYPE_P(exception_msg) != IS_STRING )
                    zend_bailout();
            }
            else
            {
                zend_bailout();
            }

            break;
        }


        if( Z_ISREF_P(return_value) )
            zend_unwrap_reference(return_value);
    }
    while(0);

    return return_value;
}

char const *ZTYPE_TO_STR( int M_TYPE )
{
    switch( M_TYPE )
    {
        case IS_DOUBLE:
            return "float";
        break;
        case IS_LONG:
            return "int";
        break;
        case IS_STRING:
            return "string";
        break;
        case IS_ARRAY:
            return "array";
        break;
        case IS_OBJECT:
            return "stdClass";
        break;
    }
}