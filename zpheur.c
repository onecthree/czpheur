/**
 * Author: Sultan Ilham
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <php.h>
#include <php_ini.h>
#include <ext/standard/info.h>

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

#include "php_zpheur.h"
#include "zpheur.h"

ZEND_DECLARE_MODULE_GLOBALS(zpheur)

static PHP_GINIT_FUNCTION(zpheur)
{
	array_init(&(zpheur_globals->DOTENV));
	array_init(&(zpheur_globals->CSRF));
}

#include <Zpheur/Functions/httpverb_arginfo.h>
#include <Zpheur/Functions/is_appns_arginfo.h>
#include <Zpheur/Functions/clfile_arginfo.h>
#include <Zpheur/Functions/filetoclass_arginfo.h>
#include <Zpheur/Functions/zpheur_version_arginfo.h>
#include <Zpheur/Strings/contains_arginfo.h>
#include <Zpheur/Randomizes/random_chars_arginfo.h>

#include <Zpheur/Encryptions/Random/Generator/csrf_token_arginfo.h>

HashTable* zpheur_fake_get_gc( zend_object* zobj, zval** table, int* n )
{
	*n = 0;
	*table = NULL;
	return NULL;
}

PHP_INI_MH(OnUpdateZpheurIniHandleEntryMain)
{
	// zpheur_globals.ZpheurIniHandleEntryMain = new_value;

	ZPHEUR_G(ZpheurIniHandleEntryMain) = new_value;
	return SUCCESS;
}

PHP_INI_BEGIN()
	PHP_INI_ENTRY("zpheur.hem", "0", PHP_INI_ALL, OnUpdateZpheurIniHandleEntryMain)
PHP_INI_END();

static const zend_function_entry ext_functions[] = {
	ZEND_NS_FE("Zpheur\\Functions", httpverb, httpverb_arginfo)
	ZEND_NS_FE("Zpheur\\Functions", is_appns, is_appns_arginfo)
	ZEND_NS_FE("Zpheur\\Functions", clfile, clfile_arginfo)
	ZEND_NS_FE("Zpheur\\Functions", filetoclass, filetoclass_arginfo)
	ZEND_FE(zpheur_version, zpheur_version_arginfo)

	ZEND_NS_FE("Zpheur\\Strings", contains, contains_arginfo)
	ZEND_NS_FE("Zpheur\\Encryptions\\Random\\Generator", csrf_token, csrf_token_arginfo)
	ZEND_NS_FE("Zpheur\\Randomizes", random_chars, random_chars_arginfo)

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


  	ZEND_MODULE_STARTUP_N(Zpheur_Actions_Console_DefaultAction)(INIT_FUNC_ARGS_PASSTHRU);
  	ZEND_MODULE_STARTUP_N(Zpheur_Actions_Http_DefaultAction)(INIT_FUNC_ARGS_PASSTHRU);
  	ZEND_MODULE_STARTUP_N(Zpheur_Actions_WebSocket_DefaultAction)(INIT_FUNC_ARGS_PASSTHRU);
  	
	ZEND_MODULE_STARTUP_N(Zpheur_Actions_Middleware)(INIT_FUNC_ARGS_PASSTHRU);
	ZEND_MODULE_STARTUP_N(Zpheur_Actions_Middleware_InvalidArgumentException)(INIT_FUNC_ARGS_PASSTHRU);

  	ZEND_MODULE_STARTUP_N(Zpheur_Actions_Reflection_ActionResolver)(INIT_FUNC_ARGS_PASSTHRU);
  	ZEND_MODULE_STARTUP_N(Zpheur_Actions_Reflection_ArgumentResolver)(INIT_FUNC_ARGS_PASSTHRU);
  	
  	ZEND_MODULE_STARTUP_N(Zpheur_Caches_SimpleCache)(INIT_FUNC_ARGS_PASSTHRU);
  	ZEND_MODULE_STARTUP_N(Zpheur_Caches_SimpleCache_CacheInterface)(INIT_FUNC_ARGS_PASSTHRU);  	

  	ZEND_MODULE_STARTUP_N(Zpheur_Caches_Adapter_Filesystem)(INIT_FUNC_ARGS_PASSTHRU);
  	ZEND_MODULE_STARTUP_N(Zpheur_Caches_Adapter_Filesystem_ItemTagHandler)(INIT_FUNC_ARGS_PASSTHRU);

  	ZEND_MODULE_STARTUP_N(Zpheur_Caches_Cache_Adapter_Memcached)(INIT_FUNC_ARGS_PASSTHRU);

  	ZEND_MODULE_STARTUP_N(Zpheur_Consoles_Input_InputArgument)(INIT_FUNC_ARGS_PASSTHRU);
	ZEND_MODULE_STARTUP_N(Zpheur_Consoles_Runtime_Application)(INIT_FUNC_ARGS_PASSTHRU);
	
	ZEND_MODULE_STARTUP_N(Zpheur_DataTransforms_Dotenv)(INIT_FUNC_ARGS_PASSTHRU);
	ZEND_MODULE_STARTUP_N(Zpheur_Dependencies_ServiceLocator_Container)(INIT_FUNC_ARGS_PASSTHRU);
	
	ZEND_MODULE_STARTUP_N(Zpheur_Schemes_Http_Foundation_ParameterBag)(INIT_FUNC_ARGS_PASSTHRU);
	ZEND_MODULE_STARTUP_N(Zpheur_Schemes_Http_Foundation_HeaderBag)(INIT_FUNC_ARGS_PASSTHRU);
	ZEND_MODULE_STARTUP_N(Zpheur_Schemes_Http_Foundation_InputBag)(INIT_FUNC_ARGS_PASSTHRU);
	ZEND_MODULE_STARTUP_N(Zpheur_Schemes_Http_Foundation_Kernel)(INIT_FUNC_ARGS_PASSTHRU);
	ZEND_MODULE_STARTUP_N(Zpheur_Schemes_Http_Foundation_ServerBag)(INIT_FUNC_ARGS_PASSTHRU);

	ZEND_MODULE_STARTUP_N(Zpheur_Schemes_Http_Message_Request_RequestInterface)(INIT_FUNC_ARGS_PASSTHRU);
	ZEND_MODULE_STARTUP_N(Zpheur_Schemes_Http_Message_Response_ResponseInterface)(INIT_FUNC_ARGS_PASSTHRU);
	ZEND_MODULE_STARTUP_N(Zpheur_Schemes_Http_Message_HeaderTrait)(INIT_FUNC_ARGS_PASSTHRU);
	ZEND_MODULE_STARTUP_N(Zpheur_Schemes_Http_Message_Request)(INIT_FUNC_ARGS_PASSTHRU);
	ZEND_MODULE_STARTUP_N(Zpheur_Schemes_Http_Message_Response)(INIT_FUNC_ARGS_PASSTHRU);

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
		_ext_cli_entry_main();
	
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
