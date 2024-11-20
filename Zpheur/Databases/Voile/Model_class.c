#include <php.h>
#include <ext/standard/info.h>
#include <php_zpheur.h>
#include <Zend/zend_attributes.h>
#include <ext/spl/spl_iterators.h>
#include <Zend/zend_interfaces.h>

#include <string.h>
#include <zpheur.h>
#include <include/onecstr.h>
#include <include/runtime.h>
#include <include/regexp.h>
#include "Model_arginfo.h"


#define INP_CONSTANT_AOBJECTID              7399721430
#define INP_CONSTANT_ASTRING                74585703
#define INP_CONSTANT_ANUMBER                74089924
#define INP_CONSTANT_ABOOLEAN               728329180
#define INP_CONSTANT_ADATETIME              7289705691
#define INP_CONSTANT_AARRAY                 7276491
#define INP_CONSTANT_AOBJECT                73997206
#define INP_CONSTANT_ADECIMAL128            729206983456

#define INP_CONSTANT_RIGHT_OBJECTID         899721430
#define INP_CONSTANT_RIGHT_STRING           12785703
#define INP_CONSTANT_RIGHT_INT              11716
#define INP_CONSTANT_RIGHT_BOOL             110318
#define INP_CONSTANT_RIGHT_UTCDATETIME      941489705691
#define INP_CONSTANT_RIGHT_ARRAY            1096491
#define INP_CONSTANT_RIGHT_STDCLASS         1276788965
#define INP_CONSTANT_RIGHT_DECIMAL128       79206983456

#define INP_TOKEN_SYMBOL_BACKSLASH          '\\'
#define INP_TOKEN_CHAR_EOF                  '\0'
#define INP_TOKEN_SYMBOL_PIPE               '|'

static inline zend_object* __attribute__ ((always_inline))
voile_model_constructor_init( zend_class_entry* target_model_ce )
{
    char*        model_classname_src = target_model_ce->name->val;
    size_t       model_classname_len = target_model_ce->name->len;
    zend_object* model_class = php_class_init(model_classname_src, model_classname_len);

    return model_class;
}

static inline zend_ulong __attribute__ ((always_inline))
field_type_finder( zend_string* type_str_field_zs, zend_property_info* zpi_property, char* property_name, zend_object* model_class )
{
    zend_ulong      target_field_type = 99;
    unsigned long   input_constant = 0;
    unsigned long   left = 0;
    unsigned long   right = 0;
    char            input;

    for( int i = 0; i <= type_str_field_zs->len; i += 1 )
    {
        input = type_str_field_zs->val[i];

        switch( input )
        {
            case INP_TOKEN_SYMBOL_BACKSLASH:
                input_constant = 0;
                continue;
            break;
            case INP_TOKEN_CHAR_EOF:
            case INP_TOKEN_SYMBOL_PIPE:
                if( left )
                {
                    switch( left )
                    {
                        case INP_CONSTANT_AOBJECTID:
                            if( input_constant == INP_CONSTANT_RIGHT_OBJECTID )
                            {
                                target_field_type = 0;
                                goto __typ_skip_error;
                            }
                        break;
                        case INP_CONSTANT_ASTRING:
                            if( input_constant == INP_CONSTANT_RIGHT_STRING )
                            {
                                target_field_type = 1;
                                goto __typ_skip_error;
                            }
                        break;
                        case INP_CONSTANT_ANUMBER:
                            if( input_constant == INP_CONSTANT_RIGHT_INT )
                            {
                                target_field_type = 2;
                                goto __typ_skip_error;
                            }
                        break;
                        case INP_CONSTANT_ABOOLEAN:
                            if( input_constant == INP_CONSTANT_RIGHT_BOOL )
                            {
                                target_field_type = 3;
                                goto __typ_skip_error;
                            }
                        break;
                        case INP_CONSTANT_ADATETIME:
                            if( input_constant == INP_CONSTANT_RIGHT_UTCDATETIME )
                            {
                                target_field_type = 4;
                                goto __typ_skip_error;
                            }
                        break;
                        case INP_CONSTANT_AARRAY:
                            if( input_constant == INP_CONSTANT_RIGHT_ARRAY )
                            {
                                target_field_type = 5;
                                goto __typ_skip_error;
                            }
                        break;
                        case INP_CONSTANT_AOBJECT:
                            if( input_constant == INP_CONSTANT_RIGHT_STDCLASS )
                            {
                                target_field_type = 6;
                                goto __typ_skip_error;
                            }
                        break;
                        case INP_CONSTANT_ADECIMAL128:
                            if( input_constant == INP_CONSTANT_RIGHT_DECIMAL128 )
                            {
                                target_field_type = 7;
                                goto __typ_skip_error;
                            }
                        break;
                    }

                    input_constant = 0;
                    continue;
                }

                left = input_constant;
                input_constant = 0;
                continue;
            break;
        }

        if( input == INP_TOKEN_CHAR_EOF )
            break;

        input_constant *= 10;
        input_constant += input;

        if( input_constant > INP_CONSTANT_RIGHT_UTCDATETIME )
            input_constant = 0;
    }

    __typ_error_section:
    {

        onec_string* error_message;
        onec_string_init(error_message);

        #define VOILE_NS    "Zpheur\\Databases\\Voile\\Schema\\AbstractionType"
        #define MONGOD_NS   "MongoDB\\BSON"

        switch( left )
        {
            case INP_CONSTANT_AOBJECTID:
                onec_string_append(error_message, 1, VOILE_NS"\\AObjectId" " and " MONGOD_NS"\\ObjectId");
            break;
            case INP_CONSTANT_ASTRING:
                onec_string_append(error_message, 1, VOILE_NS"\\AString" " and " "string");
            break;
            case INP_CONSTANT_ANUMBER:
                onec_string_append(error_message, 1, VOILE_NS"\\ANumber" " and " "int");
            break;
            case INP_CONSTANT_ABOOLEAN:
                onec_string_append(error_message, 1, VOILE_NS"\\ABoolean" " and " "bool");
            break;
            case INP_CONSTANT_ADATETIME:
                onec_string_append(error_message, 1, VOILE_NS"\\ADateTime" " and " MONGOD_NS"\\UTCDateTime");
            break;
            case INP_CONSTANT_AARRAY:
                onec_string_append(error_message, 1, VOILE_NS"\\AArray" " and " "array");
            break;
            case INP_CONSTANT_AOBJECT:
                onec_string_append(error_message, 1, VOILE_NS"\\AObject" " and " "stdClass");
            break;
            case INP_CONSTANT_ADECIMAL128:
                onec_string_append(error_message, 1, VOILE_NS"\\ADecimal128" " and " MONGOD_NS"\\Decimal128");
            break;
        }


        zend_string* error_message_zs = zend_string_init(error_message->val, error_message->len, 0);
        onec_string_release(error_message);

        zend_error_at(
            E_ERROR,
            zpi_property->ce->info.user.filename,
            zpi_property->ce->info.user.line_start,
            "%s::$%s requires a pair-type for %s",
            model_class->ce->name->val,
            property_name,
            error_message_zs->val
        );         

        return 99;
    }

    __typ_skip_error:
    return target_field_type;
}

