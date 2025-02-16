
#ifndef H_INCLUDE_REGEXP
#define H_INCLUDE_REGEXP

#define PCRE2_CODE_UNIT_WIDTH 8

#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <pcre2.h>

#define MAX_SAFE_PCRE2_UCHAR_LENGTH 1024

bool regexp_match(
    unsigned char* const pattern_src,
    size_t pattern_size,
    unsigned char* const subject_src,
    size_t subject_size
);

bool regexp_match_result(
    unsigned char* const pattern_src,
    size_t pattern_size,
    unsigned char* const subject_src,
    size_t subject_size,
    char** result_src,
    size_t* result_len
);

char* regexp_replace(
    unsigned char* const pattern_src,
    size_t pattern_len,
    unsigned char* const subject_src,
    size_t subject_len,
    unsigned char* const replacement_src,
    size_t replacement_len,
    char** matched_src,
    size_t* matched_len,
    int* matched_bool
);

#endif