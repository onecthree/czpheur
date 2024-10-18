
#ifndef H_INCLUDE_REGEXP
#define H_INCLUDE_REGEXP

#define PCRE2_CODE_UNIT_WIDTH 8

#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <pcre2.h>

#define MAX_SAFE_PCRE2_UCHAR_LENGTH 1024

static inline size_t _regexp_strlen( PCRE2_UCHAR* foo );

bool regexp_match(
	unsigned char* const 	pattern_src,
	size_t 					pattern_size,
	unsigned char* const 	subject_src,
	size_t 					subject_size
);

bool regexp_match_result(
	unsigned char* const 	pattern_src,
	size_t 					pattern_size,
	unsigned char* const 	subject_src,
	size_t 					subject_size,
	char** 					result_ptr
);

char const* regexp_replace(
	unsigned char* const 	pattern_v,
	unsigned char* const 	subject_v,
	unsigned char* const 	replacement_v,
	int 					*match
);

#endif