#define FIELD_NAMESPACE "Zpheur\\Databases\\Voile\\Schema\\Field"
static inline voile_model_typeinfo* __attribute__ ((always_inline))
voile_parse_model_typeinfo( zend_object* model_class )
{
    HashTable *typelist;
    ALLOC_HASHTABLE(typelist); // HtStack for type list per field/property
    zend_hash_init(typelist, 0, NULL, ZVAL_PTR_DTOR, 0);

    HashTable* typepair;
    ALLOC_HASHTABLE(typepair); // HtStack for field container (AObjectId, AString, etc)
    zend_hash_init(typepair, 0, NULL, ZVAL_PTR_DTOR, 0);
        
    HashTable* attributes_argument;       
    ALLOC_HASHTABLE(attributes_argument);     
    zend_hash_init(attributes_argument, 0, NULL, ZVAL_PTR_DTOR, 0);

    /**
     *  All property must be protected modifier
     */
    zend_ulong property_counts = model_class->ce->default_properties_count;
    ZEND_HASH_FOREACH_PTR(&model_class->ce->properties_info, zend_property_info* zpi_property )
    {
        /* Skip first property (voile) */
        if( property_counts < 2 )
            continue;

        property_counts -= 1;

        // Update for offset begin
        char*   property_name_src = zpi_property->name->val + 3;
        size_t  property_name_len = zpi_property->name->len - 3;

        zend_string* public_name = zend_string_init(property_name_src, property_name_len, 0);

        /* Iterate each attribute as key-value pair arguments */
        if( zpi_property->attributes == NULL )
        {   
            zend_error_at(
                E_ERROR,
                zpi_property->ce->info.user.filename,
                zpi_property->ce->info.user.line_start,
                "%s::$%s require attribute \"%s\"",
                model_class->ce->name->val, property_name_src, FIELD_NAMESPACE
            );
        }

        zval zv_typelist;
        zval zv_field_type;

        zend_string* type_str_field_zs = zend_type_to_string(zpi_property->type);

        ZVAL_STRINGL(&zv_typelist, type_str_field_zs->val, type_str_field_zs->len);
        zend_hash_str_update_ind(typelist, property_name_src, property_name_len, &zv_typelist);

        /* Set to zv_field_type */
        zend_ulong field_type =
            field_type_finder(type_str_field_zs, zpi_property, property_name_src, model_class);
        
        ZVAL_LONG(&zv_field_type, field_type);

        zend_hash_str_update_ind(typepair, property_name_src, property_name_len, &zv_field_type);

        bool    is_attribute_exists = false;
        size_t  lineno            = 1;
        
        /* Iterate each attribute as key-value pair arguments */
        ZEND_HASH_FOREACH_PTR(zpi_property->attributes, zend_attribute* attribute_context)
        {
            if( zend_string_equals_cstr(attribute_context->name, FIELD_NAMESPACE, sizeof(FIELD_NAMESPACE) - 1) )
            {
                is_attribute_exists = true;

                if(! attribute_context->argc )
                {
                    zend_error_at(
                        E_ERROR, zpi_property->ce->info.user.filename, lineno,
                        "%s::$%s requires at least 1 argument named \"default\" for \"Field\" attribute",
                        model_class->ce->name->val, property_name_src
                    );
                }

                bool is_default_param_exists = false;

                for( int i = 0; i < attribute_context->argc; i += 1 )
                {

                    int             arg_len = attribute_context->args[i].name->len;
                    unsigned long   arg_sum = 0;

                    for( int j = 0; j < arg_len; ++j )
                    {
                        arg_sum = (arg_sum * 10) + attribute_context->args[i].name->val[j];

                        /* Is sum are more than limit, break it */
                        if( arg_sum > ATTR_NAMED_PARAM__REQUIRED )
                            break;
                    }

                    switch( arg_sum )
                    {
                        case ATTR_NAMED_PARAM__DEFAULT: // default
                            is_default_param_exists = true;
                        break;
                        case ATTR_NAMED_PARAM__REQUIRED: // required
                            // TODO:
                        break;
                        case ATTR_NAMED_PARAM__MIN:
                            // TODO:
                        break;
                        case ATTR_NAMED_PARAM__MAX:
                            // TODO:
                        break;
                        default:
                            zend_error_at(
                                E_ERROR, zpi_property->ce->info.user.filename, lineno,
                                "%s::$%s have uknown argument named '%s' at attribute \"%s\"",
                                model_class->ce->name->val, property_name_src,
                                attribute_context->args[i].name->val, FIELD_NAMESPACE
                            );
                        break;
                    }
                } // Per-argument attribute

                if(! is_default_param_exists )
                {
                    zend_error_at(
                        E_ERROR, zpi_property->ce->info.user.filename, lineno,
                        "%s::$%s require at least 1 argument named \"default\" for attribute \"%s\"",
                        model_class->ce->name->val, property_name_src, FIELD_NAMESPACE
                    );
                }
                
                HashTable* attribute_arguments;
                ALLOC_HASHTABLE(attribute_arguments);
                zend_hash_init(attribute_arguments, 0, NULL, ZVAL_PTR_DTOR, 0);

                /* Iterate each attribute arguments as key-value pair */
                for( int i = 0; i < attribute_context->argc; ++i )
                {
                    zend_hash_update_ind(
                        attribute_arguments,
                        attribute_context->args[i].name,
                        &attribute_context->args[i].value
                    );
                }

                /* Property set by name -> value */
                zval target_attribute_arguments;
                ZVAL_ARR(&target_attribute_arguments, attribute_arguments);

                zend_hash_str_update_ind(attributes_argument, property_name_src, property_name_len, &target_attribute_arguments);
            }
        }
        ZEND_HASH_FOREACH_END(); // Per-attribute

        if(! is_attribute_exists )
        {
            zend_error_at(
                E_ERROR, zpi_property->ce->info.user.filename, lineno,
                "%s::$%s require attribute function \"%s\"",
                model_class->ce->name->val, zpi_property->name->val, FIELD_NAMESPACE
            );
        }

    } ZEND_HASH_FOREACH_END(); // Per-property

    zval target_typelist;
    ZVAL_ARR(&target_typelist, typelist);

    zval target_typepair;
    ZVAL_ARR(&target_typepair, typepair);

    zval target_attributes;
    ZVAL_ARR(&target_attributes, attributes_argument);

    voile_model_typeinfo* model_info    = (voile_model_typeinfo*)emalloc(sizeof(voile_model_typeinfo));
    model_info->typelist_field          = target_typelist;
    model_info->typepair_field          = target_typepair;
    model_info->attribute_field         = target_attributes;

    return model_info;
}


PHP_METHOD(Model, __construct)
{
    char*   db_uri_src = NULL;
    size_t  db_uri_len = 0;

    char*   db_name_src = NULL;
    size_t  db_name_len = 0;

    ZEND_PARSE_PARAMETERS_START(2, 2)
        Z_PARAM_STRING(db_uri_src, db_uri_len)
        Z_PARAM_STRING(db_name_src, db_name_len)
    ZEND_PARSE_PARAMETERS_END();

    zend_object* model_class =
        voile_model_constructor_init(zend_get_called_scope(execute_data));

    voile*              voile_ptr = emalloc(sizeof(voile));     
    HashTable*          ht_cm_attributes = model_class->ce->attributes;

    zval model_classname;
    ZVAL_STRINGL(&model_classname, model_class->ce->name->val, model_class->ce->name->len);

    /* Fetch model attributes */
    zval*               class_attributes = model_class->ce->attributes->arPacked;
    zend_attribute*     class_attributes_value = class_attributes->value.ptr;

    /* Iterate all attribute scope, and check class specific for \Collection */
    for( int index = 0; index < class_attributes_value->argc; ++index )
        #define COLLECTION_NAMESPACE "Zpheur\\Databases\\Voile\\Schema\\Collection"
        if( zend_string_equals_cstr(class_attributes_value[index].name, COLLECTION_NAMESPACE, sizeof(COLLECTION_NAMESPACE) - 1) )
            voile_ptr->collection = class_attributes_value[index].args[0].value;

    zval empty_filter;
    object_init_ex(&empty_filter, zend_standard_class_def);

    zval empty_cursor;
    object_init_ex(&empty_cursor, zend_standard_class_def);

    zval empty_projection;
    ZVAL_EMPTY_ARRAY(&empty_projection);

    zval empty_group;
    ZVAL_EMPTY_ARRAY(&empty_group);

    HashTable *array_document;
    ALLOC_HASHTABLE(array_document);
    zend_hash_init(array_document, 0, NULL, ZVAL_PTR_DTOR, 0);

    HashTable *array_update_doc;
    ALLOC_HASHTABLE(array_update_doc);
    zend_hash_init(array_update_doc, 0, NULL, ZVAL_PTR_DTOR, 0);

    HashTable *array_unset_doc;
    ALLOC_HASHTABLE(array_unset_doc);
    zend_hash_init(array_unset_doc, 0, NULL, ZVAL_PTR_DTOR, 0);

    HashTable *array_sort;
    ALLOC_HASHTABLE(array_sort);
    zend_hash_init(array_sort, 0, NULL, ZVAL_PTR_DTOR, 0);

    HashTable *array_option;
    ALLOC_HASHTABLE(array_option);
    zend_hash_init(array_option, 0, NULL, ZVAL_PTR_DTOR, 0);

    zval db_uri;
    zval db_name;

    ZVAL_STRINGL(&db_uri, db_uri_src, db_uri_len);
    ZVAL_STRINGL(&db_name, db_name_src, db_name_len);

    zval manager;

    voile_ptr->model_classname     = model_classname;
    voile_ptr->model_class         = *model_class;
    voile_ptr->filter              = empty_filter;
    voile_ptr->cursor              = empty_cursor;
    voile_ptr->projection          = empty_projection;
    voile_ptr->group               = empty_group;
    voile_ptr->limit               = 0;
    voile_ptr->skip                = 0;
    voile_ptr->modelinfo           = *voile_parse_model_typeinfo(model_class);
    voile_ptr->document            = *array_document;
    voile_ptr->update_doc          = *array_update_doc;
    voile_ptr->unset_doc           = *array_unset_doc;
    voile_ptr->sort                = *array_sort;
    voile_ptr->option              = *array_option;
    voile_ptr->db_uri              = db_uri;
    voile_ptr->db_name             = db_name;
    voile_ptr->count               = false;
    voile_ptr->get_called          = false;
    voile_ptr->filtered            = false;

    zend_object *manager_class = php_class_init("MongoDB\\Driver\\Manager", sizeof("MongoDB\\Driver\\Manager") - 1);

    zval *params___construct = safe_emalloc(1, sizeof(zval), 0);
    ZVAL_ZVAL(&params___construct[0], &voile_ptr->db_uri, 1, 0);
    php_class_call_method(manager_class, "__construct", sizeof("__construct") - 1, 1, params___construct, false);
    ZVAL_OBJ(&manager, manager_class);

    voile_ptr->manager     = manager;

    zval target_voile_ptr;
    ZVAL_PTR(&target_voile_ptr, (void*)voile_ptr);

    zend_this_update_property("voile", &target_voile_ptr);
}

