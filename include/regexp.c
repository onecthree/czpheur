
#include <string.h>
#include <stdbool.h>
#include "regexp.h"


bool regexp_match(
    unsigned char* const pattern_src,
    size_t pattern_size,
    unsigned char* const subject_src,
    size_t subject_size
)
{
    int error_code;
    PCRE2_SIZE error_offset;
    PCRE2_UCHAR* result_src = (PCRE2_UCHAR*)"";
    PCRE2_SIZE result_len = 0;
    PCRE2_SPTR pattern = pattern_src;
    PCRE2_SPTR subject = subject_src; 
    pcre2_code* re =
        pcre2_compile(pattern, pattern_size, PCRE2_UTF, &error_code, &error_offset, NULL);

    if(! re ) {
        PCRE2_UCHAR error[MAX_SAFE_PCRE2_UCHAR_LENGTH];
        pcre2_get_error_message(error_code, error, sizeof(error));
        return false;
    }

    pcre2_jit_compile(re, PCRE2_JIT_COMPLETE);
    pcre2_match_data *match_data = pcre2_match_data_create_from_pattern(re, NULL);
    if( pcre2_match(re, subject, subject_size, 0, 0, match_data, NULL) < 0 ) {
        result_src = NULL;
        result_len = 0;
    }

    pcre2_substring_get_bynumber(match_data, 0, &result_src, &result_len);
    pcre2_match_data_free(match_data);
    pcre2_code_free(re);

    if(! result_src )
        return false;

    return true;
}

bool regexp_match_result(
    unsigned char* const pattern_src, size_t pattern_size,
    unsigned char* const subject_src, size_t subject_size,
    char** result_src, size_t* result_len
)
{
    int error_code; PCRE2_SIZE error_offset;
    PCRE2_UCHAR* _result_src = (PCRE2_UCHAR*)"";
    PCRE2_SIZE _result_len = 0;
    PCRE2_SPTR pattern = pattern_src;
    PCRE2_SPTR subject = subject_src; 
    pcre2_code* re = 
        pcre2_compile(pattern, pattern_size, PCRE2_UTF, &error_code, &error_offset, NULL);

    if(! re ) {
        PCRE2_UCHAR error[MAX_SAFE_PCRE2_UCHAR_LENGTH];
        pcre2_get_error_message(error_code, error, sizeof(error));
        return false;
    }

    pcre2_jit_compile(re, PCRE2_JIT_COMPLETE);
    pcre2_match_data* match_data = pcre2_match_data_create_from_pattern(re, NULL);
    if( pcre2_match(re, subject, subject_size, 0, PCRE2_DOLLAR_ENDONLY, match_data, NULL) >= 0 ) {
        pcre2_substring_get_bynumber(match_data, 0, &_result_src, &_result_len);
    }

    pcre2_match_data_free(match_data);
    pcre2_code_free(re);

    if(! _result_len )
        return false;

    *result_len = _result_len;
    *result_src = (char*)malloc(sizeof(char) * _result_len);
    strncpy(*result_src, (char*)_result_src, _result_len);
    return true;
}

char* regexp_replace(
    unsigned char* const pattern_src, size_t pattern_len,
    unsigned char* const subject_src, size_t subject_len,
    unsigned char* const replacement_src, size_t replacement_len,
    char** matched_src, size_t* matched_len,
    int* matched_bool
)
{
    int error; PCRE2_SIZE erroffset;
    PCRE2_UCHAR* _matched_src = (PCRE2_UCHAR*)"";
    PCRE2_SIZE _matched_len = 0;

    PCRE2_UCHAR result_src[1024] = "";
    PCRE2_SIZE result_len = 1024;

    PCRE2_SPTR subject = subject_src;
    PCRE2_SPTR pattern = pattern_src;
    PCRE2_SPTR replacement = replacement_src;

    pcre2_code* re = pcre2_compile(pattern, pattern_len, 0, &error, &erroffset, NULL);

    if(! re ) return NULL;

    pcre2_jit_compile(re, PCRE2_JIT_COMPLETE);
    pcre2_match_data* match_data = pcre2_match_data_create_from_pattern(re, NULL);
    int rc = pcre2_substitute(
        re, subject, subject_len, 0,
        PCRE2_SUBSTITUTE_MATCHED | PCRE2_SUBSTITUTE_GLOBAL | PCRE2_SUBSTITUTE_EXTENDED, match_data, NULL,
        replacement, replacement_len, result_src, &result_len
    );
    pcre2_code_free(re);

    if( rc >= 0 ) {
        pcre2_substring_get_bynumber(match_data, 0, &_matched_src, &_matched_len);
        *matched_bool = 1;
    }
    pcre2_match_data_free(match_data);

    if( (matched_src != NULL && matched_len != NULL) && !result_len )
    {
        *matched_src = (char*)malloc(1);
        (*matched_src)[0] = '\0';
        *matched_len = 0;
        return NULL;
    }
    
    if( matched_src != NULL && matched_len != NULL )
    {
        *matched_src = (char*)malloc((_matched_len * sizeof(char)) + 1);
        strncpy((*matched_src), (char*)_matched_src, _matched_len);
        *matched_len = _matched_len;
        (*matched_src)[_matched_len] = '\0';
    }

    char* retval = (char*)malloc((result_len * sizeof(char)) + 1);
    strncpy(retval, (char*)result_src, result_len);
    result_src[result_len] = '\0';
    
    return retval;
}