#include <php.h>
#include <ext/standard/info.h>
#include <php_zpheur.h>

#include "contains_arginfo.h"


#define CONTAINS_CHAR_WHITESPACE	' '
#define CONTAINS_CHAR_NULL			'\0'

PHP_FUNCTION(contains)
{
	char*    string_src = NULL;
	size_t   string_len = 0;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_STRING(string_src, string_len)
	ZEND_PARSE_PARAMETERS_END();

 	size_t 	index = 0;
 	while( index < string_len )
 	{
 		char input = string_src[index];

 		if( input != CONTAINS_CHAR_WHITESPACE )
 			RETURN_TRUE;

 		index += 1;
 	}

	RETURN_FALSE;
}