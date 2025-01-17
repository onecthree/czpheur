
#include <php.h>
#include <php_main.h>
#include <ext/standard/info.h>
#include <php_zpheur.h>
#include <zpheur.h>
#include "runtime.h"

#include <ext/standard/basic_functions.h>
#include <ext/spl/php_spl.h>
#include <ext/spl/spl_functions.h>

#if PHP_VERSION_ID < 80400 // spl_engine.h removed in PHP ^8.4.x
#include <ext/spl/spl_engine.h>
#endif

#include <ext/spl/spl_array.h>
#include <ext/spl/spl_directory.h>
#include <ext/spl/spl_iterators.h>
#include <ext/spl/spl_exceptions.h>
#include <ext/spl/spl_observer.h>
#include <ext/spl/spl_dllist.h>
#include <ext/spl/spl_fixedarray.h>
#include <ext/spl/spl_heap.h>
#include <zend_exceptions.h>
#include <zend_interfaces.h>


#include <zend.h>
#include <zend_compile.h>
#include <zend_execute.h>
#include <zend_API.h>
#include <zend_stack.h>
#include <zend_constants.h>
#include <zend_extensions.h>
#include <zend_exceptions.h>
#include <zend_closures.h>
#include <zend_generators.h>
#include <zend_vm.h>
#include <zend_float.h>
#include <zend_fibers.h>
#include <zend_weakrefs.h>
#include <zend_inheritance.h>
#include <zend_observer.h>

#include <Zend/zend_exceptions.h>

inline __attribute__ ((always_inline))
char* zstr_cstr( zend_string* string )
{   
    if(! string ) return "";
    
    string->val[string->len] = '\0';
    return string->val;
}

zval* php_call_enum_method( zend_class_entry* ce, char* const function_name_src, size_t function_name_len, zend_long param_counts, zval* params )
{
    zval function_name;
    ZVAL_STRINGL(&function_name, function_name_src, function_name_len);
    zend_fcall_info fci;
    zend_fcall_info_cache fci_cache;

    zend_string* func_name = zend_string_init(function_name_src, function_name_len, 0);
    zend_function* func = zend_hash_find_ptr(&ce->function_table, func_name);

    if( func == NULL ) // Function not found
    {
        zend_string_release(func_name);
        return NULL;
    }

    zval* retval = (zval*)emalloc(sizeof(zval));

    /* {{ */
    // fci.size = sizeof(fci);
    // fci.function_name = function_name;
    // fci.retval = retval;
    // fci.params = params;
    // fci.object = NULL;
    // fci.param_count = param_counts;
    // fci.named_params = named_params;
    /*  }} */

    fci.size = sizeof(fci);
    fci.function_name = function_name;
    fci.retval = retval;
    fci.params = params;
    fci.object = NULL;
    fci.param_count = param_counts;
    fci.named_params = NULL;

    // REMOVE: uncompleted init scope, see next
    // fci_cache.called_scope = NULL;
    // fci_cache.calling_scope = NULL;
    // fci_cache.function_handler = func;

    fci_cache.called_scope = NULL;
    fci_cache.calling_scope = NULL;
    fci_cache.object = NULL;
    fci_cache.function_handler = func;

    if ( zend_call_function(&fci, &fci_cache) == SUCCESS )
    {
        zend_string_release(func_name);
        return retval;
    }

    return NULL;
}

zend_result php_call_function( char* const function_name_src, size_t function_name_len, zend_long param_counts, HashTable* named_params, zval* params, zval* retval )
{
    zend_fcall_info fci;
    zend_fcall_info_cache fcc;
    zval function_name;

    ZVAL_STRINGL(&function_name, function_name_src, function_name_len);

    zend_function* function_ptr =
        zend_hash_str_find_ptr(
            EG(function_table), "main", sizeof("main") - 1
        );

    fci.size = sizeof(fci);
    fci.function_name = function_name;
    fci.retval = retval;
    fci.params = params;
    fci.object = NULL;
    fci.param_count = param_counts;
    fci.named_params = named_params;

    fcc.called_scope = NULL;
    fcc.calling_scope = NULL;
    fcc.object = NULL;
    fcc.function_handler = function_ptr;

    if( zend_call_function(&fci, &fcc) != SUCCESS )
        return FAILURE;

    return SUCCESS;
}

/**
 * experimental
 * {{
int php_execute_file( char* const file_path, int flag_mode )
{
   zend_string *zs_file_path = zend_string_init(file_path, strlen(file_path), 0);
   zval dummy;
   zend_file_handle file_handle;
   zend_op_array *new_op_array;
   zval result;
   int ret;

//    zend_stream_init_filename_ex(&file_handle, class_file);
//    ret = php_stream_open_for_zend_ex(&file_handle, USE_PATH|STREAM_OPEN_FOR_INCLUDE);
   file_handle.type = ZEND_HANDLE_FILENAME;
   file_handle.filename = zs_file_path;

   zend_string *opened_path;
   if (!file_handle.opened_path) {
       file_handle.opened_path = zend_string_copy(zs_file_path);
   }

   opened_path = zend_string_copy(file_handle.opened_path);

   ZVAL_NULL(&dummy);
   if (zend_hash_add(&EG(included_files), opened_path, &dummy)) {
       switch( flag_mode )
       {
           case REQUIRE:
               new_op_array = zend_compile_file(&file_handle, ZEND_REQUIRE);
           break;
           case REQUIRE_ONCE:
               new_op_array = zend_compile_file(&file_handle, ZEND_REQUIRE_ONCE);
           break;
           case INCLUDE:
               new_op_array = zend_compile_file(&file_handle, ZEND_INCLUDE);
           break;
           case INCLUDE_ONCE:
               new_op_array = zend_compile_file(&file_handle, ZEND_INCLUDE_ONCE);
           break;
       }
   } else {
       new_op_array = NULL;
   }

   zend_string_release_ex(opened_path, 0);
   if (new_op_array) {
       uint32_t orig_jit_trace_num = EG(jit_trace_num);

       ZVAL_UNDEF(&result);
       zend_execute(new_op_array, &result);
       EG(jit_trace_num) = orig_jit_trace_num;

       destroy_op_array(new_op_array);
       efree(new_op_array);
       if (!EG(exception)) {
           zval_ptr_dtor(&result);
       }

       zend_destroy_file_handle(&file_handle);
       zend_string_release(zs_file_path);

       return 0;
   }

   zend_destroy_file_handle(&file_handle);
   zend_string_release(zs_file_path);

   return 0;
}
}} */

