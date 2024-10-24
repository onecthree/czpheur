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
    char*   http_method_src = NULL;
    size_t  http_method_len = 0;

    char*   http_uri_src = NULL;
    size_t  http_uri_len = 0;

    ZEND_PARSE_PARAMETERS_START(2, 2)
        Z_PARAM_STRING(http_method_src, http_method_len)
        Z_PARAM_STRING(http_uri_src, http_uri_len)
    ZEND_PARSE_PARAMETERS_END();

    zval* route_source =
        zend_hash_str_find(
            Z_ARR_P(zend_this_read_property("route_source")),
            http_method_src,
            http_method_len
        );

    if( route_source == NULL )
    {
        zend_error(E_ERROR, "route_source not found");
    }

    HashTable *segments;
    ALLOC_HASHTABLE(segments);
    zend_hash_init(segments, 0, NULL, ZVAL_PTR_DTOR, 0);

    HashTable *context_dispatched;
    ALLOC_HASHTABLE(context_dispatched);
    zend_hash_init(context_dispatched, 0, NULL, ZVAL_PTR_DTOR, 0);

    furouter_fund* route_fund = (furouter_fund*)emalloc(sizeof(furouter_fund));
        route_fund->class = "";
        route_fund->method = "";
        route_fund->order = -1;

    onec_string* path_value;
    onec_string_init(path_value);


    furouter_target_uri     target_uri_src[TARGET_URI_MAX_LENGTH_AS_REV];
    size_t                  target_uri_len = TARGET_URI_MAX_LENGTH_AS_REV;

    // Parse current request url to struct formatted
    if(! static_furouter_target_uri_parse(
            http_uri_src,
            (void*)&target_uri_src,
            &target_uri_len
        )
    )
    {
        goto target_uri_max_length;
    }

    // Finder route
    // When route target not found, is ignored as default declaration value
    ZEND_HASH_FOREACH_STR_KEY_VAL(Z_ARR_P(route_source), zend_string* route_target, zval* action_target)
    {
        // return 1 if found otherwise is 0
        if( static_furouter_finder(
            route_target->val, 
            route_target->len, 
            &route_fund, 
            // &target_uri_src, 
            target_uri_src, 
            target_uri_len, 
            &segments, 
            path_value)
        )
        {
            zval* class_z = zend_hash_index_find(Z_ARR_P(action_target), 0);
            zval* method_z = zend_hash_index_find(Z_ARR_P(action_target), 1);
            route_fund->class = Z_STRVAL_P(class_z);
            route_fund->method = Z_STRVAL_P(method_z);
            route_fund->order = 1;

            break;
        }

        onec_string_reset(path_value);
    }
    ZEND_HASH_FOREACH_END();

    target_uri_max_length:
    onec_string_release(path_value);

    zval order;
    ZVAL_LONG(&order, route_fund->order);
    zend_hash_update_ind(context_dispatched,
        zend_string_init("order", sizeof("order") - 1, 0),
        &order
    );

    zval class_src;
    ZVAL_STRING(&class_src, route_fund->class);
    zend_hash_update_ind(context_dispatched,
        zend_string_init("class", sizeof("class") - 1, 0),
        &class_src
    );

    zval method_src;
    ZVAL_STRING(&method_src, route_fund->method);
    zend_hash_update_ind(context_dispatched,
        zend_string_init("method", sizeof("method") - 1, 0),
        &method_src
    );

    zval segment_src;
    ZVAL_ARR(&segment_src, segments);
    zend_hash_update_ind(context_dispatched,
        zend_string_init("segments", sizeof("segments") - 1, 0),
        &segment_src
    );

    /**
     *
     * Under test stack move from malloc'ed
     *  
    furouter_target_release(&target_uri_src, target_uri_len);
    furouter_target_release(&g_target_uri_src, target_uri_len);
    furouter_fund_release(&route_fund);
     *
     */

    RETURN_ARR(context_dispatched);
}

ZEND_MINIT_FUNCTION( Zpheur_Schemes_Http_Routing_Dispatcher )
{
    zend_class_entry ce;

    INIT_NS_CLASS_ENTRY(ce, "Zpheur\\Schemes\\Http\\Routing", "Dispatcher", zpheur_schemes_http_routing_dispatcher_class_method);
    zpheur_schemes_http_routing_dispatcher_class_entry = zend_register_internal_class(&ce);
    zpheur_schemes_http_routing_dispatcher_class_entry->ce_flags |= ZEND_ACC_ALLOW_DYNAMIC_PROPERTIES;

    zend_declare_property_null(zpheur_schemes_http_routing_dispatcher_class_entry, "route_source", sizeof("route_source") - 1, ZEND_ACC_PUBLIC);

    return SUCCESS;
}