#include <php.h>
#include <php_zpheur.h>
#include <ext/standard/info.h>
#include <Zend/zend_attributes.h>
#include <ext/spl/spl_iterators.h>
#include <Zend/zend_interfaces.h>

#include <string.h>
#include <zpheur.h>
#include <include/runtime.h>
#include <include/regexp.h>
#include "Model_arginfo.h"
#include "Collection_arginfo.h"


static int voile_cursor_to_collection_apply(zend_object_iterator* iter, void* puser, bool once )
{
    zval* data;

    voile_data_puser*   ptr_data_puser  = (voile_data_puser*)puser;
    zval*               return_value    = &ptr_data_puser->return_object;
    zval*               typelist_field  = &ptr_data_puser->typelist_field;
    zval*               property_field  = &ptr_data_puser->typepair_field;
    zval*               attribute_field = &ptr_data_puser->attribute_field;
    zval*               zv_voile        = &ptr_data_puser->_voile;

    data = iter->funcs->get_current_data(iter);

    if (EG(exception)) {
        return ZEND_HASH_APPLY_STOP;
    }
    if (Z_ISUNDEF_P(data)) {
        return ZEND_HASH_APPLY_STOP;
    }
    Z_TRY_ADDREF_P(data);

    zend_object* collection = php_class_init(ptr_data_puser->model_classname);
    // PoC for name change at runtime
    // zend_string* new_class_name = zend_string_init("InternSession", sizeof("InternSession") - 1, 0);
    // collection->ce->name = new_class_name;
    // -- end --

    zend_update_property(collection->ce, collection, "voile", sizeof("voile") - 1, zv_voile);

    HashTable *properties = zend_std_get_properties(Z_OBJ_P(data));

    /* Type field checks */
    ZEND_HASH_FOREACH_STR_KEY(Z_ARR_P(property_field), zend_string* name)
    {
        if(! zend_string_equals_cstr(name, "voile", sizeof("voile") - 1) && zend_hash_exists_ind(properties, name) )
        {            
            zval* value = zend_hash_find_ind(properties, name);

            int typeinfo = zval_get_long(zend_hash_find_ind(Z_ARR_P(property_field), name));

            switch( typeinfo )
            {
                case 0: // AObjectId
                {
                    zval *field_attrinfo = zend_hash_find_ind(Z_ARR_P(attribute_field), name);
                    ZEND_HASH_FOREACH_STR_KEY_VAL(Z_ARR_P(field_attrinfo), zend_string *attr_key, zval *attr_elem)
                    {
                        if( zend_string_equals_cstr(attr_key, "serialize", sizeof("serialize") - 1) == true )
                        {
                            if( Z_TYPE_P(attr_elem) == 3 )
                            {
                                value = php_class_call_method(Z_OBJ_P(value), "__toString", sizeof("__toString") - 1, 0, NULL, false);
                            }
                        }
                    } ZEND_HASH_FOREACH_END();
                }
                break;
                case 1: // AString
                {
                    zval *typelist = zend_hash_find_ind(Z_ARR_P(typelist_field), name);
                    char* enum_name = NULL;

                    zend_string* zs_typelist = zval_get_string(typelist);

                    if(
                        regexp_match_result(
                            "(?<=\\|).*Domain\\\\Enum\\\\Model\\\\.*.*(?=\\|)", 
                            sizeof("(?<=\\|).*Domain\\\\Enum\\\\Model\\\\.*.*(?=\\|)") - 1,
                            zs_typelist->val,
                            zs_typelist->len,
                            &enum_name) == true )
                    {
                        zend_class_entry *enum_ce = php_enum_init(enum_name);

                        if( !enum_ce || (enum_ce->ce_flags & ZEND_ACC_ENUM) != ZEND_ACC_ENUM || Z_TYPE_P(value) != IS_STRING ) 
                        {
                            zend_error(E_ERROR, "The enumeration cast-conversion for Type\\AString only supports string-backed value enums");
                        }

                        zval* params_from = safe_emalloc(1, sizeof(zval), 0);
                        ZVAL_STRING(&params_from[0], Z_STRVAL_P(value));

                        value = php_call_enum_method(enum_ce, "from", 1, params_from);
                    }
                }
                break;
                case 2: // ANumber
                {
                    zval *typelist = zend_hash_find_ind(Z_ARR_P(typelist_field), name);
                    char* enum_name = NULL;

                    zend_string* zs_typelist = zval_get_string(typelist);

                    if( 
                        regexp_match_result(
                            "(?<=\\|).*Domain\\\\Enum\\\\Model\\\\.*.*(?=\\|)", 
                            sizeof("(?<=\\|).*Domain\\\\Enum\\\\Model\\\\.*.*(?=\\|)") - 1,
                            zs_typelist->val,
                            zs_typelist->len,
                            &enum_name) == true )
                    {
                        zend_class_entry *enum_ce = php_enum_init(enum_name);

                        if( !enum_ce || (enum_ce->ce_flags & ZEND_ACC_ENUM) != ZEND_ACC_ENUM || Z_TYPE_P(value) != IS_LONG ) 
                        {
                            zend_error(E_ERROR, "The enumeration cast-conversion for Type\\ANumber only supports int-backed value enums");
                        }

                        zval* params_from = safe_emalloc(1, sizeof(zval), 0);
                        ZVAL_LONG(&params_from[0], zval_get_long(value));

                        value = php_call_enum_method(enum_ce, "from", 1, params_from);
                    }
                }
                break;  
                case 4: // ADateTime
                {
                    zval *field_attrinfo = zend_hash_find_ind(Z_ARR_P(attribute_field), name);
                    ZEND_HASH_FOREACH_STR_KEY_VAL(Z_ARR_P(field_attrinfo), zend_string* attr_key, zval* attr_elem)
                    {   
                        if( zend_string_equals_cstr(attr_key, "serialize", sizeof("serialize") - 1) == true )
                        {
                            if( Z_TYPE_P(attr_elem) == 3 )
                            {
                                value = php_class_call_method(Z_OBJ_P(value), "toDateTime", sizeof("toDateTime") - 1, 0, NULL, false);
                            }
                        }
                    } ZEND_HASH_FOREACH_END();
                }
                break;
            }

            zend_update_property(collection->ce, collection, name->val, name->len, value);
        }
        else // bila index collection dari user model tidak ada
        {
            // Set default value to undefined field from MongoDB result
            zval*   property_field        = zend_hash_str_find(Z_ARR_P(attribute_field), name->val, name->len);
            zval*   attr_default_value    = zend_hash_str_find(Z_ARR_P(property_field), "default", sizeof("default") - 1);

            zend_update_property(collection->ce, collection, name->val, name->len, attr_default_value);
        }
    } ZEND_HASH_FOREACH_END();

    zval target_collection;
    ZVAL_OBJ(&target_collection, collection);

    if( once )
    {
        add_next_index_zval(return_value, &target_collection);
    }
    else
    {
        ZVAL_ZVAL(return_value, &target_collection, 1, 0);
    }

    return ZEND_HASH_APPLY_KEEP;
}

