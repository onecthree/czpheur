#include <php.h>
#include <ext/standard/info.h>
#include <ext/standard/basic_functions.h>
#include <php_zpheur.h>

#include <openssl/rand.h>
#include <include/onecstr.h>
#include "csrf_token_arginfo.h"

PHP_FUNCTION(csrf_token)
{
	char* 	prefix_src = "";
	size_t  prefix_len = 0;

	ZEND_PARSE_PARAMETERS_START(0, 1)
		Z_PARAM_OPTIONAL
		Z_PARAM_STRING(prefix_src, prefix_len)
	ZEND_PARSE_PARAMETERS_END();

	zend_string* cache_path;
	onec_stringlc random, retval, target_path;
	
	onec_string_initlc(random);
	onec_string_initlc(retval);
	onec_string_initlc(target_path);

	// Buffer to store the random bytes
	unsigned char buffer[16];
    
    if( RAND_bytes(buffer, sizeof(buffer)) != 1 )
        zend_error(E_ERROR, "Error generating random bytes");

   	for( int i = 0; i < sizeof(buffer); i++ )
   	{
		char hex[3];
		sprintf(hex, "%02x", buffer[i]);
		hex[2] = '\0';
        onec_string_insertlc(random, hex);
   	}

	// store as file
	if(! (cache_path = php_getenv("CSRF_TOKEN_FILECACHE_PATH", sizeof("CSRF_TOKEN_FILECACHE_PATH") - 1)) )
    	php_error_docref(NULL, E_ERROR, "ERROR csrf_token can't find path env");

	zval* base_path = zend_get_constant(zend_string_init("APP_BASEPATH", sizeof("APP_BASEPATH") - 1, 0));

	onec_string_trimlc(random);
	char* const _prefix = prefix_len ? "_" : "";
	onec_string_appendlc(target_path, 5, Z_STRVAL_P(base_path), 
		cache_path->val, prefix_src, _prefix, random.val);
	onec_string_appendlc(retval, 3, prefix_src, _prefix, random.val);

	onec_string_trimlc(target_path);
	FILE* file_cache = fopen(target_path.val, "w");

	onec_string_trimlc(retval);
	ZVAL_STRINGL(return_value, retval.val, retval.len);

	if(! file_cache )
		zend_error(E_ERROR, "ERROR csrf_token can't create file cache");

	fclose(file_cache);
}