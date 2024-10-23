
#include <php.h>
#include <ext/standard/info.h>
#include <php_zpheur.h>

#include "clfile_arginfo.h"


PHP_FUNCTION(clfile)
{
	char*	class_name_src = NULL;
	size_t 	class_name_len = 0;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_STRING(class_name_src, class_name_len)
	ZEND_PARSE_PARAMETERS_END();
	
	if( class_name_len && !isalpha(class_name_src[0]) )
		php_error_docref(
			NULL, E_ERROR, 
			"the identifier for the class name cannot begin with a digit");

	/**
	 * Attention: Z_PARAM_STRING return a ref of zval_string struct
	 * instead a copy if you do manipulated string, it will changed to PHP-land.
	 * Make a copy instead
	 */
	unsigned char _class_name_src[class_name_len + 1];

	_class_name_src[0] = tolower(class_name_src[0]);
	_class_name_src[class_name_len] = '\0';

	for( size_t i = 1; i < class_name_len; ++i )
	{
		if( class_name_src[i] == '\\' )
			_class_name_src[i] = '/';
		else
			_class_name_src[i] = class_name_src[i];
	}

	RETURN_STRINGL(_class_name_src, class_name_len);
}