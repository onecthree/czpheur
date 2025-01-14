#include <php.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <include/onecstr.h>
#include "furouter.h"


/**
 * Application request URI parser
 */
int static_furouter_target_uri_parse( char* url, void* ptr_local_uri_target_src, size_t* dest_uri_target_len )
{
    int error_no = -1;
    size_t local_uri_target_len = 0;

    onec_stringlc path_value;
    onec_string_initlc(path_value);

    furouter_target_context context = {0};
        context.index = 1;
        context.path_index = 0;
        context.state = STATE_TARGET_INIT_SCOPE;

    int context_type = 0;
    int current_type = 0;

    while( true )
    {
        context.input = url[context.index];

        if( context.state == STATE_TARGET_INIT_SCOPE )
        {
            context_type = 0;
            context.state = STATE_TARGET_FIND_TYPE_PATH_SCOPE;
            context.path_skip = false;

            onec_string_resetlc(path_value);
        }

        context.index += 1;

        switch( context.input )
        {   
            case TOKEN_SYMBOL_QUERY: // if query, backward to slash path
            case TOKEN_CHAR_NULL: // if null, it mean main index
                context.index -= 1;
            case TOKEN_SYMBOL_SLASH: // if begin/end path
                context.state = STATE_TARGET_INIT_SCOPE;
                context.path_index += 1;

                ((furouter_target_uri*)
                    ptr_local_uri_target_src)[local_uri_target_len].val = onec_string_getlc(path_value);
                ((furouter_target_uri*)
                    ptr_local_uri_target_src)[local_uri_target_len].len = path_value.len;

                switch( context_type )
                {
                    case PATH_TYPE_ALPHA:
                        ((furouter_target_uri*)ptr_local_uri_target_src)[local_uri_target_len].type = PATH_IMPL_ALPHA;
                    break;
                    case PATH_TYPE_NUM:
                        ((furouter_target_uri*)ptr_local_uri_target_src)[local_uri_target_len].type = PATH_IMPL_NUM;
                    break;
                    case PATH_TYPE_ALNUM:
                        ((furouter_target_uri*)ptr_local_uri_target_src)[local_uri_target_len].type = PATH_IMPL_ALNUM;
                    break;
                    case PATH_TYPE_ALPHA | PATH_TYPE_RANDOM:
                    case PATH_TYPE_NUM | PATH_TYPE_RANDOM:
                    case PATH_TYPE_ALNUM | PATH_TYPE_RANDOM:
                    case PATH_TYPE_RANDOM:
                        ((furouter_target_uri*)ptr_local_uri_target_src)[local_uri_target_len].type = PATH_IMPL_RANDOM;
                    break;
                    default:
                        ((furouter_target_uri*)ptr_local_uri_target_src)[local_uri_target_len].type = PATH_IMPL_ASTERIK;
                    break;
                }

                local_uri_target_len += 1;

                if( local_uri_target_len > TARGET_URI_MAX_LENGTH_AS_INDEX )
                    goto uri_max_length;

                switch( context.input )
                {
                    case TOKEN_SYMBOL_QUERY:
                    case TOKEN_CHAR_NULL:
                        goto end_parse;
                    break;
                    default:
                        if( url[context.index] == TOKEN_CHAR_NULL )
                            goto end_parse;

                        context.state = STATE_TARGET_INIT_SCOPE;
                    break;
                }
            break;
            case TOKEN_ALPHA_LOWER_START ... TOKEN_ALPHA_LOWER_END:
            case TOKEN_ALPHA_UPPER_START ... TOKEN_ALPHA_UPPER_END: 
                context_type |= PATH_TYPE_ALPHA;
                onec_string_putlc(path_value, context.input);
            break;
            case TOKEN_DIGIT_START ... TOKEN_DIGIT_END: 
                context_type |= PATH_TYPE_NUM;
                onec_string_putlc(path_value, context.input);
            break;
            default:        
                context_type |= PATH_TYPE_RANDOM;
                onec_string_putlc(path_value, context.input);
            break;
        }
    }
    
    end_parse:
    *dest_uri_target_len = local_uri_target_len;
    return 1;

    php_end_parse:
    furouter_error(E_ERROR, NULL, "unexpected error: [0] failed to reallocate the memory");
#ifndef PHP_VERSION
    exit(1);
#endif
    return 1;

    uri_max_length:
    *dest_uri_target_len = 32;
    return 0;
}

