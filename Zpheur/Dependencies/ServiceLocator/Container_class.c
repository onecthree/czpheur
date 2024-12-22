#include <php.h>
#include <ext/standard/info.h>
#include <php_zpheur.h>

#include <string.h>
#include <zpheur.h>
#include <include/runtime.h>
#include <include/onecstr.h>
#include <Zpheur/Actions/Reflection/ArgumentResolver_arginfo.h>
#include "Container_arginfo.h"

zend_object_handlers container_object_handlers;

#define CONTAINER_T_CLASSES_MAX 64
#define CONTAINER_T_SCALARS_MAX 64

typedef struct _container_class_pack
{
    zend_object* object;
    onec_string* class_name;
} container_class_pack;

typedef struct _container_scalar_pack
{
    zval* value;
    onec_string* scalar_name;
} container_scalar_pack;

typedef struct _container_t
{
    container_class_pack classes_src[CONTAINER_T_CLASSES_MAX];
    size_t classes_len;
    container_scalar_pack scalars_src[CONTAINER_T_SCALARS_MAX];
    size_t scalars_len;
    zend_object* argument_resolver; // don't need release
} container_t;

typedef struct _container_object
{
    container_t* container;
    zend_object std;
} container_object;

void free_container_object_context( container_t* container )
{
    if( container )
    {
        for( int i = 0; i < container->classes_len; i++ ) {
            container_class_pack pack = container->classes_src[i];
            php_class_call_dtor(pack.object);
            zend_object_release(pack.object);
            onec_string_release(pack.class_name);
        }
        container->classes_len = 0;

        for( int i = 0; i < container->scalars_len; i++ ) {
            container_scalar_pack pack = container->scalars_src[i];

            if( Z_ISREF_P(pack.value) ) {
                zend_unwrap_reference(pack.value);
            }

            zend_uchar type = Z_TYPE_P(pack.value);

            if( (1 << type) & (BITW_IS_ARRAY) ) {
                zend_hash_destroy(Z_ARR_P(pack.value));
                FREE_HASHTABLE(Z_ARR_P(pack.value));
            }

            if( (1 << type) & (BITW_IS_STRING) ) {
                zend_string_release(Z_STR_P(pack.value));
            }

            efree(pack.value);
            onec_string_release(pack.scalar_name);
        }
        container->scalars_len = 0;

        efree(container);
    }
}

void free_container_object( zend_object* object )
{
    // php_printf("clean up from free container\n");
    container_object* instance = ZPHEUR_GET_OBJECT(container_object, object);
    zend_object_std_dtor(&instance->std);

    if( instance->container ) {
        for( int i = 0; i < instance->container->classes_len; i++ ) {
            container_class_pack pack = instance->container->classes_src[i];
            php_class_call_dtor(pack.object);
            zend_object_release(pack.object);
            onec_string_release(pack.class_name);
        }
        instance->container->classes_len = 0;

        for( int i = 0; i < instance->container->scalars_len; i++ ) {
            container_scalar_pack pack = instance->container->scalars_src[i];

            if( Z_ISREF_P(pack.value) )
                zend_unwrap_reference(pack.value);

            zend_uchar type = Z_TYPE_P(pack.value);

            if( (1 << type) & (BITW_IS_ARRAY) ) {
                zend_hash_destroy(Z_ARR_P(pack.value));
                FREE_HASHTABLE(Z_ARR_P(pack.value));
            }

            if( (1 << type) & (BITW_IS_STRING) ) {
                zend_string_release(Z_STR_P(pack.value));
            }

            efree(pack.value);
            onec_string_release(pack.scalar_name);
        }
        instance->container->scalars_len = 0;

        efree(instance->container);
    }
}

