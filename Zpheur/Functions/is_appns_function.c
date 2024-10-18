#include <php.h>
#include <ext/standard/info.h>
#include <php_zpheur.h>

#include "is_appns_arginfo.h"

PHP_FUNCTION(is_appns)
{
	char *class_name = NULL;
	size_t class_name_len = 0;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_STRING(class_name, class_name_len)
	ZEND_PARSE_PARAMETERS_END();

	if( class_name_len < 5 )
	{
		RETURN_BOOL(false);
	}

	if( class_name[0] != 'A' || (class_name[1] + class_name[2]) != 224 ||
		class_name[3] != '\\' )
	{
		RETURN_BOOL(false);
	}

	RETURN_BOOL(true);
}