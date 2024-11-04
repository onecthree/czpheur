/**
 * Author: Sultan Ilham
 */

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <php.h>
#include <php_ini.h>
#include <ext/standard/info.h>
#include <php_zpheur.h>
#include <zpheur.h>
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

ZEND_DECLARE_MODULE_GLOBALS(zpheur)

static PHP_GINIT_FUNCTION(zpheur)
{
	array_init(&(zpheur_globals->DOTENV));
	array_init(&(zpheur_globals->CSRF));
}

#include <Zpheur/DataTransforms/Dotenv/Env_arginfo.h>
#include <Zpheur/Functions/httpverb_arginfo.h>
#include <Zpheur/Functions/is_appns_arginfo.h>
#include <Zpheur/Functions/clfile_arginfo.h>
#include <Zpheur/Functions/filetoclass_arginfo.h>
#include <Zpheur/Functions/zpheur_version_arginfo.h>
#include <Zpheur/Strings/contains_arginfo.h>

#include <Zpheur/Encryptions/Random/Generator/csrf_token_arginfo.h>

PHP_INI_MH(OnUpdateZpheurIniHandleEntryMain)
{
	zpheur_globals.ZpheurIniHandleEntryMain = new_value;
	return SUCCESS;
}

PHP_INI_BEGIN()
	PHP_INI_ENTRY("zpheur.handle_entry_main", "1", PHP_INI_ALL, OnUpdateZpheurIniHandleEntryMain)
PHP_INI_END();

static const zend_function_entry ext_functions[] = {
	ZEND_NS_FE("Zpheur\\DataTransforms\\Dotenv", env, env_arginfo)
	ZEND_NS_FE("Zpheur\\Globals", httpverb, httpverb_arginfo)
	ZEND_NS_FE("Zpheur\\Globals", is_appns, is_appns_arginfo)
	ZEND_NS_FE("Zpheur\\Globals", clfile, arginfo_clfile)
	ZEND_NS_FE("Zpheur\\Globals", filetoclass, filetoclass_arginfo)
	ZEND_FE(zpheur_version, zpheur_version_arginfo)

	ZEND_NS_FE("Zpheur\\Strings", contains, contains_arginfo)
	ZEND_NS_FE("Zpheur\\Encryptions\\Random\\Generator", csrf_token, csrf_token_arginfo)

	ZEND_FE_END
};

void (*original_zend_error_cb)(int errtype, zend_string *errfile, const uint32_t errline, zend_string *errstr);
void zpheur_error_cb( int errtype, zend_string *errfile, const uint32_t errline, zend_string *errstr )
{
	zval* error_handler =
		zend_get_constant_str("APP_ERROR_HANDLER", sizeof("APP_ERROR_HANDLER")-1);

	if( error_handler != NULL && zval_get_long(error_handler) & _ZWEB_ERROR_HANDLER )
	{
		if( errtype & E_FATAL_ERRORS )
			zend_bailout();
		else
			zpheur_errorhandler(errtype, errfile, errline, errstr);
	}

	original_zend_error_cb(errtype, errfile, errline, errstr);
}

