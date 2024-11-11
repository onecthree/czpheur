
#include <string.h>
#include "regexp.h"


bool regexp_match( unsigned char* const pattern_src, size_t pattern_size, unsigned char* const subject_src, size_t subject_size )
{
    int error_code;
    PCRE2_SIZE error_offset;
    PCRE2_UCHAR* result_src = (PCRE2_UCHAR*)"";
    PCRE2_SIZE result_len = 0;
    PCRE2_SPTR pattern = pattern_src;
    PCRE2_SPTR subject = subject_src; 
    pcre2_code *regexp =
        pcre2_compile(pattern, PCRE2_ZERO_TERMINATED, PCRE2_UTF, &error_code, &error_offset, NULL);

    if(! regexp ) {
        PCRE2_UCHAR error[MAX_SAFE_PCRE2_UCHAR_LENGTH];
        pcre2_get_error_message(error_code, error, sizeof(error));
        return false;
    }

    pcre2_match_data *match_data = pcre2_match_data_create_from_pattern(regexp, NULL);
    if( pcre2_match(regexp, subject, subject_size, 0, 0, match_data, NULL) < 0 ) {
        result_src = NULL;
        result_len = 0;
    }

    pcre2_substring_get_bynumber(match_data, 0, &result_src, &result_len);
    pcre2_match_data_free(match_data);
    pcre2_code_free(regexp);

    if(! result_src )
        return false;

    return true;
}

bool regexp_match_result( unsigned char* const pattern_src, size_t pattern_size, unsigned char* const subject_src, size_t subject_size, char** result_ptr )
{
    int error_code;
    PCRE2_SIZE error_offset;
    PCRE2_UCHAR* result_src = (PCRE2_UCHAR*)"";
    PCRE2_SIZE result_len = 0;
    PCRE2_SPTR pattern = pattern_src;
    PCRE2_SPTR subject = subject_src; 
    pcre2_code* regexp = 
        pcre2_compile(pattern, PCRE2_ZERO_TERMINATED, PCRE2_UTF, &error_code, &error_offset, NULL);

    if(! regexp ) {
        PCRE2_UCHAR error[MAX_SAFE_PCRE2_UCHAR_LENGTH];
        pcre2_get_error_message(error_code, error, sizeof(error));
        return false;
    }

    pcre2_match_data* match_data = pcre2_match_data_create_from_pattern(regexp, NULL);
    if( pcre2_match(regexp, subject, subject_size, 0, 0, match_data, NULL) < 0 ) {
        result_src = NULL;
        result_len = 0;
    }

    pcre2_substring_get_bynumber(match_data, 0, &result_src, &result_len);
    pcre2_match_data_free(match_data);
    pcre2_code_free(regexp);

    if(! result_src )
        return false;

    *result_ptr = (char*)malloc(sizeof(char) * result_len);
    strncpy(*result_ptr, (char*)result_src, result_len);
    return true;
}

char* regexp_replace( unsigned char* const pattern_v, unsigned char* const subject_v, unsigned char* const replacement_v, int* match )
{
    int error;
    PCRE2_SIZE erroffset;
    PCRE2_UCHAR* result_src = (PCRE2_UCHAR*)"";
    PCRE2_SIZE result_len = 0;
    const PCRE2_SPTR subject = subject_v;  
    const PCRE2_SPTR pattern = pattern_v;
    const PCRE2_SPTR replacement = replacement_v;

    pcre2_code *re = pcre2_compile(pattern, PCRE2_ZERO_TERMINATED, 0, &error, &erroffset, 0);

    if (re == 0)
        return (char*)"";

    pcre2_jit_compile(re, PCRE2_JIT_COMPLETE);
    int rc = pcre2_substitute(
        re, subject, PCRE2_ZERO_TERMINATED, 0,
        PCRE2_SUBSTITUTE_GLOBAL | PCRE2_SUBSTITUTE_EXTENDED, 0, 0,
        replacement, PCRE2_ZERO_TERMINATED, result_src, &result_len
    );
    pcre2_code_free(re);

    if( !result_len )
        return (char*)"";
        
    char* retval = (char*)malloc((result_len * sizeof(char)) + 1);
    strncpy(retval, (char*)result_src, result_len);
    result_src[result_len] = '\0';
    
    if( rc )
        *match = 1;
    
    return retval;
}