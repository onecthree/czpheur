#pragma once

#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

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