/**
 * Application route parser
 */
onec_string* static_furouter_route_uri_parse( char* route, char* class_name, char* method_name )
{
    int error_no = -1;

    onec_stringlc path_value;
    onec_string_initlc(path_value);

    onec_string* full_target;
    onec_string_init(full_target);

    size_t inner_value = 0;
    char   inner_impl;
    char*  _temporare_merge_str = malloc(0 * sizeof(char));
    bool   is_placeholder = false;

    furouter_target_context context = {0};

        context.index = 1;
        context.path_index = 0;
        context.state = STATE_TARGET_INIT_SCOPE;

    unsigned int                context_type = 0;
    int                         current_type = 0;

    while( true )
    {
        early_start:

        context.input = route[context.index];

        if( context.state == STATE_TARGET_INIT_SCOPE )
        {
            context_type = 0;

            context.state = STATE_TARGET_BEGIN_TYPE_PATH_SCOPE;
            context.path_skip = false;
            is_placeholder = false;
            inner_value = 0;

            onec_string_resetlc(path_value);
        }

        context.index += 1;

        switch( context.state )
        {
            case STATE_TARGET_ENDED_PATH_SCOPE:
                context.path_index += 1;

                switch( context_type )
                {
                    case PATH_TYPE_SKIP: break;
                    case PATH_TYPE_ALPHA:
                        inner_impl = PATH_IMPL_ALPHA;
                    break;
                    case PATH_TYPE_NUM:
                        inner_impl = PATH_IMPL_NUM;
                    break;
                    case PATH_TYPE_ALNUM:
                        inner_impl = PATH_IMPL_ALNUM;
                    break;
                    case PATH_TYPE_ALPHA | PATH_TYPE_RANDOM:
                    case PATH_TYPE_NUM | PATH_TYPE_RANDOM:
                    case PATH_TYPE_ALNUM | PATH_TYPE_RANDOM:
                    case PATH_TYPE_RANDOM:
                        inner_impl = PATH_IMPL_RANDOM;
                    break;
                    default:
                        inner_impl = PATH_IMPL_ASTERIK;
                    break;
                }
                
                char* __path_value = onec_string_getlc(path_value);
                fixed_merge_str(_temporare_merge_str, "%c%s ", inner_impl, __path_value);
                onec_string_append(full_target, 1, _temporare_merge_str);
                free(__path_value);

                switch( context.input )
                {
                    case TOKEN_CHAR_NULL:
                        goto end_parse;
                    break;
                    default:    
                        if( route[context.index] == TOKEN_CHAR_NULL )
                            goto end_parse;

                        context.state = STATE_TARGET_INIT_SCOPE;
                    break;
                }
            break;
            case STATE_TARGET_BEGIN_TYPE_PATH_SCOPE: // Being search
                switch( context.input )
                {   
                    case TOKEN_CHAR_NULL:
                    case TOKEN_SYMBOL_SLASH:
                        context.index -= 1;
                        context.state = STATE_TARGET_ENDED_PATH_SCOPE;
                    break;
                    case TOKEN_SYMBOL_QUERY:
                        error_no = 1;
                        goto php_end_parse;
                    break;
                    case TOKEN_ALPHA_LOWER_START ... TOKEN_ALPHA_LOWER_END:
                    case TOKEN_ALPHA_UPPER_START ... TOKEN_ALPHA_UPPER_END: 
                        context.state = STATE_TARGET_ALPHA_TYPE_PATH_SCOPE;
                        context.index -= 1;
                        context_type |= PATH_TYPE_ALPHA;
                    break;
                    case TOKEN_DIGIT_START ... TOKEN_DIGIT_END: 
                        context.state = STATE_TARGET_NUM_TYPE_PATH_SCOPE;
                        context.index -= 1;
                        context_type |= PATH_TYPE_NUM;
                    break;
                    case TOKEN_SYMBOL_COLON:
                        context.state = STATE_PLACEHOLDER_BEGIN_TYPE_PATH_SCOPE;
                        context_type |= PATH_TYPE_SKIP;
                    break; 
                    default:        
                        context.state = STATE_TARGET_RANDOM_TYPE_PATH_SCOPE;
                        context.index -= 1;
                        context_type |= PATH_TYPE_RANDOM;
                    break;
                }   
            break; // STATE_TARGET_FIND_TYPE_PATH_SCOPE
            case STATE_TARGET_FIND_TYPE_PATH_SCOPE: // Mid search
                switch( context.input )
                {   
                    case TOKEN_CHAR_NULL:
                    case TOKEN_SYMBOL_SLASH:
                        context.index -= 1;
                        context.state = STATE_TARGET_ENDED_PATH_SCOPE;
                    break;
                    case TOKEN_SYMBOL_QUERY:
                        error_no = 2;
                        goto php_end_parse;
                    break;
                    case TOKEN_ALPHA_LOWER_START ... TOKEN_ALPHA_LOWER_END:
                    case TOKEN_ALPHA_UPPER_START ... TOKEN_ALPHA_UPPER_END: 
                        context.state = STATE_TARGET_ALPHA_TYPE_PATH_SCOPE;
                        context.index -= 1;
                        context_type |= PATH_TYPE_ALPHA;
                    break;
                    case TOKEN_DIGIT_START ... TOKEN_DIGIT_END: 
                        context.state = STATE_TARGET_NUM_TYPE_PATH_SCOPE;
                        context.index -= 1;
                        context_type |= PATH_TYPE_NUM;
                    break;
                    default:        
                        context.state = STATE_TARGET_RANDOM_TYPE_PATH_SCOPE;
                        context.index -= 1;
                        context_type |= PATH_TYPE_RANDOM;
                    break;
                }
            break;
            case STATE_TARGET_ALPHA_TYPE_PATH_SCOPE:
                switch( context.input )
                {   
                    case TOKEN_ALPHA_LOWER_START ... TOKEN_ALPHA_LOWER_END:
                    case TOKEN_ALPHA_UPPER_START ... TOKEN_ALPHA_UPPER_END:
                        onec_string_putlc(path_value, context.input);
                    break;
                    default:
                        context.state = STATE_TARGET_FIND_TYPE_PATH_SCOPE;
                        context.index -= 1;
                    break;
                }
            break;
            case STATE_TARGET_NUM_TYPE_PATH_SCOPE:
                switch( context.input )
                {
                    case TOKEN_DIGIT_START ... TOKEN_DIGIT_END:
                        onec_string_putlc(path_value, context.input);
                    break;
                    default:
                        context.state = STATE_TARGET_FIND_TYPE_PATH_SCOPE;
                        context.index -= 1;
                    break;
                }
            break;
            case STATE_TARGET_RANDOM_TYPE_PATH_SCOPE:
                switch( context.input )
                {
                    case TOKEN_CHAR_NULL:
                    case TOKEN_SYMBOL_SLASH:
                    case TOKEN_SYMBOL_QUERY:
                        context.state = STATE_TARGET_FIND_TYPE_PATH_SCOPE;
                        context.index -= 1;
                    break;
                    case TOKEN_ALPHA_LOWER_START ... TOKEN_ALPHA_LOWER_END:
                    case TOKEN_ALPHA_UPPER_START ... TOKEN_ALPHA_UPPER_END:
                    case TOKEN_DIGIT_START ... TOKEN_DIGIT_END:
                        context.state = STATE_TARGET_FIND_TYPE_PATH_SCOPE;
                        context.index -= 1;
                    break;
                    default:
                        onec_string_putlc(path_value, context.input);
                    break;
                }
            break;
            case STATE_PLACEHOLDER_BEGIN_TYPE_PATH_SCOPE:
                switch( context.input )
                {
                    case TOKEN_ALPHA_LOWER_START ... TOKEN_ALPHA_LOWER_END:
                    case TOKEN_ALPHA_UPPER_START ... TOKEN_ALPHA_UPPER_END:
                    case TOKEN_SYMBOL_UNDERSCORE:
                        context.state = STATE_PLACEHOLDER_NAME_TYPE_PATH_SCOPE;
                        context.index -= 1;
                    break;
                    case TOKEN_DIGIT_START ... TOKEN_DIGIT_END:
                    default:
                        error_no = 3;
                        goto php_end_parse;
                    break;
                }
            break;
            case STATE_PLACEHOLDER_NAME_TYPE_PATH_SCOPE:
                switch( context.input )
                {
                    case TOKEN_ALPHA_LOWER_START ... TOKEN_ALPHA_LOWER_END:
                    case TOKEN_ALPHA_UPPER_START ... TOKEN_ALPHA_UPPER_END:
                    case TOKEN_DIGIT_START ... TOKEN_DIGIT_END:
                    case TOKEN_SYMBOL_UNDERSCORE:
                        onec_string_putlc(path_value, context.input);
                    break;
                    case TOKEN_SYMBOL_OPEN_ROUND_CURLY:
                        context.state = STATE_PLACEHOLDER_INNER_TYPE_PATH_SCOPE;
                    break;
                    default:    
                        error_no = 4;
                        goto php_end_parse;
                    break;
                }
            break;
            case STATE_PLACEHOLDER_INNER_TYPE_PATH_SCOPE:
                switch( context.input )
                {
                    case TOKEN_ALPHA_LOWER_START ... TOKEN_ALPHA_LOWER_END:
                        switch( context.input )
                        {
                            case 100 ... 126:
                                inner_value *= 1000;
                            break;
                            case 32 ... 99:
                                inner_value *= 100;
                            break;
                            default:
                                goto php_end_parse;
                            break;
                        }   
                        inner_value += context.input;

                        if( inner_value > PLACEHOLDER_TYPE_RANDOM )
                        {
                            error_no = 5;
                            goto php_end_parse;
                        }
                    break;
                    case TOKEN_SYMBOL_CLOSE_ROUND_CURLY:
                        context.state = STATE_PLACEHOLDER_END_TYPE_PATH_SCOPE;
                        is_placeholder = true;

                        switch( inner_value )
                        {
                            case PLACEHOLDER_TYPE_RANDOM:
                                inner_impl = PLACEHOLDER_IMPL_RANDOM;
                            break;
                            case PLACEHOLDER_TYPE_ALPHA:
                                inner_impl = PLACEHOLDER_IMPL_ALPHA;
                            break;
                            case PLACEHOLDER_TYPE_ALNUM:
                                inner_impl = PLACEHOLDER_IMPL_ALNUM;
                            break;
                            case PLACEHOLDER_TYPE_NUM:
                                inner_impl = PLACEHOLDER_IMPL_NUM;
                            break;
                            case PLACEHOLDER_TYPE_ANY:
                                inner_impl = PLACEHOLDER_IMPL_ANY;
                            break;
                            default:
                                error_no = 6;
                                goto php_end_parse;
                            break;
                        }
                    break;
                    default:
                        error_no = 7;
                        goto php_end_parse;
                    break;
                }
            break;
            case STATE_PLACEHOLDER_END_TYPE_PATH_SCOPE:
                switch( context.input )
                {
                    case TOKEN_CHAR_NULL:
                    case TOKEN_SYMBOL_SLASH:
                        context.state = STATE_TARGET_FIND_TYPE_PATH_SCOPE;
                        context.index -= 1;
                    break;
                    default:
                        error_no = 8;
                        goto php_end_parse;
                    break;
                }
            break;
        }
    }
    
    end_parse:
    free(_temporare_merge_str);
    return full_target;

    php_end_parse:
    free(_temporare_merge_str);
    
    php_file_free:
    switch( error_no )
    {
        // case 0: furouter_error(E_ERROR, NULL, "[furouter_route_uri_parse] [0] failed to open file target: no such file or directory \"%s\"", file_target); break;
        case 1: furouter_error(E_ERROR, NULL, "[furouter_route_uri_parse] [1] syntax_error: unexpected identifier"); break;
        case 2: furouter_error(E_ERROR, NULL, "[furouter_route_uri_parse] [2] syntax_error: unexpected identifier"); break;
        case 3: furouter_error(E_ERROR, NULL, "[furouter_route_uri_parse] [3] syntax_error: unexpected identifier"); break;
        case 4: furouter_error(E_ERROR, NULL, "[furouter_route_uri_parse] [4] syntax_error: unexpected identifier"); break;
        case 5: furouter_error(E_ERROR, NULL, "[furouter_route_uri_parse] [5] syntax_error: unexpected identifier"); break;
        case 6: furouter_error(E_ERROR, NULL, "[furouter_route_uri_parse] [6] syntax_error: unexpected placeholder type"); break;
        case 7: furouter_error(E_ERROR, NULL, "[furouter_route_uri_parse] [7] syntax_error: unexpected identifier"); break;
        case 8: furouter_error(E_ERROR, NULL, "[furouter_route_uri_parse] [8] syntax_error: unexpected identifier placeholder closing"); break;
    }

#ifndef PHP_VERSION
    exit(1);
#endif
    return (void*)NULL;
}

