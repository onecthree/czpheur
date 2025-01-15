

#ifndef H_INCLUDE_RANDOM
#define H_INCLUDE_RANDOM

#include <stdio.h>
#include <openssl/rand.h>

#define CHARS_RANDOMIZE_DEFAULT_CHARSET \
"abcdABCDefghEFGHijklIJKLmnopMNOPqrstQRSTuvwxUVWXyzYZ0123456789#!~*&%^@?+"

int single_digit_rand( int min, int max );
int random_chars( char* chars_src, unsigned int chars_len, size_t rand_len, char* str_src, size_t* str_len );

#endif