#include <php.h>
#include <php_zpheur.h>
#include "ext/standard/info.h"

#include <include/onecstr.h>
#include "Env_arginfo.h"


PHP_FUNCTION(env)
{
    zval*   value;
    char*   key_src = NULL;
    size_t  key_len = 0;

    zval *zv_value;

	ZEND_PARSE_PARAMETERS_START(1, 2)
		Z_PARAM_STRING(key_src, key_len)
        Z_PARAM_OPTIONAL
        Z_PARAM_ZVAL(zv_value)
	ZEND_PARSE_PARAMETERS_END();

    onec_string* vname;
    onec_string_init(vname);

    onec_string_append(vname, 2, "ENV_", key_src);

    zend_string* envname = zend_string_init(vname->val, vname->len, 0);
    zval* env_value;

    // if( (env_value = zend_get_constant(envname)) != NULL )
    // {
    //     onec_string_release(vname);
    //     RETURN_ZVAL(env_value, 1, 0);
    // }

    onec_string_release(vname);
    RETURN_NULL();
}