static inline void __setter_type_error( zval *model_classname, char *name, zval *zv_value, char const *type_reserved )
{
    char const *act_type = zend_get_type_by_const(Z_TYPE_P(zv_value));

    zend_throw_error(zend_ce_exception,
    "Cannot assign type %s to property %s::$%s of type %s", act_type, Z_STRVAL_P(model_classname), name, type_reserved);
}

PHP_METHOD(Model, __get)
{
    char*      field_src = NULL;
    size_t     field_len = 0;

    ZEND_PARSE_PARAMETERS_START(1, 1)
        Z_PARAM_STRING(field_src, field_len)
    ZEND_PARSE_PARAMETERS_END();


    voile*                  voile_ptr           = ((voile*)zend_this_read_property("voile")->value.ptr);
    voile_model_typeinfo*   modelinfo           = &voile_ptr->modelinfo;
    HashTable* ht_property_field = Z_ARR(modelinfo->typepair_field);

    if(! (((voile*)zend_this_read_property("voile")->value.ptr)->get_called) )
    {
        php_error_docref(NULL, E_ERROR, "Model must be called to database");
        RETURN_NULL();
    }

    if( field_len == 2 && strncmp("id", field_src, 2) == 0 )
    {
        field_src = (char*)"_id";
        field_len = 3;
    }

    HashTable*  properties = zend_std_get_properties(Z_OBJ_P(getThis()));
    bool        is_field_exists = zend_hash_str_exists_ind(ht_property_field, field_src, field_len);

    if( is_field_exists )
    {
        zval* returned_field =
            zend_read_property(Z_OBJCE_P(getThis()), Z_OBJ_P(getThis()), field_src, field_len, 0, NULL);

        RETURN_ZVAL(returned_field, 1, 0);
    }   

    RETURN_NULL();
}

void _set_internal( char* name_src, size_t name_len, zval* zv_value, zval* scope )
{
    voile*                  voile_ptr               = ((voile*)zend_read_property(Z_OBJCE_P(scope), Z_OBJ_P(scope), "voile", sizeof("voile") - 1, 0, NULL)->value.ptr);
    voile_model_typeinfo*   modelinfo               = &voile_ptr->modelinfo;
    HashTable*              ht_document             = &voile_ptr->document;
    zval*                   model_classname         = &voile_ptr->model_classname;
    HashTable*              property_fields_type    = Z_ARR(modelinfo->typepair_field);


    if(! zend_hash_str_exists_ind(property_fields_type, name_src, name_len) )
    {
        zend_throw_error(zend_ce_error_exception,
                "Assigning to undefined property: %s::$%s", Z_STRVAL_P(model_classname), name_src);

        return;
    }

    zval* property_field_type =
        zend_hash_find_ind(property_fields_type, zend_string_init(name_src, name_len, 0));

    switch( zval_get_long(property_field_type) )
    {
        case 0: // AObjectId
            switch( Z_TYPE_P(zv_value) )
            {
                case IS_STRING:
                {
                    zend_object* objectid = php_class_init("MongoDB\\BSON\\ObjectId", sizeof("MongoDB\\BSON\\ObjectId") - 1);

                    zval* params___construct = safe_emalloc(1, sizeof(zval), 0);
                    ZVAL_ZVAL(&params___construct[0], zv_value, 1, 0);

                    php_class_call_constructor(objectid, 1, params___construct);

                    ZVAL_OBJ(zv_value, objectid);
                }
                break;
                case IS_OBJECT:
                    // check if ojid
                    if( zend_string_equals_cstr(Z_OBJ_P(zv_value)->ce->name, "MongoDB\\BSON\\ObjectId", sizeof("MongoDB\\BSON\\ObjectId") - 1) )
                    {
                        break;
                    }
                default: 
                    __setter_type_error(model_classname, name_src, zv_value, "Type\\AObjectId");
                break;
            }
        break;
        case 1: // AString
            if( Z_TYPE_P(zv_value) != IS_STRING )
                __setter_type_error(model_classname, name_src, zv_value, "Type\\AString");
        break;
        case 2: // ANumber
            if( Z_TYPE_P(zv_value) != IS_LONG )
                __setter_type_error(model_classname, name_src, zv_value, "Type\\ANumber");
        break;
        case 3: // ABoolean
            if( Z_TYPE_P(zv_value) != IS_TRUE && Z_TYPE_P(zv_value) != IS_FALSE )
                __setter_type_error(model_classname, name_src, zv_value, "Type\\ABoolean");
        break;
        case 4: // ADateTime
        {
            switch( Z_TYPE_P(zv_value) )
            {
                case IS_OBJECT:
                {
                    // check if ojid
                    if( zend_string_equals_cstr(Z_OBJ_P(zv_value)->ce->name, "DateTime", sizeof("DateTime") - 1) )
                    {
                        zend_object* utcdatetime = php_class_init("MongoDB\\BSON\\UTCDateTime", sizeof("MongoDB\\BSON\\UTCDateTime") - 1);

                        zval *params___construct = safe_emalloc(1, sizeof(zval), 0);
                        ZVAL_ZVAL(&params___construct[0], zv_value, 1, 0);

                        php_class_call_constructor(utcdatetime, 1, params___construct);

                        ZVAL_OBJ(zv_value, utcdatetime);

                        break;
                    }
                }
                default: 
                    __setter_type_error(model_classname, name_src, zv_value, "Type\\ADateTime");
                break;
            }
        }
        break;
        case 5: // AArray
            if( Z_TYPE_P(zv_value) != IS_ARRAY)
                __setter_type_error(model_classname, name_src, zv_value, "Type\\AArray");
        break;
        case 6: // AObject
            if( Z_TYPE_P(zv_value) != IS_OBJECT)
                __setter_type_error(model_classname, name_src, zv_value, "Type\\AObject");
        break;
        case 7: // ADecimal128
        {
            switch( Z_TYPE_P(zv_value) )
            {
                case IS_STRING:
                case IS_DOUBLE:
                case IS_LONG:
                {
                    zend_object* decimal128 = php_class_init("MongoDB\\BSON\\Decimal128", sizeof("MongoDB\\BSON\\Decimal128") - 1);

                    zval* params___construct = safe_emalloc(1, sizeof(zval), 0);
                    ZVAL_ZVAL(&params___construct[0], zv_value, 1, 0);

                    php_class_call_constructor(decimal128, 1, params___construct);

                    ZVAL_OBJ(zv_value, decimal128);
                }
                break;
                default: 
                    __setter_type_error(model_classname, name_src, zv_value, "Type\\ADecimal128");
                break;
            }
        }
        break;
    }

    if(! EG(exception) )
    { 
        zval target_value;
        ZVAL_DUP(&target_value, zv_value);

        zend_hash_update(ht_document, zend_string_init(name_src, name_len, 0), &target_value);
    }
}