void php_execute_file( char* const filepath, int flag_mode, zend_op_array* t_op_array )
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

zval* php_call_closure( zval* callable_func, zend_object* callable_obj, size_t param_counts, zval* params )
{
    zend_fcall_info fci;
    zend_fcall_info_cache fci_cache;
    zval* retval = (zval*)emalloc(sizeof(zval));

    fci.size = sizeof(fci);
    fci.function_name = *callable_func;
    fci.retval = retval;
    fci.params = params;
    fci.object = callable_obj;
    fci.param_count = param_counts;
    fci.named_params = NULL;

    fci_cache.called_scope = NULL;
    fci_cache.calling_scope = NULL;
    fci_cache.object = NULL;
    fci_cache.function_handler = NULL;

    if( zend_call_function(&fci, &fci_cache) == SUCCESS )
        return retval;

    return NULL;
}

inline __attribute__ ((always_inline))
zend_class_entry* php_class_exists( zend_string* class_name )
{
    return zend_lookup_class(class_name);
}

// TODO improve args of class_name with fixed length given
zend_class_entry* php_enum_init( char* const class_name )
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

// TODO improve clean up resource
void php_call_static_method( zend_object* object, char* const method_src, size_t method_len, zend_long param_counts, zval* params, zval* retval )
{
    zval function_name;
    ZVAL_STRINGL(&function_name, method_src, method_len);

    zval _object;
    ZVAL_OBJ(&_object, object);

    call_user_function(NULL, &_object, &function_name, retval, param_counts, params);

    // Do need to free'd
    // zval_ptr_dtor(&object);
    // zval_ptr_dtor(&function_name);
}


inline __attribute__ ((always_inline)) zend_object*
php_class_init_ce( zend_class_entry* ce )
{
    zval object;
    object_init_ex(&object, ce);
    return Z_OBJ_P(&object);
}

zend_object* php_class_init( char* class_name_src, size_t class_name_len )
{
    zval object;
    zend_string* class_name;
    zend_class_entry* ce;

    class_name = zend_string_init(class_name_src, class_name_len, 0);
    ce = zend_lookup_class(class_name);
    
    if( ce == NULL )
        php_error_docref(NULL, E_ERROR, "Class \"%s\" not found", class_name->val);

    object_init_ex(&object, ce);
    zend_string_release(class_name);

    return Z_OBJ_P(&object);
}

zend_object* php_class_init_silent( char* class_name_src, size_t class_name_len )
{
    zval object;
    zend_string* class_name;
    zend_class_entry* ce;

    class_name = zend_string_init(class_name_src, class_name_len, 0);
    ce = zend_lookup_class(class_name);
    
    if( ce == NULL )
        return NULL;

    object_init_ex(&object, ce);
    zend_string_release(class_name);

    return Z_OBJ_P(&object);
}

void php_class_call_constructor( zend_object* object, zend_long param_counts, zval* params )
{
    zval return_value;
    zend_string* method;
    zend_result result;

    method = zend_string_init("__construct", sizeof("__construct") - 1, 0);
    result = zend_call_method_if_exists(
        object, method, &return_value, param_counts, params
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

void php_class_call_dtor( zend_object* object )
{
    zval return_value;
    zend_string* method;
    zend_result result;

    method = zend_string_init("__destruct", sizeof("__destruct") - 1, 0);
    result = zend_call_method_if_exists(object, method, &return_value, 0, NULL);

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

void php_class_call_method_stacked( zend_object* zend_object_class, char* const method_src, 
size_t method_len, zend_long param_counts, zval* params, zval* return_value, bool silent )
{
    zend_string* method;
    zend_result result;

    method = zend_string_init(method_src, method_len, 0);
    result = zend_call_method_if_exists(
        zend_object_class, method, return_value, param_counts, params
    );

    zend_string_release(method);

    if( result != SUCCESS && !silent )
    {
        php_error_docref(NULL,
            E_CORE_ERROR, "Call to undefined method %s::%s()",
            zend_object_class->ce->name->val, method_src);
    }

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
}

zval* php_class_call_method( zend_object* zend_object_class, char* const method_src, size_t method_len, zend_long param_counts, zval* params, bool silent )
{
    zval* return_value;
    zend_string* method;
    zend_result result;

    return_value = (zval*)emalloc(sizeof(zval));
    method = zend_string_init(method_src, method_len, 0);
    result = zend_call_method_if_exists(
        zend_object_class, method, return_value, param_counts, params
    );

    zend_string_release(method);

    if( result != SUCCESS && !silent )
    {
        php_error_docref(NULL,
            E_CORE_ERROR, "Call to undefined method %s::%s()",
            zend_object_class->ce->name->val, method_src);
    }

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

char* const ZTYPE_TO_STR( char M_TYPE )
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
        case IS_NULL:
            return "null";
        break;
        default:
            return "UKNOWN:0";
        break;
    }
}