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

typedef struct _change_items
{
	size_t line_no;
	char* key_src;
	size_t key_len;
} change_items;

typedef struct _dotenv_t
{
	onec_string* source_path;
	onec_string* cache_path;
	change_items changes_src[128];
	size_t changes_len;
} dotenv_t;

typedef struct _dotenv_object
{
    dotenv_t* common;
    zend_object std;
} dotenv_object;

void free_dotenv_object( zend_object* object )
{
    dotenv_object* instance = ZPHEUR_GET_OBJECT(dotenv_object, object);
    zend_object_std_dtor(&instance->std);

    if( instance->common ) {
    	if( instance->common->source_path ) {
    		onec_string_release(instance->common->source_path);
    	}
    	if( instance->common->cache_path ) {
    		onec_string_release(instance->common->cache_path);
    	}
    	// if( instance->common->stores ) {
    	// 	zend_hash_destroy(instance->common->stores);
    	// 	FREE_HASHTABLE(instance->common->stores);
    	// }
    	// if( instance->common->comments ) {
    	// 	zend_hash_destroy(instance->common->comments);
    	// 	FREE_HASHTABLE(instance->common->comments);   		
    	// }

        efree(instance->common);
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

    object->common = emalloc(sizeof(dotenv_t));
    object->common->source_path = NULL;
    object->common->cache_path = NULL;
    object->common->changes_len = 0;

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

   	instance->common->source_path =
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

    onec_string* source_path = instance->common->source_path;

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

    onec_string* source_path = instance->common->source_path;

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

PHP_METHOD(Dotenv, set)
{
	char* key_src = NULL;
	size_t key_len = 0;

	zval* value = NULL;

	ZEND_PARSE_PARAMETERS_START(2, 2)
		Z_PARAM_STRING(key_src, key_len)
		Z_PARAM_ZVAL(value)
	ZEND_PARSE_PARAMETERS_END();

    dotenv_object* instance =
    	ZPHEUR_ZVAL_GET_OBJECT(dotenv_object, getThis());
    zval* stores = zend_this_read_property("stores");

    if(! zend_hash_str_exists(Z_ARR_P(stores), key_src, key_len) ) {
    	php_error_docref(NULL, E_ERROR, "\"%*s\" key is not exists", (int)key_len, key_src);
   		RETURN_FALSE;
    }

    char type = Z_TYPE_P(value);
   	if(! (1 << type & (MAY_BE_STRING | MAY_BE_LONG | MAY_BE_DOUBLE | MAY_BE_NULL)) ) {
   		php_error_docref(NULL, E_ERROR, "value must be type of string, integer, float, or null, %s given", ZTYPE_TO_STR(type));
   		RETURN_FALSE;
   	}	

    zval* env = zend_hash_str_find(Z_ARR_P(stores), key_src, key_len);

   	size_t* len = &instance->common->changes_len;
    instance->common->changes_src[*len] = (change_items){
    	.line_no = zval_get_long(zend_hash_index_find(Z_ARR_P(env), 0)),
    	.key_src = key_src,
    	.key_len = key_len,
    };
    *len = *len + 1;

    zval number;
    switch( type ) {
	    case IS_STRING:
		    zend_hash_index_update(Z_ARR_P(env), 2, value);
	    break;
	    case IS_LONG:
	    	ZVAL_STR(&number, zend_long_to_str(zval_get_long(value)));
		    zend_hash_index_update(Z_ARR_P(env), 2, &number);
	    break;
	    case IS_DOUBLE:
	    	ZVAL_STR(&number, zend_double_to_str(zval_get_double(value)));
		    zend_hash_index_update(Z_ARR_P(env), 2, &number);
	    break;
    }
    zend_hash_index_update(Z_ARR_P(env), 1, value);

	RETURN_TRUE;
}

PHP_METHOD(Dotenv, save)
{
	ZEND_PARSE_PARAMETERS_NONE();

    dotenv_object* instance =
    	ZPHEUR_ZVAL_GET_OBJECT(dotenv_object, getThis());
    zval* stores = zend_this_read_property("stores");

   	change_items tmp, *items = instance->common->changes_src;
    size_t line_changes_size = instance->common->changes_len, lower;

	/* {{{ Sorting lower to high */
    for( int i = 0; i < line_changes_size; i++ ) {
    	lower = i;

    	for( int ii = i + 1; ii < line_changes_size; ii++ ) {
    		if( items[ii].line_no < items[lower].line_no ) {
    			lower = ii;
    		}
    	}

    	tmp = items[lower];
    	items[lower] = items[i];
    	items[i] = tmp;
    }
    /* }}} */

	FILE* dotenv_fd;
    char* source_path_src = instance->common->source_path->val;
    size_t source_path_len = instance->common->source_path->len;
    source_path_src[source_path_len] = '\0';

    onec_string* buffer;
    onec_string_init(buffer);

	if(! (dotenv_fd = fopen(source_path_src, "r+")) ) {
		php_error_docref(NULL, E_ERROR, "%s: Failed to open file", source_path_src);
		RETURN_NULL();
	}

	char* line_src = NULL;
	size_t line_no = 1, line_current = 0, line_len;
	ssize_t read;

	while( (read = getline(&line_src, &line_len, dotenv_fd)) > -1 ) {
		if( line_current < line_changes_size && items[line_current].line_no == line_no ) {
			char* key_src = items[line_current].key_src;
			size_t key_len = items[line_current].key_len;
			key_src[key_len] = '\0';

			zval* env = zend_hash_str_find(Z_ARR_P(stores), key_src, key_len);
			zval* item = zend_hash_index_find(Z_ARR_P(env), 1);
			zend_string* item_str;

			switch( Z_TYPE_P(item) ) {
				case IS_STRING:
					onec_string_append(buffer, 4,  key_src, "='", Z_STRVAL_P(item), "'\n");
				break;
				case IS_LONG:
					item_str = zend_long_to_str(zval_get_long(item));
					item_str->val[item_str->len] = '\0';
					onec_string_append(buffer, 4,  key_src, "=", item_str->val, "\n");
				break;
				case IS_DOUBLE:
					item_str = zend_double_to_str(zval_get_double(item));
					item_str->val[item_str->len] = '\0';
					onec_string_append(buffer, 4,  key_src, "=", item_str->val, "\n");
				break;
				case IS_NULL:
					onec_string_append(buffer, 2,  key_src, "=\n");
				break;
			}

			line_current++;
		} else {
			// line_src[line_len] = '\0'; // prev_size error
			onec_string_append(buffer, 1, line_src);
		}
		line_no++;
	}
	buffer->val[buffer->len] = '\0';

	rewind(dotenv_fd);
	fprintf(dotenv_fd, "%*s", (int)buffer->len, buffer->val);


	fclose(dotenv_fd);
	onec_string_release(buffer);
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