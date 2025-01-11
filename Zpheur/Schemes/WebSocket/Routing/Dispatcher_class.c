#include <php.h>
#include <ext/standard/info.h>
#include <php_zpheur.h>

#include <string.h>
#include <zpheur.h>
#include <include/runtime.h>
#include <include/furouter.h>
#include "Dispatcher_arginfo.h"


PHP_METHOD(RoutingDispatcher, __construct)
{
    zval* route_source = NULL;

    ZEND_PARSE_PARAMETERS_START(1, 1)
        Z_PARAM_ARRAY(route_source)
    ZEND_PARSE_PARAMETERS_END();

    zend_this_update_property("route_source", route_source);
}

PHP_METHOD(RoutingDispatcher, dispatch)
{
    char* websocket_method_src = NULL;
    size_t websocket_method_len = 0;

    char* websocket_uri_src = NULL;
    size_t websocket_uri_len = 0;

    ZEND_PARSE_PARAMETERS_START(2, 2)
        Z_PARAM_STRING(websocket_method_src, websocket_method_len)
        Z_PARAM_STRING(websocket_uri_src, websocket_uri_len)
    ZEND_PARSE_PARAMETERS_END();

    zval* route_source =
        zend_hash_str_find(
            Z_ARR_P(zend_this_read_property("route_source")),
            websocket_method_src,
            websocket_method_len
        );

    if( route_source == NULL )
        zend_error(E_ERROR, "route_source not found");

    HashTable *segments;
    ALLOC_HASHTABLE(segments);
    zend_hash_init(segments, 0, NULL, ZVAL_PTR_DTOR, 0);

    HashTable *context_dispatched;
    ALLOC_HASHTABLE(context_dispatched);
    zend_hash_init(context_dispatched, 4, NULL, ZVAL_PTR_DTOR, 0);

    // Stack init
    furouter_fund route_fund = {
        .class_name = zend_string_init_fast("", 0),
        .method_name = zend_string_init_fast("", 0),
        .order = -1,
    };

    furouter_target_uri     target_uri_src[TARGET_URI_MAX_LENGTH_AS_REV];
    size_t                  target_uri_len = TARGET_URI_MAX_LENGTH_AS_REV;

    // Parse current request url to struct formatted
    if(! static_furouter_target_uri_parse(websocket_uri_src, (void*)&target_uri_src, &target_uri_len) )
        goto target_uri_max_length;

    // Loop throught hashtable of application route's
    // When route target not found, is ignored as default declaration value
    ZEND_HASH_FOREACH_STR_KEY_VAL(Z_ARR_P(route_source), zend_string* route_target, zval* action_target)
    {
        // return 1 if found otherwise is 0
        if( static_furouter_finder(route_target, (void*)&target_uri_src, target_uri_len, &segments) )
        {
            zval* class_z = zend_hash_index_find(Z_ARR_P(action_target), 0);
            zval* method_z = zend_hash_index_find(Z_ARR_P(action_target), 1);
            route_fund.class_name = zval_get_string(class_z);
            route_fund.method_name = zval_get_string(method_z);
            route_fund.order = 1;

            break;
        }
    }
    ZEND_HASH_FOREACH_END();

    target_uri_max_length:

    for( int i = 0; i < target_uri_len; i++ )
        free(target_uri_src[i].val);

    zval order, class_src, method_src, segment_src;

    ZVAL_LONG(&order, route_fund.order);
    ZVAL_STRINGL(&class_src, route_fund.class_name->val, route_fund.class_name->len);
    ZVAL_STRINGL(&method_src, route_fund.method_name->val, route_fund.method_name->len);
    ZVAL_ARR(&segment_src, segments);

    zend_hash_str_update(context_dispatched, "order", sizeof("order") - 1, &order);
    zend_hash_str_update(context_dispatched, "class", sizeof("class") - 1, &class_src);
    zend_hash_str_update(context_dispatched, "method", sizeof("method") - 1, &method_src);
    zend_hash_str_update(context_dispatched, "segments", sizeof("segments") - 1, &segment_src);

    zend_string_release(route_fund.class_name);
    zend_string_release(route_fund.method_name);

    RETURN_ARR(context_dispatched);
}

ZEND_MINIT_FUNCTION( Zpheur_Schemes_WebSocket_Routing_Dispatcher )
{
    zend_class_entry ce;

    INIT_NS_CLASS_ENTRY(ce, "Zpheur\\Schemes\\WebSocket\\Routing", "Dispatcher", zpheur_schemes_websocket_routing_dispatcher_class_method);
    zpheur_schemes_websocket_routing_dispatcher_class_entry = zend_register_internal_class(&ce);
    zpheur_schemes_websocket_routing_dispatcher_class_entry->ce_flags |= ZEND_ACC_ALLOW_DYNAMIC_PROPERTIES;

    zend_declare_property_null(zpheur_schemes_websocket_routing_dispatcher_class_entry, "route_source", sizeof("route_source") - 1, ZEND_ACC_PUBLIC);

    return SUCCESS;
}