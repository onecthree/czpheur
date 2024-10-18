
#ifndef H_INCLUDE_ZERROR
#define H_INCLUDE_ZERROR

zend_string* trim_error_message( char const* errstr );

void zpheur_errorhandler( int errtype, zend_string *errfile, const uint32_t errline, zend_string *errstr );

#endif