zend_object* create_container_object( zend_class_entry* ce )
{
    container_object* object = 
        ecalloc(1, sizeof(container_object) + zend_object_properties_size(ce));

    zend_object_std_init(&object->std, ce);
    object_properties_init(&object->std, ce);

    memcpy(&container_object_handlers, zend_get_std_object_handlers(), sizeof(container_object_handlers));
    container_object_handlers.offset = XtOffsetOf(container_object, std);
    container_object_handlers.free_obj = free_container_object;
    object->std.handlers = &container_object_handlers;

    object->container = emalloc(sizeof(container_t));
    object->container->argument_resolver = NULL;
    object->container->classes_len = 0;
    object->container->scalars_len = 0;

    return &object->std;
}

PHP_METHOD(Container, __construct)
{
    ZEND_PARSE_PARAMETERS_NONE();
}

PHP_METHOD(Container, setArgumentResolver)
{
    zval* object;

    ZEND_PARSE_PARAMETERS_START(1, 1)
        Z_PARAM_OBJECT_OF_CLASS(object, zpheur_actions_reflection_argumentresolver_class_entry)
    ZEND_PARSE_PARAMETERS_END();

    container_object* instance = ZPHEUR_ZVAL_GET_OBJECT(container_object, getThis());
    instance->container->argument_resolver = Z_OBJ_P(object);

    RETURN_ZVAL(getThis(), 1, 0);
}

bool __attribute__ ((noinline))
container_class_exists( container_t* container, char* name_src, size_t name_len )
{
    bool found = false;
    zend_string* service_name = zend_string_init(name_src, name_len, 0);
    for( int i = 0; i < container->classes_len; i++ )
    {
        container_class_pack pack = container->classes_src[i];
        if( zend_string_equals_cstr(service_name, pack.class_name->val, pack.class_name->len) )
        {
            found = true;
            goto found;
        }
    }

    found:
    zend_string_release(service_name);
    return found;
}

bool __attribute__ ((noinline))
container_scalar_exists( container_t* container, char* name_src, size_t name_len )
{
    bool found = false;
    zend_string* service_name = zend_string_init(name_src, name_len, 0);
    for( int i = 0; i < container->scalars_len; i++ )
    {
        container_scalar_pack pack = container->scalars_src[i];
        if( zend_string_equals_cstr(service_name, pack.scalar_name->val, pack.scalar_name->len) )
        {
            found = true;
            goto found;
        }
    }

    found:
    zend_string_release(service_name);
    return found;
}

zend_object* __attribute__ ((noinline))
container_class_find( container_t* container, zend_string* name )
{
    for( int i = 0; i < container->classes_len; i++ )
    {
        container_class_pack pack = container->classes_src[i];
        if( zend_string_equals_cstr(name, pack.class_name->val, pack.class_name->len) ||
            (pack.object->ce->parent && zend_string_equals(name, pack.object->ce->parent->name)) ) {

            if( pack.object->gc.refcount < 1 ) {
                pack.object->gc.refcount++;
            }
            return pack.object;
        }
    }

    return NULL;
}

zval* __attribute__ ((noinline))
container_scalar_find( container_t* container, zend_string* name )
{
    zval* retval = NULL;
    for( int i = 0; i < container->scalars_len; i++ )
    {
        container_scalar_pack pack = container->scalars_src[i];
        if( zend_string_equals_cstr(name, pack.scalar_name->val, pack.scalar_name->len) )
        {
            retval = pack.value;
        }
    }

    return retval;
}

void __attribute__ ((noinline))
container_class_add_service( container_t* container, char* name_src, size_t name_len, zval* service )
{
    zend_string* name = zend_string_init(name_src, name_len, 0);
    for( int i = 0; i < container->classes_len; i++ )
    {
        container_class_pack pack = container->classes_src[i];
        if( zend_string_equals_cstr(name, pack.class_name->val, pack.class_name->len) )
        {
            zend_object_release(pack.object);
            pack.object = NULL;
            pack.object = Z_OBJ_P(service);
            goto no_add;
        }
    }

    zval object; ZVAL_COPY(&object, service);
    onec_string* class_name = 
        onec_string_initd(name_src, name_len);

    container->classes_src[container->classes_len] = (container_class_pack)
    {
        .class_name = class_name,
        .object = Z_OBJ(object),
    };
    container->classes_len++;

    no_add:

    zend_string_release(name);
    return;
}