/**
 * Application route finder
 */
int static_furouter_finder( zend_string* route_current, void* target_uri_src, size_t target_uri_len, HashTable** placeholder )
{
    furouter_finder_context context = {0};
    onec_stringlc path_value;
    onec_string_initlc(path_value);

        context.index       = 0;
        context.path_index  = 0;
        context.state       = STATE_FINDER_TYPE_SCOPE;
        context.route_count = 0;
        
    while( context.index <= route_current->len ) // follow char length of each route list choosed
    {
        context.input = route_current->val[context.index];
        context.index += 1;

        if( context.path_index >= target_uri_len )
            goto return_not_found;

        switch( context.state )
        {
            case STATE_FINDER_TYPE_SCOPE:
                context.local_path_len = 0;
                context.route_count += 1;
                onec_string_resetlc(path_value);
                // onec_string_reset(path_value);

                switch( context.input )
                {
                    case PATH_IMPL_ASTERIK:
                        if( ((furouter_target_uri*)target_uri_src)[context.path_index].type == context.input )
                        {
                            switch( route_current->val[context.index] )
                            {   
                                case TOKEN_SYMBOL_WHITESPACE:
                                    context.index += 1;
                                    context.path_index += 1;
                                break;
                                default:
                                    goto early_return;
                                break;
                            }
                        }
                        else
                            goto return_not_found;
                    break;
                    case PATH_IMPL_ALPHA:
                    case PATH_IMPL_NUM:
                    case PATH_IMPL_ALNUM:
                    case PATH_IMPL_RANDOM:
                        if( ((furouter_target_uri*)target_uri_src)[context.path_index].type == context.input )
                            context.state = STATE_FINDER_WORD_CMP_SCOPE;
                        else
                            goto return_not_found;
                    break;
                    case PLACEHOLDER_IMPL_ALPHA:
                    case PLACEHOLDER_IMPL_NUM:
                    case PLACEHOLDER_IMPL_ALNUM:
                    case PLACEHOLDER_IMPL_RANDOM:
                        switch( context.input - ((furouter_target_uri*)target_uri_src)[context.path_index].type )
                        {
                            case PLACEHOLDER_IMPL_ALPHA - PATH_IMPL_ALPHA:
                            case PLACEHOLDER_IMPL_NUM - PATH_IMPL_NUM:
                            case PLACEHOLDER_IMPL_ALNUM - PATH_IMPL_ALNUM:
                            case PLACEHOLDER_IMPL_RANDOM - PATH_IMPL_RANDOM:
                                context.state = STATE_FINDER_PLACEHOLDER_SCOPE;
                            break;
                            default:
                                goto return_not_found;
                            break;
                        }
                    break;
                    case PLACEHOLDER_IMPL_ANY:
                        if( ((furouter_target_uri*)target_uri_src)[context.path_index].type != PATH_IMPL_ASTERIK )
                            context.state = STATE_FINDER_PLACEHOLDER_SCOPE;
                        else
                            goto return_not_found;
                    break; 
                }
            break;
            case STATE_FINDER_WORD_CMP_SCOPE:
                switch( context.input )
                {
                    case TOKEN_CHAR_NULL:
                    case TOKEN_SYMBOL_WHITESPACE:
                        if( context.local_path_len != (((furouter_target_uri*)target_uri_src)[context.path_index].len) )
                            goto return_not_found; 

                        context.state = STATE_FINDER_TYPE_SCOPE;
                        context.path_index += 1;
                    break;  
                    default:
                        if( context.input != ((furouter_target_uri*)target_uri_src)[context.path_index].val[context.local_path_len] )
                            goto return_not_found;
                        else
                            context.local_path_len += 1;
                    break;
                }
            break;
            case STATE_FINDER_PLACEHOLDER_SCOPE:
                switch( context.input )
                {
                    case TOKEN_CHAR_NULL:
                    case TOKEN_SYMBOL_WHITESPACE:
                    {
                        #ifdef PHP_VERSION

                        onec_string_trimlc(path_value);
                        zval _segment;
                        zend_string* placeholder_value =  // Do not release, php keep it track
                        zend_string_init(
                            ((furouter_target_uri*)target_uri_src)[context.path_index].val,
                            ((furouter_target_uri*)target_uri_src)[context.path_index].len,
                            0
                        );
                        ZVAL_STR(&_segment, placeholder_value);
                        zend_hash_str_update(*placeholder, path_value.val, path_value.len, &_segment);
                        #endif

                        context.state = STATE_FINDER_TYPE_SCOPE;
                        context.path_index += 1;
                    }
                    break;
                    default:    
                        onec_string_putlc(path_value, context.input);
                    break;
                }
            break;
        }
    }

    early_return:
    if( context.route_count == target_uri_len )
        return 1;

    return_not_found:
    return 0;
}

