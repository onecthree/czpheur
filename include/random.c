
#include <stdio.h>
#include <openssl/rand.h>
#include "random.h"

int single_digit_rand( int min, int max )
{
	unsigned char buf[1];
	int digit;

	_rand:
	if( RAND_bytes(buf, sizeof(buf)) != 1 ) {
        return -1;
    }

    digit = *buf % 10;
    if( digit >= min && digit <= max ) {
    	return digit;
    }
    goto _rand;
}


int random_chars( char* chars_src, unsigned int chars_len, size_t rand_len, char* str_src, size_t* str_len )
{
	int level, level_max, chars_total;
	int rand, mulp, pos;

	if(! chars_src ) {
		chars_src = CHARS_RANDOMIZE_DEFAULT_CHARSET;
		chars_len = sizeof(CHARS_RANDOMIZE_DEFAULT_CHARSET) - 1;
	}

	if( chars_len < 8 || chars_len % 4 ) {
		return 0;
	}

	level = 0;
	level_max = chars_len / 40;
	chars_total = chars_len / 4;
	chars_len -= 4;

	for( int i = 0; i < rand_len; i++ ) {
		rand = single_digit_rand(0, chars_total);
		mulp = rand * 4;
		pos = mulp;

		if( pos >= chars_len ) {
			pos = pos % chars_len;
		}	

		pos += level * 40;
		if( pos > chars_len ) {
			pos = chars_len - mulp;
		}		

		pos += single_digit_rand(0, 3);

		str_src[*str_len] = chars_src[pos];
		*str_len += 1;

		level += 1;
		if( level > level_max ) {
			level = 0;
		}
	}
	str_src[*str_len] = '\0';

	return 1;
}
