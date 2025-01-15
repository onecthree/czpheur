#include <php.h>
#include <ext/standard/info.h>
#include <php_zpheur.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <include/onecstr.h>
#include "dotenv.h"


void static inline __attribute__ ((always_inline))
tcast_check( int64_t* tcast_bool_inc, int* tcast_numeric_state,  dotenv_context* context )
{
	if( (*tcast_numeric_state) < 2 )
	{
		switch( context->input )
		{
			case '-':
				if( (*tcast_bool_inc) != 0 )
					(*tcast_numeric_state) = 2;
			break;
			case '0' ... '9': break;
			case '.':
			default:
				(*tcast_numeric_state) += 1;
			break;
		}
	}

	if( (*tcast_bool_inc) > -1 )
	{
		if( (*tcast_bool_inc) < TCAST_BOOL_FALSE_LONG_CONSTANT )
		{
			if( context->input < 100 )
				(*tcast_bool_inc) *= 100;
			else
				(*tcast_bool_inc) *= 1000;

			(*tcast_bool_inc) += context->input;
		}
		else
			(*tcast_bool_inc) = -1;
	}
}

int dotenv_unsafe_parse( char* source_path_src, size_t source_path_len, HashTable** target_key_value, HashTable** target_comments, bool save_comment, bool type_cast )
{
	FILE* file_env = fopen(source_path_src, "r");
	int error_no = -1;
	size_t error_line, error_column;
	// onec_string_getlc

	// On fopen error call 
	if( !file_env ) {
		error_no = 0;
		goto exit_parse;
	}

	// Type cast
	int64_t tcast_bool_inc = 0;
	int tcast_numeric_state = 0;

	onec_stringlc de_comment;
	onec_string_initlc(de_comment);

    onec_stringlc de_lvalue, de_rvalue;
    onec_string_initlc(de_lvalue);
    onec_string_initlc(de_rvalue);

	dotenv_context* context = (dotenv_context*)malloc(sizeof(dotenv_context));
	context->state = STATE_INIT_SCOPE;
	context->under_comment = false;
	context->line = 1;

	// dotenvlc_value value = (dotenv_value*)malloc(sizeof(dotenv_value));
	dotenvlc_value value;
	dotenv_value_context* value_context = (dotenv_value_context*)malloc(sizeof(dotenv_value_context));
	int increments = 0;

	while( (context->input = fgetc(file_env)) )
	{
		if( context->state == STATE_INIT_SCOPE )
		{
			context->prev_state	= STATE_BEGIN_INDENTIFIER_SCOPE;
			context->state = STATE_COMMENT_SCOPE;

			context->column = 0;
			context->under_comment	= false;

            value.rvalue = NULL;
            value.lvalue = NULL;

			value_context->under_single_quote = false;
			value_context->under_double_quote = false;
		}

		context->column += 1;

		switch( context->state )
		{
			case STATE_COMMENT_SCOPE:
				switch( context->input )
				{
					case TOKEN_SYMBOL_EOF:
						context->state = STATE_EOF_SCOPE;
					break;
					case TOKEN_SYMBOL_NEWLINE:
						if( context->under_comment )
						{
							// Parse/fetch comments per line
							#ifndef PHP_VERSION
							dotenv_printout("# [%ld] `%s`\n", context->line, onec_string_getlc(de_comment));
							#else
							// Fetch over here for php
							onec_string_trimlc(de_comment);
							// zval comment;
							// ZVAL_STRINGL(&comment, de_comment.val, de_comment.len);
							zend_string* _comment = zend_string_init(de_comment.val, de_comment.len, 0);
							zval comment; ZVAL_STR(&comment, _comment);
							zend_hash_index_update((*target_comments), context->line, &comment);
							#endif
							onec_string_resetlc(de_comment);	
						}

						context->state = STATE_NEWLINE_SCOPE;
						fseek(file_env, -1, SEEK_CUR);
					break;
					case TOKEN_SYMBOL_TABSPACE:
					case TOKEN_SYMBOL_WHITESPACE:
						if(! context->under_comment )
						{
							break;
						}
					default:
						if( context->under_comment )
						{
							onec_string_putlc(de_comment, context->input);
							break;
						}

						switch( context->prev_state )
						{
							case STATE_BEGIN_INDENTIFIER_SCOPE:
								increments = -1;
								context->state = STATE_BEGIN_INDENTIFIER_SCOPE;
							break;
							case STATE_NON_QUOTE_VALUE_SCOPE:
								increments = -2;
								context->state = STATE_END_SCOPE;
							break;
							default:
								error_no = 1;
								goto end_parse;
							break;
						}

						context->column += increments;
						fseek(file_env, increments, SEEK_CUR);
					break;
					case TOKEN_SYMBOL_HASHMARK:
						if( save_comment )
							context->under_comment = true;
						else
							context->state = STATE_SKIP_COMMENT_SCOPE;
					break;
				}
			break;
			case STATE_SKIP_COMMENT_SCOPE:
				switch( context->input )
				{
					case TOKEN_SYMBOL_NEWLINE:
						context->state = STATE_NEWLINE_SCOPE;
						fseek(file_env, -1, SEEK_CUR);
					break; 
					case TOKEN_SYMBOL_EOF:
						context->state = STATE_EOF_SCOPE;
					break; 
				}
			break;
			case STATE_ESCAPE_SCOPE:
				context->state = context->prev_state;
	           	onec_string_putlc(de_rvalue, context->input);
			break;
			case STATE_BEGIN_INDENTIFIER_SCOPE:
				switch( context->input )
				{
					case TOKEN_SYMBOL_EOF:
						context->column -= 1;
						context->state = STATE_EOF_SCOPE;
						fseek(file_env, -1, SEEK_CUR);
					break;
					case TOKEN_SYMBOL_BREAKLINE:
						context->column -= 1;
						context->line 	+= 1;
					break;
					case TOKEN_SYMBOL_TABSPACE:
						context->column += 3;
					case TOKEN_SYMBOL_WHITESPACE:
					break;
					case TOKEN_DIGIT_START ... TOKEN_DIGIT_END:
						error_line = context->line;
						error_column = context->column;
						error_no = 2;
						goto end_parse;
					break;	
					case TOKEN_SYMBOL_UNDERSCORE:
					case TOKEN_ALPHA_LOWER_START ... TOKEN_ALPHA_LOWER_END:
					case TOKEN_ALPHA_UPPER_START ... TOKEN_ALPHA_UPPER_END:
                    	onec_string_putlc(de_lvalue, context->input);
						context->state = STATE_INDENTIFIER_SCOPE;
					break;
					default:
						error_line = context->line;
						error_column = context->column;
						error_no = 3;
						goto end_parse;
					break;
				}
			break;
			case STATE_INDENTIFIER_SCOPE:
				switch( context->input )
				{
					case OPERATOR_SYMBOL_EQUAL:
						context->column -= 1;
						context->state = STATE_OPERATOR_SCOPE;
						fseek(file_env, -1, SEEK_CUR);
					break;
					case TOKEN_DIGIT_START ... TOKEN_DIGIT_END:
					case TOKEN_ALPHA_LOWER_START ... TOKEN_ALPHA_LOWER_END:
					case TOKEN_ALPHA_UPPER_START ... TOKEN_ALPHA_UPPER_END:
					case TOKEN_SYMBOL_UNDERSCORE:
	                    onec_string_putlc(de_lvalue, context->input);
					break;
					default:
						error_line = context->line;
						error_column = context->column;
						error_no = 4;
						goto end_parse;
					break;
				}	
			break;
			case STATE_OPERATOR_SCOPE:
				context->state = STATE_FIND_VALUE_SCOPE;
                // value->lvalue = onec_string_initd(de_lvalue.val, de_lvalue.len);
                value.lvalue = &de_lvalue;
			break;
			case STATE_FIND_VALUE_SCOPE:
				switch( context->input )
				{	
					case TOKEN_SYMBOL_EOF:
						context->state = STATE_EOF_SCOPE;
						// value->rvalue = onec_string_initd(de_rvalue.val, de_rvalue.len);
						value.rvalue = &de_rvalue;
						fseek(file_env, -1, SEEK_CUR);
					break;
					case TOKEN_SYMBOL_NEWLINE:
						context->state = STATE_NEWLINE_SCOPE;
						// value->rvalue = onec_string_initd(de_rvalue.val, de_rvalue.len);
						value.rvalue = &de_rvalue;
						fseek(file_env, -1, SEEK_CUR);
					break;
					case TOKEN_SYMBOL_TABSPACE:
						context->column += 3;
					case TOKEN_SYMBOL_WHITESPACE:
						error_line = context->line;
						error_column = context->column;
						error_no = 5;
						goto end_parse;
					break;
					case TOKEN_SYMBOL_SINGLE_QUOTE:
						context->state = STATE_SINGLE_QUOTE_VALUE_SCOPE;
						value_context->under_single_quote = true;
					break;
					case TOKEN_SYMBOL_DOUBLE_QUOTE:
						context->state = STATE_DOUBLE_QUOTE_VALUE_SCOPE;
						value_context->under_double_quote = true;
					break;
					case TOKEN_SYMBOL_BACKSLASH:
						context->prev_state = STATE_NON_QUOTE_VALUE_SCOPE;
						context->state = STATE_ESCAPE_SCOPE;
					break; 
					default:
						context->state = STATE_NON_QUOTE_VALUE_SCOPE;
						fseek(file_env, -1, SEEK_CUR);
					break;
				}
			break;
			case STATE_NON_QUOTE_VALUE_SCOPE:
				switch( context->input )
				{
					case TOKEN_SYMBOL_EOF:
						context->state = STATE_EOF_SCOPE;
						value.rvalue = &de_rvalue;
						fseek(file_env, -1, SEEK_CUR);
					break;
					case TOKEN_SYMBOL_NEWLINE:
						context->state = STATE_NEWLINE_SCOPE;
						// value->rvalue = onec_string_initd(de_rvalue.val, de_rvalue.len);
						value.rvalue = &de_rvalue;
						fseek(file_env, -1, SEEK_CUR);

						// value_context->under_escape = false;
					break;
					case TOKEN_SYMBOL_TABSPACE:
						context->column += 3;
					case TOKEN_SYMBOL_WHITESPACE:
						context->prev_state = STATE_NON_QUOTE_VALUE_SCOPE;
						context->state = STATE_COMMENT_SCOPE;	
						// value->rvalue = onec_string_initd(de_rvalue.val, de_rvalue.len);
						value.rvalue = &de_rvalue;
					break;
					case TOKEN_SYMBOL_SINGLE_QUOTE:
						context->state = STATE_SINGLE_QUOTE_VALUE_SCOPE;
						value_context->under_single_quote = true;
					break;
					case TOKEN_SYMBOL_DOUBLE_QUOTE:
						context->state = STATE_DOUBLE_QUOTE_VALUE_SCOPE;
						value_context->under_double_quote = true;
					break;
					case TOKEN_SYMBOL_BACKSLASH:
						context->prev_state = STATE_NON_QUOTE_VALUE_SCOPE;
						context->state = STATE_ESCAPE_SCOPE;
					break;
					default:
						// tcast_check every char grouping, checking everytime per char
						// triggering when differential char found for target group (string, integer, float)
						if( type_cast ) {
							tcast_check(&tcast_bool_inc, &tcast_numeric_state, context);
						}

                       	onec_string_putlc(de_rvalue, context->input);
                    break;
				}
			break;
			case STATE_DOUBLE_QUOTE_VALUE_SCOPE:
				switch( context->input )
				{
					case TOKEN_SYMBOL_EOF:
						if( value_context->under_double_quote )
						{
							error_line = context->line;
							error_column = context->column;
							error_no = 6;
							goto end_parse;
						}

						context->state = STATE_EOF_SCOPE;
						// value->rvalue = onec_string_initd(de_rvalue.val, de_rvalue.len);
						value.rvalue = &de_rvalue;
						fseek(file_env, -1, SEEK_CUR);
					break;
					case TOKEN_SYMBOL_BACKSLASH:
						context->prev_state = STATE_DOUBLE_QUOTE_VALUE_SCOPE;
						context->state = STATE_ESCAPE_SCOPE;
					break;
					case TOKEN_SYMBOL_NEWLINE:
						if( value_context->under_double_quote )
						{
							error_line = context->line;
							error_column = context->column;
							error_no = 7;
							goto end_parse;
						}

						context->state = STATE_NEWLINE_SCOPE;
						// value->rvalue = onec_string_initd(de_rvalue.val, de_rvalue.len);
						value.rvalue = &de_rvalue;
						fseek(file_env, -1, SEEK_CUR);
					break;
					case TOKEN_SYMBOL_DOUBLE_QUOTE:
						context->state = STATE_NON_QUOTE_VALUE_SCOPE;
						value_context->under_double_quote = false;
					break;
					case TOKEN_SYMBOL_TABSPACE:
						context->column += 3; 
					default:
						// tcast_check every char grouping, checking everytime per char
						// triggering when differential char found for target group (string, integer, float)
						if( type_cast ) {
							tcast_check(&tcast_bool_inc, &tcast_numeric_state, context);
						}

                       	onec_string_putlc(de_rvalue, context->input);
                    break;
				}
			break;
			case STATE_SINGLE_QUOTE_VALUE_SCOPE:
				switch( context->input )
				{
					case TOKEN_SYMBOL_EOF:
						if( value_context->under_single_quote )
						{
							error_line = context->line;
							error_column = context->column;
							error_no = 8;
							goto end_parse;
						}

						context->state = STATE_EOF_SCOPE;
						// value->rvalue = onec_string_initd(de_rvalue.val, de_rvalue.len);
						value.rvalue = &de_rvalue;
						fseek(file_env, -1, SEEK_CUR);
					break;
					case TOKEN_SYMBOL_NEWLINE:
						if( value_context->under_single_quote )
						{
							error_line = context->line;
							error_column = context->column;
							error_no = 9;
							goto end_parse;
						}

						context->state = STATE_NEWLINE_SCOPE;
						// value->rvalue = onec_string_initd(de_rvalue.val, de_rvalue.len);
						value.rvalue = &de_rvalue;
						fseek(file_env, -1, SEEK_CUR);
					break;
					case TOKEN_SYMBOL_SINGLE_QUOTE:
						context->state = STATE_NON_QUOTE_VALUE_SCOPE;
						value_context->under_single_quote = false;
					break; 
					case TOKEN_SYMBOL_TABSPACE:
						context->column += 3;
					default:
						// tcast_check every char grouping, checking everytime per char
						// triggering when differential char found for target group (string, integer, float)
						if( type_cast ) {
							tcast_check(&tcast_bool_inc, &tcast_numeric_state, context);
						}

                       	onec_string_putlc(de_rvalue, context->input);
                    break;
				}
			break;
			case STATE_NEWLINE_SCOPE:
			case STATE_END_SCOPE:
			case STATE_EOF_SCOPE:
				if( value.lvalue && value.rvalue ) {
					// Fetch/parse key-value pair
					#ifndef PHP_VERSION
	                dotenv_printout("%ld - `%s` = `%s`\n", context->line, onec_string_getlc(value.lvalue), onec_string_get(value.rvalue));
	                #else

	                // Fetch for php here
	                zval array_pack;
	                array_init(&array_pack);

	                zval line;
	                ZVAL_LONG(&line, context->line);
	                add_next_index_zval(&array_pack, &line);

	                /* get rvalue */
	                zval value_pair;

	                if( type_cast )
	                {
		                switch( tcast_numeric_state )
		                {
			                case 0: // long
			                	if( value.rvalue->len ) {
			                		ZVAL_LONG(&value_pair, atoi(value.rvalue->val));
			                	} else {
			                		ZVAL_NULL(&value_pair);
			                	}
		                	break;
				            case 1: // float
				            	ZVAL_DOUBLE(&value_pair, strtod(value.rvalue->val, NULL));
				            break;
					        default: // bool or string
					        	if( tcast_bool_inc > -1 ) 
					        	{
					        		switch( tcast_bool_inc )
						        	{
						        		case TCAST_BOOL_TRUE_LONG_CONSTANT:
						        			ZVAL_TRUE(&value_pair);
						        		break;
						        		case TCAST_BOOL_FALSE_LONG_CONSTANT:
						        			ZVAL_FALSE(&value_pair);
						        		break;	
						        		default:
						        			goto put_as_string;
						        		break;
						        	}

						        	goto clean_tcast;
						        }
					        	
					        	put_as_string: // string
					        	{
					        		zend_string* _value_pair = zend_string_init(value.rvalue->val, value.rvalue->len, 0);
					        		ZVAL_STR(&value_pair, _value_pair);
					        	}
					        break;
		                }

		                clean_tcast:
		                tcast_numeric_state = 0;
		                tcast_bool_inc = 0;
		                /* -- get rvalue -- */
		            }
		            else
		            {
		                ZVAL_STRINGL(&value_pair, value.rvalue->val, value.rvalue->len);
		            }

	                add_next_index_zval(&array_pack, &value_pair);

                	zval env_value;
                	ZVAL_STRINGL(&env_value, value.rvalue->val, value.rvalue->len);
	                add_next_index_zval(&array_pack, &env_value);

	                // Store as [key] = value
					zend_hash_str_update((*target_key_value),
						value.lvalue->val, value.lvalue->len, &array_pack);
	                #endif

				    onec_string_resetlc(de_rvalue);
	                onec_string_resetlc(de_lvalue);
   				}

                switch( context->state )
                {
	                case STATE_NEWLINE_SCOPE:
                    	context->line += 1;
	                break;
	                case STATE_EOF_SCOPE:
             			goto end_parse;
	                break;
                }

				context->state = STATE_INIT_SCOPE;
			break;
			default:
				error_no = 10;
				goto end_parse;
			break;
		}
	}


	end_parse:

	// free(value);
	free(context);
	free(value_context);
	fclose(file_env);

    exit_parse:

    switch( error_no )
    {
        case 0: dotenv_error(E_ERROR, NULL, "[dotenv] [0] failed to open file target: no such file or directory \"%s\"", source_path_src); break;
        case 1: dotenv_error(E_ERROR, NULL, "[dotenv] [1] uncaught_error: unexpected error while parsing"); break;
        case 2: dotenv_error(E_ERROR, NULL, "[dotenv] [2] syntax_error: identifier starts immediately after numeric literal at line %ld, column %ld", error_line, error_column); break;
        case 3 ... 5:
        		dotenv_error(E_ERROR, NULL, "[dotenv] [5] syntax_error: unexpected identifier at line %ld, column %ld", error_line, error_column); break;
        case 6 ... 7: 
        		dotenv_error(E_ERROR, NULL, "[dotenv] [7] syntax_error: unclosed quotation mark after the character string \" at line %ld, column %ld", error_line, error_column); break;
        case 8 ... 9: 
        		dotenv_error(E_ERROR, NULL, "[dotenv] [8] syntax_error: unclosed quotation mark after the character string ' at line %ld, column %ld", error_line, error_column); break;
        case 10: dotenv_error(E_ERROR, NULL, "[dotenv] [10] uncaught_error: unexpected error while parsing"); break;
    }

 	return 0;
}

