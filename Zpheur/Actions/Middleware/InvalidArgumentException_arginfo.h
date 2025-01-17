
#ifndef H_ZPHEUR_ACTIONS_MIDDLEWARE_INVALIDARGUMENTEXCEPTION
#define H_ZPHEUR_ACTIONS_MIDDLEWARE_INVALIDARGUMENTEXCEPTION

#include "ext/standard/info.h"
#include "ext/standard/php_standard.h"

#include "zend_API.h"
#include "zend_interfaces.h"
#include "zend_exceptions.h"


zend_class_entry* zpheur_caches_middleware_invalidargumentexception_class_entry;


static const zend_function_entry zpheur_caches_middleware_invalidargumentexception_class_method[] = {
    // PHP_ME(Middleware_InvalidArgumentException, init, Middleware_InvalidArgumentException_init_arginfo, ZEND_ACC_PUBLIC | ZEND_ACC_CTOR)
	ZEND_FE_END
};

#endif