void __attribute__ ((noinline))
container_scalar_add_service( container_t* container, char* name_src, size_t name_len, zval* service )
{
    zend_string* name = zend_string_init(name_src, name_len, 0);
    for( int i = 0; i < container->scalars_len; i++ )
    {
        container_scalar_pack pack = container->scalars_src[i];
        if( zend_string_equals_cstr(name, pack.scalar_name->val, pack.scalar_name->len) )
        {
            if( Z_ISREF_P(pack.value) )
                zend_unwrap_reference(pack.value);

            if( (1 << Z_TYPE_P(pack.value)) & (BITW_IS_ARRAY) )
            {
                /**
                 * TODO: under issue
                 * Is having a issue when deep constructor happen,
                 * Looks alike ZendMM free'd by wrong offset, and
                 * corrupted or invalid free are happen
                 */
                zend_hash_destroy(Z_ARR_P(pack.value));
                FREE_HASHTABLE(Z_ARR_P(pack.value));
            }
            efree(pack.value);

            zval* new_ref_value = ecalloc(1, sizeof(zval));
            ZVAL_COPY(new_ref_value, service);
            ZVAL_MAKE_REF_EX(new_ref_value, 1);

            pack.value = new_ref_value;
            goto no_add;
        }
    }

    onec_string* scalar_name = 
        onec_string_initd(name_src, name_len);

    zval* new_ref_value = ecalloc(1, sizeof(zval));
    ZVAL_COPY(new_ref_value, service);
    ZVAL_MAKE_REF_EX(new_ref_value, 1);

    container->scalars_src[container->scalars_len] = (container_scalar_pack)
    {
        .scalar_name = scalar_name,
        .value = new_ref_value,
    };
    container->scalars_len++;

    /**
     * Should it free?
     */
    // switch( Z_TYPE_P(service) )
    // {
    //     case IS_STRING: zend_string_release(Z_STR_P(service)); break;
    // }
    // efree(service);

    no_add:

    zend_string_release(name);
    return;
}