zend_result c_spl_iterator_apply(zval *obj, c_spl_iterator_apply_func_t apply_func, void *puser)
{
    zend_object_iterator   *iter;
    zend_class_entry       *ce = Z_OBJCE_P(obj);

    iter = ce->get_iterator(ce, obj, 0);

    if( EG(exception) )
        goto done;

    iter->index = 0;

    if( iter->funcs->rewind )
    {
        iter->funcs->rewind(iter);

        if( EG(exception) )
            goto done;
    }

    while( iter->funcs->valid(iter) == SUCCESS )
    {
        if( EG(exception) ) 
            goto done;

        if( apply_func(iter, puser, 1) == ZEND_HASH_APPLY_STOP || EG(exception) )
            goto done;

        iter->index++;

        iter->funcs->move_forward(iter);

        if( EG(exception) )
            goto done;
    }

done:
    if( iter )
        zend_iterator_dtor(iter);

    return EG(exception) ? FAILURE : SUCCESS;
}
/* }}} */


PHP_METHOD(Collection, __construct)
{
    zval *items;
    zval *cursor;
    zval *data_puser;
    ZEND_PARSE_PARAMETERS_START(3, 3)
        Z_PARAM_ZVAL(items)
        Z_PARAM_ZVAL(cursor)
        Z_PARAM_ZVAL(data_puser)
    ZEND_PARSE_PARAMETERS_END();

    zend_class_entry*       ce      = Z_OBJCE_P(cursor);
    zend_object_iterator*   iter    = ce->get_iterator(ce, cursor, 0);

    zval target_iter;
    ZVAL_PTR(&target_iter, (void*)iter);

    zend_this_update_property("all", items);
    zend_this_update_property("cursor", cursor);
    zend_this_update_property("data_puser", data_puser);
    zend_this_update_property("iter", &target_iter);
}