PHP_METHOD(Model, __set)
{
    char*       name_src = NULL;
    size_t      name_len = 0;

    zval *zv_value;

    ZEND_PARSE_PARAMETERS_START(2, 2)
        Z_PARAM_STRING(name_src, name_len)
        Z_PARAM_ZVAL(zv_value)
    ZEND_PARSE_PARAMETERS_END();

    _set_internal(name_src, name_len, zv_value, getThis());
}

PHP_METHOD(Model, unset)
{
    zval*   names = NULL;

    ZEND_PARSE_PARAMETERS_START(1, 1)
        Z_PARAM_ZVAL(names)
    ZEND_PARSE_PARAMETERS_END();

    HashTable* update_doc = &((voile*)zend_this_read_property("voile")->value.ptr)->update_doc;
    zval* array_inc = zend_hash_str_find(update_doc, "$unset", sizeof("$unset") - 1);

    if(! array_inc )
    {
        HashTable* ht_array_inc;
        ALLOC_HASHTABLE(ht_array_inc);
        zend_hash_init(ht_array_inc, 0, NULL, ZVAL_PTR_DTOR, 0);

        zval _array_inc;
        ZVAL_ARR(&_array_inc, ht_array_inc);

        zend_hash_str_update(update_doc, "$unset", sizeof("$unset") - 1, &_array_inc);

        array_inc = &_array_inc;
    }

    if( Z_TYPE_P(names) == IS_STRING )
    {
        zval empty_string;
        ZVAL_STRINGL(&empty_string, "", sizeof("") - 1);
        zend_hash_update(Z_ARR_P(array_inc), zval_get_string(names), &empty_string);
    } else
    if( Z_TYPE_P(names) == IS_ARRAY )
    {
        ZEND_HASH_FOREACH_VAL(Z_ARR_P(names), zval* name)
        {
            if( !name || Z_TYPE_P(name) != IS_STRING )
            {
                zend_error(E_ERROR, "argument 1 is only support string and array of string");
            }

            zval empty_string;
            ZVAL_STRINGL(&empty_string, "", sizeof("") - 1);
            zend_hash_update(Z_ARR_P(array_inc), zval_get_string(name), &empty_string);
        }
        ZEND_HASH_FOREACH_END();
    }
    else
    {
        zend_error(E_ERROR, "argument 1 is only support string and array of string");
    }
}

/* Public non-static method */
PHP_METHOD(Model, delete)
{
    bool bulk_write_detail = false;

    ZEND_PARSE_PARAMETERS_START(0, 1)
        Z_PARAM_OPTIONAL
        Z_PARAM_BOOL(bulk_write_detail)
    ZEND_PARSE_PARAMETERS_END();

    voile*      voile_ptr   = ((voile*)zend_this_read_property("voile")->value.ptr);
    zval*       manager      = &voile_ptr->manager;
    zval*       db_name      = &voile_ptr->db_name;
    zval*       filter       = &voile_ptr->filter;
    zval*       collection   = &voile_ptr->collection;
    HashTable*  option       = &voile_ptr->option;

    zend_object *bulk_write = php_class_init("MongoDB\\Driver\\BulkWrite", sizeof("MongoDB\\Driver\\BulkWrite") - 1);

    php_class_call_constructor(bulk_write, 0, NULL);

    zval* params_delete = safe_emalloc(1, sizeof(zval), 0);
    ZVAL_ZVAL(&params_delete[0], filter, 0, 0);

    php_class_call_method(bulk_write, "delete", sizeof("delete") - 1, 1, params_delete, false);

    zval* params_executeBulkWrite = safe_emalloc(3, sizeof(zval), 0);

    char*   db_name_src = Z_STRVAL_P(db_name);
    char*   collection_src = Z_STRVAL_P(collection);

    int database_collection_len = snprintf(NULL, 0, "%s.%s", db_name_src, collection_src);
    char database_collection_src[database_collection_len + 1];
    sprintf(database_collection_src, "%s.%s", db_name_src, collection_src);
    database_collection_src[database_collection_len] = '\0';

    ZVAL_STRING(&params_executeBulkWrite[0], database_collection_src);
    ZVAL_OBJ(&params_executeBulkWrite[1], bulk_write);
    ZVAL_ARR(&params_executeBulkWrite[2], option);

    zval *retval = php_class_call_method(Z_OBJ_P(manager), "executeBulkWrite", sizeof("executeBulkWrite") - 1, 3, params_executeBulkWrite, false);

    /* Capture error exception */
    if( EG(exception) )
    { 
        /* Fetch exception error message */
        zend_object*    error_exception;
        zval*           exception_msg;

        error_exception = EG(exception);
        exception_msg   = zend_target_read_property_ex(zend_ce_exception, error_exception, "message");

        /* Clear the exception */
        zend_clear_exception();

        /* Throw error to handler */
        zend_throw_error(zend_ce_exception, "%s", Z_STRVAL_P(exception_msg));

        RETURN_NULL();
    }

    if( bulk_write_detail )
        RETURN_ZVAL(retval, 0, 0);

    zval* n_removed = php_class_call_method(Z_OBJ_P(retval), "getDeletedCount", sizeof("getDeletedCount") - 1, 0, NULL, false);

    RETURN_ZVAL(n_removed, 0, 0);
}

/* Public method */
PHP_METHOD(Model, update)
{
    zval* query;

    ZEND_PARSE_PARAMETERS_START(1, 1)
        Z_PARAM_ZVAL(query)
    ZEND_PARSE_PARAMETERS_END();

    ZEND_HASH_FOREACH_STR_KEY_VAL(Z_ARR_P(query), zend_string* name, zval* value)
    {
        zend_update_property(Z_OBJCE_P(getThis()), Z_OBJ_P(getThis()), name->val, name->len, value);
    }
    ZEND_HASH_FOREACH_END();

    /* Call save */
    php_class_call_method(Z_OBJ_P(getThis()), "save", sizeof("save") - 1, 0, NULL, false);
}