PHP_METHOD(Container, getService)
{
    char* name_src = NULL;
    size_t name_len = 0;
    zval service;

    ZEND_PARSE_PARAMETERS_START(1, 1)
        Z_PARAM_STRING(name_src, name_len)
    ZEND_PARSE_PARAMETERS_END();

    container_object* instance = ZPHEUR_ZVAL_GET_OBJECT(container_object, getThis());
    zend_object* argument_resolver = instance->container->argument_resolver;

    zend_string* service_name = zend_string_init(name_src, name_len, 0);
    ZVAL_UNDEF(&service);

    zend_object* argument_class;
    zval* argument_scalar;

    if( zend_string_equals_cstr(service_name, CONTAINER_CLASS_NAME_SRC, CONTAINER_CLASS_NAME_LEN) )
    {   
        ZVAL_COPY(&service, getThis());
        goto end_check;
    }

    // If class exists 
    if( (argument_class = container_class_find(instance->container, service_name)) )
    {   
        ZVAL_OBJ(&service, argument_class);
        Z_ADDREF_P(&service); // Try to increment for long-live scope
        goto end_check;
    }

    // If scalar exists
    if( (argument_scalar = container_scalar_find(instance->container, service_name)) )
    {
        Z_ADDREF_P(argument_scalar);
        ZVAL_COPY(&service, argument_scalar);
        goto end_check;
    }

    zend_object* target_class =
        php_class_init_silent(service_name->val, service_name->len);

    if( target_class )
    {   
        zval* params_resolve = safe_emalloc(2, sizeof(zval), 0);
        zval zv_service_name; ZVAL_STR(&zv_service_name, service_name);
        params_resolve[0] = zv_service_name;
        zend_string* __construct = zend_string_init("__construct", sizeof("__construct") - 1, 0);
        zval zv___construct; ZVAL_STR(&zv___construct, __construct);
        params_resolve[1] = zv___construct;

        zval* params_of_resolve =
            php_class_call_method(argument_resolver,
                "resolve", sizeof("resolve") - 1,
                2, params_resolve, 0
            );

        efree(params_resolve);
        zend_string_release(__construct);

        if( Z_TYPE_P(params_of_resolve) == IS_NULL )
        {
            php_class_call_constructor(target_class, 0, NULL);
        }
        else
        {
            zend_long params_num = zend_hash_num_elements(Z_ARR_P(params_of_resolve));
            zval* params___construct = safe_emalloc(params_num, sizeof(zval), 0);
            ZEND_HASH_FOREACH_NUM_KEY_VAL(Z_ARR_P(params_of_resolve), zend_long index, zval* service)
            {
                params___construct[index] = *service;
            }
            ZEND_HASH_FOREACH_END();
            php_class_call_constructor(target_class, params_num, params___construct);

            zend_hash_destroy(Z_ARR_P(params_of_resolve));
            FREE_HASHTABLE(Z_ARR_P(params_of_resolve));
            efree(params___construct);
        }
        efree(params_of_resolve);

        zval* return_self =
            php_class_call_method(target_class, "servicePin", sizeof("servicePin") - 1, 0, NULL, 0);

        container_class_add_service(instance->container, service_name->val, service_name->len, return_self);
        // ZVAL_COPY(&service, return_self);
        service = *return_self;

        efree(return_self);
    }
    else // class are not exists
    {
        php_error_docref(NULL, E_ERROR, 
            "parameter ($%s) not found in container list or class does not exist", service_name->val
        );
    }

    zend_object_release(target_class);

    end_check:
    zend_string_release(service_name);

    RETURN_ZVAL(&service, 0, 0);
}

