
#ifndef H_ZPHEUR_DATABASES_VOILE_MODEL
#define H_ZPHEUR_DATABASES_VOILE_MODEL

#include <php.h>
#include <ext/standard/info.h>
#include <ext/standard/php_standard.h>

#include <zend_API.h>
#include <zend_interfaces.h>
#include <zend_exceptions.h>


#define ATTR_NAMED_PARAM__DEFAULT           111229896
#define ATTR_NAMED_PARAM__REQUIRED          1253587510
#define ATTR_NAMED_PARAM__MIN               12060
#define ATTR_NAMED_PARAM__MAX               11990

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

typedef enum {
    REF_TYPE_OTHER,      /* Must be 0 */
    REF_TYPE_FUNCTION,
    REF_TYPE_GENERATOR,
    REF_TYPE_FIBER,
    REF_TYPE_PARAMETER,
    REF_TYPE_TYPE,
    REF_TYPE_PROPERTY,
    REF_TYPE_CLASS_CONSTANT,
    REF_TYPE_ATTRIBUTE
} reflection_type_t;

typedef struct {
    zval                    obj;
    void*                   ptr;
    zend_class_entry*       ce;
    reflection_type_t       ref_type;
    zend_object             zo;
} reflection_object;

static inline reflection_object *reflection_object_from_obj(zend_object *obj) {
    return (reflection_object*)((char*)(obj) - XtOffsetOf(reflection_object, zo));
}

typedef struct
{
    zval                    typepair_field;
    zval                    attribute_field;
    zval                    typelist_field;
} voile_model_typeinfo;

typedef struct
{
    zval                    model_classname;
    zend_object             model_class;
    zval                    db_name;
    zval                    db_uri;
    zval                    collection;
    zval                    filter;
    zval                    projection;
    zval                    group;
    zend_ulong              limit;
    zend_ulong              skip;
    HashTable               sort;
    zval                    manager;
    zval                    cursor;
    zval                    container;
    bool                    count;
    bool                    get_called;
    bool                    filtered;
    HashTable               document;
    HashTable               unset_doc;
    HashTable               update_doc;
    HashTable               option;
    voile_model_typeinfo    modelinfo;
} voile;

typedef struct
{
    HashTable               a_objectid;
    HashTable               a_string;
    HashTable               a_number;
} voile_abstraction_type;

typedef struct
{
    zval                    return_object;
    zval                    typelist_field;
    zval                    typepair_field;
    zval                    attribute_field;
    zend_string*            model_classname;
    zval                    _voile;
} voile_data_puser;

// zend_class_entry* zpheur_databases_voile_model_class_entry;
zend_class_entry* zpheur_databases_voile_model_class_entry;