/* Public method */
PHP_METHOD(Model, save)
{
    ZEND_PARSE_PARAMETERS_NONE();

    voile*      voile_ptr           = ((voile*)zend_this_read_property("voile")->value.ptr);
    zval        manager             = voile_ptr->manager;
    zval*       filter              = &voile_ptr->filter;
    bool        filtered            = voile_ptr->filtered;
    zval*       collection          = &voile_ptr->collection;
    zval*       database_name       = &voile_ptr->db_name;
    HashTable*  option              = &voile_ptr->option;
    HashTable*  document            = &voile_ptr->document;
    HashTable*  update_doc          = &voile_ptr->update_doc;
    HashTable*  unset_doc           = &voile_ptr->unset_doc;

    zend_object *bulk_write = php_class_init("MongoDB\\Driver\\BulkWrite", sizeof("MongoDB\\Driver\\BulkWrite") - 1);
    php_class_call_constructor(bulk_write, 0, NULL);

    // HashTable* update_doc;
    // ALLOC_HASHTABLE(update_doc);
    // zend_hash_init(update_doc, 0, NULL, ZVAL_PTR_DTOR, 0);

    zval* inserted_object;
    bool is_new_document = false;

    zval target_document;
    ZVAL_ARR(&target_document, document);

    if(! zend_hash_num_elements(document) )
    {
        ZEND_HASH_FOREACH_PTR(&Z_OBJCE_P(getThis())->properties_info, zend_property_info* zpi_property )
        {   
            if( (zpi_property->flags & ZEND_ACC_PROTECTED) )
            {
                char*   property_name_src = zpi_property->name->val + 3;
                size_t  property_name_len = zpi_property->name->len - 3;

                if( property_name_len == 3 && !strncmp("_id", property_name_src, property_name_len) )
                    continue;

                if( property_name_len == 5 && !strncmp("voile", property_name_src, property_name_len) )
                    continue;

                zval* prop_value =
                    zend_read_property(
                        Z_OBJCE_P(getThis()), Z_OBJ_P(getThis()),
                        property_name_src, property_name_len, 1, NULL
                    );

                if( prop_value && Z_TYPE_P(prop_value) != IS_NULL && Z_TYPE_P(prop_value) != IS_UNDEF )
                {
                    _set_internal(property_name_src, property_name_len, prop_value, getThis());
                }
            }
        } ZEND_HASH_FOREACH_END();
    }

    if( filtered )
    {
        int values_targeted = 0;

        while( true )
        {
            if( zend_hash_num_elements(document) )
            {
                zval target_document;
                ZVAL_ARR(&target_document, document);

                zend_hash_str_update_ind(update_doc, "$set", sizeof("$set") - 1, &target_document);

                values_targeted += 1;
            }

            if( zend_hash_num_elements(update_doc) )
            {
                values_targeted += 1;
            }

            if( values_targeted )
                break;

            RETURN_NULL();
        }

        zval target_wrapper_document;
        ZVAL_ARR(&target_wrapper_document, update_doc);

        zval *params_update = safe_emalloc(3, sizeof(zval), 0);
        ZVAL_ZVAL(&params_update[0], filter, 0, 0); // Filter
        ZVAL_ZVAL(&params_update[1], &target_wrapper_document, 0, 0); // Object

        HashTable* mass_option;
        ALLOC_HASHTABLE(mass_option);
        zend_hash_init(mass_option, 0, NULL, ZVAL_PTR_DTOR, 0);

        zval multi_assert;
        ZVAL_TRUE(&multi_assert);
        zend_hash_update(mass_option, zend_string_init("multi", sizeof("multi") - 1, 0), &multi_assert);

        zval upsert_assert;
        ZVAL_FALSE(&upsert_assert);
        zend_hash_update(mass_option, zend_string_init("upsert", sizeof("upsert") - 1, 0), &upsert_assert);

        ZVAL_ARR(&params_update[2], mass_option); // Object

        php_class_call_method(bulk_write, "update", sizeof("update") - 1, 3, params_update, false);
    }
    else // Create/new doc
    {
        zval* params_insert = safe_emalloc(1, sizeof(zval), 0);

        ZVAL_ARR(&params_insert[0], document);

        is_new_document = true;
        inserted_object = php_class_call_method(bulk_write, "insert", sizeof("insert") - 1, 1, params_insert, false);
    }

    zval* params_executeBulkWrite = safe_emalloc(3, sizeof(zval), 0);

    int database_collection_len = snprintf(NULL, 0, "%s.%s", Z_STRVAL_P(database_name), Z_STRVAL_P(collection));
    char database_collection_src[database_collection_len + 1];
    sprintf(database_collection_src, "%s.%s", Z_STRVAL_P(database_name), Z_STRVAL_P(collection));
    database_collection_src[database_collection_len] = '\0';

    ZVAL_STRINGL(&params_executeBulkWrite[0], database_collection_src, database_collection_len);
    ZVAL_OBJ(&params_executeBulkWrite[1], bulk_write);
    ZVAL_ARR(&params_executeBulkWrite[2], option);

    zval target_manager;
    ZVAL_COPY(&target_manager, &manager);

    php_class_call_method(Z_OBJ(target_manager), "executeBulkWrite", sizeof("executeBulkWrite") - 1, 3, params_executeBulkWrite, false);


    /* Capture error exception */
    if( EG(exception) )
    { 
        /* Fetch exception error message */
        zend_object*    error_exception;
        zval*           exception_msg;

        error_exception = EG(exception);
        exception_msg   = zend_target_read_property_ex(zend_ce_exception, error_exception, "message");

        /* Clear the exception */
        zend_clear_exception();

        /* Throw error to handler */
        zend_throw_error(zend_ce_exception, "%s", Z_STRVAL_P(exception_msg));
        
        RETURN_NULL();
    }

    if( is_new_document )
    {
        RETURN_ZVAL(inserted_object, 1, 0);
    }

    // exit_unsave:
    // RETURN_NULL();
}

/* Public method */
PHP_METHOD(Model, project)
{
    zval* projection;

    ZEND_PARSE_PARAMETERS_START(1, 1)
        Z_PARAM_ZVAL(projection)
    ZEND_PARSE_PARAMETERS_END();

    zend_class_entry* std_ce = zend_standard_class_def;

    zval empty_obj;
    object_init_ex(&empty_obj, std_ce);

    ZEND_HASH_FOREACH_KEY_VAL(Z_ARR_P(projection), zend_ulong index, zend_string* name, zval* value )
    {
        if( name )
        {
            zend_update_property_ex(Z_OBJCE(empty_obj), Z_OBJ(empty_obj), name, value);
        }
    } ZEND_HASH_FOREACH_END();

    ((voile*)zend_this_read_property("voile")->value.ptr)->projection = empty_obj;

    RETURN_ZVAL(getThis(), 1, 0);
}

PHP_METHOD(Model, group)
{
    zval *zv_group;

    ZEND_PARSE_PARAMETERS_START(1, 1)
            Z_PARAM_ZVAL(zv_group)
    ZEND_PARSE_PARAMETERS_END();

    zend_class_entry *std_ce = zend_standard_class_def;

    zval empty_obj;
    object_init_ex(&empty_obj, std_ce);

    ZEND_HASH_FOREACH_KEY_VAL(Z_ARR_P(zv_group), zend_ulong index, zend_string* name, zval* value )
    {
        if( name )
        {
            zend_update_property_ex(Z_OBJCE(empty_obj), Z_OBJ(empty_obj), name, value);
        }
    } ZEND_HASH_FOREACH_END();

    ((voile*)zend_this_read_property("voile")->value.ptr)->group = empty_obj;

    RETURN_ZVAL(getThis(), 1, 0);
}

PHP_METHOD(Model, filter)
{
    zval* filter;

    ZEND_PARSE_PARAMETERS_START(1, 1)
            Z_PARAM_ZVAL(filter)
    ZEND_PARSE_PARAMETERS_END();

    zend_class_entry *std_ce = zend_standard_class_def;

    zval empty_obj;
    object_init_ex(&empty_obj, std_ce);

    ZEND_HASH_FOREACH_KEY_VAL(Z_ARR_P(filter), zend_ulong index, zend_string* name, zval* value)
    {
        if( name )
        {
            zend_update_property_ex(Z_OBJCE(empty_obj), Z_OBJ(empty_obj), name, value);
        }
    } ZEND_HASH_FOREACH_END();

    ((voile*)zend_this_read_property("voile")->value.ptr)->filter = empty_obj;
    ((voile*)zend_this_read_property("voile")->value.ptr)->filtered = true;

    RETURN_ZVAL(getThis(), 1, 0);
}

