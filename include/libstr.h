
#ifndef H_INCLUDE_LIBSTR
#define H_INCLUDE_LIBSTR

#include <stdlib.h>

// char* md5_file_sum( char const* filename );

char* substr(const char *src, int m, int n);

unsigned long str_to_repnum( char* source_src, size_t source_len, size_t limit );

#endif