
#ifndef ONEC_STRING_INITIALIZE
#define ONEC_STRING_INITIALIZE

#include <stdlib.h>

#define ONEC_STRING_INIT_LENGTH 1024

typedef struct _onec_string
{
	char*     val;
	size_t    size;
	size_t    len;
} onec_string;

typedef struct _onec_stringlc
{
	char      val[ONEC_STRING_INIT_LENGTH];
	size_t    len;
} onec_stringlc;

#define onec_string_init( onec_str ) _onec_string_init(&onec_str)
void _onec_string_init( onec_string** onec_str );

#define onec_string_put( onec_str, ch_val ) _onec_string_put(&onec_str, ch_val)
void _onec_string_put( onec_string** onec_str, char ch_val );

#define onec_string_append( onec_str, size, ... ) _onec_string_append(&onec_str, size, __VA_ARGS__)
void _onec_string_append( onec_string** onec_str, size_t size, ... );

#define onec_string_insert( onec_str, string ) _onec_string_insert(&onec_str, string)
void _onec_string_insert( onec_string** onec_str, char* const string );

#define onec_string_get( onec_str ) _onec_string_get(&onec_str)
char* _onec_string_get( onec_string** onec_str );

#define onec_string_reset( onec_str ) _onec_string_reset(&onec_str)
void _onec_string_reset( onec_string** onec_str );

#define onec_string_release( onec_str ) _onec_string_release(&onec_str)
void _onec_string_release( onec_string** onec_str );

#define onec_string_initlc( onec_str ) _onec_string_initlc(&onec_str)
void _onec_string_initlc( onec_stringlc* onec_str );

#define onec_string_putlc( onec_str, ch_val ) _onec_string_putlc(&onec_str, ch_val)
void _onec_string_putlc( onec_stringlc* onec_str, char ch_val );

#define onec_string_appendlc( onec_str, size, ... ) _onec_string_appendlc(&onec_str, size, __VA_ARGS__)
void _onec_string_appendlc( onec_stringlc* onec_str, size_t size, ... );

#define onec_string_getlc( onec_str ) _onec_string_getlc(&onec_str)
char* _onec_string_getlc( onec_stringlc* onec_str );

#define onec_string_trimlc( onec_str ) _onec_string_trimlc(&onec_str)
void _onec_string_trimlc( onec_stringlc* onec_str );

#define onec_string_insertlc( onec_str, string ) _onec_string_insertlc(&onec_str, string)
void _onec_string_insertlc( onec_stringlc* onec_str, char* const string );

#define onec_string_resetlc( onec_str ) _onec_string_resetlc(&onec_str)
void _onec_string_resetlc( onec_stringlc* onec_str );

#endif