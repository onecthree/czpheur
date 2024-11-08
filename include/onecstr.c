
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include "onecstr.h"


onec_string* __attribute__ ((noinline))
_onec_string_initd( char* const string, size_t length )
{
	onec_string* onec_str = (onec_string*)malloc(sizeof(onec_string));
	onec_str->val = (char*)malloc(sizeof(char) * length);
	onec_str->size = length * 2;
	onec_str->len = length;

	for( int i = 0; i < length; i += 1 )
		onec_str->val[i] = string[i];

	onec_str->val[length] = '\0';

	return onec_str;
}

void inline __attribute__ ((always_inline))
_onec_string_init( onec_string** onec_str )
{
	*onec_str = (onec_string*)malloc(sizeof(onec_string));
	(*onec_str)->val = (char*)malloc(4 * sizeof(char));
	(*onec_str)->size = 4;
	(*onec_str)->len = 0;
}

void inline __attribute__ ((always_inline))
_onec_string_put( onec_string** onec_str, char ch_val )
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

void inline __attribute__ ((always_inline))
_onec_string_insert( onec_string** onec_str, char* const string )
{
	char c;
	size_t index = 0;
	
	while( (c = string[index]) != '\0' )
	{
		_onec_string_put(onec_str, c);
		index += 1;
	}
}

char* _onec_string_get( onec_string** onec_str )
{
	char *_onec_str = (char*)malloc((1 + (*onec_str)->len) * sizeof(char));
	strncpy(_onec_str, (*onec_str)->val, (*onec_str)->len);
	_onec_str[(*onec_str)->len] = '\0';

	return _onec_str;
}

void inline __attribute__ ((always_inline))
_onec_string_reset( onec_string** onec_str )
{
	memset((*onec_str)->val, 4, sizeof(char));
	(*onec_str)->size = 4;
	(*onec_str)->len = 0;
}

void inline __attribute__ ((always_inline))
_onec_string_release( onec_string** onec_str )
{	
	if( (*onec_str)->val )
		free((*onec_str)->val);
	
	if( (*onec_str) )
		free(*onec_str);
}

void inline __attribute ((always_inline))
_onec_string_initlc( onec_stringlc* onec_str )
{
	onec_str->val[0] = '\0';
	onec_str->len = 0;
}

void inline __attribute__ ((always_inline))
_onec_string_putlc( onec_stringlc* onec_str, char ch_val )
{
	onec_str->val[onec_str->len] = ch_val;
	onec_str->len += 1;
}

void _onec_string_appendlc( onec_stringlc* onec_str, size_t size, ... )
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
			_onec_string_putlc(onec_str, c);
			index += 1;
		}
	}
	va_end(args);
}

char* _onec_string_getlc( onec_stringlc* onec_str )
{
	char* _onec_str = (char*)malloc((1 + onec_str->len) * sizeof(char));
	strncpy(_onec_str, onec_str->val, onec_str->len);
	_onec_str[onec_str->len] = '\0';
	return _onec_str;
}

void inline __attribute__ ((always_inline))
_onec_string_trimlc( onec_stringlc* onec_str )
{
	onec_str->val[onec_str->len] = '\0';
}

void inline __attribute__ ((always_inline))
_onec_string_insertlc( onec_stringlc* onec_str, char* const string )
{
	char c;
	size_t index = 0;
	
	while( (c = string[index]) != '\0' )
	{
		_onec_string_putlc(onec_str, c);
		index += 1;
	}
}

void inline __attribute__ ((always_inline))
_onec_string_resetlc( onec_stringlc* onec_str )
{
	onec_str->val[0] = '\0';
	onec_str->len = 0;
}