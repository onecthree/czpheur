#include <php.h>
#include <ext/standard/info.h>
#include <php_zpheur.h>

#include <zpheur.h>
#include <include/runtime.h>
#include <include/onecstr.h>
#include "StreamHandler_arginfo.h"


PHP_METHOD(StreamHandler, __construct)
{
	char*		path_log_src = NULL;
	size_t  	path_log_len = 0;

	zend_ulong  level;

	ZEND_PARSE_PARAMETERS_START(2, 2)
		Z_PARAM_STRING(path_log_src, path_log_len)
		Z_PARAM_LONG(level)
	ZEND_PARSE_PARAMETERS_END();

	zval path_log;
	ZVAL_STRINGL(&path_log, path_log_src, path_log_len);

	zval target_level;
	ZVAL_LONG(&target_level, level);

	zend_this_update_property("path_log", &path_log);
	zend_this_update_property("level", &target_level);
}

PHP_METHOD(StreamHandler, datetimeObject)
{
	zval* datetime;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_ZVAL(datetime)
	ZEND_PARSE_PARAMETERS_END();

	if( zend_string_equals_cstr(Z_OBJCE_P(datetime)->name, "DateTime", sizeof("DateTime") - 1) )
	{
		zval target_datetime;
		ZVAL_COPY(&target_datetime, datetime);
		zend_this_update_property("datetime_object", &target_datetime);
	}
    else
    {
        zend_error(E_ERROR, "argument must be DateTime::class object");
    }

	RETURN_ZVAL(getThis(), 1, 0);
}	

PHP_METHOD(StreamHandler, getLevel)
{
	ZEND_PARSE_PARAMETERS_NONE();

	zval* level = zend_this_read_property("level");

	RETURN_ZVAL(level, 1, 0);
}

PHP_METHOD(StreamHandler, datetimeFormat)
{
	char*          datetime_format_src = NULL;
	size_t         datetime_format_len = 0;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_STRING(datetime_format_src, datetime_format_len)
	ZEND_PARSE_PARAMETERS_END();	

	zval datetime_format;
	ZVAL_STRINGL(&datetime_format, datetime_format_src, datetime_format_len);

	zend_this_update_property("datetime_format", &datetime_format);

	RETURN_ZVAL(getThis(), 1, 0);
}	

PHP_METHOD(StreamHandler, setDelimiter)
{
	char*         delimiter_src = NULL;
	size_t        delimiter_len = 0;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_STRING(delimiter_src, delimiter_len)
	ZEND_PARSE_PARAMETERS_END();

	zval delimiter;

	if( delimiter_len )
	{
	  	size_t new_delimiter_len = snprintf(NULL, 0, "\n%s\n", delimiter_src);
	  	char new_delimiter_src[new_delimiter_len + 1];
	  	php_sprintf(new_delimiter_src, "\n%s\n", delimiter_src);
	  	new_delimiter_src[new_delimiter_len] = '\0';

		ZVAL_STRINGL(&delimiter, new_delimiter_src, new_delimiter_len);
	}
	else
	{
		ZVAL_STRINGL(&delimiter, "\n", sizeof("\n") - 1);	
	}

	zend_this_update_property("delimiter", &delimiter);

	RETURN_ZVAL(getThis(), 1, 0);
}

PHP_METHOD(StreamHandler, setLogFormat)
{
	char*         format_src = NULL;
	size_t        format_len = 0;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_STRING(format_src, format_len)
	ZEND_PARSE_PARAMETERS_END();

	zval log_format;
	ZVAL_STRINGL(&log_format, format_src, format_len);

	zend_this_update_property("format_log", &log_format);

	RETURN_ZVAL(getThis(), 1, 0);
}

onec_string* 	format_log( char* const format, char* const datetime, char* const level, char* const message );
char* const 	get_level( zend_ulong level );

PHP_METHOD(StreamHandler, log)
{
	char*        log_src = NULL;
	size_t       log_len = 0;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_STRING(log_src, log_len)
	ZEND_PARSE_PARAMETERS_END();	

	zval* path_log 			= zend_this_read_property("path_log");
	zval* delimiter 		= zend_this_read_property("delimiter");
	zval* datetime_obj 		= zend_this_read_property("datetime_object");
	zval* datetime_format 	= zend_this_read_property("datetime_format");
	zval* format 			= zend_this_read_property("format_log");
	zval* level 			= zend_this_read_property("level");

	php_stream *log_file = 
		php_stream_open_wrapper_ex(Z_STRVAL_P(path_log), "a", 0, NULL, NULL);

    if( !log_file )
    {
        zend_error(
            E_ERROR,
            "File log \"%s\" not found",
            Z_STRVAL_P(path_log)
        );
    }

    zval* params_format = safe_emalloc(1, sizeof(zval) , 0);
    ZVAL_ZVAL(&params_format[0], datetime_format, 1, 0);

    zval* datetime = 
    	php_class_call_method(Z_OBJ_P(datetime_obj), "format", sizeof("format") - 1, 1, params_format, 0);

    onec_string* new_msg = format_log(
    	Z_STRVAL_P(format),
    	Z_STRVAL_P(datetime),
    	(char* const)get_level(zval_get_long(level)),
    	(char* const)log_src
    );

  	onec_string_append(new_msg, 1, Z_STRVAL_P(delimiter));

    php_stream_write(log_file, new_msg->val, new_msg->len);

    onec_string_release(new_msg);
}

