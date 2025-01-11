#include <php.h>
#include <ext/standard/info.h>
#include <php_zpheur.h>

#include <string.h>
#include <zpheur.h>
#include <include/runtime.h>
#include "DispatchItem_arginfo.h"


PHP_METHOD(DispatchItem, __construct)
{
    
}

ZEND_MINIT_FUNCTION(Zpheur_Schemes_WebSocket_Routing_Dispatcher_DispatchItem)
{
    zend_class_entry ce;

    INIT_NS_CLASS_ENTRY(ce, "Zpheur\\Schemes\\WebSocket\\Routing\\Dispatcher", "DispatchItem", zpheur_schemes_websocket_routing_dispatcher_dispatchitem_class_method);
    zpheur_schemes_websocket_routing_dispatcher_dispatchitem_class_entry = zend_register_internal_class(&ce);
    zpheur_schemes_websocket_routing_dispatcher_dispatchitem_class_entry->ce_flags |= ZEND_ACC_ALLOW_DYNAMIC_PROPERTIES;

    zend_declare_property_null(zpheur_schemes_websocket_routing_dispatcher_dispatchitem_class_entry, "request", sizeof("request") - 1, ZEND_ACC_PUBLIC);

    return SUCCESS;
}