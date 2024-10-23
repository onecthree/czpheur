
#include <php.h>
#include <ext/standard/info.h>
#include <php_zpheur.h>

#include "zpheur_version_arginfo.h"
#include <include/onecstr.h>

PHP_FUNCTION(zpheur_version)
{
	zend_long target_version = 0;

	ZEND_PARSE_PARAMETERS_START(0, 1)
		Z_PARAM_OPTIONAL
		Z_PARAM_LONG(target_version)
	ZEND_PARSE_PARAMETERS_END();

	char* 		__string_src;
	size_t      __string_len;

	if( target_version & _Z_ONLY_NUMBER )
	{
		__string_src = (char* const)PHP_ZPHEUR_VERSION_NUMBER;
		__string_len = sizeof(PHP_ZPHEUR_VERSION_NUMBER) - 1;
	} else
	if( target_version & _Z_ONLY_TAG )
	{
		__string_src = (char* const)PHP_ZPHEUR_VERSION_TAG;
		__string_len = sizeof(PHP_ZPHEUR_VERSION_TAG) - 1;
	}
	else
	{
		onec_string* _zpheur_version;
		onec_string_init(_zpheur_version);

		onec_string_append(_zpheur_version, 3, 
			PHP_ZPHEUR_VERSION_NUMBER, "-", PHP_ZPHEUR_VERSION_TAG);

		__string_src = (char* const)_zpheur_version->val;
		__string_len = _zpheur_version->len;
	}	
 
	RETURN_STRINGL(__string_src, __string_len);
}