
#ifndef H_INCLUDE_FUROUTER
#define H_INCLUDE_FUROUTER

#include <php.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <include/onecstr.h>

/**
 * Dummy struct for testing out outside PHP runtime/env
 */
#ifndef PHP_VERSION
typedef struct
{
    int todo;
} HashTable;
#endif

#ifdef PHP_VERSION
#define furouter_error( err_type, todo, ... ) zend_error(err_type, __VA_ARGS__)
#else
#define furouter_error( err_type, todo, ... ) printf(__VA_ARGS__); printf("\n")
#endif

#ifndef E_ERROR
#define E_ERROR 1
#endif
 
#define FUROUTER_DISABLE_STDOUT 1
// #define FUROUTER_ENABLE_STDOUT 2

#ifndef FUROUTER_ENABLE_STDOUT
#ifdef FUROUTER_DISABLE_STDOUT
#define furouter_printout(...) 
#endif
#endif

#ifndef FUROUTER_DISABLE_STDOUT
#ifdef FUROUTER_ENABLE_STDOUT

#ifdef PHP_VERSION
#define furouter_printout( ... ) php_printf(__VA_ARGS__)
#else
#define furouter_printout( ... ) printf(__VA_ARGS__)
#endif

#endif
#endif

#define TARGET_URI_MAX_LENGTH_AS_REV            32
#define TARGET_URI_MAX_LENGTH_AS_INDEX          TARGET_URI_MAX_LENGTH_AS_REV - 1

#define TOKEN_ALPHA_LOWER_START                 'a'
#define TOKEN_ALPHA_LOWER_END                   'z'
#define TOKEN_ALPHA_UPPER_START                 'A'
#define TOKEN_ALPHA_UPPER_END                   'Z'
#define TOKEN_DIGIT_START                       '0'
#define TOKEN_DIGIT_END                         '9'
#define TOKEN_SYMBOL_QUERY                      '?'
#define TOKEN_SYMBOL_SLASH                      '/'
#define TOKEN_SYMBOL_COLON                      ':'
#define TOKEN_SYMBOL_UNDERSCORE                 '_'
#define TOKEN_SYMBOL_OPEN_ROUND_CURLY           '('
#define TOKEN_SYMBOL_CLOSE_ROUND_CURLY          ')'
#define TOKEN_SYMBOL_UNDERSCORE                 '_'
#define TOKEN_CHAR_NULL                         '\0'
#define TOKEN_SYMBOL_HASHMARK                   '#'
#define TOKEN_SYMBOL_NEWLINE                    '\n'
#define TOKEN_SYMBOL_WHITESPACE                 ' '

#define STATE_TARGET_INIT_SCOPE                 1
#define STATE_TARGET_BEGIN_TYPE_PATH_SCOPE      2
#define STATE_TARGET_FIND_TYPE_PATH_SCOPE       3
#define STATE_TARGET_ALPHA_TYPE_PATH_SCOPE      4
#define STATE_TARGET_NUM_TYPE_PATH_SCOPE        5
#define STATE_TARGET_RANDOM_TYPE_PATH_SCOPE     6
#define STATE_TARGET_SKIP_PATH_SCOPE            7
#define STATE_TARGET_ENDED_PATH_SCOPE           8
#define STATE_PLACEHOLDER_BEGIN_TYPE_PATH_SCOPE 9
#define STATE_PLACEHOLDER_NAME_TYPE_PATH_SCOPE  10
#define STATE_PLACEHOLDER_INNER_TYPE_PATH_SCOPE 11
#define STATE_PLACEHOLDER_END_TYPE_PATH_SCOPE   12

