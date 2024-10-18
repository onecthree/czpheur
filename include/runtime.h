
#ifndef H_INCLUDE_RUNTIME
#define H_INCLUDE_RUNTIME

#include "php.h"
#include "php_main.h"
#include "ext/standard/info.h"
#include "php_zpheur.h"

#include "ext/spl/php_spl.h"
#include "ext/spl/spl_functions.h"
#include "ext/spl/spl_engine.h"
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

#define REQUIRE 0
#define REQUIRE_ONCE 1
#define INCLUDE 2
#define INCLUDE_ONCE 3


zval *php_call_enum_method( zend_class_entry *enum_ce, const char *function_name, unsigned int param_counts, zval *params );

zval *php_call_function( const char *function_name, unsigned int param_counts, zval *params );

zval *php_call_static_method_2( zend_object *obj, const char *function_name, unsigned int param_counts, zval *params );

void php_execute_file( char const *filepath, int flag_mode, zend_op_array* t_op_array );

zval* php_call_closure( zval *z_closure, zend_object *o_closure, unsigned int param_counts, zval *params );
inline static zend_class_entry *php_class_exists( zend_string *class_name );

zend_class_entry *php_enum_init( char const* class_name );

void php_call_static_method(
        zend_object *obj,
        const char *method_name, 
        size_t method_name_length, 
        int num_params, 
        zval* params,
        zval* retval
    );

zend_object *php_class_init_self_ex( char const* class_name );

zend_object *php_class_init_ex( char const* class_name );

zend_object *php_class_init( char const* class_name );

void php_class_call_constructor(
        zend_object*    zend_object_class, 
        unsigned int    param_counts, 
        zval*           params
    );

zval* php_class_call_method(
        zend_object*    zend_object_class, 
        char*           method_src,
        zend_ulong      method_len,
        unsigned int    param_counts, 
        zval*           params, 
        bool            silent
    );

char const *ZTYPE_TO_STR( int M_TYPE );

#endif