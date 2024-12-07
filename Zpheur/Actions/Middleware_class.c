#include <php.h>
#include <ext/standard/info.h>
#include <php_zpheur.h>

#include <Zend/zend_attributes.h>
#include <ext/spl/spl_iterators.h>
#include <Zend/zend_interfaces.h>

#include <string.h>
#include <stdlib.h>
#include <include/onecstr.h>
#include <zpheur.h>
#include <include/runtime.h>
#include <zpheur.h>
#include "Middleware_arginfo.h"

zend_object_handlers middleware_object_handlers;

#define MIDDLEWARE_T_LIST_MAX 64
#define MIDDLEWARE_T_ROUTE_MAX 64

typedef struct _middleware_list
{
    onec_string* middleware_name;
    zval* properties;
} middleware_list;

typedef struct _middleware_route
{
    middleware_list middlewares_src[MIDDLEWARE_T_LIST_MAX];
    size_t middlewares_len;
    onec_string* action_name;
} middleware_route;

typedef struct _middleware_common_t
{
    middleware_list before_middlewares_src[MIDDLEWARE_T_LIST_MAX];
    size_t before_middlewares_len;
    middleware_list after_middlewares_src[MIDDLEWARE_T_LIST_MAX];
    size_t after_middlewares_len;
    middleware_route cached_middlewares_src[MIDDLEWARE_T_ROUTE_MAX];
    size_t cached_middlewares_len;
} middleware_common_t;

typedef struct _middleware_object
{
    middleware_common_t* common;
    zend_object std;
} middleware_object;

void free_middleware_object(zend_object* object)
{
    middleware_object* instance = ZPHEUR_GET_OBJECT(middleware_object, object);

    zend_object_std_dtor(&instance->std);
    if( instance->common )
    {
        for( int i = 0; i < instance->common->cached_middlewares_len; i++ )
        {
            middleware_route* cached_middlewares =
                &instance->common->cached_middlewares_src[i];

            onec_string_release(cached_middlewares->action_name);
            for( int j = 0; j < cached_middlewares->middlewares_len; j++ )
            {
                onec_string_release(cached_middlewares->middlewares_src[j].middleware_name);
                efree(cached_middlewares->middlewares_src[j].properties); // Array doesnt release
            }
        }

        for( int i = 0; i < instance->common->before_middlewares_len; i++ )
        {
            middleware_list* before_middlewares =
                &instance->common->before_middlewares_src[i];

            onec_string_release(before_middlewares->middleware_name);
            efree(before_middlewares->properties);
        }

        for( int i = 0; i < instance->common->after_middlewares_len; i++ )
        {
            middleware_list* after_middlewares =
                &instance->common->after_middlewares_src[i];

            onec_string_release(after_middlewares->middleware_name);
            efree(after_middlewares->properties);
        }

        efree(instance->common);       
    }
}

zend_object* create_middleware_object( zend_class_entry* ce )
{
    middleware_object* object = 
        ecalloc(1, sizeof(middleware_object) + zend_object_properties_size(ce));

    zend_object_std_init(&object->std, ce);
    object_properties_init(&object->std, ce);

    memcpy(&middleware_object_handlers, zend_get_std_object_handlers(), sizeof(middleware_object_handlers));
    middleware_object_handlers.offset = XtOffsetOf(middleware_object, std);
    middleware_object_handlers.free_obj = free_middleware_object;
    object->std.handlers = &middleware_object_handlers;

    object->common = ecalloc(1, sizeof(middleware_common_t));
    object->common->before_middlewares_len = 0;
    object->common->after_middlewares_len = 0;
    object->common->cached_middlewares_len = 0;

    return &object->std;
}

int inline __attribute__ ((always_inline))
zend_string_equals_cstr_bt_middleware( zend_string* s1 )
{
    if(! s1 ) return 0;

    char* const s2 = "Middleware";
    size_t len = sizeof("Middleware") - 1;
    size_t s1_len, s2_len; size_t index = 1;

    for( ;index <= len; index++ )
    {
        s1_len = s1->len - index;
        s2_len = len - index;

        if( !s1->val[s1_len] || s1->val[s1_len] != s2[s2_len] )
            return 0;
    }

    if( s1->len > len && s1->val[s1->len - index] != '\\' )
        return 0;

    return 1;
}