#define STATE_FINDER_INIT_SCOPE                 12
#define STATE_FINDER_PATH_SKIP_SCOPE            13
#define STATE_FINDER_HASH_SKIP_SCOPE            14
#define STATE_FINDER_END_SKIP_SCOPE             15
#define STATE_FINDER_TYPE_SCOPE                 16
#define STATE_FINDER_PATH_LEN_SCOPE             17
#define STATE_FINDER_WORD_CMP_SCOPE             18
#define STATE_FINDER_PLACEHOLDER_SCOPE          19
#define STATE_FINDER_CLASS_ACT_SCOPE            20
#define STATE_FINDER_METHOD_ACT_SCOPE           21
#define STATE_FINDER_END_PARSE_SCOPE            22

#define PATH_TYPE_SKIP                          1 << 0
#define PATH_TYPE_ALPHA                         1 << 1
#define PATH_TYPE_NUM                           1 << 2
#define PATH_TYPE_ALNUM                         1 << 1 | 1 << 2
#define PATH_TYPE_RANDOM                        1 << 3

#define PATH_IMPL_ASTERIK                       '*'
#define PATH_IMPL_ALPHA                         '@'
#define PATH_IMPL_NUM                           '%'
#define PATH_IMPL_ALNUM                         '&'
#define PATH_IMPL_RANDOM                        '$'

#define PLACEHOLDER_TYPE_RANDOM                 11497110100111109
#define PLACEHOLDER_TYPE_ALPHA                  9710811210497
#define PLACEHOLDER_TYPE_ALNUM                  97108110117109
#define PLACEHOLDER_TYPE_NUM                    110117109
#define PLACEHOLDER_TYPE_ANY                    97110121

#define PLACEHOLDER_IMPL_RANDOM                 '-'
#define PLACEHOLDER_IMPL_ALPHA                  '^'
#define PLACEHOLDER_IMPL_ALNUM                  '+'
#define PLACEHOLDER_IMPL_NUM                    '!'
#define PLACEHOLDER_IMPL_ANY                    '.'

typedef union _furouter_target_type_path
{
  unsigned int state;
  unsigned char type_of[3];
} furouter_target_type_path;

typedef struct _furouter_target_context
{
    char        input;
    size_t      index;
    size_t      path_index;
    bool        path_skip;
    int         state;
} furouter_target_context;

typedef struct _furouter_finder_context
{
    char        input;
    int         state;
    size_t      index;
    size_t      front_path_len;
    size_t      local_path_len;
    size_t      path_index;
    bool        path_valid;
    bool        route_valid;
    long        route_count;
} furouter_finder_context;

typedef struct _furouter_target_uri
{
    char*      val;
    size_t     len;
    char       type;
} furouter_target_uri;

typedef struct _furouter_fund
{
    zend_string*    class_name;
    zend_string*    method_name;
    long            order;
} furouter_fund;

int static_furouter_target_uri_parse( char* url, void* ptr_local_uri_target_src, size_t* dest_uri_target_len );
onec_string* static_furouter_route_uri_parse( char* route, char* class_name, char* method_name );
// int static_furouter_finder( zend_string* route_current, furouter_fund** route_fund, void* target_uri_src, size_t target_uri_len, HashTable** placeholder );
int static_furouter_finder( zend_string* route_current, void* target_uri_src, size_t target_uri_len, HashTable** placeholder );
int cliteral_static_furouter_finder( char* route_current_src, size_t route_current_len, void* target_uri_src, size_t target_uri_len, HashTable** placeholder );

#define fixed_merge_str( str_ptr, ... ) \
    size_t _temporare_merge_str_len = snprintf(NULL, 0, __VA_ARGS__); \
    char* __temporare_merge_str = realloc(str_ptr, _temporare_merge_str_len + 1); \
    if(! __temporare_merge_str ) \
    { \
        free(__temporare_merge_str); \
        furouter_printout("unexpected error"); \
        goto end_parse; \
    } \
    str_ptr = __temporare_merge_str; \
    sprintf(str_ptr, __VA_ARGS__); \
    str_ptr[_temporare_merge_str_len] = '\0'; \

#endif