PHP_METHOD(Model, find)
{
    zval* filter;

    ZEND_PARSE_PARAMETERS_START(1, 1)
        Z_PARAM_ZVAL(filter)
    ZEND_PARSE_PARAMETERS_END();

    zend_class_entry*   model_classname = zend_get_called_scope(execute_data);
    zend_string*        model_classname_zs = model_classname->name;
    zend_object*        model = php_class_init(model_classname_zs->val, model_classname_zs->len);

    HashTable *array;
    ALLOC_HASHTABLE(array);
    zend_hash_init(array, 0, NULL, ZVAL_PTR_DTOR, 0);

    /* Handling filter as MongoDB ObjectId */
    switch( Z_TYPE_P(filter) )
    {
        case IS_STRING:
        {
            zend_object* object_id = php_class_init("MongoDB\\BSON\\ObjectId", sizeof("MongoDB\\BSON\\ObjectId") - 1);

            zval* params___construct = safe_emalloc(1, sizeof(zval), 0);
            ZVAL_ZVAL(&params___construct[0], filter, 0, 0);
            
            php_class_call_method(object_id, "__construct", sizeof("__construct") - 1, 1, params___construct, true);

            zval target_object_id;
            ZVAL_OBJ(&target_object_id, object_id); 

            zend_hash_str_update(array, "_id", sizeof("_id") - 1, &target_object_id);

            ZVAL_ARR(filter, array);
        }
        break;
        case IS_OBJECT:
        {
            bool is_object_id =
                zend_string_equals_cstr(
                    Z_OBJCE_P(filter)->name, 
                    "MongoDB\\BSON\\ObjectId", 
                    sizeof("MongoDB\\BSON\\ObjectId") - 1
                );

            if(! is_object_id )
                zend_error(E_ERROR, "target object only support mongodb");

            zend_hash_str_update(array, "_id", sizeof("_id") - 1, filter);
            ZVAL_ARR(filter, array);
        }
        break;
        case IS_ARRAY: /* Otherwise it's normal filter */
            zend_hash_destroy(array);
        break;
        default: /* Other type not supported */
            zend_throw_error(zend_ce_exception,
                "Argument #1 ($filter) must be of type array|string, %s given,", ZTYPE_TO_STR(Z_TYPE_P(filter)));
            // zend_hash_destroy(array);
        break;
    }

    /* Call constructor */
    php_class_call_constructor(model, 0, NULL);

    zval *params_filter = safe_emalloc(1, sizeof(zval), 0);
    ZVAL_ZVAL(&params_filter[0], filter, 0, 0);

    /* Call filter */
    zval* instance = php_class_call_method(model, "filter", sizeof("filter") - 1, 1, params_filter, true);

    RETURN_ZVAL(instance, 0, 0);
}

PHP_METHOD(Model, it)
{
    ZEND_PARSE_PARAMETERS_NONE();

    zend_class_entry*   model_classname = zend_get_called_scope(execute_data);
    zend_string*        model_classname_zs = model_classname->name;
    zend_object*        model = php_class_init(model_classname_zs->val, model_classname_zs->len);

    /* Call constructor */
    php_class_call_constructor(model, 0, NULL);

    RETURN_OBJ(model);
}

PHP_METHOD(Model, sum)
{
    char*   field_src = NULL;
    size_t  field_len = 0;

    ZEND_PARSE_PARAMETERS_START(1, 1)
        Z_PARAM_STRING(field_src, field_len)
    ZEND_PARSE_PARAMETERS_END();

    /*
    $amount = field -> "amount"

    [
        '_id' => null,
        'total' =>
        [
            '$sum' => '$amount'
        ]
    ]
    */

    HashTable *group_query;
    ALLOC_HASHTABLE(group_query);
    zend_hash_init(group_query, 0, NULL, ZVAL_PTR_DTOR, 0);

    zval zv_null;
    // ZVAL_NULL(&zv_null);
    ZVAL_STRINGL(&zv_null, "$payment_dest_no", sizeof("$payment_dest_no") - 1);
    zend_hash_update_ind(group_query, zend_string_init("_id", sizeof("_id") - 1, 0), &zv_null);

    HashTable* sum_query;
    ALLOC_HASHTABLE(sum_query);
    zend_hash_init(sum_query, 0, NULL, ZVAL_PTR_DTOR, 0);

    zval field;
    zend_string* field_zs = zend_string_concat2("$", sizeof("$") - 1, field_src, field_len);
    ZVAL_STRINGL(&field, field_zs->val, field_zs->len);

    zend_hash_update_ind(sum_query, zend_string_init("$sum", sizeof("$sum") - 1, 0), &field);

    zval target_sum_query;
    ZVAL_ARR(&target_sum_query, sum_query);
    zend_hash_update_ind(group_query, zend_string_init("total", sizeof("total") - 1, 0), &target_sum_query);

    zval *params_group = safe_emalloc(1, sizeof(zval), 0);
    ZVAL_ARR(&params_group[0], group_query);

    php_class_call_method(Z_OBJ_P(getThis()), "group", sizeof("group") - 1, 1, params_group, false);


    /* Prepare params for get */
    zval *params_get = safe_emalloc(1, sizeof(zval), 0);
    ZVAL_TRUE(&params_get[0]);

    /* Call get */
    zval* result = php_class_call_method(Z_OBJ_P(getThis()), "get", sizeof("get") - 1, 1, params_get, false);

    /* Capture error exception */
    if( EG(exception) )
    { 
        /* Fetch exception error message */
        zend_object*    error_exception;
        zval*           exception_msg;

        error_exception = EG(exception);
        exception_msg   = zend_target_read_property_ex(zend_ce_exception, error_exception, "message");

        /* Clear the exception */
        zend_clear_exception();

        /* Throw error to handler */
        zend_throw_error(zend_ce_exception, "%s", Z_STRVAL_P(exception_msg));

        RETURN_NULL();
    }


    /* Transform cursor to array */
    zval* array_cursor = php_class_call_method(Z_OBJ_P(result), "toArray", sizeof("toArray") - 1, 0, NULL, false);


    if( zend_hash_num_elements(Z_ARR_P(array_cursor)) )
    {
        zval* obj = zend_hash_index_find(Z_ARR_P(array_cursor), 0);
        obj = zend_read_property(Z_OBJCE_P(obj), Z_OBJ_P(obj), "total", sizeof("total") - 1, 0, NULL);   

        RETURN_ZVAL(obj, 1, 0);
    }

    zend_object* zero_adecimal128 = php_class_init("MongoDB\\BSON\\Decimal128", sizeof("MongoDB\\BSON\\Decimal128") - 1);

    zval* params___construct = safe_emalloc(1, sizeof(zval), 0);
    ZVAL_STRINGL(&params___construct[0], "0", sizeof("0") - 1);

    php_class_call_constructor(zero_adecimal128, 1, params___construct);

    RETURN_OBJ(zero_adecimal128);
}

PHP_METHOD(Model, all)
{
    ZEND_PARSE_PARAMETERS_NONE();

    zend_class_entry*   model_classname = zend_get_called_scope(execute_data);
    zend_string*        model_classname_zs = model_classname->name;
    zend_object*        model = php_class_init(model_classname_zs->val, model_classname_zs->len);

    /* Call constructor */
    php_class_call_constructor(model, 0, NULL);

    /* Call get */
    zval* return_get = php_class_call_method(model, "get", sizeof("get") - 1, 0, NULL, 0);

    RETURN_ZVAL(return_get, 0, 0);
}

