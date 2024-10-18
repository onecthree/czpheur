#include <php.h>
#include <ext/standard/info.h>
#include <php_zpheur.h>

#include <openssl/rand.h>
#include <include/onecstr.h>
#include "csrf_token_arginfo.h"

PHP_FUNCTION(csrf_token)
{
	char* 	prefix_src = NULL;
	size_t  prefix_len = 0;

	ZEND_PARSE_PARAMETERS_START(0, 1)
		Z_PARAM_OPTIONAL
		Z_PARAM_STRING(prefix_src, prefix_len)
	ZEND_PARSE_PARAMETERS_END();

	// // fetch dotenv file
	// zval* cache_path;
    // zval* dotenv_context = &ZPHEUR_G(DOTENV);

	// // create random
	// onec_string* random;
	// onec_string_init(random);
	// unsigned char buffer[16]; // Buffer to store the random bytes
    
    // if (RAND_bytes(buffer, sizeof(buffer)) != 1)
    //     zend_error(E_ERROR, "Error generating random bytes");

   	// for (int i = 0; i < sizeof(buffer); i++)
   	// {
	// 	char hex[3]; sprintf(hex, "%02x", buffer[i]); hex[2] = '\0';
    //     onec_string_insert(random, hex);
   	// }

	// // store as file
	// if(! (cache_path = dotenv_find(dotenv_context, "CSRF_TOKEN_FILECACHE_PATH", sizeof("CSRF_TOKEN_FILECACHE_PATH") - 1)) )
    // 	zend_error(E_ERROR, "ERROR csrf_token can't find path env");

	// zval* base_path = zend_get_constant(zend_string_init("APP_BASEPATH", sizeof("APP_BASEPATH") - 1, 0));

	// onec_string* retval;
	// onec_string* target_path;
	// onec_string_init(retval);
	// onec_string_init(target_path);

	// char* random_src = onec_string_get(random);

	// if( prefix_len )
	// {
	// 	onec_string_append(target_path, 5, Z_STRVAL_P(base_path), Z_STRVAL_P(cache_path), prefix_src, "_", random_src);
	// 	onec_string_append(retval, 3, prefix_src, "_", random_src);
	// }
	// else
	// {
	// 	onec_string_append(target_path, 3, Z_STRVAL_P(base_path), Z_STRVAL_P(cache_path), random_src);
	// 	onec_string_insert(retval, random_src);
	// }

	// char* 			target_path_src	= onec_string_get(target_path);
	// FILE* 			file_cache 		= fopen(target_path_src, "w");
	// zend_string*    retval_zs       = zend_string_init(retval->val, retval->len, 0);

	// /* Clean-up */
	// free(random_src);
	// onec_string_release(random);
	// onec_string_release(retval);
	// onec_string_release(target_path);

	// if(! file_cache )
	// 	zend_error(E_ERROR, "ERROR csrf_token can't create file cache");

	// fclose(file_cache);

	// RETURN_STR(retval_zs);
}