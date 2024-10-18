
#include <php.h>
#include <ext/standard/info.h>
#include <php_zpheur.h>

#include "clfile_arginfo.h"


PHP_FUNCTION(clfile)
{
	char *class_name_src = NULL;
	size_t class_name_len = 0;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_STRING(class_name_src, class_name_len)
	ZEND_PARSE_PARAMETERS_END();

	char *class_name = malloc((class_name_len * sizeof(char)) + 1);
	strncpy(class_name, class_name_src, class_name_len);


	if( class_name_len < 1 || !isalpha(class_name[0]) )
	{
		zend_error(E_ERROR, "class kurang bos");
	}

	class_name[0] = tolower(class_name[0]);
	for( int i = 1; i < class_name_len; ++i )
	{
		if( class_name[i] == '\\' )
		{
			class_name[i] = '/';
		}
	}

	RETURN_STRINGL(class_name, class_name_len);
}