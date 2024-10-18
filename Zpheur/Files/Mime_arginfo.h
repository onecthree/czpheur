
#ifndef H_ZPHEUR_FILES_MIME
#define H_ZPHEUR_FILES_MIME

#include "ext/standard/info.h"
#include "ext/standard/php_standard.h"

#include "zend_API.h"
#include "zend_interfaces.h"
#include "zend_exceptions.h"


zend_class_entry* zpheur_files_mime_class_entry;

ZEND_BEGIN_ARG_INFO_EX(Mime___construct_arginfo, 0, 0, 0)
ZEND_END_ARG_INFO()

PHP_METHOD(Mime, __construct);

static const zend_function_entry zpheur_files_mime_class_method[] = {
    PHP_ME(Mime, __construct, Mime___construct_arginfo, ZEND_ACC_PUBLIC )
	ZEND_FE_END
};

#endif