PHP_METHOD(Middleware, __construct)
{
    HashTable* middlewares = NULL;

    ZEND_PARSE_PARAMETERS_START(1, 1)
        Z_PARAM_ARRAY_HT(middlewares);
    ZEND_PARSE_PARAMETERS_END();

    middleware_object* instance = 
        ZPHEUR_ZVAL_GET_OBJECT(middleware_object, getThis());
        
    middleware_route* cached_middlewares_src = 
        instance->common->cached_middlewares_src;

    ZEND_HASH_FOREACH_STR_KEY_VAL(middlewares, zend_string* key, zval* middleware)
    {
        if(! key )
        {
            php_error_docref(NULL, E_ERROR, "key must be stringable of class name");
        }

        if( Z_TYPE_P(middleware) != IS_ARRAY )
        {
            php_error_docref(NULL, E_ERROR, "middleware list must be type of array");
        }

        cached_middlewares_src[instance->common->cached_middlewares_len].action_name =
            onec_string_initd(key->val, key->len);

        size_t index = 0;
        ZEND_HASH_FOREACH_VAL(Z_ARR_P(middleware), zval* each_middleware)
        {
            if( Z_TYPE_P(each_middleware) != IS_ARRAY )
            {
                php_error_docref(NULL, E_ERROR, "middleware value must be type of array");
            }

            zval* middleware_name = zend_hash_index_find(Z_ARR_P(each_middleware), 0);
            zval* properties = zend_hash_index_find(Z_ARR_P(each_middleware), 1);

            if( !middleware_name || Z_TYPE_P(middleware_name) != IS_STRING )
                php_error_docref(NULL, E_ERROR, "format are not supported 11");

            if( !properties || Z_TYPE_P(properties) != IS_ARRAY )
                php_error_docref(NULL, E_ERROR, "format are not supported 22");

            zend_string* name = zval_get_string(middleware_name);
            cached_middlewares_src[instance->common->cached_middlewares_len]
                .middlewares_src[index].middleware_name = onec_string_initd(name->val, name->len);

            zval* new_ref_value = ecalloc(1, sizeof(zval));
            ZVAL_COPY(new_ref_value, properties);
            // ZVAL_MAKE_REF_EX(new_ref_value, 1);

            cached_middlewares_src[instance->common->cached_middlewares_len]
                .middlewares_src[index].properties = new_ref_value;

            index++;
            zend_string_release(name);
        }
        ZEND_HASH_FOREACH_END();
        cached_middlewares_src[instance->common->cached_middlewares_len].middlewares_len = index;

        instance->common->cached_middlewares_len++;
    }
    ZEND_HASH_FOREACH_END();
}

PHP_METHOD(Middleware, setBeforeRequest)
{
    HashTable*   middlewares;

    ZEND_PARSE_PARAMETERS_START(1, 1)
        Z_PARAM_ARRAY_HT(middlewares)
    ZEND_PARSE_PARAMETERS_END();

    middleware_object* instance = 
        ZPHEUR_ZVAL_GET_OBJECT(middleware_object, getThis());

    middleware_list* before_middlewares_src = 
        instance->common->before_middlewares_src;

    ZEND_HASH_FOREACH_VAL(middlewares, zval* middleware)
    {
        if( Z_TYPE_P(middleware) != IS_ARRAY )
            php_error_docref(NULL, E_ERROR, "Middleware must be array"); 

        if( zend_hash_num_elements(Z_ARR_P(middleware)) != 2 )
            php_error_docref(NULL, E_ERROR, "Array of arg must be 2 argument");                

        zval* middleware_name = zend_hash_index_find(Z_ARR_P(middleware), 0);
        zval* properties = zend_hash_index_find(Z_ARR_P(middleware), 1);

        if( Z_TYPE_P(middleware_name) != IS_STRING )
            php_error_docref(NULL, E_ERROR, "middleware name must be type of string"); 

        if( Z_TYPE_P(properties) != IS_ARRAY )
            php_error_docref(NULL, E_ERROR, "properties must be type of array"); 

        zend_string* name = zval_get_string(middleware_name);
        before_middlewares_src[instance->common->before_middlewares_len]
            .middleware_name = onec_string_initd(name->val, name->len);

        zval* new_ref_value = ecalloc(1, sizeof(zval));
        ZVAL_COPY(new_ref_value, properties);
        // ZVAL_MAKE_REF_EX(new_ref_value, 1);

        before_middlewares_src[instance->common->before_middlewares_len]
            .properties = new_ref_value;

        instance->common->before_middlewares_len++;
        zend_string_release(name);

    } ZEND_HASH_FOREACH_END();

    RETURN_ZVAL(getThis(), 1, 0);
}