int dotenv_safe_parse( char* source_path_src, size_t source_path_len, HashTable** target_key_value, HashTable** target_comments, bool save_comment, bool type_cast )
{
	source_path_src[source_path_len] = '\0';

	FILE* file_env = fopen(source_path_src, "r");
	int error_no = -1;
	size_t error_line, error_column;

	// On fopen error call 
	if( !file_env )
	{
		error_no = 0;
		goto exit_parse;
	}

	// Type cast
	int64_t tcast_bool_inc = 0;
	int tcast_numeric_state = 0;

	onec_string* de_comment;
	onec_string_init(de_comment);

    onec_string* de_lvalue;
    onec_string* de_rvalue;
    onec_string_init(de_lvalue);
    onec_string_init(de_rvalue);

	dotenv_context* context = (dotenv_context*)malloc(sizeof(dotenv_context));
	context->state = STATE_INIT_SCOPE;
	context->under_comment = false;
	context->line = 1;

	dotenv_value* value = (dotenv_value*)malloc(sizeof(dotenv_value));
	dotenv_value_context* value_context = (dotenv_value_context*)malloc(sizeof(dotenv_value_context));
	int increments = 0;

	while( (context->input = fgetc(file_env)) )
	{
		if( context->state == STATE_INIT_SCOPE )
		{
			context->prev_state	= STATE_BEGIN_INDENTIFIER_SCOPE;
			context->state = STATE_COMMENT_SCOPE;

			context->column = 0;
			context->under_comment = false;

            value->rvalue = NULL;
            value->lvalue = NULL;

			value_context->under_single_quote = false;
			value_context->under_double_quote = false;
		}

		context->column += 1;

		switch( context->state )
		{
			case STATE_COMMENT_SCOPE:
				switch( context->input )
				{
					case TOKEN_SYMBOL_EOF:
						context->state = STATE_EOF_SCOPE;
					break;
					case TOKEN_SYMBOL_NEWLINE:
						if( context->under_comment )
						{
							// Parse/fetch comments per line
							#ifndef PHP_VERSION
							dotenv_printout("# [%ld] `%s`\n", context->line, onec_string_get(de_comment));
							#else
							// Fetch over here for php
							zend_string* _comment = zend_string_init(de_comment->val, de_comment->len, 0);
							zval comment; ZVAL_STR(&comment, _comment);
							// zval comment;
							// ZVAL_STRINGL(&comment, de_comment->val, de_comment->len);
							zend_hash_index_update((*target_comments), context->line, &comment);
							#endif
							onec_string_reset(de_comment);	
						}

						context->state = STATE_NEWLINE_SCOPE;
						fseek(file_env, -1, SEEK_CUR);
					break;
					case TOKEN_SYMBOL_TABSPACE:
					case TOKEN_SYMBOL_WHITESPACE:
						if(! context->under_comment ) {
							break;
						}
					default:
						if( context->under_comment ) {
							onec_string_put(de_comment, context->input);
							break;
						}

						switch( context->prev_state )
						{
							case STATE_BEGIN_INDENTIFIER_SCOPE:
								increments = -1;
								context->state = STATE_BEGIN_INDENTIFIER_SCOPE;
							break;
							case STATE_NON_QUOTE_VALUE_SCOPE:
								increments = -2;
								context->state = STATE_END_SCOPE;
							break;
							default:
								error_no = 1;
								goto end_parse;
							break;
						}

						context->column += increments;
						fseek(file_env, increments, SEEK_CUR);
					break;
					case TOKEN_SYMBOL_HASHMARK:
						if( save_comment )
							context->under_comment = true;
						else
							context->state = STATE_SKIP_COMMENT_SCOPE;
					break;
				}
			break;
			case STATE_SKIP_COMMENT_SCOPE:
				switch( context->input )
				{
					case TOKEN_SYMBOL_NEWLINE:
						context->state = STATE_NEWLINE_SCOPE;
						fseek(file_env, -1, SEEK_CUR);
					break; 
					case TOKEN_SYMBOL_EOF:
						context->state = STATE_EOF_SCOPE;
					break; 
				}
			break;
			case STATE_ESCAPE_SCOPE:
				context->state = context->prev_state;
	           	onec_string_put(de_rvalue, context->input);
			break;
			case STATE_BEGIN_INDENTIFIER_SCOPE:
				switch( context->input )
				{
					case TOKEN_SYMBOL_EOF:
						context->column -= 1;
						context->state = STATE_EOF_SCOPE;
						fseek(file_env, -1, SEEK_CUR);
					break;
					case TOKEN_SYMBOL_BREAKLINE:
						context->column -= 1;
						context->line 	+= 1;
					break;
					case TOKEN_SYMBOL_TABSPACE:
						context->column += 3;
					case TOKEN_SYMBOL_WHITESPACE:
					break;
					case TOKEN_DIGIT_START ... TOKEN_DIGIT_END:
						error_line = context->line;
						error_column = context->column;
						error_no = 2;
						goto end_parse;
					break;	
					case TOKEN_SYMBOL_UNDERSCORE:
					case TOKEN_ALPHA_LOWER_START ... TOKEN_ALPHA_LOWER_END:
					case TOKEN_ALPHA_UPPER_START ... TOKEN_ALPHA_UPPER_END:
                    	onec_string_put(de_lvalue, context->input);
						context->state = STATE_INDENTIFIER_SCOPE;
					break;
					default:
						error_line = context->line;
						error_column = context->column;
						error_no = 3;
						goto end_parse;
					break;
				}
			break;
			case STATE_INDENTIFIER_SCOPE:
				switch( context->input )
				{
					case OPERATOR_SYMBOL_EQUAL:
						context->column -= 1;
						context->state = STATE_OPERATOR_SCOPE;
						fseek(file_env, -1, SEEK_CUR);
					break;
					case TOKEN_DIGIT_START ... TOKEN_DIGIT_END:
					case TOKEN_ALPHA_LOWER_START ... TOKEN_ALPHA_LOWER_END:
					case TOKEN_ALPHA_UPPER_START ... TOKEN_ALPHA_UPPER_END:
					case TOKEN_SYMBOL_UNDERSCORE:
	                    onec_string_put(de_lvalue, context->input);
					break;
					default:
						error_line = context->line;
						error_column = context->column;
						error_no = 4;
						goto end_parse;
					break;
				}	
			break;
			case STATE_OPERATOR_SCOPE:
				context->state = STATE_FIND_VALUE_SCOPE;
                value->lvalue = de_lvalue;
			break;
			case STATE_FIND_VALUE_SCOPE:
				switch( context->input )
				{	
					case TOKEN_SYMBOL_EOF:
						context->state = STATE_EOF_SCOPE;
						value->rvalue = de_rvalue;
						fseek(file_env, -1, SEEK_CUR);
					break;
					case TOKEN_SYMBOL_NEWLINE:
						context->state = STATE_NEWLINE_SCOPE;
						value->rvalue = de_rvalue;
						fseek(file_env, -1, SEEK_CUR);
					break;
					case TOKEN_SYMBOL_TABSPACE:
						context->column += 3;
					case TOKEN_SYMBOL_WHITESPACE:
						error_line = context->line;
						error_column = context->column;
						error_no = 5;
						goto end_parse;
					break;
					case TOKEN_SYMBOL_SINGLE_QUOTE:
						context->state = STATE_SINGLE_QUOTE_VALUE_SCOPE;
						value_context->under_single_quote = true;
					break;
					case TOKEN_SYMBOL_DOUBLE_QUOTE:
						context->state = STATE_DOUBLE_QUOTE_VALUE_SCOPE;
						value_context->under_double_quote = true;
					break;
					case TOKEN_SYMBOL_BACKSLASH:
						context->prev_state = STATE_NON_QUOTE_VALUE_SCOPE;
						context->state = STATE_ESCAPE_SCOPE;
					break; 
					default:
						context->state = STATE_NON_QUOTE_VALUE_SCOPE;
						fseek(file_env, -1, SEEK_CUR);
					break;
				}
			break;
			case STATE_NON_QUOTE_VALUE_SCOPE:
				switch( context->input )
				{
					case TOKEN_SYMBOL_EOF:
						context->state = STATE_EOF_SCOPE;
						value->rvalue = de_rvalue;
						fseek(file_env, -1, SEEK_CUR);
					break;
					case TOKEN_SYMBOL_NEWLINE:
						context->state = STATE_NEWLINE_SCOPE;
						value->rvalue = de_rvalue;
						fseek(file_env, -1, SEEK_CUR);
						// value_context->under_escape = false;
					break;
					case TOKEN_SYMBOL_TABSPACE:
						context->column += 3;
					case TOKEN_SYMBOL_WHITESPACE:
						context->prev_state = STATE_NON_QUOTE_VALUE_SCOPE;
						context->state = STATE_COMMENT_SCOPE;	
						value->rvalue = de_rvalue;
					break;
					case TOKEN_SYMBOL_SINGLE_QUOTE:
						context->state = STATE_SINGLE_QUOTE_VALUE_SCOPE;
						value_context->under_single_quote = true;
					break;
					case TOKEN_SYMBOL_DOUBLE_QUOTE:
						context->state = STATE_DOUBLE_QUOTE_VALUE_SCOPE;
						value_context->under_double_quote = true;
					break;
					case TOKEN_SYMBOL_BACKSLASH:
						context->prev_state = STATE_NON_QUOTE_VALUE_SCOPE;
						context->state = STATE_ESCAPE_SCOPE;
					break;
					default:
						// tcast_check every char grouping, checking everytime per char
						// triggering when differential char found for target group (string, integer, float)
						if( type_cast ) {
							tcast_check(&tcast_bool_inc, &tcast_numeric_state, context);
						}

                       	onec_string_put(de_rvalue, context->input);
                    break;
				}
			break;
			case STATE_DOUBLE_QUOTE_VALUE_SCOPE:
				switch( context->input )
				{
					case TOKEN_SYMBOL_EOF:
						if( value_context->under_double_quote )
						{
							error_line = context->line;
							error_column = context->column;
							error_no = 6;
							goto end_parse;
						}

						context->state = STATE_EOF_SCOPE;
						value->rvalue = de_rvalue;
						fseek(file_env, -1, SEEK_CUR);
					break;
					case TOKEN_SYMBOL_BACKSLASH:
						context->prev_state = STATE_DOUBLE_QUOTE_VALUE_SCOPE;
						context->state = STATE_ESCAPE_SCOPE;
					break;
					case TOKEN_SYMBOL_NEWLINE:
						if( value_context->under_double_quote )
						{
							error_line = context->line;
							error_column = context->column;
							error_no = 7;
							goto end_parse;
						}

						context->state = STATE_NEWLINE_SCOPE;
						value->rvalue = de_rvalue;
						fseek(file_env, -1, SEEK_CUR);
					break;
					case TOKEN_SYMBOL_DOUBLE_QUOTE:
						context->state = STATE_NON_QUOTE_VALUE_SCOPE;
						value_context->under_double_quote = false;
					break;
					case TOKEN_SYMBOL_TABSPACE:
						context->column += 3; 
					default:
						if( type_cast )
							tcast_check(&tcast_bool_inc, &tcast_numeric_state, context);

                       	onec_string_put(de_rvalue, context->input);
                    break;
				}
			break;
			case STATE_SINGLE_QUOTE_VALUE_SCOPE:
				switch( context->input )
				{
					case TOKEN_SYMBOL_EOF:
						if( value_context->under_single_quote )
						{
							error_line = context->line;
							error_column = context->column;
							error_no = 8;
							goto end_parse;
						}

						context->state = STATE_EOF_SCOPE;
						value->rvalue = de_rvalue;
						fseek(file_env, -1, SEEK_CUR);
					break;
					case TOKEN_SYMBOL_NEWLINE:
						if( value_context->under_single_quote )
						{
							error_line = context->line;
							error_column = context->column;
							error_no = 9;
							goto end_parse;
						}

						context->state = STATE_NEWLINE_SCOPE;
						value->rvalue = de_rvalue;
						fseek(file_env, -1, SEEK_CUR);
					break;
					case TOKEN_SYMBOL_SINGLE_QUOTE:
						context->state = STATE_NON_QUOTE_VALUE_SCOPE;
						value_context->under_single_quote = false;
					break; 
					case TOKEN_SYMBOL_TABSPACE:
						context->column += 3;
					default:
						if( type_cast )
							tcast_check(&tcast_bool_inc, &tcast_numeric_state, context);

                       	onec_string_put(de_rvalue, context->input);
                    break;
				}
			break;
			case STATE_NEWLINE_SCOPE:
			case STATE_END_SCOPE:
			case STATE_EOF_SCOPE:
				if( value->lvalue && value->rvalue ) {
					// Fetch/parse key-value pair
					#ifndef PHP_VERSION
	                dotenv_printout("%ld - `%s` = `%s`\n", context->line, onec_string_get(value->lvalue), onec_string_get(value->rvalue));
	                #else

	                // Fetch for php here
	                zval array_pack;
	                array_init(&array_pack);

	                zval line;
	                ZVAL_LONG(&line, context->line);
	                add_next_index_zval(&array_pack, &line);

	                /* get rvalue */
	                zval value_pair;
	                if( type_cast )
	                {
		                switch( tcast_numeric_state )
		                {
			                case 0: // long
			                	if( value->rvalue->len ) {
			                		ZVAL_LONG(&value_pair, atoi(value->rvalue->val));
			                	} else {
			                		ZVAL_NULL(&value_pair);
			                	}
		                	break;
				            case 1: // float
				            	ZVAL_DOUBLE(&value_pair, strtod(value->rvalue->val, NULL));
				            break;
					        default: // bool or string
					        	if( tcast_bool_inc > -1 ) 
					        	{
					        		switch( tcast_bool_inc )
						        	{
						        		case TCAST_BOOL_TRUE_LONG_CONSTANT:
						        			ZVAL_TRUE(&value_pair);
						        		break;
						        		case TCAST_BOOL_FALSE_LONG_CONSTANT:
						        			ZVAL_FALSE(&value_pair);
						        		break;	
						        		default:
						        			goto put_as_string;
						        		break;
						        	}

						        	goto clean_tcast;
						        }
					        	
					        	put_as_string: // string
					        	{
					        		zend_string* _value_pair = zend_string_init(value->rvalue->val, value->rvalue->len, 0);
					        		ZVAL_STR(&value_pair, _value_pair);
					        	}
					        break;
		                }

		                clean_tcast:
		                tcast_numeric_state = 0;
		                tcast_bool_inc = 0;
		                /* -- get rvalue -- */
		            }
		            else
		            {
		                ZVAL_STRINGL(&value_pair, value->rvalue->val, value->rvalue->len);
		            }

	                add_next_index_zval(&array_pack, &value_pair);

                	zval env_value;
                	ZVAL_STRINGL(&env_value, value->rvalue->val, value->rvalue->len);
	                add_next_index_zval(&array_pack, &env_value);

	                // Store as [key] = value
					zend_hash_str_update((*target_key_value),
						value->lvalue->val, value->lvalue->len, &array_pack);
	                #endif

				    onec_string_reset(de_rvalue);
	                onec_string_reset(de_lvalue);
   				}

                switch( context->state )
                {
	                case STATE_NEWLINE_SCOPE:
                    	context->line += 1;
	                break;
	                case STATE_EOF_SCOPE:
             			goto end_parse;
	                break;
                }

				context->state = STATE_INIT_SCOPE;
			break;
			default:
				error_no = 10;
				goto end_parse;
			break;
		}
	}


	end_parse:

	free(value);
	free(context);
	free(value_context);

	onec_string_release(de_comment);
    onec_string_release(de_lvalue);
    onec_string_release(de_rvalue);

	fclose(file_env);

    exit_parse:

    switch( error_no )
    {
        case 0: dotenv_error(E_ERROR, NULL, "[dotenv] [0] failed to open file target: no such file or directory \"%s\"", source_path_src); break;
        case 1: dotenv_error(E_ERROR, NULL, "[dotenv] [1] uncaught_error: unexpected error while parsing"); break;
        case 2: dotenv_error(E_ERROR, NULL, "[dotenv] [2] syntax_error: identifier starts immediately after numeric literal at line %ld, column %ld", error_line, error_column); break;
        case 3 ... 5:
        		dotenv_error(E_ERROR, NULL, "[dotenv] [5] syntax_error: unexpected identifier at line %ld, column %ld", error_line, error_column); break;
        case 6 ... 7: 
        		dotenv_error(E_ERROR, NULL, "[dotenv] [7] syntax_error: unclosed quotation mark after the character string \" at line %ld, column %ld", error_line, error_column); break;
        case 8 ... 9: 
        		dotenv_error(E_ERROR, NULL, "[dotenv] [8] syntax_error: unclosed quotation mark after the character string ' at line %ld, column %ld", error_line, error_column); break;
        case 10: dotenv_error(E_ERROR, NULL, "[dotenv] [10] uncaught_error: unexpected error while parsing"); break;
    }

 	return 0;
}

