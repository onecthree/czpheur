#include <php.h>
#include <ext/standard/info.h>
#include <php_zpheur.h>

#include <include/onecstr.h>
#include "filetoclass_arginfo.h"


#define FILETOCLASS_TOKEN_SYMBOL_SLASH 	'/'
#define FILETOCLASS_TOKEN_SYMBOL_DOT   	'.'

#define FILETOCLASS_STATE_FIND_APP    0
#define FILETOCLASS_STATE_CALC_APP    10932
#define FILETOCLASS_STATE_EXPORT      2

#define FILETOCLASS_TARGET_SUM_APP    10932

PHP_FUNCTION(filetoclass)
{
    char*   filename_src = NULL;
    size_t  filename_len = 0;

    ZEND_PARSE_PARAMETERS_START(1, 1)
    	Z_PARAM_STRING(filename_src, filename_len)
    ZEND_PARSE_PARAMETERS_END();

    int 		 context_state = FILETOCLASS_STATE_FIND_APP;
    size_t  	 index = 0;
    size_t  	 app_sum = 0;
    char 		 input;
    zend_string* classname_zs;

    onec_string* classname;
    onec_string_init(classname);

    while( (input = filename_src[index]) != '\0' )
    {
    	index += 1;

    	switch( context_state )
    	{
    		case FILETOCLASS_STATE_FIND_APP:
    			switch( input )
    			{
    				case FILETOCLASS_TOKEN_SYMBOL_SLASH:
    					context_state = FILETOCLASS_STATE_CALC_APP;
    				break;
    			}
    		break;
    		case FILETOCLASS_STATE_CALC_APP:
				switch( input )
				{
    				case FILETOCLASS_TOKEN_SYMBOL_SLASH:
    					if( app_sum == FILETOCLASS_TARGET_SUM_APP )
    					{
    						context_state = FILETOCLASS_STATE_EXPORT;
    						onec_string_append(classname, 1, "App\\");
    						break;
    					}

    					context_state = FILETOCLASS_STATE_FIND_APP;
    					index -= 1;
    					app_sum = 0;
    				break;
    				default:
    					if( app_sum > FILETOCLASS_TARGET_SUM_APP )
    					{
	    					context_state = FILETOCLASS_STATE_FIND_APP;
	    					app_sum = 0;
	    					break;
    					}

    					app_sum *= 10;
    					app_sum += (int)input;
    				break;
    			}
    		break;
    		case FILETOCLASS_STATE_EXPORT:
    			switch( input )
				{
					case FILETOCLASS_TOKEN_SYMBOL_DOT:
						goto end_parse;
					break;
    				case FILETOCLASS_TOKEN_SYMBOL_SLASH:
    					onec_string_put(classname, '\\');
    				break;
    				default:
    					onec_string_put(classname, input);
    				break;
    			}
    		break;
    	}
    }


    end_parse:

	classname_zs = zend_string_init(classname->val, classname->len, 0);
    onec_string_release(classname);

  	if( classname_zs->len )
  		RETURN_STR(classname_zs);

  	RETURN_NULL();
}