#include <php.h>
#include <ext/standard/info.h>
#include <php_zpheur.h>
#include <Zend/zend_exceptions.h>

#include <include/runtime.h>
#include <zpheur.h>
#include "StreamLog_arginfo.h"

PHP_METHOD(StreamLog, __construct)
{
	zval* name;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ZVAL(name)
	ZEND_PARSE_PARAMETERS_END();

	zend_this_update_property("name", name);

#define LEVEL_COUNT 2

	char const* level_list[LEVEL_COUNT] = {
		"info_handlers",
		"warning_handlers",
	};

	size_t level_len[LEVEL_COUNT] = {
		sizeof("info_handlers") - 1,
		sizeof("warning_handlers") - 1,
	};

	for( int i = 0; i < LEVEL_COUNT; ++i )
	{
		HashTable* handlers;
		ALLOC_HASHTABLE(handlers);
		zend_hash_init(handlers, 0, NULL, ZVAL_PTR_DTOR, 0);

		zval handler;
		ZVAL_ARR(&handler, handlers);

		zend_update_property(
			Z_OBJCE_P(getThis()), Z_OBJ_P(getThis()),
			level_list[i], level_len[i], 
			&handler
		);
	}

}

PHP_METHOD(StreamLog, pushHandler)
{
	zval*		streamhandler;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ZVAL(streamhandler)
	ZEND_PARSE_PARAMETERS_END();

	// Should check if object was \StreamHandler?
	// TODO: add checker

	zval*  zv_streamhandler_level_info =
		php_class_call_method(Z_OBJ_P(streamhandler), "getLevel", sizeof("getLevel") - 1, 0, NULL, 0);

	zval target_streamhandler;
	zval *handlers;

	ZVAL_COPY(&target_streamhandler, streamhandler);

	switch( zval_get_long(zv_streamhandler_level_info) )
	{
		case 1: handlers = zend_this_read_property("info_handlers"); break;
		case 2: handlers = zend_this_read_property("warning_handlers"); break;
	}

	add_next_index_zval(handlers, &target_streamhandler);

	RETURN_ZVAL(getThis(), 1, 0);
}

PHP_METHOD(StreamLog, info)
{
	char*        message_src = NULL;
	size_t       message_len = 0;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_STRING(message_src, message_len)
	ZEND_PARSE_PARAMETERS_END();

	zval* handlers = zend_this_read_property("info_handlers");

	ZEND_HASH_FOREACH_VAL(Z_ARR_P(handlers), zval* handler)
	{
		zval* params_log = safe_emalloc(1, sizeof(zval), 0);
		ZVAL_STRINGL(&params_log[0], message_src, message_len);

		php_class_call_method(Z_OBJ_P(handler), "log", sizeof("log") - 1, 1, params_log, 0);
	}
	ZEND_HASH_FOREACH_END();

	RETURN_ZVAL(getThis(), 1, 0);
}


PHP_METHOD(StreamLog, warning)
{
	char*        message_src = NULL;
	size_t       message_len = 0;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_STRING(message_src, message_len)
	ZEND_PARSE_PARAMETERS_END();

	zval* handlers = zend_this_read_property("warning_handlers");

	ZEND_HASH_FOREACH_VAL(Z_ARR_P(handlers), zval* handler)
	{
		zval* params_log = safe_emalloc(1, sizeof(zval), 0);
		ZVAL_STRINGL(&params_log[0], message_src, message_len);

		php_class_call_method(Z_OBJ_P(handler), "log", sizeof("log") - 1, 1, params_log, 0);
	}
	ZEND_HASH_FOREACH_END();

	RETURN_ZVAL(getThis(), 1, 0);
}

ZEND_MINIT_FUNCTION(Zpheur_Logs_StreamLog)
{
    zend_class_entry ce;
    INIT_NS_CLASS_ENTRY(ce, "Zpheur\\Logs", "StreamLog", zpheur_logs_streamlog_class_method);
    zpheur_logs_streamlog_class_entry = zend_register_internal_class(&ce);
    zpheur_logs_streamlog_class_entry->ce_flags |= ZEND_ACC_NO_DYNAMIC_PROPERTIES;

    zend_declare_property_null(zpheur_logs_streamlog_class_entry, "name", sizeof("name") - 1, ZEND_ACC_PUBLIC);

    zend_declare_property_null(zpheur_logs_streamlog_class_entry, "info_handlers", sizeof("info_handlers") - 1, ZEND_ACC_PUBLIC);
    zend_declare_property_null(zpheur_logs_streamlog_class_entry, "warning_handlers", sizeof("warning_handlers") - 1, ZEND_ACC_PUBLIC);  

    return SUCCESS;
}