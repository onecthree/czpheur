
#ifndef H_ZPHEUR_FUNCTIONS_HTTPVERB
#define H_ZPHEUR_FUNCTIONS_HTTPVERB

ZEND_BEGIN_ARG_INFO_EX(httpverb_arginfo, 0, 0, 1)
    ZEND_ARG_TYPE_INFO(0, request_method, IS_STRING | IS_FALSE, 0)
ZEND_END_ARG_INFO();

#endif