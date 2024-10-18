#ifndef ONEC_STRING_INITIALIZE
#define ONEC_STRING_INITIALIZE

#include <stdlib.h>

typedef struct
{
	char*     val;
	size_t    size;
	size_t    len;
} onec_string;

#define onec_string_init( onec_str ) _onec_string_init(&onec_str)
void _onec_string_init( onec_string** onec_str );

#define onec_string_put( onec_str, ch_val ) _onec_string_put(&onec_str, ch_val)
void _onec_string_put( onec_string** onec_str, char ch_val );

#define onec_string_append( onec_str, size, ... ) _onec_string_append(&onec_str, size, __VA_ARGS__)
void _onec_string_append( onec_string** onec_str, size_t size, ... );

#define onec_string_insert( onec_str, string ) _onec_string_insert(&onec_str, string)
void _onec_string_insert( onec_string** onec_str, char* string );

#define onec_string_get( onec_str ) _onec_string_get(&onec_str)
char* _onec_string_get( onec_string** onec_str );

#define onec_string_reset( onec_str ) _onec_string_reset(&onec_str)
void _onec_string_reset( onec_string** onec_str );

#define onec_string_release( onec_str ) _onec_string_release(&onec_str)
void _onec_string_release( onec_string** onec_str );

#endif