PHP_METHOD(Container, getServiceFromArray)
{
    HashTable* names;

    ZEND_PARSE_PARAMETERS_START(1, 1)
        Z_PARAM_ARRAY_HT(names)
    ZEND_PARSE_PARAMETERS_END();


    HashTable* arguments;
    // ALLOC_HASHTABLE(arguments);
    arguments = pemalloc(sizeof(HashTable), 0);
    zend_hash_init(arguments, 0 , NULL, ZVAL_PTR_DTOR, 0);

    container_object* instance = ZPHEUR_ZVAL_GET_OBJECT(container_object, getThis());
    zend_object* argument_resolver = instance->container->argument_resolver;

    ZEND_HASH_FOREACH_VAL(names, zval* value)
    {
        zend_string* service_name = zval_get_string(value);

        zend_object* argument_class;
        zval* argument_scalar;

        if( zend_string_equals_cstr(service_name, CONTAINER_CLASS_NAME_SRC, CONTAINER_CLASS_NAME_LEN) )
        {   
            zval copy;
            ZVAL_COPY(&copy, getThis());
            zend_hash_next_index_insert(arguments, &copy);
            goto end_check;
        }

        // If class exists 
        if( (argument_class = container_class_find(instance->container, service_name)) )
        {   
            zval arguments_ret;
            ZVAL_OBJ(&arguments_ret, argument_class);
            Z_ADDREF_P(&arguments_ret); // Try to increment for long-live scope
            zend_hash_next_index_insert(arguments, &arguments_ret);
            goto end_check;
        }

        // If scalar exists
        if( (argument_scalar = container_scalar_find(instance->container, service_name)) )
        {
            Z_ADDREF_P(argument_scalar);
            zend_hash_next_index_insert(arguments, argument_scalar);
            goto end_check;
        }

        zend_object* target_class =
            php_class_init_silent(service_name->val, service_name->len);

        if( target_class )
        {   
            zval* params_resolve = safe_emalloc(2, sizeof(zval), 0);
            zval zv_service_name; ZVAL_STR(&zv_service_name, service_name);
            params_resolve[0] = zv_service_name;
            zend_string* __construct = zend_string_init("__construct", sizeof("__construct") - 1, 0);
            zval zv___construct; ZVAL_STR(&zv___construct, __construct);
            params_resolve[1] = zv___construct;

            zval* params_of_resolve =
                php_class_call_method(argument_resolver,
                    "resolve", sizeof("resolve") - 1,
                    2, params_resolve, 0
                );

            efree(params_resolve);
            zend_string_release(__construct);

            if( Z_TYPE_P(params_of_resolve) == IS_NULL )
            {
                php_class_call_constructor(target_class, 0, NULL);
            }
            else
            {
                zend_long params_num = zend_hash_num_elements(Z_ARR_P(params_of_resolve));
                zval* params___construct = safe_emalloc(params_num, sizeof(zval), 0);
                ZEND_HASH_FOREACH_NUM_KEY_VAL(Z_ARR_P(params_of_resolve), zend_long index, zval* service)
                {
                    params___construct[index] = *service;
                }
                ZEND_HASH_FOREACH_END();
                php_class_call_constructor(target_class, params_num, params___construct);

                zend_hash_destroy(Z_ARR_P(params_of_resolve));
                FREE_HASHTABLE(Z_ARR_P(params_of_resolve));
                efree(params___construct);
            }
            efree(params_of_resolve);

            zval* return_self =
                php_class_call_method(target_class, "servicePin", sizeof("servicePin") - 1, 0, NULL, 0);

            container_class_add_service(instance->container, service_name->val, service_name->len, return_self);
            zend_hash_next_index_insert(arguments, return_self);

            efree(return_self);
        }
        else // class are not exists
        {
            // zval arguments_ret;
            // ZVAL_NULL(&arguments_ret);   
            // zend_hash_next_index_insert(arguments, &arguments_ret);
            php_error_docref(NULL, E_ERROR, 
                "parameter ($%s) not found in container list or class does not exist", service_name->val
            );
        }

        zend_object_release(target_class);

        end_check:
        zend_string_release(service_name);
    }
    ZEND_HASH_FOREACH_END();

    RETURN_ARR(arguments);
}

PHP_METHOD(Container, listAllServices)
{
    char* from_service_src = NULL;
    size_t from_service_len = 0;

    ZEND_PARSE_PARAMETERS_START(1, 1)
        Z_PARAM_STRING(from_service_src, from_service_len)
    ZEND_PARSE_PARAMETERS_END();

    container_object* instance = ZPHEUR_ZVAL_GET_OBJECT(container_object, getThis());

    HashTable* services;
    ALLOC_HASHTABLE(services);
    zend_hash_init(services, 0, NULL, ZVAL_PTR_DTOR, 0);

    if( strncmp("classes", from_service_src, sizeof("classes")-1) == 0 )
    {
        for( int i = 0; i < instance->container->classes_len; i++ )
        {
            container_class_pack pack = instance->container->classes_src[i];
            zval object; ZVAL_OBJ_COPY(&object, pack.object);
            zend_hash_str_update(services, pack.class_name->val, pack.class_name->len, &object);
        }

        goto skip_error;
    }

    if( strncmp("scalars", from_service_src, sizeof("scalars")-1) == 0 )
    {
        for( int i = 0; i < instance->container->scalars_len; i++ )
        {
            container_scalar_pack pack = instance->container->scalars_src[i];
            zval copy; ZVAL_COPY(&copy, pack.value);
            zend_hash_str_update(services, pack.scalar_name->val, pack.scalar_name->len, &copy);
        }

        goto skip_error;
    }

    php_error_docref(NULL, E_ERROR,
            "parameter ($fromService) must be \"classes\" or \"scalars\", \"%s\" given", from_service_src);

    skip_error:
    RETURN_ARR(services);
}

