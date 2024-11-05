
#ifndef H_INCLUDE_ZPHEUR_HEM
#define H_INCLUDE_ZPHEUR_HEM

#include <php.h>
#include <php_ini.h>
#include <ext/standard/info.h>
#include <php_zpheur.h>
#include <Zend/zend_smart_str.h>
#include <Zend/zend_smart_string.h>
#include <ext/standard/html.h>
#include <Zend/zend_builtin_functions.h>
#include <zend_language_parser.h>
#include <Zend/zend_attributes.h>
#include <ext/spl/spl_iterators.h>
#include <Zend/zend_interfaces.h>

#include <include/onecstr.h>
#include <include/zerror.h>
#include <include/runtime.h>


#define UseParamArguments "UseParamArguments", sizeof("UseParamArguments") - 1

zend_result _ext_cli_entry_main( void );

#endif