#include <php.h>
#include <ext/standard/info.h>
#include <php_zpheur.h>

#include <string.h>
#include <zpheur.h>
#include <include/runtime.h>
#include <include/furouter.h>
#include <include/strings.h>
#include <Zpheur/Schemes/Http/Routing/Route_arginfo.h>
#include "Dispatcher_arginfo.h"


zend_object_handlers dispatcher_object_handlers;

#define HTTP_DISPATCHER_STATIC_STABLE 1 << 1
#define ROUTE_URI_LINE_MAX_CAPACITY 64

typedef struct _route_uri_line {
    char* uri_src;
    size_t uri_len;
    char* class_src;
    size_t class_len;
    char* method_src;
    size_t method_len;
} route_uri_line;

typedef struct _dispatcher_common_t
{
    route_uri_line route_get_src[ROUTE_URI_LINE_MAX_CAPACITY];
    size_t route_get_len;
    route_uri_line route_post_src[ROUTE_URI_LINE_MAX_CAPACITY];
    size_t route_post_len;
    route_uri_line route_put_src[ROUTE_URI_LINE_MAX_CAPACITY];
    size_t route_put_len;
    route_uri_line route_patch_src[ROUTE_URI_LINE_MAX_CAPACITY];
    size_t route_patch_len;
    route_uri_line route_delete_src[ROUTE_URI_LINE_MAX_CAPACITY];
    size_t route_delete_len;
    route_uri_line route_options_src[ROUTE_URI_LINE_MAX_CAPACITY];
    size_t route_options_len;
    bool static_table;
} dispatcher_common_t;

typedef struct _dispatcher_object
{
    dispatcher_common_t* common;
    zend_object std;
} dispatcher_object;

void free_dispatcher_object(zend_object* object)
{
    dispatcher_object* instance = ZPHEUR_GET_OBJECT(dispatcher_object, object);

    zend_object_std_dtor(&instance->std);
    if( instance->common )
    {
        efree(instance->common);
    }
}

zend_object* create_dispatcher_object( zend_class_entry* ce )
{
    dispatcher_object* object = 
        ecalloc(1, sizeof(dispatcher_object) + zend_object_properties_size(ce));

    zend_object_std_init(&object->std, ce);
    object_properties_init(&object->std, ce);

    memcpy(&dispatcher_object_handlers, zend_get_std_object_handlers(), sizeof(dispatcher_object_handlers));
    dispatcher_object_handlers.offset = XtOffsetOf(dispatcher_object, std);
    dispatcher_object_handlers.free_obj = free_dispatcher_object;
    object->std.handlers = &dispatcher_object_handlers;
    object->common = ecalloc(1, sizeof(dispatcher_common_t));
    object->common->route_get_len = 0;
    object->common->route_post_len = 0;
    object->common->route_put_len = 0;
    object->common->route_patch_len = 0;
    object->common->route_delete_len = 0;
    object->common->route_options_len = 0;
    object->common->static_table = false;

    return &object->std;
}

PHP_METHOD(RoutingDispatcher, __construct)
{
    zval* route_source = NULL;
    zend_long options = 1 << 0;

    ZEND_PARSE_PARAMETERS_START(1, 2)
        Z_PARAM_ARRAY(route_source)
        Z_PARAM_OPTIONAL
        Z_PARAM_LONG(options)
    ZEND_PARSE_PARAMETERS_END();

    if( options & HTTP_DISPATCHER_STATIC_STABLE ) {
        dispatcher_object* instance = 
            ZPHEUR_ZVAL_GET_OBJECT(dispatcher_object, getThis());
        instance->common->static_table = true;

        ZEND_HASH_FOREACH_KEY_VAL(Z_ARR_P(route_source), zend_long index, zend_string* key, zval* value)
        {
            zval* class;
            zval* method;
            route_uri_line* route;      
            size_t* route_len;

            if( zend_string_equals_cstr(key, "GET", sizeof("GET") - 1) ) {
                route = &instance->common->route_get_src[
                    instance->common->route_get_len
                ];
                route_len = &instance->common->route_get_len;
            } else
            if( zend_string_equals_cstr(key, "POST", sizeof("POST") - 1) ) {
                route = &instance->common->route_post_src[
                    instance->common->route_post_len
                ];
                route_len = &instance->common->route_post_len;
            } else
            if( zend_string_equals_cstr(key, "PUT", sizeof("PUT") - 1) ) {
                route = &instance->common->route_put_src[
                    instance->common->route_put_len
                ];
                route_len = &instance->common->route_put_len;
            } else
            if( zend_string_equals_cstr(key, "PATCH", sizeof("PATCH") - 1) ) {
                route = &instance->common->route_patch_src[
                    instance->common->route_patch_len
                ];
                route_len = &instance->common->route_patch_len;
            } else
            if( zend_string_equals_cstr(key, "DELETE", sizeof("DELETE") - 1) ) {
                route = &instance->common->route_delete_src[
                    instance->common->route_delete_len
                ];
                route_len = &instance->common->route_delete_len;
            } else
            if( zend_string_equals_cstr(key, "OPTIONS", sizeof("OPTIONS") - 1) ) {
                route = &instance->common->route_options_src[
                    instance->common->route_options_len
                ];
                route_len = &instance->common->route_options_len;
            }

            zend_long inner_index;
            zend_string* inner_key;
            zval* inner_value;
            ZEND_HASH_FOREACH_KEY_VAL(Z_ARR_P(value), inner_index, inner_key, inner_value)
            {
                class = zend_hash_index_find(Z_ARR_P(inner_value), 0);
                method = zend_hash_index_find(Z_ARR_P(inner_value), 1);
                route->uri_src = inner_key->val;
                route->uri_len = inner_key->len;
                route->class_src = Z_STRVAL_P(class);
                route->class_len = Z_STRLEN_P(class);
                route->method_src = Z_STRVAL_P(method);
                route->method_len = Z_STRLEN_P(method);

                route++;
                *route_len = *route_len + 1;
            }
            ZEND_HASH_FOREACH_END();
        }
        ZEND_HASH_FOREACH_END();
    } else {
        zend_this_update_property("route_source", route_source);
    }
}

