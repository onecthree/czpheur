
#include <string.h>

#include "regexp.h"

bool regexp_match(
    unsigned char* const    pattern_src,
    size_t                  pattern_size,
    unsigned char* const    subject_src,
    size_t                  subject_size
)
{
    int errorcode;
    size_t resultlen;
    PCRE2_SIZE erroroffset;
    PCRE2_UCHAR *result;
    PCRE2_SPTR pattern = pattern_src;
    PCRE2_SPTR subject = subject_src; 
    pcre2_code *regexp = pcre2_compile(pattern, PCRE2_ZERO_TERMINATED, PCRE2_UTF, &errorcode, &erroroffset, NULL);

    if(regexp)
    {
        pcre2_match_data *match_data = pcre2_match_data_create_from_pattern(regexp, NULL);

        if( pcre2_match(regexp, subject, subject_size, 0, 0, match_data, NULL) >= 0 )
        {
            pcre2_substring_get_bynumber(match_data, 0, &result, &resultlen);
        }
        else
        {
            result = NULL;
            resultlen = 0;
        }

        pcre2_match_data_free(match_data);
        pcre2_code_free(regexp);
    } else {
        PCRE2_UCHAR error[256];
        pcre2_get_error_message(errorcode, error, sizeof(error));
    }

    if( result )
        return true;
    return false;
}

bool regexp_match_result(
    unsigned char* const    pattern_src,
    size_t                  pattern_size,
    unsigned char* const    subject_src,
    size_t                  subject_size,
    char**                  result_ptr
)
{
    int errorcode;
    size_t resultlen;
    PCRE2_SIZE erroroffset;
    PCRE2_UCHAR *result;
    PCRE2_SPTR pattern = pattern_src;
    PCRE2_SPTR subject = subject_src; 
    pcre2_code *regexp = pcre2_compile(pattern, PCRE2_ZERO_TERMINATED, PCRE2_UTF, &errorcode, &erroroffset, NULL);

    if(regexp)
    {
        pcre2_match_data *match_data = pcre2_match_data_create_from_pattern(regexp, NULL);

        if(pcre2_match(regexp, subject, subject_size, 0, 0, match_data, NULL) >= 0)
        {
            pcre2_substring_get_bynumber(match_data, 0, &result, &resultlen);
        } else {
            result = NULL;
            resultlen = 0;
        }

        pcre2_match_data_free(match_data);
        pcre2_code_free(regexp);
    } else {
        PCRE2_UCHAR error[256];
        pcre2_get_error_message(errorcode, error, sizeof(error));
    }

    if( result )
    {
        *result_ptr = realloc(*result_ptr, resultlen);
        strncpy(*result_ptr, (char* const)result, resultlen);

        return true;
    }

    return false;
}

static inline size_t _regexp_strlen( PCRE2_UCHAR* str )
{
    size_t length = 0;

    for( ; *str != '\0'; (*str)++ )
    {
        if( length > MAX_SAFE_PCRE2_UCHAR_LENGTH )
            return (size_t)(0);
        
        length++;
    }

    return length;
}

char const* regexp_replace(
    unsigned char* const    pattern_v,
    unsigned char* const    subject_v,
    unsigned char* const    replacement_v,
    int*                    match
)
{
    int error;
    PCRE2_SIZE erroffset;

    const PCRE2_SPTR subject = subject_v;  
    const PCRE2_SPTR pattern = pattern_v;
    const PCRE2_SPTR replacement = replacement_v;

    pcre2_code *re = pcre2_compile(pattern, PCRE2_ZERO_TERMINATED, 0, &error, &erroffset, 0);

    if (re == 0)
        return "";

    pcre2_jit_compile(re, PCRE2_JIT_COMPLETE);

    PCRE2_UCHAR result[MAX_SAFE_PCRE2_UCHAR_LENGTH];
    PCRE2_SIZE outlen = sizeof(result) / sizeof(PCRE2_UCHAR);

    int rc = pcre2_substitute(re, subject, PCRE2_ZERO_TERMINATED, 0, PCRE2_SUBSTITUTE_GLOBAL | PCRE2_SUBSTITUTE_EXTENDED, 0, 0, replacement, PCRE2_ZERO_TERMINATED, result, &outlen);

    pcre2_code_free(re);
        
    // size_t result_size = strlen((char* const)result);
    size_t result_size = _regexp_strlen(result);
    char* retval = malloc(result_size * sizeof(char));
    strncpy(retval, (char* const)result, result_size);
    
    if( rc )
        *match = 1;
    
    return retval;
}