void cached_middlewares_get(
    middleware_common_t* context, char* action_name_src, size_t action_name_len, HashTable* table
)
{
    zend_string* action_name = zend_string_init(action_name_src, action_name_len, 0);
    for( int i = 0; i < context->cached_middlewares_len; i++ )
    {
        onec_string* current_name = context->cached_middlewares_src[i].action_name;
        if( zend_string_equals_cstr(action_name, current_name->val, current_name->len) )
        {
            for( int j = 0; j < context->cached_middlewares_src[i].middlewares_len; j++ )
            {
                zend_string* name = zend_string_init(
                    context->cached_middlewares_src[i].middlewares_src[j].middleware_name->val,
                    context->cached_middlewares_src[i].middlewares_src[j].middleware_name->len,
                0);

                zval zv_name; ZVAL_STR(&zv_name, name);

                zval* middleware = emalloc(sizeof(zval));
                array_init(middleware);

                add_next_index_zval(middleware, &zv_name);
                add_next_index_zval(middleware, context->cached_middlewares_src[i].middlewares_src[j].properties);

                zend_hash_next_index_insert(table, middleware);
                efree(middleware);
            }

            break;
        }
    }
    zend_string_release(action_name);
}

PHP_METHOD(Middleware, setAfterRequest)
{
    HashTable* middlewares;

    ZEND_PARSE_PARAMETERS_START(1, 1)
        Z_PARAM_ARRAY_HT(middlewares)
    ZEND_PARSE_PARAMETERS_END();

    middleware_object* instance = 
        ZPHEUR_ZVAL_GET_OBJECT(middleware_object, getThis());

    middleware_list* after_middlewares_src = 
        instance->common->after_middlewares_src;

    ZEND_HASH_FOREACH_VAL(middlewares, zval* middleware)
    {
        if( Z_TYPE_P(middleware) != IS_ARRAY )
            php_error_docref(NULL, E_ERROR, "Middleware must be array"); 

        if( zend_hash_num_elements(Z_ARR_P(middleware)) != 2 )
            php_error_docref(NULL, E_ERROR, "Array of arg must be 2 argument");                

        zval* middleware_name = zend_hash_index_find(Z_ARR_P(middleware), 0);
        zval* properties = zend_hash_index_find(Z_ARR_P(middleware), 1);

        if( Z_TYPE_P(middleware_name) != IS_STRING )
            php_error_docref(NULL, E_ERROR, "middleware name must be type of string"); 

        if( Z_TYPE_P(properties) != IS_ARRAY )
            php_error_docref(NULL, E_ERROR, "properties must be type of array"); 

        zend_string* name = zval_get_string(middleware_name);
        after_middlewares_src[instance->common->after_middlewares_len]
            .middleware_name = onec_string_initd(name->val, name->len);

        zval* new_ref_value = ecalloc(1, sizeof(zval));
        ZVAL_COPY(new_ref_value, properties);
        // ZVAL_MAKE_REF_EX(new_ref_value, 1);

        after_middlewares_src[instance->common->after_middlewares_len]
            .properties = new_ref_value;

        instance->common->after_middlewares_len++;
        zend_string_release(name);

    } ZEND_HASH_FOREACH_END();

    RETURN_ZVAL(getThis(), 1, 0);
}