PHP_METHOD(RoutingDispatcher, dispatch)
{
    char* http_method_src = NULL;
    size_t http_method_len = 0;

    char* http_uri_src = NULL;
    size_t http_uri_len = 0;

    ZEND_PARSE_PARAMETERS_START(2, 2)
        Z_PARAM_STRING(http_method_src, http_method_len)
        Z_PARAM_STRING(http_uri_src, http_uri_len)
    ZEND_PARSE_PARAMETERS_END();

    dispatcher_object* instance = 
        ZPHEUR_ZVAL_GET_OBJECT(dispatcher_object, getThis());

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

    furouter_target_uri target_uri_src[TARGET_URI_MAX_LENGTH_AS_REV];
    size_t target_uri_len = TARGET_URI_MAX_LENGTH_AS_REV;

    // Parse current request url to struct formatted
    if(! static_furouter_target_uri_parse(http_uri_src, (void*)&target_uri_src, &target_uri_len) )
        goto target_uri_max_length;

    if(! instance->common->static_table ) {
        zval* route_source =
            zend_hash_str_find(
                Z_ARR_P(zend_this_read_property("route_source")),
                http_method_src,
                http_method_len
            );

        if( route_source == NULL )
            zend_error(E_ERROR, "route_source not found");

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
    } else {
        route_uri_line* route_src;
        size_t route_len = 0, route_size;
        size_t target_verb = string_to_digits(http_method_src, http_method_len, LOCALVERB_OPTIONS);

        switch( target_verb ) {
            case LOCALVERB_GET:
                route_src = instance->common->route_get_src;
                route_size = instance->common->route_get_len;
            break;
            case LOCALVERB_POST:
                route_src = instance->common->route_post_src;
                route_size = instance->common->route_post_len;
            break;
            case LOCALVERB_PUT:
                route_src = instance->common->route_put_src;
                route_size = instance->common->route_put_len;
            break;
            case LOCALVERB_PATCH:
                route_src = instance->common->route_patch_src;
                route_size = instance->common->route_patch_len;
            break;
            case LOCALVERB_DELETE:
                route_src = instance->common->route_delete_src;
                route_size = instance->common->route_delete_len;
            break;
            case LOCALVERB_OPTIONS:
                route_src = instance->common->route_options_src;
                route_size = instance->common->route_options_len;
            break;
            default:
                php_error_docref(NULL, E_ERROR,
                    "%s: Unknown request incoming HTTP method verb", http_method_src);
            break;
        }

        for( int i = 0; i < route_size; i++ ) {
            route_uri_line route = route_src[i];

            // return 1 if found otherwise is 0
            if( cliteral_static_furouter_finder(route.uri_src, route.uri_len,
                (void*)&target_uri_src, target_uri_len, &segments) )
            {
                route_fund.class_name = zend_string_init(route.class_src, route.class_len, 0);
                route_fund.method_name = zend_string_init(route.method_src, route.method_len, 0);
                route_fund.order = 1;

                break;
            }
        }
    }

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

ZEND_MINIT_FUNCTION( Zpheur_Schemes_Http_Routing_Dispatcher )
{
    zend_class_entry ce;

    INIT_NS_CLASS_ENTRY(ce, "Zpheur\\Schemes\\Http\\Routing", "Dispatcher", zpheur_schemes_http_routing_dispatcher_class_method);
    zpheur_schemes_http_routing_dispatcher_class_entry = zend_register_internal_class(&ce);
    zpheur_schemes_http_routing_dispatcher_class_entry->ce_flags |= ZEND_ACC_ALLOW_DYNAMIC_PROPERTIES;
    zpheur_schemes_http_routing_dispatcher_class_entry->create_object = create_dispatcher_object;

    zend_register_long_constant("STATIC_TABLE", sizeof("STATIC_TABLE") - 1, 1 << 1, CONST_CS | CONST_PERSISTENT, module_number);
    zend_declare_property_null(zpheur_schemes_http_routing_dispatcher_class_entry, "route_source", sizeof("route_source") - 1, ZEND_ACC_PUBLIC);

    return SUCCESS;
}