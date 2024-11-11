
#ifndef H_ZPHEUR_DEPENDENCIES_SERVICELOCATOR_CONTAINER
#define H_ZPHEUR_DEPENDENCIES_SERVICELOCATOR_CONTAINER

#include "ext/standard/info.h"
#include "ext/standard/php_standard.h"

#include "zend_API.h"
#include "zend_interfaces.h"
#include "zend_exceptions.h"


#define CONTAINER_CLASS_NAME_SRC "Zpheur\\Dependencies\\ServiceLocator\\Container"
#define CONTAINER_CLASS_NAME_LEN sizeof(CONTAINER_CLASS_NAME_SRC) - 1


zend_class_entry* zpheur_dependencies_servicelocator_container_class_entry;

ZEND_BEGIN_ARG_INFO_EX(Container___construct_arginfo, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(Container_get_arginfo, 1, 1, IS_MIXED, 0)
    ZEND_ARG_TYPE_INFO(0, name, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(Container_getOf_arginfo, 1, 1, IS_MIXED, 0)
    ZEND_ARG_TYPE_INFO(0, names, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(Container_set_arginfo, 0, 3, _IS_BOOL, 0)
    ZEND_ARG_TYPE_INFO(0, name, IS_STRING, 0)
    ZEND_ARG_TYPE_INFO(0, value, IS_STRING, 0)
    ZEND_ARG_TYPE_INFO(0, setToClasses, IS_TRUE, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(Container_has_arginfo, 0, 1, _IS_BOOL, 0)
    ZEND_ARG_TYPE_INFO(0, name, IS_STRING, 0)
ZEND_END_ARG_INFO()


PHP_METHOD(Container, __construct);
PHP_METHOD(Container, get);
PHP_METHOD(Container, getOf);
PHP_METHOD(Container, set);
PHP_METHOD(Container, has);


static const zend_function_entry zpheur_dependencies_servicelocator_container_class_method[] = {
    PHP_ME(Container, __construct, Container___construct_arginfo, ZEND_ACC_PUBLIC | ZEND_ACC_CTOR )
    PHP_ME(Container, get, Container_get_arginfo, ZEND_ACC_PUBLIC)
    PHP_ME(Container, getOf, Container_getOf_arginfo, ZEND_ACC_PUBLIC)
    PHP_ME(Container, set, Container_set_arginfo, ZEND_ACC_PUBLIC)
    PHP_ME(Container, has, Container_has_arginfo, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};

#endif