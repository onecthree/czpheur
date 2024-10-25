#include <php.h>
#include <ext/standard/info.h>
#include <php_zpheur.h>

#include "httpverb_arginfo.h"


static int httpverb_tokenizing_verb( char* request_method_src, int request_method_len )
{
	unsigned char c;
	unsigned long method_t = 0;
	for( int i = 0; i < request_method_len; i += 1 )
	{
		c = request_method_src[i];
		switch( c )
		{
			case 65 ... 90:
				method_t *= 100;
				method_t += c;
			break;
			default: goto end_break; break;
		}

		if( method_t > HTTPTOKEN_OPTIONS )
		{
			end_break:
			method_t = 0;
			break;
		}
	}

	switch( method_t )
	{
		case HTTPTOKEN_GET: 	return HTTPVERB_GET; break;
		case HTTPTOKEN_POST: 	return HTTPVERB_POST; break;
		case HTTPTOKEN_PUT: 	return HTTPVERB_PUT; break;
		case HTTPTOKEN_PATCH: 	return HTTPVERB_PATCH; break;
		case HTTPTOKEN_DELETE: 	return HTTPVERB_DELETE; break;
		case HTTPTOKEN_OPTIONS:	return HTTPVERB_OPTIONS; break;
		default: return HTTPVERB_UKNOWN; break;
	}
}

PHP_FUNCTION(httpverb)
{
	char*   request_method_src = NULL;
	size_t  request_method_len = 0;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_STRING(request_method_src, request_method_len)
	ZEND_PARSE_PARAMETERS_END();

	if(! httpverb_tokenizing_verb(request_method_src, request_method_len) )
		RETURN_BOOL(false);

	RETURN_STRINGL(request_method_src, request_method_len);
}