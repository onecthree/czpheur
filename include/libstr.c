
// #include <ext/standard/md5.h>
// #include <openssl/md5.h>

#include <php.h>
#include <php_ini.h>
#include <ext/standard/info.h>
#include <Zend/zend_types.h>

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "string.h"

// char* md5_file_sum( char const* filename )
// {
//     unsigned char c[MD5_DIGEST_LENGTH];
//     int i;

//     FILE *file = fopen(filename, "rb");
//     MD5_CTX mdContext;

//     int bytes;
//     unsigned char data[1024];

//     if(file == NULL)
//     {
//         zend_error(E_ERROR, "%s can't be opened.\n", filename);
//         return 0;
//     }

//     MD5_Init(&mdContext);
//     while((bytes = fread(data, 1, 1024, file)) != 0)
//         MD5_Update(&mdContext, data, bytes);
//     MD5_Final(c, &mdContext);

//     int retval_len = 0;
//     char* retval = (char*)malloc(sizeof(char) * 33);

//     for(i = 0; i < MD5_DIGEST_LENGTH; i++)
//     {   
//         // if( retval_len >= retval_size )
//         // {
//         //     retval_size = retval_size * 2;
//         //     retval = (char*)realloc(retval, retval_size);
//         // }

//         char buffer[6];
//         sprintf(buffer, "%02x", c[i]);

//         retval[retval_len] = buffer[0];
//         ++retval_len;

//         retval[retval_len] = buffer[1];
//         ++retval_len;
//     }
//     retval[retval_len] = '\0';

//     if( file )
//         fclose(file);

//     return retval;
// }

// Don't inline the function; it's stored in virtue symbol_table
unsigned long string_to_digits( char* source_src, size_t source_len, size_t limit )
{
    unsigned long repnum = 0; 
    size_t index = 0;

    for( ;index < source_len; index++ )
    {
        int c = (int)(source_src[index]);
        repnum *= c > 99 ? 1000 : c > 9 ? 100 : 10;
        repnum += c;

        if( repnum > limit )
            return 0;
    }

    return repnum;
}