#define LOG_TOKEN_CHAR_NULL         	'\0'
#define LOG_TOKEN_SYMBOL_PERCENT    	'%'
#define LOG_STATE_NORMAL_TEXT       	1
#define LOG_STATE_FORMAT_TEXT       	2
#define LOG_FORMAT_COND_DATETIME    	'd'
#define LOG_FORMAT_COND_LEVEL       	'l'
#define LOG_FORMAT_COND_MESSAGE     	'm'

onec_string* format_log( char* const format, char* const datetime, char* const level, char* const message )
{
    onec_string* log;
    onec_string_init(log);

    char        input;
    size_t      index = 0;
    uint8_t     state = LOG_STATE_NORMAL_TEXT;

    while( true )
    {
        input = format[index];
        index += 1;

        switch( state )
        {
            case LOG_STATE_NORMAL_TEXT:
                switch( input )
                {
                    case LOG_TOKEN_CHAR_NULL:
                        goto end_return;
                    break;
                    case LOG_TOKEN_SYMBOL_PERCENT:
                        state = LOG_STATE_FORMAT_TEXT;
                    break;
                    default:
                        onec_string_put(log, input);
                    break;
                }
            break;
            case LOG_STATE_FORMAT_TEXT:
                switch( input )
                {
                    case LOG_FORMAT_COND_DATETIME:
                        onec_string_append(log, 1, datetime);
                    break;
                    case LOG_FORMAT_COND_LEVEL:
                        onec_string_append(log, 1, level);
                    break;
                    case LOG_FORMAT_COND_MESSAGE:
                        onec_string_append(log, 1, message);                        
                    break;
                }

                state = LOG_STATE_NORMAL_TEXT;
            break;
        }
    }

    end_return:
    return log;
}

char* const get_level( zend_ulong level )
{
	switch( level )
	{
		case 1: return (char* const)"INFO"; break;
		case 2: return (char* const)"WARNING"; break;	
	}

	return (char* const)"UKNOWN";
}

ZEND_MINIT_FUNCTION(Zpheur_Logs_StreamLog_StreamHandler)
{
    zend_class_entry ce;
    INIT_NS_CLASS_ENTRY(ce, "Zpheur\\Logs\\StreamLog", "StreamHandler", zpheur_logs_streamlog_streamhandler_class_method);
    zpheur_logs_streamlog_streamhandler_class_entry = zend_register_internal_class(&ce);
    zpheur_logs_streamlog_streamhandler_class_entry->ce_flags |= ZEND_ACC_NO_DYNAMIC_PROPERTIES;

    zend_declare_class_constant_stringl(zpheur_logs_streamlog_streamhandler_class_entry, "DEFAULT_DELIMITER", sizeof("DEFAULT_DELIMITER") - 1, "", sizeof("") - 1);

    zend_declare_property_null(zpheur_logs_streamlog_streamhandler_class_entry, "path_log", sizeof("path_log") - 1, ZEND_ACC_PUBLIC);  
    zend_declare_property_null(zpheur_logs_streamlog_streamhandler_class_entry, "level", sizeof("level") - 1, ZEND_ACC_PUBLIC);  
    zend_declare_property_null(zpheur_logs_streamlog_streamhandler_class_entry, "datetime_object", sizeof("datetime_object") - 1, ZEND_ACC_PUBLIC);   
    zend_declare_property_null(zpheur_logs_streamlog_streamhandler_class_entry, "datetime_format", sizeof("datetime_format") - 1, ZEND_ACC_PUBLIC);   
    zend_declare_property_null(zpheur_logs_streamlog_streamhandler_class_entry, "delimiter", sizeof("delimiter") - 1, ZEND_ACC_PUBLIC);   
    zend_declare_property_null(zpheur_logs_streamlog_streamhandler_class_entry, "format_log", sizeof("format_log") - 1, ZEND_ACC_PUBLIC);   

    return SUCCESS;
}