PHP_METHOD(Middleware, listAllMiddlewares)
{
    char* middleware_type_src = NULL;
    size_t middleware_type_len = 0;

    ZEND_PARSE_PARAMETERS_START(1, 1)
        Z_PARAM_STRING(middleware_type_src, middleware_type_len)
    ZEND_PARSE_PARAMETERS_END();

    HashTable* middlewares;
    ALLOC_HASHTABLE(middlewares);
    zend_hash_init(middlewares, 0, NULL, ZVAL_PTR_DTOR, 0);

    middleware_object* instance = 
        ZPHEUR_ZVAL_GET_OBJECT(middleware_object, getThis());

    if( strncmp("cached", middleware_type_src, middleware_type_len) == 0 )
    {
        for( int i = 0; i < instance->common->cached_middlewares_len; i++ )
        {
            zval* ht_route = emalloc(sizeof(zval));
            array_init(ht_route);
            onec_string* action_name = instance->common->cached_middlewares_src[i].action_name;
            for( int j = 0; j < instance->common->cached_middlewares_src[i].middlewares_len; j++ )
            {
                zval* ht_middleware = emalloc(sizeof(zval));
                array_init(ht_middleware);
                onec_string* _middleware_name = instance->common->cached_middlewares_src[i].middlewares_src[j].middleware_name;
                zend_string* middleware_name = zend_string_init(_middleware_name->val, _middleware_name->len, 0);
                zval zv_middleware_name; ZVAL_STR(&zv_middleware_name, middleware_name);
                zend_hash_next_index_insert(Z_ARR_P(ht_middleware), &zv_middleware_name);
                zend_hash_next_index_insert(Z_ARR_P(ht_middleware), 
                    instance->common->cached_middlewares_src[i].middlewares_src[j].properties
                );       

                zend_hash_next_index_insert(Z_ARR_P(ht_route), ht_middleware);         
                efree(ht_middleware);
            }

            zend_hash_str_update(middlewares, action_name->val, action_name->len, ht_route);
            efree(ht_route);
        }

        goto skip_error;
    }

    if( strncmp("before", middleware_type_src, middleware_type_len) == 0 )
    {
        for( int i = 0; i < instance->common->before_middlewares_len; i++ )
        {
            zval* ht_middleware = emalloc(sizeof(zval));
            array_init(ht_middleware);
            onec_string* _middleware_name = instance->common->before_middlewares_src[i].middleware_name;
            zend_string* middleware_name = zend_string_init(_middleware_name->val, _middleware_name->len, 0);
            zval zv_middleware_name; ZVAL_STR(&zv_middleware_name, middleware_name);
            zend_hash_next_index_insert(Z_ARR_P(ht_middleware), &zv_middleware_name);
            zend_hash_next_index_insert(Z_ARR_P(ht_middleware), 
                instance->common->before_middlewares_src[i].properties
            );       

            zend_hash_next_index_insert(middlewares, ht_middleware);         
            efree(ht_middleware);
        }

        goto skip_error;
    }

    if( strncmp("after", middleware_type_src, middleware_type_len) == 0 )
    {
        for( int i = 0; i < instance->common->after_middlewares_len; i++ )
        {
            zval* ht_middleware = emalloc(sizeof(zval));
            array_init(ht_middleware);
            onec_string* _middleware_name = instance->common->after_middlewares_src[i].middleware_name;
            zend_string* middleware_name = zend_string_init(_middleware_name->val, _middleware_name->len, 0);
            zval zv_middleware_name; ZVAL_STR(&zv_middleware_name, middleware_name);
            zend_hash_next_index_insert(Z_ARR_P(ht_middleware), &zv_middleware_name);
            zend_hash_next_index_insert(Z_ARR_P(ht_middleware), 
                instance->common->after_middlewares_src[i].properties
            );       

            zend_hash_next_index_insert(middlewares, ht_middleware);         
            efree(ht_middleware);
        }

        goto skip_error;
    }

    zend_hash_destroy(middlewares);
    FREE_HASHTABLE(middlewares);

    php_error_docref(NULL, E_ERROR,
            "parameter ($middlewareType) must be \"cached\", \"before\", or \"after\"");

    skip_error:
    RETURN_ARR(middlewares);
}