/* {{{ PHP_MINIT_FUNCTION */
static PHP_MINIT_FUNCTION(zpheur)
{
    REGISTER_INI_ENTRIES();

	zend_register_stringl_constant("EMPTY_STRING", sizeof("EMPTY_STRING") - 1, "", sizeof("") - 1, CONST_CS | CONST_PERSISTENT, module_number);
	zend_register_stringl_constant("ZPHEUR", sizeof("ZPHEUR") - 1, "Zpheur", sizeof("Zpheur") - 1, CONST_CS | CONST_PERSISTENT, module_number);

	zend_register_stringl_constant("PRODUCTION", sizeof("PRODUCTION") - 1, "production", sizeof("production") - 1, CONST_CS | CONST_PERSISTENT, module_number);
	zend_register_stringl_constant("STAGING", sizeof("STAGING") - 1, "staging", sizeof("staging") - 1, CONST_CS | CONST_PERSISTENT, module_number);
	zend_register_stringl_constant("DEVELOPMENT", sizeof("DEVELOPMENT") - 1, "development", sizeof("development") - 1, CONST_CS | CONST_PERSISTENT, module_number);
	zend_register_long_constant("PERMANENT_REDIRECT", sizeof("PERMANENT_REDIRECT") - 1, 1, CONST_CS | CONST_PERSISTENT, module_number);

	zend_register_long_constant("PHP_ERROR_HANDLER", sizeof("PHP_ERROR_HANDLER") - 1, 1 << 0, CONST_CS | CONST_PERSISTENT, module_number);
	zend_register_long_constant("ZCLI_ERROR_HANDLER", sizeof("ZCLI_ERROR_HANDLER") - 1, 1 << 1, CONST_CS | CONST_PERSISTENT, module_number);
	zend_register_long_constant("ZWEB_ERROR_HANDLER", sizeof("ZWEB_ERROR_HANDLER") - 1, 1 << 2, CONST_CS | CONST_PERSISTENT, module_number);

	/* {{{ Constant register for "zpheur_function" */
	zend_register_long_constant("Z_ONLY_TAG", sizeof("Z_ONLY_TAG") - 1, 1 << 1, CONST_CS | CONST_PERSISTENT, module_number);
	zend_register_long_constant("Z_ONLY_NUMBER", sizeof("Z_ONLY_NUMBER") - 1, 1 << 2, CONST_CS | CONST_PERSISTENT, module_number);
	/* }}} */ 


  	ZEND_MODULE_STARTUP_N(Zpheur_Actions_Http_DefaultAction)(INIT_FUNC_ARGS_PASSTHRU);
  	ZEND_MODULE_STARTUP_N(Zpheur_Actions_Reflection_ActionResolver)(INIT_FUNC_ARGS_PASSTHRU);
  	ZEND_MODULE_STARTUP_N(Zpheur_Actions_Reflection_ArgumentResolver)(INIT_FUNC_ARGS_PASSTHRU);

  	ZEND_MODULE_STARTUP_N(Zpheur_Caches_Adapter_Filesystem_ItemTagHandler)(INIT_FUNC_ARGS_PASSTHRU);
  	ZEND_MODULE_STARTUP_N(Zpheur_Caches_Adapter_Filesystem)(INIT_FUNC_ARGS_PASSTHRU);

  	ZEND_MODULE_STARTUP_N(Zpheur_Consoles_Input_InputArgument)(INIT_FUNC_ARGS_PASSTHRU);
	ZEND_MODULE_STARTUP_N(Zpheur_Consoles_Runtime_Application)(INIT_FUNC_ARGS_PASSTHRU);
	
	ZEND_MODULE_STARTUP_N(Zpheur_DataTransforms_Dotenv_Dotenv)(INIT_FUNC_ARGS_PASSTHRU);
	ZEND_MODULE_STARTUP_N(Zpheur_Dependencies_ServiceLocator_Container)(INIT_FUNC_ARGS_PASSTHRU);
	
	ZEND_MODULE_STARTUP_N(Zpheur_Schemes_Http_Foundation_ParameterBag)(INIT_FUNC_ARGS_PASSTHRU);
	ZEND_MODULE_STARTUP_N(Zpheur_Schemes_Http_Foundation_HeaderBag)(INIT_FUNC_ARGS_PASSTHRU);
	ZEND_MODULE_STARTUP_N(Zpheur_Schemes_Http_Foundation_InputBag)(INIT_FUNC_ARGS_PASSTHRU);
	ZEND_MODULE_STARTUP_N(Zpheur_Schemes_Http_Foundation_Kernel)(INIT_FUNC_ARGS_PASSTHRU);
	ZEND_MODULE_STARTUP_N(Zpheur_Schemes_Http_Foundation_Request)(INIT_FUNC_ARGS_PASSTHRU);
	ZEND_MODULE_STARTUP_N(Zpheur_Schemes_Http_Foundation_ServerBag)(INIT_FUNC_ARGS_PASSTHRU);

	ZEND_MODULE_STARTUP_N(Zpheur_Schemes_Http_Message_HeaderInterface)(INIT_FUNC_ARGS_PASSTHRU);
	ZEND_MODULE_STARTUP_N(Zpheur_Schemes_Http_Message_HeaderTrait)(INIT_FUNC_ARGS_PASSTHRU);
	ZEND_MODULE_STARTUP_N(Zpheur_Schemes_Http_Message_Response)(INIT_FUNC_ARGS_PASSTHRU);

	ZEND_MODULE_STARTUP_N(Zpheur_Schemes_Http_Middleware_Next)(INIT_FUNC_ARGS_PASSTHRU);
	ZEND_MODULE_STARTUP_N(Zpheur_Schemes_Http_Middleware)(INIT_FUNC_ARGS_PASSTHRU);

	ZEND_MODULE_STARTUP_N(Zpheur_Schemes_Http_Responder_ErrorRequest)(INIT_FUNC_ARGS_PASSTHRU);

	ZEND_MODULE_STARTUP_N(Zpheur_Schemes_Http_Routing_Dispatcher_DispatchItem)(INIT_FUNC_ARGS_PASSTHRU);
	ZEND_MODULE_STARTUP_N(Zpheur_Schemes_Http_Routing_Dispatcher)(INIT_FUNC_ARGS_PASSTHRU);
	ZEND_MODULE_STARTUP_N(Zpheur_Schemes_Http_Routing_Route)(INIT_FUNC_ARGS_PASSTHRU);

	ZEND_MODULE_STARTUP_N(Zpheur_Files_Mime)(INIT_FUNC_ARGS_PASSTHRU);
		
	#ifdef ZPHEUR_REQUIRE_VOILE
	ZEND_MODULE_STARTUP_N(Zpheur_Databases_Voile)(INIT_FUNC_ARGS_PASSTHRU);
	ZEND_MODULE_STARTUP_N(Zpheur_Databases_Voile_Model)(INIT_FUNC_ARGS_PASSTHRU);
	ZEND_MODULE_STARTUP_N(Zpheur_Databases_Voile_Collection)(INIT_FUNC_ARGS_PASSTHRU);
	ZEND_MODULE_STARTUP_N(Zpheur_Databases_Voile_Schema_Collection)(INIT_FUNC_ARGS_PASSTHRU);
	ZEND_MODULE_STARTUP_N(Zpheur_Databases_Voile_Schema_Field)(INIT_FUNC_ARGS_PASSTHRU);

	ZEND_MODULE_STARTUP_N(Zpheur_Databases_Voile_Schema_AbstractionType_AObjectId)(INIT_FUNC_ARGS_PASSTHRU);
	ZEND_MODULE_STARTUP_N(Zpheur_Databases_Voile_Schema_AbstractionType_AString)(INIT_FUNC_ARGS_PASSTHRU);
	ZEND_MODULE_STARTUP_N(Zpheur_Databases_Voile_Schema_AbstractionType_ANumber)(INIT_FUNC_ARGS_PASSTHRU);
	ZEND_MODULE_STARTUP_N(Zpheur_Databases_Voile_Schema_AbstractionType_ABoolean)(INIT_FUNC_ARGS_PASSTHRU);
	ZEND_MODULE_STARTUP_N(Zpheur_Databases_Voile_Schema_AbstractionType_ADateTime)(INIT_FUNC_ARGS_PASSTHRU);
	ZEND_MODULE_STARTUP_N(Zpheur_Databases_Voile_Schema_AbstractionType_AArray)(INIT_FUNC_ARGS_PASSTHRU);
	ZEND_MODULE_STARTUP_N(Zpheur_Databases_Voile_Schema_AbstractionType_AObject)(INIT_FUNC_ARGS_PASSTHRU);
	ZEND_MODULE_STARTUP_N(Zpheur_Databases_Voile_Schema_AbstractionType_ADecimal128)(INIT_FUNC_ARGS_PASSTHRU);
	#endif

	ZEND_MODULE_STARTUP_N(Zpheur_Databases_Phase)(INIT_FUNC_ARGS_PASSTHRU);
	ZEND_MODULE_STARTUP_N(Zpheur_Databases_Phase_Model)(INIT_FUNC_ARGS_PASSTHRU);

	ZEND_MODULE_STARTUP_N(Zpheur_Databases_Phase_Schema_AbstractionType_AInt)(INIT_FUNC_ARGS_PASSTHRU);
	ZEND_MODULE_STARTUP_N(Zpheur_Databases_Phase_Schema_AbstractionType_AVarChar)(INIT_FUNC_ARGS_PASSTHRU);

	ZEND_MODULE_STARTUP_N(Zpheur_Databases_Phase_Schema_Table)(INIT_FUNC_ARGS_PASSTHRU);
	ZEND_MODULE_STARTUP_N(Zpheur_Databases_Phase_Schema_Column)(INIT_FUNC_ARGS_PASSTHRU);

	ZEND_MODULE_STARTUP_N(Zpheur_Logs_StreamLog)(INIT_FUNC_ARGS_PASSTHRU);
	ZEND_MODULE_STARTUP_N(Zpheur_Logs_StreamLog_StreamHandler)(INIT_FUNC_ARGS_PASSTHRU);
	ZEND_MODULE_STARTUP_N(Zpheur_Logs_StreamLog_Level)(INIT_FUNC_ARGS_PASSTHRU);

	original_zend_error_cb = zend_error_cb;
    zend_error_cb = zpheur_error_cb;

	return SUCCESS;
}
/* }}} */

