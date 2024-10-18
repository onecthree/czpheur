
#include <php.h>
#include <ext/standard/info.h>
#include <php_zpheur.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include "onecstr.h"

#define onec_string_init( onec_str ) _onec_string_init(&onec_str)
void _onec_string_init( onec_string** onec_str )
{
	*onec_str = (onec_string*)malloc(sizeof(onec_string));
	(*onec_str)->val = (char*)malloc(4 * sizeof(char));
	(*onec_str)->size = 4;
	(*onec_str)->len = 0;
}

#define onec_string_put( onec_str, ch_val ) _onec_string_put(&onec_str, ch_val)
void _onec_string_put( onec_string** onec_str, char ch_val )
{
	if( (*onec_str)->len >= (*onec_str)->size )
	{
		(*onec_str)->size *= 2;
		char* _onec_str_val = (char*)realloc((*onec_str)->val, (*onec_str)->size);
		if(! _onec_str_val )
		{
			free(_onec_str_val);
			printf("unexpected_error: error while realloc some variable");
			exit(0);
		}

		(*onec_str)->val = _onec_str_val;
	}

	(*onec_str)->val[(*onec_str)->len] = ch_val;
	(*onec_str)->len += 1;
}

#define onec_string_append( onec_str, size, ... ) _onec_string_append(&onec_str, size, __VA_ARGS__)
void _onec_string_append( onec_string** onec_str, size_t size, ... )
{
	va_list args;
	va_start(args, size);
	for( size_t i = 0; i < size; i += 1 )
	{
		char* const string = va_arg(args, char* const);
		char c;
		size_t index = 0;
		
		while( (c = string[index]) != '\0' )
		{
			_onec_string_put(onec_str, c);
			index += 1;
		}
	}
	va_end(args);
}

#define onec_string_insert( onec_str, string ) _onec_string_insert(&onec_str, string)
void _onec_string_insert( onec_string** onec_str, char* string )
{
	char c;
	size_t index = 0;
	
	while( (c = string[index]) != '\0' )
	{
		_onec_string_put(onec_str, c);
		index += 1;
	}
}

#define onec_string_get( onec_str ) _onec_string_get(&onec_str)
char* _onec_string_get( onec_string** onec_str )
{
	char *_onec_str = (char*)malloc((1 + (*onec_str)->len) * sizeof(char));
	strncpy(_onec_str, (*onec_str)->val, (*onec_str)->len);
	_onec_str[(*onec_str)->len] = '\0';

	return _onec_str;
}

#define onec_string_reset( onec_str ) _onec_string_reset(&onec_str)
void _onec_string_reset( onec_string** onec_str )
{
	memset((*onec_str)->val, 4, sizeof(char));
	(*onec_str)->size = 4;
	(*onec_str)->len = 0;
}

#define onec_string_release( onec_str ) _onec_string_release(&onec_str)
void _onec_string_release( onec_string** onec_str )
{	
	if( (*onec_str)->val )
		free((*onec_str)->val);
	
	if( (*onec_str) )
		free(*onec_str);
}
