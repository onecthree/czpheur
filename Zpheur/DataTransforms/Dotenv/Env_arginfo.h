
#ifndef H_ZPHEUR_DATATRANSFORMS_DOTENV_ENV
#define H_ZPHEUR_DATATRANSFORMS_DOTENV_ENV


#include "ext/standard/info.h"
#include "ext/standard/php_standard.h"

#include "zend_API.h"
#include "zend_interfaces.h"
#include "zend_exceptions.h"


ZEND_BEGIN_ARG_INFO_EX(env_arginfo, 0, 0, 1)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, env_name, IS_STRING, 0, "\"\"")
    ZEND_ARG_TYPE_INFO(0, env_value, IS_STRING, 0)
ZEND_END_ARG_INFO()

#endif