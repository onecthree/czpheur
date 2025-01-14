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

zend_object_handlers dotenv_object_handlers;

typedef struct _dotenv_t
{
	onec_string* source_path;
	onec_string* cache_path;
} dotenv_t;

typedef struct _dotenv_object
{
    dotenv_t* dotenv;
    zend_object std;
} dotenv_object;

void free_dotenv_object( zend_object* object )
{
    dotenv_object* instance = ZPHEUR_GET_OBJECT(dotenv_object, object);
    zend_object_std_dtor(&instance->std);

    if( instance->dotenv ) {
    	if( instance->dotenv->source_path ) {
    		onec_string_release(instance->dotenv->source_path);
    	}
    	if( instance->dotenv->cache_path ) {
    		onec_string_release(instance->dotenv->cache_path);
    	}
    	// if( instance->dotenv->stores ) {
    	// 	zend_hash_destroy(instance->dotenv->stores);
    	// 	FREE_HASHTABLE(instance->dotenv->stores);
    	// }
    	// if( instance->dotenv->comments ) {
    	// 	zend_hash_destroy(instance->dotenv->comments);
    	// 	FREE_HASHTABLE(instance->dotenv->comments);   		
    	// }

        efree(instance->dotenv);
    }
}

zend_object* create_dotenv_object( zend_class_entry* ce )
{
    dotenv_object* object = 
        ecalloc(1, sizeof(dotenv_object) + zend_object_properties_size(ce));

    zend_object_std_init(&object->std, ce);
    object_properties_init(&object->std, ce);

    memcpy(&dotenv_object_handlers, zend_get_std_object_handlers(), sizeof(dotenv_object_handlers));
    dotenv_object_handlers.offset = XtOffsetOf(dotenv_object, std);
    dotenv_object_handlers.free_obj = free_dotenv_object;
    object->std.handlers = &dotenv_object_handlers;

    object->dotenv = emalloc(sizeof(dotenv_t));
    object->dotenv->source_path = NULL;
    object->dotenv->cache_path = NULL;

    return &object->std;
}

PHP_METHOD(Dotenv, __construct)
{
	char*   source_path_src = NULL;
	size_t  source_path_len = 0;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_STRING(source_path_src, source_path_len)
	ZEND_PARSE_PARAMETERS_END();

    dotenv_object* instance =
    	ZPHEUR_ZVAL_GET_OBJECT(dotenv_object, getThis());

   	instance->dotenv->source_path =
		onec_string_initd(source_path_src, source_path_len);
}