PHP_METHOD(Collection, count)
{
    ZEND_PARSE_PARAMETERS_NONE();

    zval* count = zend_this_read_property("count");
    
    if( Z_TYPE_P(count) == IS_NULL )
    {
        zval*   cursor      = zend_this_read_property("cursor");
        zval*   data_puser  = zend_this_read_property("data_puser");

        voile_data_puser *ptr_data_puser = (voile_data_puser*)data_puser->value.ptr;

        c_spl_iterator_apply(cursor, voile_cursor_to_collection_apply, (void*)ptr_data_puser);
        zval array_result = ptr_data_puser->return_object;

        zval long_count;
        zend_ulong array_len = zend_hash_num_elements(Z_ARR(array_result));
        ZVAL_LONG(&long_count, array_len);

        zend_this_update_property("count", &long_count);
        zend_this_update_property("all", &array_result);
        
        RETURN_LONG(array_len);
    }

    RETURN_LONG(zval_get_long(count));
}

PHP_METHOD(Collection, current)         
{
    ZEND_PARSE_PARAMETERS_NONE();

    zval* count = zend_this_read_property("count");
    
    if( Z_TYPE_P(count) == IS_NULL )
    {
        zval* data_puser = zend_this_read_property("data_puser");

        voile_data_puser *ptr_data_puser = (voile_data_puser*)data_puser->value.ptr;

        zend_object_iterator* iter = (zend_object_iterator*)zend_this_read_property("iter")->value.ptr;

        voile_cursor_to_collection_apply(iter, (void*)ptr_data_puser, 0);

        zval* all = zend_this_read_property("all");

        add_next_index_zval(all, &ptr_data_puser->return_object);

        RETURN_ZVAL(&ptr_data_puser->return_object, 1, 0);
    }

    zval* all       = zend_this_read_property("all");
    zval* position  = zend_this_read_property("position");
    zval* current   = zend_hash_index_find(Z_ARR_P(all), zval_get_long(position));
   
    RETURN_ZVAL(current, 1, 0);
}

PHP_METHOD(Collection, key)
{
    ZEND_PARSE_PARAMETERS_NONE();
    
    zval* position = zend_this_read_property("position");
    RETURN_LONG(zval_get_long(position));
}

PHP_METHOD(Collection, next)
{
    ZEND_PARSE_PARAMETERS_NONE();

    zval *count = zend_this_read_property("count");
    
    if( Z_TYPE_P(count) == IS_NULL )
    {
        zend_object_iterator* iter =
            (zend_object_iterator*)zend_this_read_property("iter")->value.ptr;

        iter->index += 1;
        iter->funcs->move_forward(iter);
    }

    zval* position = zend_this_read_property("position");   
    zend_ulong increment = zval_get_long(position) + 1;
    ZVAL_LONG(position, increment);
}

PHP_METHOD(Collection, rewind)
{
    ZEND_PARSE_PARAMETERS_NONE();

    zval* count = zend_this_read_property("count");
    
    if( Z_TYPE_P(count) == IS_NULL )
    {
        zval *cursor = zend_this_read_property("cursor");
        php_class_call_method(Z_OBJ_P(cursor), "rewind", sizeof("rewind") - 1, 0, NULL, false);
    }
    else
    {
        zval* position  = zend_this_read_property("position");
        zend_ulong increment = 0;
        ZVAL_LONG(position, increment);
    }
}

PHP_METHOD(Collection, valid)
{
    ZEND_PARSE_PARAMETERS_NONE();

    zval *count = zend_this_read_property("count");

    bool is_valid;
    
    if( Z_TYPE_P(count) == IS_NULL )
    {
        zval *cursor = zend_this_read_property("cursor");
        
        is_valid = zval_get_long(php_class_call_method(Z_OBJ_P(cursor), "valid", sizeof("valid") - 1, 0, NULL, false));

        if(! is_valid )
        {
            zval* all = zend_this_read_property("all");

            zval count;
            ZVAL_LONG(&count, zend_hash_num_elements(Z_ARR_P(all)));
   
            zend_update_property(Z_OBJCE_P(getThis()), Z_OBJ_P(getThis()), "count", sizeof("count") - 1, &count);
        }
    }
    else
    {
        zval* count     = zend_this_read_property("count");
        zval* position  = zend_this_read_property("position");

        is_valid = zval_get_long(count) - zval_get_long(position);
    }

    RETURN_BOOL(is_valid);
}