/**
 * Application route finder
 */
int cliteral_static_furouter_finder( char* route_current_src, size_t route_current_len, void* target_uri_src, size_t target_uri_len, HashTable** placeholder )
{
    furouter_finder_context context = {0};
    onec_stringlc path_value;
    onec_string_initlc(path_value);

        context.index       = 0;
        context.path_index  = 0;
        context.state       = STATE_FINDER_TYPE_SCOPE;
        context.route_count = 0;
        
    while( context.index <= route_current_len ) // follow char length of each route list choosed
    {
        context.input = route_current_src[context.index];
        context.index += 1;

        if( context.path_index >= target_uri_len )
            goto return_not_found;

        switch( context.state )
        {
            case STATE_FINDER_TYPE_SCOPE:
                context.local_path_len = 0;
                context.route_count += 1;
                onec_string_resetlc(path_value);
                // onec_string_reset(path_value);

                switch( context.input )
                {
                    case PATH_IMPL_ASTERIK:
                        if( ((furouter_target_uri*)target_uri_src)[context.path_index].type == context.input )
                        {
                            switch( route_current_src[context.index] )
                            {   
                                case TOKEN_SYMBOL_WHITESPACE:
                                    context.index += 1;
                                    context.path_index += 1;
                                break;
                                default:
                                    goto early_return;
                                break;
                            }
                        }
                        else
                            goto return_not_found;
                    break;
                    case PATH_IMPL_ALPHA:
                    case PATH_IMPL_NUM:
                    case PATH_IMPL_ALNUM:
                    case PATH_IMPL_RANDOM:
                        if( ((furouter_target_uri*)target_uri_src)[context.path_index].type == context.input )
                            context.state = STATE_FINDER_WORD_CMP_SCOPE;
                        else
                            goto return_not_found;
                    break;
                    case PLACEHOLDER_IMPL_ALPHA:
                    case PLACEHOLDER_IMPL_NUM:
                    case PLACEHOLDER_IMPL_ALNUM:
                    case PLACEHOLDER_IMPL_RANDOM:
                        switch( context.input - ((furouter_target_uri*)target_uri_src)[context.path_index].type )
                        {
                            case PLACEHOLDER_IMPL_ALPHA - PATH_IMPL_ALPHA:
                            case PLACEHOLDER_IMPL_NUM - PATH_IMPL_NUM:
                            case PLACEHOLDER_IMPL_ALNUM - PATH_IMPL_ALNUM:
                            case PLACEHOLDER_IMPL_RANDOM - PATH_IMPL_RANDOM:
                                context.state = STATE_FINDER_PLACEHOLDER_SCOPE;
                            break;
                            default:
                                goto return_not_found;
                            break;
                        }
                    break;
                    case PLACEHOLDER_IMPL_ANY:
                        if( ((furouter_target_uri*)target_uri_src)[context.path_index].type != PATH_IMPL_ASTERIK )
                            context.state = STATE_FINDER_PLACEHOLDER_SCOPE;
                        else
                            goto return_not_found;
                    break; 
                }
            break;
            case STATE_FINDER_WORD_CMP_SCOPE:
                switch( context.input )
                {
                    case TOKEN_CHAR_NULL:
                    case TOKEN_SYMBOL_WHITESPACE:
                        if( context.local_path_len != (((furouter_target_uri*)target_uri_src)[context.path_index].len) )
                            goto return_not_found; 

                        context.state = STATE_FINDER_TYPE_SCOPE;
                        context.path_index += 1;
                    break;  
                    default:
                        if( context.input != ((furouter_target_uri*)target_uri_src)[context.path_index].val[context.local_path_len] )
                            goto return_not_found;
                        else
                            context.local_path_len += 1;
                    break;
                }
            break;
            case STATE_FINDER_PLACEHOLDER_SCOPE:
                switch( context.input )
                {
                    case TOKEN_CHAR_NULL:
                    case TOKEN_SYMBOL_WHITESPACE:
                    {
                        #ifdef PHP_VERSION

                        onec_string_trimlc(path_value);
                        zval _segment;
                        zend_string* placeholder_value =  // Do not release, php keep it track
                        zend_string_init(
                            ((furouter_target_uri*)target_uri_src)[context.path_index].val,
                            ((furouter_target_uri*)target_uri_src)[context.path_index].len,
                            0
                        );
                        ZVAL_STR(&_segment, placeholder_value);
                        zend_hash_str_update(*placeholder, path_value.val, path_value.len, &_segment);
                        #endif

                        context.state = STATE_FINDER_TYPE_SCOPE;
                        context.path_index += 1;
                    }
                    break;
                    default:    
                        onec_string_putlc(path_value, context.input);
                    break;
                }
            break;
        }
    }

    early_return:
    if( context.route_count == target_uri_len )
        return 1;

    return_not_found:
    return 0;
}