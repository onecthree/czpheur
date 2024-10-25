
#include <php.h>
#include <ext/standard/info.h>
#include <php_zpheur.h>

#include <string.h>
#include "is_appns_arginfo.h"


PHP_FUNCTION(is_appns)
{
	char*  class_name_src = NULL;
	size_t class_name_len = 0;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_STRING(class_name_src, class_name_len)
	ZEND_PARSE_PARAMETERS_END();

	if( class_name_len < 5 || strncmp("App\\", class_name_src, 4) != 0 )
		RETURN_BOOL(false);

	RETURN_BOOL(true);
}