PHP_METHOD(Dotenv, serialize)
{
	char*   cache_path_src = NULL;
	size_t  cache_path_len = 0;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_STRING(cache_path_src, cache_path_len)
	ZEND_PARSE_PARAMETERS_END();

    zval* key_value = zend_this_read_property("stores");

    if( Z_TYPE_P(key_value) != IS_NULL )
    {
		// onec_string* env_serialized = dotenv_cache_save(zend_hash_index_find(Z_ARR_P(key_value), 0));
		onec_string* env_serialized = dotenv_cache_save(key_value);
		php_stream* env_serialized_fd =
			php_stream_open_wrapper_ex(cache_path_src, "w", 0 | REPORT_ERRORS, NULL, NULL);
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

/**
 * As first argument is receive cached dotenv file,
 * it receive as array, if otherwise would be return null from
 * the function. 
 */
PHP_METHOD(Dotenv, unserialize)
{	
	zval* dotenv_cachesource = NULL;
	zval* callback = NULL;

	ZEND_PARSE_PARAMETERS_START(2, 2)
		Z_PARAM_ZVAL(dotenv_cachesource)
		Z_PARAM_ZVAL(callback);
	ZEND_PARSE_PARAMETERS_END();

	if( Z_TYPE_P(dotenv_cachesource) != IS_ARRAY )
		goto return_exit;

	if( Z_TYPE_P(callback) != IS_CALLABLE && Z_TYPE_P(callback) != IS_OBJECT )
		php_error_docref(
			NULL, E_ERROR,
			"Argument #2 ($callback) must be a valid callback, %s given",
			ZTYPE_TO_STR(Z_TYPE_P(callback)));

	/* {{{ */
	zend_this_update_property("env", dotenv_cachesource);
	ZEND_HASH_FOREACH_STR_KEY_VAL(Z_ARR_P(dotenv_cachesource), zend_string* env_name, zval* env_value)
	{
		zval* to_value = zend_hash_index_find(Z_ARR_P(env_value), 2);
		zend_string* to_str_value = zval_get_string(to_value);

		zval* callback_params = safe_emalloc(2, sizeof(zval), 0);
		// ZVAL_STRINGL(&callback_params[0], env_name->val, env_name->len);
		// ZVAL_STRINGL(&callback_params[1], to_str_value->val, to_str_value->len);
		zval _env_name; ZVAL_STRINGL(&_env_name, env_name->val, env_name->len);
		callback_params[0] = _env_name;
		zval _to_str_value; ZVAL_STRINGL(&_to_str_value, to_str_value->val, to_str_value->len);
		callback_params[1] = _to_str_value;

		php_call_closure(callback, Z_OBJ_P(callback), 2, callback_params);
		efree(callback_params);
	}
	ZEND_HASH_FOREACH_END();

	RETURN_NULL();
	/* }}} */

	return_exit:
	RETURN_ZVAL(getThis(), 1, 0);
}

PHP_METHOD(Dotenv, unsafeParse)
{
	bool save_comment = false;
	bool type_cast = false;

	ZEND_PARSE_PARAMETERS_START(2, 2)
		Z_PARAM_BOOL(save_comment)
		Z_PARAM_BOOL(type_cast)
	ZEND_PARSE_PARAMETERS_END();

    dotenv_object* instance =
    	ZPHEUR_ZVAL_GET_OBJECT(dotenv_object, getThis());

    onec_string* source_path = instance->dotenv->source_path;

    HashTable* stores;
    ALLOC_HASHTABLE(stores);
    zend_hash_init(stores, 0, NULL, ZVAL_PTR_DTOR, 0);

    HashTable* comments;
    ALLOC_HASHTABLE(comments);
    zend_hash_init(comments, 0, NULL, ZVAL_PTR_DTOR, 0);

	dotenv_unsafe_parse(source_path->val, source_path->len,
		&stores, &comments, 
	save_comment, type_cast);

	// zend_hash_destroy(stores);
	// FREE_HASHTABLE(stores);

	// zend_hash_destroy(comments);
	// FREE_HASHTABLE(comments);

	ZVAL_ARR(zend_this_read_property("stores"), stores);
	ZVAL_ARR(zend_this_read_property("comments"), comments);

	RETURN_ZVAL(getThis(), 1, 0);
}

PHP_METHOD(Dotenv, safeParse)
{	
	bool save_comment = false;
	bool type_cast = false;

	ZEND_PARSE_PARAMETERS_START(2, 2)
		Z_PARAM_BOOL(save_comment)
		Z_PARAM_BOOL(type_cast)
	ZEND_PARSE_PARAMETERS_END();

    dotenv_object* instance =
    	ZPHEUR_ZVAL_GET_OBJECT(dotenv_object, getThis());

    onec_string* source_path = instance->dotenv->source_path;

    HashTable* stores;
    ALLOC_HASHTABLE(stores);
    zend_hash_init(stores, 0, NULL, ZVAL_PTR_DTOR, 0);

    HashTable* comments;
    ALLOC_HASHTABLE(comments);
    zend_hash_init(comments, 0, NULL, ZVAL_PTR_DTOR, 0);

	dotenv_safe_parse(source_path->val, source_path->len,
		&stores, &comments, 
	save_comment, type_cast);

	// zend_hash_destroy(stores);
	// FREE_HASHTABLE(stores);

	// zend_hash_destroy(comments);
	// FREE_HASHTABLE(comments);

	ZVAL_ARR(zend_this_read_property("stores"), stores);
	ZVAL_ARR(zend_this_read_property("comments"), comments);

	RETURN_ZVAL(getThis(), 1, 0);
}

PHP_METHOD(Dotenv, save)
{
	ZEND_PARSE_PARAMETERS_NONE();
}

ZEND_MINIT_FUNCTION(Zpheur_DataTransforms_Dotenv)
{
    zend_class_entry ce;

    INIT_NS_CLASS_ENTRY(ce, "Zpheur\\DataTransforms", "Dotenv", zpheur_datatransforms_dotenv_dotenv_class_method);
    zpheur_datatransforms_dotenv_dotenv_class_entry = zend_register_internal_class(&ce);
    zpheur_datatransforms_dotenv_dotenv_class_entry->ce_flags |= ZEND_ACC_ALLOW_DYNAMIC_PROPERTIES;
    zpheur_datatransforms_dotenv_dotenv_class_entry->create_object = create_dotenv_object;

    zend_declare_property_null(zpheur_datatransforms_dotenv_dotenv_class_entry, "stores", sizeof("stores") - 1, ZEND_ACC_PUBLIC);
    zend_declare_property_null(zpheur_datatransforms_dotenv_dotenv_class_entry, "comments", sizeof("comments") - 1, ZEND_ACC_PUBLIC);

    return SUCCESS;
}