PHP_METHOD(Middleware, resolve)
{
    char* class_src = NULL;
    size_t class_len = 0;

    char* method_src = NULL;
    size_t method_len = 0;

    ZEND_PARSE_PARAMETERS_START(2, 2)
        Z_PARAM_STRING(class_src, class_len)
        Z_PARAM_STRING(method_src, method_len)
    ZEND_PARSE_PARAMETERS_END();

    middleware_object* instance = 
        ZPHEUR_ZVAL_GET_OBJECT(middleware_object, getThis());

    middleware_list* before_middlewares = 
        instance->common->before_middlewares_src;

    HashTable* middleware_table;
    ALLOC_HASHTABLE(middleware_table);
    zend_hash_init(middleware_table, 0, NULL, ZVAL_PTR_DTOR, 0);

    for( int i = 0; i < instance->common->before_middlewares_len; i++ )
    {
        zend_string* name = zend_string_init(
            instance->common->before_middlewares_src[i].middleware_name->val,
            instance->common->before_middlewares_src[i].middleware_name->len,
        0);
        zval zv_name; ZVAL_STR(&zv_name, name);

        zval* middleware = emalloc(sizeof(zval));
        array_init(middleware);

        add_next_index_zval(middleware, &zv_name);
        add_next_index_zval(middleware, instance->common->before_middlewares_src[i].properties);

        zend_hash_next_index_insert(middleware_table, middleware);
        efree(middleware);
    }

    onec_stringlc action;
    onec_string_initlc(action);
    onec_string_appendlc(action, 3, class_src, "::", method_src);
    onec_string_trimlc(action);

    cached_middlewares_get(instance->common, action.val, action.len, middleware_table);

    RETURN_ARR(middleware_table);
}

PHP_METHOD(Middleware, scanAndResolve)
{
    char* class_src = NULL;
    size_t class_len = 0;

    char* method_src = NULL;
    size_t method_len = 0;

    ZEND_PARSE_PARAMETERS_START(2, 2)
        Z_PARAM_STRING(class_src, class_len)
        Z_PARAM_STRING(method_src, method_len)
    ZEND_PARSE_PARAMETERS_END();

    middleware_object* instance = 
        ZPHEUR_ZVAL_GET_OBJECT(middleware_object, getThis());

    middleware_list* before_middlewares = 
        instance->common->before_middlewares_src;

    zend_object* target_class = php_class_init(class_src, class_len);
    zend_function* target_method = NULL;

    HashTable* middleware_table;
    ALLOC_HASHTABLE(middleware_table);
    zend_hash_init(middleware_table, 0, NULL, ZVAL_PTR_DTOR, 0);
    
    for( int i = 0; i < instance->common->before_middlewares_len; i++ )
    {
        zend_string* name = zend_string_init(
            instance->common->before_middlewares_src[i].middleware_name->val,
            instance->common->before_middlewares_src[i].middleware_name->len,
        0);
        zval zv_name; ZVAL_STR(&zv_name, name);

        zval* middleware = emalloc(sizeof(zval));
        array_init(middleware);

        add_next_index_zval(middleware, &zv_name);
        add_next_index_zval(middleware, instance->common->before_middlewares_src[i].properties);

        zend_hash_next_index_insert(middleware_table, middleware);
        efree(middleware);
    }

    // Iterate class method if exists
    ZEND_HASH_FOREACH_PTR(&target_class->ce->function_table, zend_function* value) 
    {
        int is_public = (value->common.fn_flags & ZEND_ACC_PUBLIC);
        int name_equalied = zend_string_equals_cstr(value->common.function_name, method_src, method_len);

        if( is_public && name_equalied )
        {
            target_method = value;
            break;
        }   
    } ZEND_HASH_FOREACH_END();

    if( target_method == NULL )
        php_error_docref(NULL, E_ERROR, "Call to undefined method %s::%s()", class_src, method_src);

    /* Class attributes */
    if( target_class->ce->attributes )
    {
        ZEND_HASH_FOREACH_PTR(target_class->ce->attributes, zend_attribute* class_attributes_value)
        {
            zval* middleware = emalloc(sizeof(zval));
            array_init(middleware);

            zval* argument = emalloc(sizeof(zval));
            array_init(argument);

            zval dest;

            if( zend_string_equals_cstr_bt_middleware(class_attributes_value->name) )
            {
                if(! class_attributes_value->argc )
                    php_error_docref(NULL, E_ERROR, "Middleware attribute must have atleast 1 argument");

                if(! zend_string_equals_cstr(class_attributes_value->args[0].name, "dest", sizeof("dest") - 1) )
                    php_error_docref(NULL, E_ERROR, "Middleware attribute first argument must be class string");

                if( Z_TYPE(class_attributes_value->args[0].value) != IS_STRING )
                    php_error_docref(NULL, E_ERROR, "Middleware attribute first argument must be class string");
 
                dest = class_attributes_value->args[0].value;

                // i = 1, for skip dest param
                for( int i = 1; i < class_attributes_value->argc; ++i )
                {
                    zval* copied = emalloc(sizeof(zval));
                    ZVAL_COPY(copied, &class_attributes_value->args[i].value);
                    zend_hash_update(Z_ARR_P(argument), class_attributes_value->args[i].name, copied);
                    efree(copied);
                }
            }

            if( Z_TYPE(dest) == IS_STRING )
            {
                // zend_string* middleware_dest = zval_get_string(&dest);
                // zval* zv_middleware_dest = emalloc(sizeof(zval));
                // ZVAL_STR(zv_middleware_dest, middleware_dest);
                // add_next_index_zval(middleware, zv_middleware_dest);
                // efree(zv_middleware_dest);

                add_next_index_zval(middleware, &dest);
                add_next_index_zval(middleware, argument);
                zend_hash_next_index_insert(middleware_table, middleware);
            } // TODO: [[else]] throw a error/exception

            efree(argument);
            efree(middleware);
        }
        ZEND_HASH_FOREACH_END();
    }

    /* Method attributes */
    if( target_method->common.attributes )
    {
        ZEND_HASH_FOREACH_PTR(target_method->common.attributes, zend_attribute *attribute)
        {
            zval* middleware = emalloc(sizeof(zval));
            array_init(middleware);

            zval* argument = emalloc(sizeof(zval));
            array_init(argument);

            zval dest;

            if( zend_string_equals_cstr_bt_middleware(attribute->name) )
            {
                if(! attribute->argc )
                    php_error_docref(NULL, E_ERROR, "Middleware attribute must have atleast 1 argument");

                if(! zend_string_equals_cstr(attribute->args[0].name, "dest", sizeof("dest") - 1) )
                    php_error_docref(NULL, E_ERROR, "Middleware attribute first argument must be class string");

                if( Z_TYPE(attribute->args[0].value) != IS_STRING )
                    php_error_docref(NULL, E_ERROR, "Middleware attribute first argument must be class string");
 
                dest = attribute->args[0].value;

                // i = 1, for skip dest param
                for( int i = 1; i < attribute->argc; ++i )
                {
                    zval* copied = emalloc(sizeof(zval));
                    ZVAL_COPY(copied, &attribute->args[i].value);
                    zend_hash_update(Z_ARR_P(argument), attribute->args[i].name, copied);
                    efree(copied);
                }
            }

            if( Z_TYPE(dest) == IS_STRING )
            {
                // zend_string* middleware_dest = zval_get_string(&dest);
                // zval* zv_middleware_dest = emalloc(sizeof(zval));
                // ZVAL_STR(zv_middleware_dest, middleware_dest);
                // add_next_index_zval(middleware, zv_middleware_dest);
                // efree(zv_middleware_dest);

                add_next_index_zval(middleware, &dest);
                add_next_index_zval(middleware, argument);
                zend_hash_next_index_insert(middleware_table, middleware);
            } // TODO: [[else]] throw a error/exception

            efree(argument);
            efree(middleware);
        } ZEND_HASH_FOREACH_END();
    }

    zend_object_release(target_class);

    RETURN_ARR(middleware_table);
}

