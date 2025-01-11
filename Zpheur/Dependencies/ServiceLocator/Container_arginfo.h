
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

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(Container_setArgumentResolver_arginfo, 1, 1, IS_VOID, 0)
    ZEND_ARG_TYPE_INFO(0, object, IS_OBJECT, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(Container_getService_arginfo, 1, 1, IS_MIXED, 0)
    ZEND_ARG_TYPE_INFO(0, serviceName, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(Container_getServiceFromArray_arginfo, 1, 1, IS_MIXED, 0)
    ZEND_ARG_TYPE_INFO(0, serviceName, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(Container_setClass_arginfo, 0, 2, IS_VOID, 0)
    ZEND_ARG_TYPE_INFO(0, class, IS_STRING, 0)
    ZEND_ARG_TYPE_INFO(0, derived_parent_class, _IS_BOOL, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(Container_setScalar_arginfo, 0, 2, IS_VOID, 0)
    ZEND_ARG_TYPE_INFO(0, key, IS_STRING, 0)
    ZEND_ARG_TYPE_INFO(0, value, IS_MIXED, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(Container_setClassFromArray_arginfo, 0, 1, IS_VOID, 0)
    ZEND_ARG_TYPE_INFO(0, array, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(Container_setScalarFromArray_arginfo, 0, 1, IS_VOID, 0)
    ZEND_ARG_TYPE_INFO(0, array, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(Container_hasService_arginfo, 0, 1, _IS_BOOL, 0)
    ZEND_ARG_TYPE_INFO(0, name, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(Container_listAllServices_arginfo, 0, 2, IS_ARRAY, 0)
    ZEND_ARG_TYPE_INFO(0, service, IS_STRING, 0)
    ZEND_ARG_TYPE_INFO(0, only_key_name, _IS_BOOL, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(Container_clean_arginfo, 0, 0, IS_VOID, 0)
ZEND_END_ARG_INFO()


PHP_METHOD(Container, __construct);
PHP_METHOD(Container, setArgumentResolver);
PHP_METHOD(Container, getService);
PHP_METHOD(Container, getServiceFromArray);

PHP_METHOD(Container, setClass);
PHP_METHOD(Container, setClassFromArray);
PHP_METHOD(Container, setScalar);
PHP_METHOD(Container, setScalarFromArray);

PHP_METHOD(Container, hasService);
PHP_METHOD(Container, listAllServices);
PHP_METHOD(Container, clean);


static const zend_function_entry zpheur_dependencies_servicelocator_container_class_method[] = {
    PHP_ME(Container, __construct, Container___construct_arginfo, ZEND_ACC_PUBLIC | ZEND_ACC_CTOR )
    PHP_ME(Container, setArgumentResolver, Container_setArgumentResolver_arginfo, ZEND_ACC_PUBLIC)
    PHP_ME(Container, getService, Container_getService_arginfo, ZEND_ACC_PUBLIC)
    PHP_ME(Container, getServiceFromArray, Container_getServiceFromArray_arginfo, ZEND_ACC_PUBLIC)

    PHP_ME(Container, setClass, Container_setClass_arginfo, ZEND_ACC_PUBLIC)
    PHP_ME(Container, setScalar, Container_setScalar_arginfo, ZEND_ACC_PUBLIC)
    PHP_ME(Container, setClassFromArray, Container_setClassFromArray_arginfo, ZEND_ACC_PUBLIC)
    PHP_ME(Container, setScalarFromArray, Container_setScalarFromArray_arginfo, ZEND_ACC_PUBLIC)

    PHP_ME(Container, hasService, Container_hasService_arginfo, ZEND_ACC_PUBLIC)
    PHP_ME(Container, listAllServices, Container_listAllServices_arginfo, ZEND_ACC_PUBLIC)
    PHP_ME(Container, clean, Container_clean_arginfo, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};

#endif