onec_string* dotenv_cache_save( zval* key_value )
{
	onec_string* env_serialized;
	onec_string_init(env_serialized);

    onec_string_append(env_serialized, 1, "<?php\n\nreturn\n[\n");
	ZEND_HASH_FOREACH_STR_KEY_VAL(Z_ARR_P(key_value), zend_string* key, zval* value_info)
    {
    	zval* line = zend_hash_index_find(Z_ARR_P(value_info), 0);
    	zval* typevalue = zend_hash_index_find(Z_ARR_P(value_info), 1);
    	zval* value = zend_hash_index_find(Z_ARR_P(value_info), 2);

    	size_t line_len = snprintf(NULL, 0, "%lu", zval_get_long(line));
    	char line_src[line_len + 1];
    	php_sprintf(line_src, "%lu", zval_get_long(line));

        onec_string_append(env_serialized, 3, "    '", key->val, "' =>\n    [\n        ");

        onec_string_append(env_serialized, 3, "0 => ", line_src, ",\n        1 => ");
        switch( Z_TYPE_P(typevalue ) )
        {
        	case IS_STRING:
        		onec_string_append(env_serialized, 3, "'", Z_STRVAL_P(value), "'");
        	break;
        	case IS_LONG:
        	case IS_DOUBLE:
        	case IS_TRUE:
        	case IS_FALSE:
        		onec_string_append(env_serialized, 1, Z_STRVAL_P(value));
        	break;
	        case IS_NULL:
	        	onec_string_append(env_serialized, 1, "null");
	        break;
 			default:
        		onec_string_append(env_serialized, 1, "':uknown'");
 			break; 
        }
        onec_string_append(env_serialized, 3, ",\n        2 => '", Z_STRVAL_P(value), "',\n    ],\n");
    }
    ZEND_HASH_FOREACH_END();
    onec_string_append(env_serialized, 1, "];");

    return env_serialized;
}