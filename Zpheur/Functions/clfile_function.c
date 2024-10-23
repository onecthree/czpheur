
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

	class_name_src[0] = tolower(class_name_src[0]);
	for( int i = 1; i < class_name_len; ++i )
		if( class_name_src[i] == '\\' )
			class_name_src[i] = '/';

	RETURN_STRINGL(class_name_src, class_name_len);
}