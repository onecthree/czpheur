
#ifndef H_INCLUDE_RUNTIME
#define H_INCLUDE_RUNTIME

#include <php.h>
#include <php_main.h>
#include <ext/standard/info.h>
#include <php_zpheur.h>
#include <zpheur.h>

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

#define REQUIRE 0
#define REQUIRE_ONCE 1
#define INCLUDE 2
#define INCLUDE_ONCE 3

char* zstr_cstr( zend_string* string );

zval* php_call_enum_method(
    zend_class_entry*   ce,
    char* const         function_name_src,
    size_t              function_name_len,
    zend_long           param_counts,
    zval*               params
);

zend_result php_call_function(
    char* const         function_name_src,
    size_t              function_name_len,
    zend_long           param_counts,
    HashTable*          named_params,
    zval*               params, 
    zval*               retval
);

// experimental
void php_execute_file(
    char* const         filepath,
    int                 flag_mode,
    zend_op_array*      t_op_array
);

zval* php_call_closure(
    zval*               callable_func,
    zend_object*        callable_obj,
    size_t              param_counts,
    zval*               params
);

zend_class_entry* php_class_exists(
    zend_string*        class_name
);

zend_class_entry* php_enum_init(
    char* const         class_name
);

void php_call_static_method(
    zend_object*        obj,
    char* const         method_src, 
    size_t              method_len, 
    zend_long           param_counts, 
    zval*               params,
    zval*               retval
);

zend_object* php_class_init_silent(
    char* class_name_src,
    size_t class_name_len 
);

zend_object* php_class_init(
    char* class_name_src,
    size_t class_name_len
);

void php_class_call_constructor(
    zend_object*        zend_object_class, 
    zend_long           param_counts, 
    zval*               params
);

zval* php_class_call_method(
    zend_object*        zend_object_class, 
    char* const         method_src,
    size_t              method_len,
    zend_long           param_counts, 
    zval*               params, 
    bool                silent
);

char* const ZTYPE_TO_STR(
    char                M_TYPE
);

#endif