PHP_METHOD(Collection, __debugInfo)
{
    ZEND_PARSE_PARAMETERS_NONE();

    zval array_result;
    zval* count = zend_this_read_property("count");
    
    if( Z_TYPE_P(count) == IS_NULL )
    {
        zval*   cursor      = zend_this_read_property("cursor");
        zval*   data_puser  = zend_this_read_property("data_puser");

        voile_data_puser *ptr_data_puser = (voile_data_puser*)data_puser->value.ptr;

        c_spl_iterator_apply(cursor, voile_cursor_to_collection_apply, (void*)ptr_data_puser);
        array_result = ptr_data_puser->return_object;

        zval long_count;
        uint64_t array_len = zend_hash_num_elements(Z_ARR(array_result));
        ZVAL_LONG(&long_count, array_len);

        zend_this_update_property("count", &long_count);
        zend_this_update_property("all", &array_result);
    }
    else
    {
        ZVAL_ZVAL(&array_result, zend_this_read_property("all"), 1, 0);
    }

    HashTable *array_value;
    ALLOC_HASHTABLE(array_value);
    zend_hash_init(array_value, 0, NULL, ZVAL_PTR_DTOR, 0);

    zval target_array_result;
    ZVAL_DUP(&target_array_result, &array_result);

    zend_hash_update_ind(array_value, zend_string_init("value", sizeof("value") - 1, 0), &target_array_result);

    RETURN_ARR(array_value);
}

PHP_METHOD(Collection, map)
{
    zval* callable;

    ZEND_PARSE_PARAMETERS_START(1, 1)
        Z_PARAM_ZVAL(callable)
    ZEND_PARSE_PARAMETERS_END();

    php_class_call_method(Z_OBJ_P(getThis()), "rewind", sizeof("rewind") - 1, 0, NULL, false);     

    zval array_result;
    array_init(&array_result);

    for( ;; )
    {
        zval* is_valid = php_class_call_method(Z_OBJ_P(getThis()), "valid", sizeof("valid") - 1, 0, NULL, false);        

        if(! zval_get_long(is_valid) )
            break;

        zval *model = php_class_call_method(Z_OBJ_P(getThis()), "current", sizeof("current") - 1, 0, NULL, false); 

        zval *params_callable = safe_emalloc(1, sizeof(zval), 0);
        ZVAL_ZVAL(&params_callable[0], model, 1, 1);

        zval *call_retval = php_call_closure(callable, Z_OBJ_P(callable), 1, params_callable);

        if( Z_TYPE_P(call_retval) != IS_NULL )
        {
            add_next_index_zval(&array_result, model);
        }

        php_class_call_method(Z_OBJ_P(getThis()), "next", sizeof("next") - 1, 0, NULL, false);   
    }

    RETURN_ZVAL(&array_result, 1, 0);
}

PHP_METHOD(Collection, filter)
{
    zval* callable;

    ZEND_PARSE_PARAMETERS_START(1, 1)
        Z_PARAM_ZVAL(callable)
    ZEND_PARSE_PARAMETERS_END();

    php_class_call_method(Z_OBJ_P(getThis()), "rewind", sizeof("rewind") - 1, 0, NULL, false);     

    zval array_result;
    array_init(&array_result);

    for( ;; )
    {
        zval* is_valid = php_class_call_method(Z_OBJ_P(getThis()), "valid", sizeof("valid") - 1, 0, NULL, false);        

        if(! zval_get_long(is_valid) )
        {
            break;
        }

        zval* model = php_class_call_method(Z_OBJ_P(getThis()), "current", sizeof("current") - 1, 0, NULL, false); 

        zval *params_callable = safe_emalloc(1, sizeof(zval), 0);
        ZVAL_ZVAL(&params_callable[0], model, 1, 1);

        zval* is_filtered = php_call_closure(callable, Z_OBJ_P(callable), 1, params_callable);

        if( is_filtered && zval_get_long(is_filtered) )
        {
            add_next_index_zval(&array_result, model);
        }

        php_class_call_method(Z_OBJ_P(getThis()), "next", sizeof("next") - 1, 0, NULL, false);   
    }

    RETURN_ZVAL(&array_result, 1, 0);
}

