
#ifndef PHP_ZPHEUR_H
#define PHP_ZPHEUR_H

#include <php.h>
#include <stdlib.h>
#include <Zend/zend_modules.h>
#include <Zend/zend_API.h>

extern zend_module_entry zpheur_module_entry;
#define phpext_zpheur_ptr &zpheur_module_entry

#define PHP_ZPHEUR_VERSION_NUMBER 	"0.2.017"
#define PHP_ZPHEUR_VERSION_TAG		"alpha"

/* For compatibility with older PHP versions */
#ifndef ZEND_PARSE_PARAMETERS_NONE
#define ZEND_PARSE_PARAMETERS_NONE() \
	ZEND_PARSE_PARAMETERS_START(0, 0) \
	ZEND_PARSE_PARAMETERS_END()
#endif

#define ZPHEUR_FUROUTER_INCLUDED 1
#define ZPHEUR_VOILE_INCLUDED 	 0
#define ZPHEUR_PHASE_INCLUDED 	 1
#define ZPHEUR_DOTENV_INCLUDED 	 1

#ifdef ZPHEUR_REQUIRE_VOILE
#undef  ZPHEUR_VOILE_INCLUDED
#define ZPHEUR_VOILE_INCLUDED 1
#endif

ZEND_BEGIN_MODULE_GLOBALS(zpheur)
	zval     		DOTENV;
	zval     		CSRF;
	zend_string*	ZpheurIniHandleEntryMain;
ZEND_END_MODULE_GLOBALS(zpheur)

ZEND_EXTERN_MODULE_GLOBALS(zpheur)

// #define ZPHEUR_G(v) ZEND_MODULE_GLOBALS_ACCESSOR(zpheur, v)
#ifdef ZTS
#define ZPHEUR_G(v) TSRMG(zpheur_globals_id, zend_zpheur_globals *, v)
#else
#define ZPHEUR_G(v) (zpheur_globals.v)
#endif

ZEND_FUNCTION(env);
ZEND_FUNCTION(httpverb);
ZEND_FUNCTION(is_appns);
ZEND_FUNCTION(clfile);
ZEND_FUNCTION(filetoclass);
ZEND_FUNCTION(contains);
ZEND_FUNCTION(csrf_token);
ZEND_FUNCTION(zpheur_version);


ZEND_MINIT_FUNCTION(Zpheur_Actions_Console_DefaultAction);

ZEND_MINIT_FUNCTION(Zpheur_Actions_Http_DefaultAction);
ZEND_MINIT_FUNCTION(Zpheur_Actions_Middleware_Next);
ZEND_MINIT_FUNCTION(Zpheur_Actions_Middleware);

ZEND_MINIT_FUNCTION(Zpheur_Actions_Reflection_ActionResolver);
ZEND_MINIT_FUNCTION(Zpheur_Actions_Reflection_ArgumentResolver);

ZEND_MINIT_FUNCTION(Zpheur_Caches_Adapter_Filesystem_ItemTagHandler);
ZEND_MINIT_FUNCTION(Zpheur_Caches_Adapter_Filesystem);

ZEND_MINIT_FUNCTION(Zpheur_Consoles_Input_InputArgument);
ZEND_MINIT_FUNCTION(Zpheur_Consoles_Runtime_Application);

ZEND_MINIT_FUNCTION(Zpheur_DataTransforms_Dotenv_Dotenv);
ZEND_MINIT_FUNCTION(Zpheur_Dependencies_ServiceLocator_Container);

ZEND_MINIT_FUNCTION(Zpheur_Schemes_Http_Foundation_ParameterBag);
ZEND_MINIT_FUNCTION(Zpheur_Schemes_Http_Foundation_HeaderBag);
ZEND_MINIT_FUNCTION(Zpheur_Schemes_Http_Foundation_InputBag);
ZEND_MINIT_FUNCTION(Zpheur_Schemes_Http_Foundation_Kernel);
ZEND_MINIT_FUNCTION(Zpheur_Schemes_Http_Foundation_Request);
ZEND_MINIT_FUNCTION(Zpheur_Schemes_Http_Foundation_ServerBag);

ZEND_MINIT_FUNCTION(Zpheur_Schemes_Http_Message_HeaderInterface);
ZEND_MINIT_FUNCTION(Zpheur_Schemes_Http_Message_HeaderTrait);
ZEND_MINIT_FUNCTION(Zpheur_Schemes_Http_Message_Response);

ZEND_MINIT_FUNCTION(Zpheur_Schemes_Http_Responder_ErrorRequest);

ZEND_MINIT_FUNCTION(Zpheur_Schemes_Http_Routing_Dispatcher_DispatchItem);
ZEND_MINIT_FUNCTION(Zpheur_Schemes_Http_Routing_Dispatcher);
ZEND_MINIT_FUNCTION(Zpheur_Schemes_Http_Routing_Route);

ZEND_MINIT_FUNCTION(Zpheur_Files_Mime);

#ifdef ZPHEUR_REQUIRE_VOILE
ZEND_MINIT_FUNCTION(Zpheur_Databases_Voile);
ZEND_MINIT_FUNCTION(Zpheur_Databases_Voile_Model);
ZEND_MINIT_FUNCTION(Zpheur_Databases_Voile_Collection);
ZEND_MINIT_FUNCTION(Zpheur_Databases_Voile_Schema_Collection);
ZEND_MINIT_FUNCTION(Zpheur_Databases_Voile_Schema_Field);

ZEND_MINIT_FUNCTION(Zpheur_Databases_Voile_Schema_AbstractionType_AObjectId);
ZEND_MINIT_FUNCTION(Zpheur_Databases_Voile_Schema_AbstractionType_AString);
ZEND_MINIT_FUNCTION(Zpheur_Databases_Voile_Schema_AbstractionType_ANumber);
ZEND_MINIT_FUNCTION(Zpheur_Databases_Voile_Schema_AbstractionType_ABoolean);
ZEND_MINIT_FUNCTION(Zpheur_Databases_Voile_Schema_AbstractionType_ADateTime);
ZEND_MINIT_FUNCTION(Zpheur_Databases_Voile_Schema_AbstractionType_AArray);
ZEND_MINIT_FUNCTION(Zpheur_Databases_Voile_Schema_AbstractionType_AObject);
ZEND_MINIT_FUNCTION(Zpheur_Databases_Voile_Schema_AbstractionType_ADecimal128);
#endif

ZEND_MINIT_FUNCTION(Zpheur_Databases_Phase);
ZEND_MINIT_FUNCTION(Zpheur_Databases_Phase_Model);

ZEND_MINIT_FUNCTION(Zpheur_Databases_Phase_Schema_AbstractionType_AInt);
ZEND_MINIT_FUNCTION(Zpheur_Databases_Phase_Schema_AbstractionType_AVarChar);

ZEND_MINIT_FUNCTION(Zpheur_Databases_Phase_Schema_Table);
ZEND_MINIT_FUNCTION(Zpheur_Databases_Phase_Schema_Column);

ZEND_MINIT_FUNCTION(Zpheur_Logs_StreamLog);
ZEND_MINIT_FUNCTION(Zpheur_Logs_StreamLog_StreamHandler);
ZEND_MINIT_FUNCTION(Zpheur_Logs_StreamLog_Level);

// [propose]
// ZEND_MINIT_FUNCTION(Zpheur_DataTransforms_Dotenv_env);

#if defined(ZTS) && defined(COMPILE_DL_ZPHEUR)
ZEND_TSRMLS_CACHE_EXTERN()
#endif

#endif	/* PHP_ZPHEUR_H */
