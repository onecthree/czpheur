#include <php.h>
#include <ext/standard/info.h>
#include <php_zpheur.h>

#include <string.h>
#include <zpheur.h>
#include <include/runtime.h>
#include "ErrorRequest_arginfo.h"


ZEND_MINIT_FUNCTION(Zpheur_Schemes_WebSocket_Responder_ErrorRequest)
{
    zend_class_entry ce;

    INIT_NS_CLASS_ENTRY(ce, "Zpheur\\Schemes\\WebSocket\\Responder", "ErrorRequest", zpheur_schemes_websocket_responder_errorrequest_class_method);
    zpheur_schemes_websocket_responder_errorrequest_class_entry = zend_register_internal_class(&ce);
    zpheur_schemes_websocket_responder_errorrequest_class_entry->ce_flags |= ZEND_ACC_TRAIT;
    zpheur_schemes_websocket_responder_errorrequest_class_entry->ce_flags |= ZEND_ACC_ALLOW_DYNAMIC_PROPERTIES;

    return SUCCESS;
}