#define UseParamArguments "UseParamArguments", sizeof("UseParamArguments") - 1
zend_result _ext_cli_entry_main( void )
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

   	params = safe_emalloc(2, sizeof(zval), 0);
   	ZVAL_ZVAL(&params[0], _argc, 1, 0);
   	ZVAL_ZVAL(&params[1], _argv, 1, 0);

   	__ext_main:
	return php_call_function("main", sizeof("main") - 1, param_counts, named_params, params, &retval);
	return SUCCESS;

	__ext_error:
	return FAILURE;
}

/* {{{ PHP_RINIT_FUNCTION */
static PHP_RINIT_FUNCTION(zpheur)  
{
	#if defined(ZTS) && defined(COMPILE_DL_zpheur)
		ZEND_TSRMLS_CACHE_UPDATE();
	#endif

	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MINFO_FUNCTION */
static PHP_MINFO_FUNCTION(zpheur)
{
    php_info_print_table_start();
		php_info_print_table_header(3, "Web framework for PHP delivered as C extension");
    php_info_print_table_end();
    php_info_print_table_start();
	    php_info_print_table_header(2, "Info", "Value");;
        php_info_print_table_row(2, "Author", "Sultan Ilham (onecthree@gmail.com)");  
        php_info_print_table_row(2, "Version", "v" PHP_ZPHEUR_VERSION_NUMBER " (" PHP_ZPHEUR_VERSION_TAG ") ");    
    php_info_print_table_end();
    php_info_print_table_start();
	    php_info_print_table_header(2, "Component", "Value");
        php_info_print_table_row(2, "FuRouter", ZPHEUR_FUROUTER_INCLUDED ? "enabled" : "disabled");  
        php_info_print_table_row(2, "Voile", ZPHEUR_VOILE_INCLUDED ? "enabled" : "disabled");    
        php_info_print_table_row(2, "Phase", ZPHEUR_PHASE_INCLUDED ? "enabled" : "disabled");    
        php_info_print_table_row(2, "Dotenv", ZPHEUR_DOTENV_INCLUDED ? "enabled" : "disabled");   
    php_info_print_table_end();
}
/* }}} */

/* {{{ PHP_RSHUTDOWN_FUNCTION */
static PHP_RSHUTDOWN_FUNCTION(zpheur)
{
	if( zend_string_equals_cstr(ZPHEUR_G(ZpheurIniHandleEntryMain), "1", 1) )
		(void)_ext_cli_entry_main();
	
	return SUCCESS;
}
/* }}} */


/* {{{ PHP_MSHUTDOWN_FUNCTION */
static PHP_MSHUTDOWN_FUNCTION(zpheur)
{
    zend_error_cb = original_zend_error_cb;
	return SUCCESS;
}
/* }}} */

/* return void */
static PHP_GSHUTDOWN_FUNCTION(zpheur)
{

}


/* {{{ zpheur_module_entry */
zend_module_entry zpheur_module_entry = {
	STANDARD_MODULE_HEADER,
	"zpheur",						/* Extension name */
	ext_functions,					/* zend_function_entry */
	PHP_MINIT(zpheur),				/* PHP_MINIT 		- Module initialization */
	PHP_MSHUTDOWN(zpheur),			/* PHP_MSHUTDOWN 	- Module shutdown */
	PHP_RINIT(zpheur),				/* PHP_RINIT 		- Request initialization */
	PHP_RSHUTDOWN(zpheur),			/* PHP_RSHUTDOWN 	- Request shutdown */
	PHP_MINFO(zpheur),				/* PHP_MINFO 		- Module info */
	PHP_ZPHEUR_VERSION_NUMBER,		/* Version 			- Module version number info*/
	// STANDARD_MODULE_PROPERTIES
	PHP_MODULE_GLOBALS(zpheur),  	/* Module globals */
    PHP_GINIT(zpheur),           	/* PHP_GINIT 		– Globals initialization */
    PHP_GSHUTDOWN(zpheur),			/* PHP_GSHUTDOWN 	– Globals shutdown */
    NULL,							/* PRSHUTDOWN() */
    STANDARD_MODULE_PROPERTIES_EX
};
/* }}} */

#ifdef COMPILE_DL_ZPHEUR
# ifdef ZTS
ZEND_TSRMLS_CACHE_DEFINE()
# endif
ZEND_GET_MODULE(zpheur)
#endif
