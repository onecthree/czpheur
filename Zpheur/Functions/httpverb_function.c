#include <php.h>
#include <ext/standard/info.h>
#include <php_zpheur.h>

#include "httpverb_arginfo.h"


#define HTTPVERB_UKNOWN		0
#define HTTPVERB_GET 		1
#define HTTPVERB_POST 		2
#define HTTPVERB_PUT		3
#define HTTPVERB_PATCH  	4
#define HTTPVERB_DELETE 	5
#define HTTPVERB_OPTIONS	6

uint8_t httpverb_tokenizing_verb( char *request_method, uint64_t request_method_len )
{
	switch( request_method_len )
	{
		case 3:
			if( strcmp(request_method, "GET") == 0 )
			{
				return HTTPVERB_GET;
			}

			if( strcmp(request_method, "PUT") == 0 )
			{
				return HTTPVERB_PUT;
			}
		break;

		case 4:
			if( strcmp(request_method, "POST") == 0 )
			{
				return HTTPVERB_POST;
			}
		break;

		case 5:
			if( strcmp(request_method, "PATCH") == 0 )
			{
				return HTTPVERB_PATCH;
			}
		break;

		case 6:
			if( strcmp(request_method, "DELETE") == 0 )
			{
				return HTTPVERB_DELETE;
			}
		break;

		case 7:
			if( strcmp(request_method, "OPTIONS") == 0 )
			{
				return HTTPVERB_OPTIONS;
			}
		break;
	}

	return HTTPVERB_UKNOWN;
}

PHP_FUNCTION(httpverb)
{
	char *request_method = NULL;
	size_t request_method_len = 0;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_STRING(request_method, request_method_len)
	ZEND_PARSE_PARAMETERS_END();

	if( httpverb_tokenizing_verb(request_method, request_method_len) == HTTPVERB_UKNOWN )
	{
		RETURN_BOOL(false);
	}

	RETURN_STRINGL(request_method, request_method_len);
}