PHP_METHOD(Container, setClass)
{
    zval* service = NULL;

    ZEND_PARSE_PARAMETERS_START(1, 1);
        Z_PARAM_OBJECT(service)
    ZEND_PARSE_PARAMETERS_END();

    container_object* instance =
        ZPHEUR_ZVAL_GET_OBJECT(container_object, getThis());

    zend_object* object = Z_OBJ_P(service);
    zend_string* class_name = object->ce->name;

    container_class_add_service(instance->container,
        class_name->val, class_name->len, service);
}

PHP_METHOD(Container, setScalar)
{
    char* name_src = NULL;
    size_t name_len = 0;
    zval* value = NULL;

    ZEND_PARSE_PARAMETERS_START(2, 2);
        Z_PARAM_STRING(name_src, name_len)
        Z_PARAM_ZVAL(value)
    ZEND_PARSE_PARAMETERS_END();

    if( Z_TYPE_P(value) == IS_ARRAY )
    {
        if(! zend_hash_num_elements(Z_ARR_P(value)) )
        {        
            php_error_docref(NULL, E_ERROR,
                "Argument #2 ($value) cannot be empty array"
            );
        }

        // if( EXPECTED(HT_IS_PACKED(Z_ARR_P(value))) )
        // {
        //     php_error_docref(NULL, E_ERROR,
        //         "Argument #2 ($value) must be type array of key-value instead packed array"
        //     );
        // }

        // ZEND_HASH_FOREACH_STR_KEY_VAL(Z_ARR_P(value), zend_string* key, zval* _value )
        ZEND_HASH_FOREACH_KEY_VAL(Z_ARR_P(value), zend_long index, zend_string* key, zval* _value)
        {
            // if(! key )
            // {
            //     php_error_docref(NULL, E_ERROR,
            //         "Key of array from argument #2 ($value) must be type of string"
            //     );
            // }

            if( !( (1 << Z_TYPE_P(_value)) & 
                (BITW_IS_STRING | BITW_IS_LONG | BITW_IS_DOUBLE)) )
            {
                php_error_docref(NULL, E_ERROR,
                    "Array value from argument #2 ($value) must be type of string, integer, or double, %s given for key ('%s')",
                    ZTYPE_TO_STR(Z_TYPE_P(_value)), key->val
                );
            }
        }
        ZEND_HASH_FOREACH_END();
    } else
    if( !( (1 << Z_TYPE_P(value)) & 
        (BITW_IS_STRING | BITW_IS_LONG | BITW_IS_DOUBLE)) )
    {
        php_error_docref(NULL, E_ERROR,
            "Argument #2 ($value) must be type of string, integer, double, or array, %s given for key ($%s)",
            ZTYPE_TO_STR(Z_TYPE_P(value)), name_src
        );
    }

    container_object* instance =
        ZPHEUR_ZVAL_GET_OBJECT(container_object, getThis());

    container_scalar_add_service(instance->container, name_src, name_len, value);
}

PHP_METHOD(Container, setClassFromArray)
{
    HashTable* services;

    ZEND_PARSE_PARAMETERS_START(1, 1)
        Z_PARAM_ARRAY_HT(services)
    ZEND_PARSE_PARAMETERS_END();

    if(! zend_hash_num_elements(services) )
    {
        php_error_docref(NULL, E_ERROR,
            "Argument #1 ($services) can not be empty array"
        );
    }

    if(! EXPECTED(HT_IS_PACKED(services)) )
    {
        php_error_docref(NULL, E_ERROR,
            "Argument #1 ($services) must be packed array instead array of key-value"
        );
    }

    container_object* instance =
        ZPHEUR_ZVAL_GET_OBJECT(container_object, getThis());

    ZEND_HASH_FOREACH_VAL(services, zval* service)
    {
        if( Z_TYPE_P(service) != IS_OBJECT )
        {
            php_error_docref(NULL, E_ERROR,
                "Array value of argument #1 ($services) must be type of object, %s given",
                ZTYPE_TO_STR(Z_TYPE_P(service))
            );
        }

        zend_object* object = Z_OBJ_P(service);
        zend_string* class_name = object->ce->name;

        container_class_add_service(instance->container,
            class_name->val, class_name->len, service);
    }
    ZEND_HASH_FOREACH_END();
}