PHP_METHOD(Model, get)
{
    bool use_cursor = false;

    ZEND_PARSE_PARAMETERS_START(0, 1)
            Z_PARAM_OPTIONAL
            Z_PARAM_BOOL(use_cursor)    
    ZEND_PARSE_PARAMETERS_END();


    zval*           target_voile_ptr    = zend_this_read_property("voile");
    voile*          voile_ptr           = ((voile*)(target_voile_ptr->value.ptr));

    zval*           model_classname     = &voile_ptr->model_classname;
    zval*           collection          = &voile_ptr->collection;
    zval*           manager             = &voile_ptr->manager;
    zval*           db_name             = &voile_ptr->db_name;
    zval            db_uri              = voile_ptr->db_uri;
    zval*           filter              = &voile_ptr->filter;
    zval*           projection          = &voile_ptr->projection;
    zval*           group               = &voile_ptr->group;
    zend_ulong      limit               = voile_ptr->limit;
    zend_ulong      skip                = voile_ptr->skip;
    HashTable*      sort                = &voile_ptr->sort;
    bool            use_counting        = voile_ptr->count;

    zend_string*    db_name_zs          = zval_get_string(db_name);
    char*           db_name_src         = db_name_zs->val;
    size_t          db_name_len         = db_name_zs->len;

    voile_ptr->get_called              = true;

    HashTable *array_command;
    ALLOC_HASHTABLE(array_command);
    zend_hash_init(array_command, 0, NULL, ZVAL_PTR_DTOR, 0);

    /* Pipeline filtering */
    HashTable *array_pipeline;
    ALLOC_HASHTABLE(array_pipeline);
    zend_hash_init(array_pipeline, 0, NULL, ZVAL_PTR_DTOR, 0);

    HashTable *array_match;
    ALLOC_HASHTABLE(array_match);
    zend_hash_init(array_match, 0, NULL, ZVAL_PTR_DTOR, 0);

    zval empty_array;
    ZVAL_EMPTY_ARRAY(&empty_array);
    zend_hash_update_ind(array_match, zend_string_init("$match", sizeof("$match") - 1, 0), filter);

    zval target_array_match;
    ZVAL_ARR(&target_array_match, array_match);
    zend_hash_index_add(array_pipeline, 0, &target_array_match);

    if( zend_hash_num_elements(Z_ARR_P(projection)) )
    {
        HashTable* project_projection;
        ALLOC_HASHTABLE(project_projection);
        zend_hash_init(project_projection, 0, NULL, ZVAL_PTR_DTOR, 0);

        zend_hash_update_ind(project_projection, zend_string_init("$project", sizeof("$project") - 1, 0), projection);

        zval target_project;
        ZVAL_ARR(&target_project, project_projection);
        zend_hash_index_add(array_pipeline, zend_hash_num_elements(array_pipeline), &target_project);
    }

    if( zend_hash_num_elements(Z_ARR_P(group)) )
    {
        HashTable* project_group;
        ALLOC_HASHTABLE(project_group);
        zend_hash_init(project_group, 0, NULL, ZVAL_PTR_DTOR, 0);

        zend_hash_update_ind(project_group, zend_string_init("$group", sizeof("$group") - 1, 0), group);

        zval target_project_group;
        ZVAL_ARR(&target_project_group, project_group);
        zend_hash_index_add(array_pipeline, zend_hash_num_elements(array_pipeline), &target_project_group);
    }

    if( zend_hash_num_elements(sort) )
    {
        HashTable* project_sort;
        ALLOC_HASHTABLE(project_sort);
        zend_hash_init(project_sort, 0, NULL, ZVAL_PTR_DTOR, 0);

        zval _sort;
        ZVAL_ARR(&_sort, sort)               ;

        zend_hash_update_ind(project_sort, zend_string_init("$sort", sizeof("$sort") - 1, 0), &_sort);

        zval target_project;
        ZVAL_ARR(&target_project, project_sort);
        zend_hash_index_add(array_pipeline, zend_hash_num_elements(array_pipeline), &target_project);
    }

    if( skip )
    {
        HashTable* project_skip;
        ALLOC_HASHTABLE(project_skip);
        zend_hash_init(project_skip, 0, NULL, ZVAL_PTR_DTOR, 0);

        zval _skip;
        ZVAL_LONG(&_skip, skip);

        zend_hash_update_ind(project_skip, zend_string_init("$skip", sizeof("$skip") - 1, 0), &_skip);

        zval target_project;
        ZVAL_ARR(&target_project, project_skip);
        zend_hash_index_add(array_pipeline, zend_hash_num_elements(array_pipeline), &target_project);
    }

    if( limit )
    {
        HashTable* project_limit;
        ALLOC_HASHTABLE(project_limit);
        zend_hash_init(project_limit, 0, NULL, ZVAL_PTR_DTOR, 0);

        zval _limit;
        ZVAL_LONG(&_limit, limit);

        zend_hash_update_ind(project_limit, zend_string_init("$limit", sizeof("$limit") - 1, 0), &_limit);

        zval target_project;
        ZVAL_ARR(&target_project, project_limit);
        zend_hash_index_add(array_pipeline, zend_hash_num_elements(array_pipeline), &target_project);
    }

    if( use_counting )
    {
        HashTable* project_count;
        ALLOC_HASHTABLE(project_count);
        zend_hash_init(project_count, 0, NULL, ZVAL_PTR_DTOR, 0);

        zval _count;
        ZVAL_STRING(&_count, "docCount");

        zend_hash_update_ind(project_count, zend_string_init("$count", sizeof("$count") - 1, 0), &_count);

        zval target_project;
        ZVAL_ARR(&target_project, project_count);
        zend_hash_index_add(array_pipeline, zend_hash_num_elements(array_pipeline), &target_project);
    }

    zval pipeline;
    ZVAL_ARR(&pipeline, array_pipeline);

    zend_class_entry *std_ce = zend_standard_class_def;
    zval empty_obj;
    object_init_ex(&empty_obj, std_ce);

    zend_hash_update(array_command, zend_string_init("aggregate", sizeof("aggregate") - 1, 0), collection);
    zend_hash_update(array_command, zend_string_init("pipeline", sizeof("pipeline") - 1, 0), &pipeline);
    zend_hash_update(array_command, zend_string_init("cursor", sizeof("cursor") - 1, 0), &empty_obj);

    zend_object *command = php_class_init("MongoDB\\Driver\\Command", sizeof("MongoDB\\Driver\\Command") - 1);

    zval *params___construct = safe_emalloc(1, sizeof(zval), 0);
    ZVAL_ARR(&params___construct[0], array_command);
    php_class_call_method(command, "__construct", sizeof("__construct") - 1, 1, params___construct, false);

    zval *params_executeCommand = safe_emalloc(2, sizeof(zval), 0);
    ZVAL_STRINGL(&params_executeCommand[0], db_name_src, db_name_len);
    ZVAL_OBJ(&params_executeCommand[1], command);

    zval target_manager;
    ZVAL_COPY(&target_manager, manager);

    zval *cursor = php_class_call_method(Z_OBJ(target_manager), "executeCommand", sizeof("executeCommand") - 1, 2, params_executeCommand, false);

    /* Capture error exception */
    if( EG(exception) )
    { 
        /* Fetch exception error message */
        zend_object*    error_exception;
        zval*           exception_msg;

        error_exception = EG(exception);
        exception_msg   = zend_target_read_property_ex(zend_ce_exception, error_exception, "message");

        /* Clear the exception */
        zend_clear_exception();

        /* Throw error to handler */
        zend_throw_error(zend_ce_exception, "%s", Z_STRVAL_P(exception_msg));
        
        RETURN_NULL();
    }

    if( use_cursor == false )
    {
        voile_model_typeinfo* model_info = &voile_ptr->modelinfo;

        zval container_result;
        array_init(&container_result);

        voile_data_puser* data_puser    = emalloc(sizeof(voile_data_puser));

        data_puser->return_object       = container_result;
        data_puser->model_classname     = zval_get_string(model_classname);
        data_puser->typelist_field      = model_info->typelist_field;
        data_puser->typepair_field      = model_info->typepair_field;
        data_puser->attribute_field     = model_info->attribute_field;
        data_puser->_voile              = *target_voile_ptr;

        zval target_data_puser;
        ZVAL_PTR(&target_data_puser, (void*)data_puser);

        zend_object *collection = php_class_init("Zpheur\\Databases\\Voile\\Collection", sizeof("Zpheur\\Databases\\Voile\\Collection") - 1);

        zval *params___construct = safe_emalloc(3, sizeof(zval), 0);

        zval empty_array;
        array_init(&empty_array);

        ZVAL_ZVAL(&params___construct[0], &empty_array, 0, 0);
        ZVAL_ZVAL(&params___construct[1], cursor, 0, 0);
        ZVAL_ZVAL(&params___construct[2], &target_data_puser, 0, 0);

        php_class_call_method(collection, "__construct", sizeof("__construct") - 1, 3, params___construct, false);

        RETURN_OBJ(collection);
    }

    RETURN_ZVAL(cursor, 1, 0);
}

PHP_METHOD(Model, create)
{
    zval *input;

    ZEND_PARSE_PARAMETERS_START(1, 1)
        Z_PARAM_ZVAL(input)
    ZEND_PARSE_PARAMETERS_END();
    
    zend_class_entry*   model_classname = zend_get_called_scope(execute_data);
    zend_string*        model_classname_zs = model_classname->name;
    zend_object*        model = php_class_init(model_classname_zs->val, model_classname_zs->len);

    /* Prepare params for constructor */
    zval *params___construct = safe_emalloc(1, sizeof(zval), 0);
    ZVAL_STR(&params___construct[0], model_classname_zs);

    /* Call constructor */
    php_class_call_constructor(model, 1, params___construct);

    /* Save input */
    ZEND_HASH_FOREACH_STR_KEY_VAL(Z_ARR_P(input), zend_string *name, zval *value)
    {
        zval *params___set = safe_emalloc(2, sizeof(zval), 0);
        ZVAL_STR(&params___set[0], name);
        ZVAL_ZVAL(&params___set[1], value, 0, 0);

        php_class_call_method(model, "__set", sizeof("__set") - 1, 2, params___set, 0);
    }
    ZEND_HASH_FOREACH_END();

    if( EG(exception) )
    { 
        /* Fetch exception error message */
        zend_object*    error_exception;
        zval*           exception_msg;

        error_exception = EG(exception);
        exception_msg   = zend_target_read_property_ex(zend_ce_exception, error_exception, "message");

        /* Clear the exception */
        zend_clear_exception();

        /* Throw error to handler */
        zend_throw_error(zend_ce_exception, "%s", Z_STRVAL_P(exception_msg));
        
        RETURN_NULL();
    }

    /* Call save */
    zval* inserted_id = php_class_call_method(model, "save", sizeof("save") - 1, 0, NULL, 0);
    RETURN_ZVAL(inserted_id, 1, 0);
}

