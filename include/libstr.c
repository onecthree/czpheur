
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

// #include <ext/standard/md5.h>

#include "string.h"

// #include <openssl/md5.h>

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

char* substr(const char *src, int m, int n)
{
    int len = n - m;
 
    char *dest = (char*)malloc(sizeof(char) * (len + 1));
 
    for (int i = m; i < n && (*(src + i) != '\0'); i++)
    {
        *dest = *(src + i);
        dest++;
    }
 
    *dest = '\0';
 
    return dest - len;
}

unsigned long str_to_repnum( char* source_src, size_t source_len, size_t limit )
{
    long   repnum = 0; 
    size_t index = 0;
    while( index < source_len )
    {
        int c = (int)source_src[index];

        if( repnum > limit )
            return 0;

        if( c > 99 )
        {
            repnum *= 1000;
        }
        else if( c > 9 )
        {
            repnum *= 100;
        }
        else
        {
            repnum *= 10;
        }

        repnum += c;
        index += 1;
    }

    return repnum;
}