PHP_METHOD(Container, setScalarFromArray)
{
    HashTable* values;

    ZEND_PARSE_PARAMETERS_START(1, 1)
        Z_PARAM_ARRAY_HT(values)
    ZEND_PARSE_PARAMETERS_END();

    if(! zend_hash_num_elements(values) )
    {
        php_error_docref(NULL, E_ERROR,
            "Argument #1 ($values) can not be empty array"
        );
    }

    if( EXPECTED(HT_IS_PACKED(values)) )
    {
        php_error_docref(NULL, E_ERROR,
            "Argument #1 ($values) must be array of key-value instead packed array"
        );
    }

    ZEND_HASH_FOREACH_STR_KEY_VAL(values, zend_string* key, zval* value)
    {
        if(! key )
        {
            php_error_docref(NULL, E_ERROR,
                "Key of array from argument #1 ($values) must be type of string"
            );
        }

        if( Z_TYPE_P(value) == IS_ARRAY )
        {
            php_error_docref(NULL, E_ERROR,
                "Array value of argument #1 ($values) from key ($%s) is array, use Zpheur\\Dependencies\\ServiceLocator\\Container::setScalar() instead",
                key->val
            );
        } else
        if( !( (1 << Z_TYPE_P(value)) & 
            (BITW_IS_STRING | BITW_IS_LONG | BITW_IS_DOUBLE)) )
        {
            php_error_docref(NULL, E_ERROR,
                "Array value of argument #1 ($values) must be type of string, integer, double, or array, %s given for key ($%s)",
                ZTYPE_TO_STR(Z_TYPE_P(value)), key->val
            );
        }

        container_object* instance =
            ZPHEUR_ZVAL_GET_OBJECT(container_object, getThis());

        container_scalar_add_service(instance->container, key->val, key->len, value);
    }
    ZEND_HASH_FOREACH_END();
}

PHP_METHOD(Container, hasService)
{
    char*   name_src = NULL;
    size_t  name_len = 0;
    zval* value;

    ZEND_PARSE_PARAMETERS_START(1, 1)
        Z_PARAM_STRING(name_src, name_len)
    ZEND_PARSE_PARAMETERS_END();

    container_object* instance = ZPHEUR_ZVAL_GET_OBJECT(container_object, getThis());

    if( container_class_exists(instance->container, name_src, name_len) )
        RETURN_TRUE;

    if( container_scalar_exists(instance->container, name_src, name_len) )
        RETURN_TRUE;

    RETURN_FALSE;
}

PHP_METHOD(Container, clean)
{
    ZEND_PARSE_PARAMETERS_NONE();
    container_object* instance = ZPHEUR_ZVAL_GET_OBJECT(container_object, getThis());
    free_container_object_context(instance->container);
}

ZEND_MINIT_FUNCTION(Zpheur_Dependencies_ServiceLocator_Container)
{
    zend_class_entry ce;

    INIT_NS_CLASS_ENTRY(ce, "Zpheur\\Dependencies\\ServiceLocator", "Container", zpheur_dependencies_servicelocator_container_class_method);
    zpheur_dependencies_servicelocator_container_class_entry = zend_register_internal_class(&ce);
    zpheur_dependencies_servicelocator_container_class_entry->ce_flags |= ZEND_ACC_NO_DYNAMIC_PROPERTIES;
    zpheur_dependencies_servicelocator_container_class_entry->create_object = create_container_object;

    return SUCCESS;
}