PHP_METHOD(Middleware, __destruct)
{
    ZEND_PARSE_PARAMETERS_NONE();
}

ZEND_MINIT_FUNCTION(Zpheur_Actions_Middleware)
{
    zend_class_entry ce;

    INIT_NS_CLASS_ENTRY(ce, "Zpheur\\Actions", "Middleware", zpheur_actions_middleware_class_method);
    zpheur_actions_middleware_class_entry = zend_register_internal_class(&ce);
    zpheur_actions_middleware_class_entry->ce_flags |= ZEND_ACC_NO_DYNAMIC_PROPERTIES;
    zpheur_actions_middleware_class_entry->create_object = create_middleware_object;
    // zpheur_schemes_http_middleware_class_entry->parent = zpheur_actions_http_defaultaction_class_entry;

    // zend_declare_property_null(zpheur_actions_middleware_class_entry, "beforeMiddlewares", sizeof("beforeMiddlewares") - 1, ZEND_ACC_PUBLIC);
    // zend_declare_property_null(zpheur_actions_middleware_class_entry, "afterMiddlewares", sizeof("afterMiddlewares") - 1, ZEND_ACC_PUBLIC);
    // zend_declare_property_null(zpheur_actions_middleware_class_entry, "cachedMiddlewares", sizeof("cachedMiddlewares") - 1, ZEND_ACC_PUBLIC);

    return SUCCESS;
}