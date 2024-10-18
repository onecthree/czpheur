#include <php.h>
#include <ext/standard/info.h>
#include <php_zpheur.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <dirent.h>
#include <openssl/md5.h>
#include <include/runtime.h>
#include "DefaultAction_arginfo.h"


PHP_METHOD(Http_DefaultAction, __construct)
{
    ZEND_PARSE_PARAMETERS_NONE();
}

PHP_METHOD(Http_DefaultAction, view)
{
    char *template_name = NULL;
    size_t template_name_len = 0;

    zval *data_mapper;
    
    ZEND_PARSE_PARAMETERS_START(1, 2)
        Z_PARAM_STRING(template_name, template_name_len)
        Z_PARAM_OPTIONAL
        Z_PARAM_ARRAY(data_mapper)
    ZEND_PARSE_PARAMETERS_END();

    zval *zv_app_path = zend_get_constant_str("APP_BASEPATH", sizeof("APP_BASEPATH") - 1);

    if( !zv_app_path )
    {
        zend_error(
            E_ERROR,
            "Undefined constant \"APP_BASEPATH\""
        );
    }

    int vie_filepath_len = snprintf(NULL, 0, "%s/app/Views/%s.phtml", Z_STRVAL_P(zv_app_path), template_name);
    char view_filepath[vie_filepath_len];
    php_sprintf(view_filepath, "%s/app/Views/%s.phtml", Z_STRVAL_P(zv_app_path), template_name);

    // check if file exists otherwise throw error
    php_stream *is_exist = php_stream_open_wrapper_ex(view_filepath, "r", 0, NULL, NULL);
    if( is_exist == NULL )
    {
        zend_error(
            E_ERROR,
            "View \"%s\" not found",
            view_filepath
        );
    }

    if( ZEND_NUM_ARGS() > 1 )
    {
        ZEND_HASH_MAP_FOREACH_STR_KEY_VAL(Z_ARR_P(data_mapper), zend_string *index, zval* val)
        {
            zend_set_local_var(index, val, 1);
        }
        ZEND_HASH_FOREACH_END();
    }

    int view_file_len = snprintf(NULL, 0, "require '%s'", view_filepath);
    char view_file[view_file_len];
    php_sprintf(view_file, "require '%s'", view_filepath);

    zval _eval_result;
    zend_eval_stringl(view_file, view_file_len, &_eval_result, view_filepath);
}

PHP_METHOD(Http_DefaultAction, exitCode)
{
    zend_ulong exit_code = 0;

    ZEND_PARSE_PARAMETERS_START(1, 1)
        Z_PARAM_LONG(exit_code)
    ZEND_PARSE_PARAMETERS_END();

    zend_register_long_constant("EXIT_CODE", sizeof("EXIT_CODE") -1, exit_code, CONST_CS | CONST_PERSISTENT, 51);

    RETURN_LONG(exit_code);
}

ZEND_MINIT_FUNCTION(Zpheur_Actions_Http_DefaultAction)
{
    zend_class_entry ce;
    INIT_NS_CLASS_ENTRY(ce, "Zpheur\\Actions\\Http", "DefaultAction", zpheur_actions_defaultaction_class_method);
    zpheur_actions_http_defaultaction_class_entry = zend_register_internal_class(&ce);
    zpheur_actions_http_defaultaction_class_entry->ce_flags |= ZEND_ACC_NO_DYNAMIC_PROPERTIES;

    return SUCCESS;
}