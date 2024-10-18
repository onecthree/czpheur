
#ifndef H_ZPHEUR_DATABASES_PHASE_SCHEMA_MODEL
#define H_ZPHEUR_DATABASES_PHASE_SCHEMA_MODEL

#include "ext/standard/info.h"
#include "ext/standard/php_standard.h"

#include "zend_API.h"
#include "zend_interfaces.h"
#include "zend_exceptions.h"


zend_class_entry* zpheur_databases_phase_model_class_entry;

ZEND_BEGIN_ARG_INFO_EX(PhaseModel___construct_arginfo, 0, 0, 1)
    ZEND_ARG_TYPE_INFO(0, model_class, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(PhaseModel_migration_arginfo, 0, 0, IS_VOID, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(PhaseModel_all_arginfo, 0, 0, IS_VOID, 0)
ZEND_END_ARG_INFO()



PHP_METHOD(PhaseModel, __construct);
PHP_METHOD(PhaseModel, migration);
PHP_METHOD(PhaseModel, all);


static const zend_function_entry zpheur_databases_phase_model_class_method[] = {
    PHP_ME(PhaseModel, __construct, PhaseModel___construct_arginfo, ZEND_ACC_PUBLIC | ZEND_ACC_CTOR)
    PHP_ME(PhaseModel, migration, PhaseModel_migration_arginfo, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
    PHP_ME(PhaseModel, all, PhaseModel_all_arginfo, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
	ZEND_FE_END
};

#endif