ZEND_BEGIN_ARG_INFO_EX(Model___construct_arginfo, 0, 0, 2)
    ZEND_ARG_TYPE_INFO(0, dbUri, IS_STRING, 0)
    ZEND_ARG_TYPE_INFO(0, dbName, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(Model_filter_arginfo, 0, 1, IS_MIXED, 0)
    ZEND_ARG_TYPE_INFO(0, filter, IS_MIXED, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(Model_find_arginfo, 0, 1, IS_MIXED, 0)
    ZEND_ARG_TYPE_INFO(0, filter, IS_ARRAY | IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(Model_it_arginfo, 0, 0, IS_MIXED, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(Model_sum_arginfo, 0, 1, IS_MIXED, 0)
    ZEND_ARG_TYPE_INFO(0, field, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(Model_project_arginfo, 0, 1, IS_MIXED, 0)
                ZEND_ARG_TYPE_INFO(0, projection, IS_MIXED, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(Model_group_arginfo, 0, 1, IS_MIXED, 0)
                ZEND_ARG_TYPE_INFO(0, group, IS_MIXED, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(Model_get_arginfo, 0, 1, IS_MIXED, 0)
    ZEND_ARG_TYPE_INFO(0, useCursor, _IS_BOOL, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(Model_all_arginfo, 0, 0, IS_MIXED, 0)
ZEND_END_ARG_INFO()

// ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(Model_first_arginfo, 0, 0, IS_MIXED, 0)
// ZEND_END_ARG_INFO()

// ERROR: stub argument given return as IS_OBJECT | IS_NULL make segfault
// FIX: given return with IS_MIXED should have fix the issue
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(Model_create_arginfo, 0, 1, IS_MIXED, 0)
    ZEND_ARG_TYPE_INFO(0, data, IS_MIXED, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(Model___get_arginfo, 0, 1, IS_VOID, 0)
    ZEND_ARG_TYPE_INFO(0, name, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(Model___set_arginfo, 0, 2, IS_VOID, 0)
    ZEND_ARG_TYPE_INFO(0, name, IS_STRING, 0)
    ZEND_ARG_TYPE_INFO(0, value, IS_MIXED, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(Model_unset_arginfo, 0, 1, IS_VOID, 0)
    ZEND_ARG_TYPE_INFO(0, name, IS_STRING | IS_ARRAY, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(Model_update_arginfo, 0, 1, IS_VOID, 0)
    ZEND_ARG_TYPE_INFO(0, query, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(Model_save_arginfo, 0, 0, IS_VOID, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(Model_delete_arginfo, 0, 0, IS_MIXED, 0)
    ZEND_ARG_TYPE_INFO(0, bulk_write_detail, IS_TRUE, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(Model_limit_arginfo, 0, 1, IS_MIXED, 0)
    ZEND_ARG_TYPE_INFO(0, value, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(Model_inc_arginfo, 0, 2, IS_VOID, 0)
    ZEND_ARG_TYPE_INFO(0, name, IS_STRING, 0)
    ZEND_ARG_TYPE_INFO(0, value, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(Model_skip_arginfo, 0, 1, IS_MIXED, 0)
    ZEND_ARG_TYPE_INFO(0, value, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(Model_sort_arginfo, 0, 2, IS_MIXED, 0)
    ZEND_ARG_TYPE_INFO(0, field, IS_STRING, 0)
    ZEND_ARG_TYPE_INFO(0, value, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(Model_startTransaction_arginfo, 0, 0, IS_VOID, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(Model_commitTransaction_arginfo, 0, 0, IS_VOID, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(Model_abortTransaction_arginfo, 0, 0, IS_VOID, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(Model_count_arginfo, 0, 0, IS_VOID, 0)
ZEND_END_ARG_INFO()


PHP_METHOD(Model, __construct);
PHP_METHOD(Model, filter);
PHP_METHOD(Model, find);
PHP_METHOD(Model, it);
PHP_METHOD(Model, sum);
PHP_METHOD(Model, project);
PHP_METHOD(Model, group);
PHP_METHOD(Model, get);
PHP_METHOD(Model, all);
PHP_METHOD(Model, create);
PHP_METHOD(Model, __get);
PHP_METHOD(Model, __set);
PHP_METHOD(Model, unset);
PHP_METHOD(Model, update);
PHP_METHOD(Model, save);
PHP_METHOD(Model, delete);
PHP_METHOD(Model, limit);
PHP_METHOD(Model, inc);
PHP_METHOD(Model, skip);
PHP_METHOD(Model, sort);
PHP_METHOD(Model, startTransaction);
PHP_METHOD(Model, commitTransaction);
PHP_METHOD(Model, abortTransaction);
PHP_METHOD(Model, count);


static const zend_function_entry zpheur_databases_voile_model_class_method[] = {
    PHP_ME(Model, __construct, Model___construct_arginfo, ZEND_ACC_PUBLIC | ZEND_ACC_CTOR)
    PHP_ME(Model, filter, Model_filter_arginfo, ZEND_ACC_PUBLIC)
    PHP_ME(Model, find, Model_find_arginfo, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
    PHP_ME(Model, it, Model_it_arginfo, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
    PHP_ME(Model, sum, Model_sum_arginfo, ZEND_ACC_PUBLIC)
    PHP_ME(Model, project, Model_project_arginfo, ZEND_ACC_PUBLIC)
    PHP_ME(Model, group, Model_group_arginfo, ZEND_ACC_PUBLIC)
    PHP_ME(Model, get, Model_get_arginfo, ZEND_ACC_PUBLIC)
    PHP_ME(Model, all, Model_all_arginfo, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
    PHP_ME(Model, create, Model_create_arginfo, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
    PHP_ME(Model, __get, Model___get_arginfo, ZEND_ACC_PUBLIC)
    PHP_ME(Model, __set, Model___set_arginfo, ZEND_ACC_PUBLIC)
    PHP_ME(Model, unset, Model_unset_arginfo, ZEND_ACC_PUBLIC)
    PHP_ME(Model, update, Model_update_arginfo, ZEND_ACC_PUBLIC)
    PHP_ME(Model, save, Model_save_arginfo, ZEND_ACC_PUBLIC)
    PHP_ME(Model, delete, Model_delete_arginfo, ZEND_ACC_PUBLIC)
    PHP_ME(Model, limit, Model_limit_arginfo, ZEND_ACC_PUBLIC)
    PHP_ME(Model, inc, Model_inc_arginfo, ZEND_ACC_PUBLIC)
    PHP_ME(Model, skip, Model_skip_arginfo, ZEND_ACC_PUBLIC)
    PHP_ME(Model, sort, Model_sort_arginfo, ZEND_ACC_PUBLIC)
    PHP_ME(Model, startTransaction, Model_startTransaction_arginfo, ZEND_ACC_PUBLIC)
    PHP_ME(Model, commitTransaction, Model_commitTransaction_arginfo, ZEND_ACC_PUBLIC)
    PHP_ME(Model, abortTransaction, Model_abortTransaction_arginfo, ZEND_ACC_PUBLIC)
    PHP_ME(Model, count, Model_count_arginfo, ZEND_ACC_PUBLIC)
	ZEND_FE_END
};

#endif