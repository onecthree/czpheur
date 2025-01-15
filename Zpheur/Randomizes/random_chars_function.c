
#include <php.h>
#include <ext/standard/info.h>
#include <php_zpheur.h>

#include <include/random.h>
#include "random_chars_arginfo.h"


PHP_FUNCTION(random_chars)
{
	zend_long random_length = 0;

	char* chars_set_src = NULL;
	size_t chars_set_len = 0;

	ZEND_PARSE_PARAMETERS_START(1, 2)
		Z_PARAM_LONG(random_length)
		Z_PARAM_OPTIONAL
		Z_PARAM_STRING(chars_set_src, chars_set_len)
	ZEND_PARSE_PARAMETERS_END();

	char buffer_src[random_length];
	size_t buffer_len = 0;

	if( random_chars(chars_set_src, chars_set_len, 
	random_length, buffer_src, &buffer_len) ) {
		zend_string* str = zend_string_init(buffer_src, buffer_len, 0);
		RETURN_STR(str);
	} 

	RETURN_FALSE;
}