// Getter for only each record guest call
PHP_METHOD(Collection, __get)
{
    char*   field_src = NULL;
    size_t  field_len = 0;

    ZEND_PARSE_PARAMETERS_START(1, 1)
        Z_PARAM_STRING(field_src, field_len)
    ZEND_PARSE_PARAMETERS_END();

    zval *count = zend_this_read_property("count");

    if( Z_TYPE_P(count) == IS_NULL )
    {        
        php_class_call_method(Z_OBJ_P(getThis()), "count", sizeof("count") - 1, 0, NULL, false);
    }

    zend_ulong long_count = zval_get_long(count);

    if( long_count == 1 )
    {
        zval*   all     = zend_this_read_property("all");
        zval*   model   = zend_hash_index_find(Z_ARR_P(all), 0);

        zval *params___get = safe_emalloc(1, sizeof(zval), 0);
        ZVAL_STRINGL(&params___get[0], field_src, field_len);

        zval* field = php_class_call_method(Z_OBJ_P(model), "__get", sizeof("__get") - 1, 1, params___get, false);     

        RETURN_ZVAL(field, 1, 0);
    }

    if( long_count > 1 )
    {
        zend_throw_error(zend_ce_error_exception,
            "collection's count must be equal to 1 for using __get method"
        );
    }
    else
    {
        zend_throw_error(zend_ce_error_exception,
            "collection's is empty"
        );
    }
}

PHP_METHOD(Collection, __invoke)
{
    ZEND_PARSE_PARAMETERS_NONE();

    zval *count = zend_this_read_property("count");

    if( Z_TYPE_P(count) == IS_NULL )
    {        
        php_class_call_method(Z_OBJ_P(getThis()), "count", sizeof("count") - 1, 0, NULL, false);
    }

    zend_ulong long_count = zval_get_long(count);

    if( long_count == 1 )
    {
        zval*   all     = zend_this_read_property("all");
        zval*   model   = zend_hash_index_find(Z_ARR_P(all), 0);

        RETURN_ZVAL(model, 1, 0);
    }

    if( long_count > 1 )
    {
        zend_throw_error(zend_ce_error_exception,
            "collection's count must be equal to 1 for using __invoke method"
        );
    }
    else
    {
        zend_throw_error(zend_ce_error_exception,
            "collection's is empty"
        );
    }
}

ZEND_MINIT_FUNCTION(Zpheur_Databases_Voile_Collection)
{
    zend_class_entry ce;

    INIT_NS_CLASS_ENTRY(ce, "Zpheur\\Databases\\Voile", "Collection", zpheur_databases_voile_model_collection_class_method);
    zpheur_databases_voile_collection_class_entry = zend_register_internal_class_ex(&ce, NULL);
    zpheur_databases_voile_collection_class_entry->ce_flags |= ZEND_ACC_NO_DYNAMIC_PROPERTIES;
    // 2 it here are mean variadic extends how much parent class will be, 2 mean zend_ce_iterator and zend_ce_countable
    zend_class_implements(zpheur_databases_voile_collection_class_entry, 2, zend_ce_iterator, zend_ce_countable);

    zval zv_empty_array;
    ZVAL_EMPTY_ARRAY(&zv_empty_array);
    zend_declare_property(zpheur_databases_voile_collection_class_entry, "all", sizeof("all") - 1, &zv_empty_array, ZEND_ACC_PROTECTED);
    zend_declare_property_null(zpheur_databases_voile_collection_class_entry, "data_puser", sizeof("data_puser") - 1, ZEND_ACC_PRIVATE);
    zend_declare_property_null(zpheur_databases_voile_collection_class_entry, "cursor", sizeof("cursor") - 1, ZEND_ACC_PRIVATE);
    zend_declare_property_null(zpheur_databases_voile_collection_class_entry, "count", sizeof("count") - 1, ZEND_ACC_PRIVATE);
    zend_declare_property_null(zpheur_databases_voile_collection_class_entry, "iter", sizeof("iter") - 1, ZEND_ACC_PRIVATE);
    zend_declare_property_long(zpheur_databases_voile_collection_class_entry, "position", sizeof("position") - 1, 0, ZEND_ACC_PRIVATE);

    return SUCCESS;
}