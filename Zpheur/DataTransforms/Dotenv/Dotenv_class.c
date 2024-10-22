#include <php.h>
#include <ext/standard/info.h>
#include <php_zpheur.h>

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <include/onecstr.h>
#include <zpheur.h>
#include <include/runtime.h>
#include <include/dotenv.h>
#include <include/libstr.h>
#include "Dotenv_arginfo.h"


PHP_METHOD(Dotenv, __construct)
{
	char*   env_path_src = NULL;
	size_t  env_path_len = 0;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_STRING(env_path_src, env_path_len)
	ZEND_PARSE_PARAMETERS_END();

	zval env_path;
	ZVAL_STRINGL(&env_path, env_path_src, env_path_len);

	zend_this_update_property("env_path", &env_path);
}

PHP_METHOD(Dotenv, serialize)
{
	char*   cache_path_src = NULL;
	size_t  cache_path_len = 0;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_STRING(cache_path_src, cache_path_len)
	ZEND_PARSE_PARAMETERS_END();

    zval* key_value = zend_this_read_property("env");

    if( Z_TYPE_P(key_value) != IS_NULL )
    {
		onec_string* env_serialized = dotenv_cache_save(zend_hash_index_find(Z_ARR_P(key_value), 0));
		php_stream* env_serialized_fd = php_stream_open_wrapper_ex(cache_path_src, "w", 0 | REPORT_ERRORS, NULL, NULL);
	    php_stream_write(env_serialized_fd, env_serialized->val, env_serialized->len); 

		onec_string_release(env_serialized);
		php_stream_close(env_serialized_fd);
	}
	else
	{
		php_error_docref(NULL, E_ERROR, "dotenv file must be parsed before serialization");
	}

    RETURN_ZVAL(getThis(), 1, 0);
}

PHP_METHOD(Dotenv, unserialize)
{	
	zval* dotenv_cachesource = NULL;
	zval* callback = NULL;

	ZEND_PARSE_PARAMETERS_START(2, 2)
		Z_PARAM_ZVAL(dotenv_cachesource)
		Z_PARAM_ZVAL(callback);
	ZEND_PARSE_PARAMETERS_END();

	if( Z_TYPE_P(dotenv_cachesource) == IS_ARRAY )
	{
		zend_this_update_property("env", dotenv_cachesource);

		ZEND_HASH_FOREACH_STR_KEY_VAL(Z_ARR_P(dotenv_cachesource), zend_string* env_name, zval* env_value)
		{
			zval* to_value = zend_hash_index_find(Z_ARR_P(env_value), 2);
			zend_string* to_str_value = zval_get_string(to_value);

			zval* callback_params = safe_emalloc(2, sizeof(zval), 0);
			ZVAL_STRINGL(&callback_params[0], env_name->val, env_name->len);
			ZVAL_STRINGL(&callback_params[1], to_str_value->val, to_str_value->len);

			php_call_closure(callback, Z_OBJ_P(callback), 2, callback_params);
		}
		ZEND_HASH_FOREACH_END();


		RETURN_NULL();
	}

	RETURN_ZVAL(getThis(), 1, 0);
}

PHP_METHOD(Dotenv, parse)	
{	
	bool save_comment = false;
	bool type_cast = false;

	ZEND_PARSE_PARAMETERS_START(2, 2)
		Z_PARAM_BOOL(save_comment)
		Z_PARAM_BOOL(type_cast)
	ZEND_PARSE_PARAMETERS_END();

    zval* env_path = zend_this_read_property("env_path");

    HashTable *key_value;
    ALLOC_HASHTABLE(key_value);
    zend_hash_init(key_value, 0, NULL, ZVAL_PTR_DTOR, 0);

    HashTable *comments;
    ALLOC_HASHTABLE(comments);
    zend_hash_init(comments, 0, NULL, ZVAL_PTR_DTOR, 0);

	dotenv_parse(Z_STRVAL_P(env_path), &key_value, &comments, save_comment, type_cast);

	zval key_value_target;
	ZVAL_ARR(&key_value_target, key_value);
	zval commens_target;
	ZVAL_ARR(&commens_target, comments);

	zval dotenv_context;
	array_init(&dotenv_context);

	add_next_index_zval(&dotenv_context, &key_value_target);
	add_next_index_zval(&dotenv_context, &commens_target);

	zend_this_update_property("env", &dotenv_context);

	RETURN_ZVAL(getThis(), 1, 0);
}

ZEND_MINIT_FUNCTION(Zpheur_DataTransforms_Dotenv_Dotenv)
{
    zend_class_entry ce;

    INIT_NS_CLASS_ENTRY(ce, "Zpheur\\DataTransforms\\Dotenv", "Dotenv", zpheur_datatransforms_dotenv_dotenv_class_method);
    zpheur_datatransforms_dotenv_dotenv_class_entry = zend_register_internal_class(&ce);
    zpheur_datatransforms_dotenv_dotenv_class_entry->ce_flags |= ZEND_ACC_ALLOW_DYNAMIC_PROPERTIES;

    zend_declare_property_null(zpheur_datatransforms_dotenv_dotenv_class_entry, "env_path", sizeof("env_path") - 1, ZEND_ACC_PUBLIC);
    zend_declare_property_null(zpheur_datatransforms_dotenv_dotenv_class_entry, "env", sizeof("env") - 1, ZEND_ACC_PUBLIC);

    return SUCCESS;
}