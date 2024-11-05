
#include <php.h>
#include <php_ini.h>
#include <ext/standard/info.h>
#include <php_zpheur.h>
#include <Zend/zend_smart_str.h>
#include <Zend/zend_smart_string.h>
#include <ext/standard/html.h>
#include <Zend/zend_builtin_functions.h>
#include <zend_language_parser.h>
#include <Zend/zend_attributes.h>
#include <ext/spl/spl_iterators.h>
#include <Zend/zend_interfaces.h>

#include <include/onecstr.h>
#include <include/zerror.h>
#include <include/runtime.h>
#include "zpheur_hem.h"

zend_always_inline zend_result _ext_cli_entry_main( void )
{
    zend_function* func_entry_call =
    	(zend_function*)zend_hash_str_find_ptr(EG(function_table), "main", sizeof("main") - 1);

    if(! func_entry_call )
    	return FAILURE;

   	zval* params = (zval*)safe_emalloc(2, sizeof(zval), 0);
	zend_long param_counts = 0;
	zval retval;

	HashTable* named_params;
    ALLOC_HASHTABLE(named_params);
    zend_hash_init(named_params, 0, NULL, ZVAL_PTR_DTOR, 0);

    zend_arg_info* args_src = func_entry_call->common.arg_info;
    zend_long args_len = func_entry_call->common.num_args;

    bool allow_params = 0;

    if( func_entry_call->common.attributes )
    {
		ZEND_HASH_FOREACH_PTR(func_entry_call->common.attributes, zend_attribute* attr) {
			allow_params = zend_string_equals_cstr(attr->name, UseParamArguments);
		} ZEND_HASH_FOREACH_END();
	}

	if( !allow_params && args_len > 0 )
		zend_error(E_ERROR, "you must use attribute");

	if( allow_params && args_len != 2 )
		zend_error(E_ERROR, "argument must be 2");

	if( !allow_params && !args_len )
		goto __ext_main;

	for( int arg_index = 0; arg_index < args_len; ++arg_index )
    {
        zend_arg_info*		argument = &args_src[arg_index];
        zend_string*		arg_type = zend_type_to_string(argument->type);
        zend_string* 		arg_name = argument->name;

        switch( arg_index )
        {
	        case 0:
	        	if(! zend_string_equals_cstr(arg_type, "int", sizeof("int") - 1) )
					zend_error(E_ERROR, "first argument type must int");

				if( !arg_type || !zend_string_equals_cstr(arg_name, "argc", sizeof("argc") - 1) )
					zend_error(E_ERROR, "first argument name must argc");
	        break;
	        case 1:
	        	if( !arg_type || !zend_string_equals_cstr(arg_type, "array", sizeof("array") - 1) )
					zend_error(E_ERROR, "second argument type must array");

				if(! zend_string_equals_cstr(arg_name, "argv", sizeof("argv") - 1) )
					zend_error(E_ERROR, "second argument name must argv");
        	break;
	        default:
	        	zend_error(E_ERROR, "too many argument");
	    	break;
        }
    }      

	zval* _argc =
		zend_hash_str_find(&EG(symbol_table), "argc", sizeof("argc") - 1);
	zval* _argv =	
		zend_hash_str_find(&EG(symbol_table), "argv", sizeof("argv") - 1);

   	zval param_1;
   	zval param_2;
   	ZVAL_STRINGL(&param_1, "argc", sizeof("argc") - 1);
   	ZVAL_STRINGL(&param_2, "argv", sizeof("argv") - 1);

   	param_counts = 2;

   	zend_hash_index_add(named_params, 0, &param_1);
   	zend_hash_index_add(named_params, 1, &param_2);

   	params = (zval*)safe_emalloc(2, sizeof(zval), 0);
   	ZVAL_ZVAL(&params[0], _argc, 1, 0);
   	ZVAL_ZVAL(&params[1], _argv, 1, 0);

   	__ext_main:
	return php_call_function("main", sizeof("main") - 1, param_counts, named_params, params, &retval);
	return SUCCESS;

	__ext_error:
	return FAILURE;
}