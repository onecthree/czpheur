
#ifndef H_INCLUDE_DOTENV
#define H_INCLUDE_DOTENV

#include <php.h>
#include <ext/standard/info.h>
#include <php_zpheur.h>

#include <php.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <include/onecstr.h>

#ifndef PHP_VERSION
typedef struct
{
    int todo;
} HashTable;
#endif

#ifdef PHP_VERSION
#define dotenv_error( err_type, todo, ... ) zend_error(err_type, __VA_ARGS__)
#else
#define dotenv_error( err_type, todo, ... ) printf(__VA_ARGS__); printf("\n")
#endif

#ifndef E_ERROR
#define E_ERROR 1
#endif

#define DOTENV_DISABLE_STDOUT 1
// #define DOTENV_ENABLE_STDOUT 2

#ifndef DOTENV_ENABLE_STDOUT
#ifdef DOTENV_DISABLE_STDOUT
#define dotenv_printout(...) 
#endif
#endif

#ifndef DOTENV_DISABLE_STDOUT
#ifdef DOTENV_ENABLE_STDOUT

#ifndef PHP_VERSION
#define dotenv_printout( ... ) printf(__VA_ARGS__)
#else
#define dotenv_printout(...) 
#endif

#endif
#endif

// Define parser needle's
#define     TOKEN_SYMBOL_WHITESPACE		' '
#define     TOKEN_SYMBOL_TABSPACE		'\t'
#define     TOKEN_SYMBOL_BREAKLINE		'\n'
#define		TOKEN_ALPHA_LOWER_START		'a'
#define     TOKEN_ALPHA_LOWER_END		'z'
#define		TOKEN_ALPHA_UPPER_START		'A'
#define     TOKEN_ALPHA_UPPER_END		'Z'
#define     TOKEN_DIGIT_START			'0'
#define     TOKEN_DIGIT_END				'9'
#define     TOKEN_SYMBOL_UNDERSCORE		'_'
#define     TOKEN_SYMBOL_NULL			'\0'

#define     TOKEN_SYMBOL_HASHMARK		'#'
#define     TOKEN_SYMBOL_NEWLINE		'\n'

#define     TOKEN_SYMBOL_SINGLE_QUOTE	'\''
#define     TOKEN_SYMBOL_DOUBLE_QUOTE	'"'
#define     TOKEN_SYMBOL_BACKSLASH		'\\'
#define     TOKEN_SYMBOL_EOF			EOF

#define     OPERATOR_SYMBOL_EQUAL		'='

#define     STATE_INIT_SCOPE             		0
#define     STATE_BEGIN_INDENTIFIER_SCOPE		1
#define     STATE_INDENTIFIER_SCOPE             2
#define     STATE_OPERATOR_SCOPE                3
#define     STATE_FIND_VALUE_SCOPE				4
#define     STATE_NON_QUOTE_VALUE_SCOPE	        5
#define     STATE_SINGLE_QUOTE_VALUE_SCOPE		6
#define     STATE_DOUBLE_QUOTE_VALUE_SCOPE		7
#define     STATE_COMMENT_SCOPE                 8
#define     STATE_SKIP_COMMENT_SCOPE            9
#define     STATE_ESCAPE_SCOPE                  10
#define     STATE_NEWLINE_SCOPE					11	
#define     STATE_END_SCOPE						12
#define     STATE_EOF_SCOPE						13

#define     QUOTE_NON_TYPE                   	0
#define     QUOTE_SINGLE_TYPE                   1
#define     QUOTE_DOUBLE_TYPE                   2

#define     ESCAPE_NON_TYPE                   	0
#define     ESCAPE_SINGLE_TYPE					1
#define     ESCAPE_DOUBLE_TYPE					2

#define     TCAST_BOOL_TRUE_LONG_CONSTANT		116114117101
#define     TCAST_BOOL_FALSE_LONG_CONSTANT		10297108115101
#define     TCAST_BOOL_TRUE_STATE				0
#define     TCAST_BOOL_FALSE_STATE				1
#define     TCAST_LONG_STATE                    2
#define     TCAST_FLOAT_STATE                   3
#define     TCAST_STRING_STATE                  4


typedef struct
{
	size_t      column;
	size_t      line;
	char        input;
	uint8_t     state;
	uint8_t     prev_state;
	bool        under_comment;
} dotenv_context;

typedef struct
{
	onec_string*	lvalue;
	onec_string*	rvalue;
} dotenv_value;

typedef struct
{
	bool      under_single_quote;
	bool      under_double_quote;
} dotenv_value_context;

inline static void tcast_check(
	int64_t* 		tcast_bool_inc,
	int* 			tcast_numeric_state, 
	dotenv_context* context 
);

int dotenv_unsafe_parse(
	char* const 	filename_env,
	HashTable** 	target_key_value,
	HashTable** 	target_comments,
	bool 			save_comment,
	bool 			type_cast
);

int dotenv_safe_parse(
	char* const 	filename_env,
	HashTable** 	target_key_value,
	HashTable** 	target_comments,
	bool 			save_comment,
	bool 			type_cast
);

onec_string* dotenv_cache_save( zval* key_value );

#endif