PHP_METHOD(Model, limit)
{
    zend_long limit = 1;

    ZEND_PARSE_PARAMETERS_START(1, 1)
        Z_PARAM_LONG(limit)
    ZEND_PARSE_PARAMETERS_END();

    ((voile*)zend_this_read_property("voile")->value.ptr)->limit = limit;

    RETURN_ZVAL(getThis(), 1, 0);
}

PHP_METHOD(Model, inc)
{
    char*   name_src = NULL;
    size_t  name_len = 0;

    zend_ulong value = 1;

    ZEND_PARSE_PARAMETERS_START(2, 2)
        Z_PARAM_STRING(name_src, name_len)
        Z_PARAM_LONG(value)
    ZEND_PARSE_PARAMETERS_END();

    HashTable* update_doc = &((voile*)zend_this_read_property("voile")->value.ptr)->update_doc;
    zval* array_inc = zend_hash_str_find(update_doc, "$inc", sizeof("$inc") - 1);

    if(! array_inc )
    {
        HashTable* ht_array_inc;
        ALLOC_HASHTABLE(ht_array_inc);
        zend_hash_init(ht_array_inc, 0, NULL, ZVAL_PTR_DTOR, 0);

        zval _array_inc;
        ZVAL_ARR(&_array_inc, ht_array_inc);

        zend_hash_str_update(update_doc, "$inc", sizeof("$inc") - 1, &_array_inc);

        array_inc = &_array_inc;
    }

    zval _value;
    ZVAL_LONG(&_value, value);
    zend_hash_str_update(Z_ARR_P(array_inc), name_src, name_len, &_value);
}

PHP_METHOD(Model, skip)
{
    zend_ulong skip;

    ZEND_PARSE_PARAMETERS_START(1, 1)
        Z_PARAM_LONG(skip)
    ZEND_PARSE_PARAMETERS_END();

    ((voile*)zend_this_read_property("voile")->value.ptr)->skip = skip;

    RETURN_ZVAL(getThis(), 1, 0);
}

PHP_METHOD(Model, sort)
{
    char*       field_src = NULL;
    size_t      field_len = 0;

    zend_ulong  sort;

    ZEND_PARSE_PARAMETERS_START(2, 2)
        Z_PARAM_STRING(field_src, field_len)
        Z_PARAM_LONG(sort)
    ZEND_PARSE_PARAMETERS_END();

    HashTable *array_sort;
    ALLOC_HASHTABLE(array_sort);
    zend_hash_init(array_sort, 0, NULL, ZVAL_PTR_DTOR, 0);

    zval target_sort;
    ZVAL_LONG(&target_sort, sort);

    zend_hash_str_update(array_sort, field_src, field_len, &target_sort);

    ((voile*)zend_this_read_property("voile")->value.ptr)->sort = *array_sort;

    RETURN_ZVAL(getThis(), 1, 0);
}

PHP_METHOD(Model, startTransaction)
{   
    ZEND_PARSE_PARAMETERS_NONE();

    voile* voile_ptr = ((voile*)zend_this_read_property("voile")->value.ptr);

    zval*           manager     = &voile_ptr->manager;
    HashTable*      option      = &voile_ptr->option;

    zval* session = php_class_call_method(Z_OBJ_P(manager), "startSession", sizeof("startSession") - 1, 0, NULL, false);

    /* Capture error exception */
    if( EG(exception) )
    { 
        /* Fetch exception error message */
        zend_object*    error_exception;
        zval*           exception_msg;

        error_exception = EG(exception);
        exception_msg   = zend_target_read_property_ex(zend_ce_exception, error_exception, "message");

        /* Clear the exception */
        zend_clear_exception();

        /* Throw error to handler */
        zend_throw_error(zend_ce_exception, "%s", Z_STRVAL_P(exception_msg));
        
        RETURN_NULL();
    }

    zend_hash_update(option, zend_string_init("session", sizeof("session") - 1, 0), session);

    php_class_call_method(Z_OBJ_P(session), "startTransaction", sizeof("startTransaction") - 1, 0, NULL, false);
}

PHP_METHOD(Model, commitTransaction)
{
    ZEND_PARSE_PARAMETERS_NONE();

    HashTable*      option      = &((voile*)zend_this_read_property("voile")->value.ptr)->option;
    zval*           session     = zend_hash_str_find(option, "session", sizeof("session") - 1);

    php_class_call_method(Z_OBJ_P(session), "commitTransaction", sizeof("commitTransaction") - 1, 0, NULL, false);
}

PHP_METHOD(Model, abortTransaction)
{
    ZEND_PARSE_PARAMETERS_NONE();

    HashTable*      option      = &((voile*)zend_this_read_property("voile")->value.ptr)->option;
    zval*           session     = zend_hash_str_find(option, "session", sizeof("session") - 1);

    php_class_call_method(Z_OBJ_P(session), "abortTransaction", sizeof("abortTransaction") - 1, 0, NULL, false);
}

PHP_METHOD(Model, count)
{
    ZEND_PARSE_PARAMETERS_NONE();

    bool* count_active = &((voile*)zend_this_read_property("voile")->value.ptr)->count;

    *count_active = true;

    zval *params_get = safe_emalloc(1, sizeof(zval), 0);
    ZVAL_BOOL(&params_get[0], true);

    /* Call get */
    zval* cursor = php_class_call_method(Z_OBJ_P(getThis()), "get", sizeof("get") - 1, 1, params_get, false);

    /* Capture error exception */
    if( EG(exception) )
    { 
        /* Fetch exception error message */
        zend_object*    error_exception;
        zval*           exception_msg;

        error_exception = EG(exception);
        exception_msg   = zend_target_read_property_ex(zend_ce_exception, error_exception, "message");

        /* Clear the exception */
        zend_clear_exception();

        /* Throw error to handler */
        zend_throw_error(zend_ce_exception, "%s", Z_STRVAL_P(exception_msg));
        
        RETURN_NULL();
    }

    /* Tranform cursor with toArray */
    zval* array_cursor = php_class_call_method(Z_OBJ_P(cursor), "toArray", sizeof("toArray") - 1, 0, NULL, false);

    *count_active = false;

    if( zend_hash_num_elements(Z_ARR_P(array_cursor)) )
    {
        zval* obj       = zend_hash_index_find(Z_ARR_P(array_cursor), 0);
        zval* retval    = zend_read_property(Z_OBJCE_P(obj), Z_OBJ_P(obj), "docCount", sizeof("docCount") - 1, 0, NULL);

        RETURN_ZVAL(retval, 1, 0);
    }

    RETURN_LONG(0);
}

ZEND_MINIT_FUNCTION(Zpheur_Databases_Voile_Model)
{
    zend_class_entry ce;

    INIT_NS_CLASS_ENTRY(ce, "Zpheur\\Databases\\Voile", "Model", zpheur_databases_voile_model_class_method);
    zpheur_databases_voile_model_class_entry = zend_register_internal_class(&ce);
    zpheur_databases_voile_model_class_entry->ce_flags |= ZEND_ACC_ALLOW_DYNAMIC_PROPERTIES;

    zend_declare_property_null(zpheur_databases_voile_model_class_entry, "voile", sizeof("voile") - 1, ZEND_ACC_PROTECTED);

    return SUCCESS;
}