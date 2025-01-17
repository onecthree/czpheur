
#ifndef H_INCLUDE_ZPHEUR
#define H_INCLUDE_ZPHEUR

#include <php.h>

#define _PHP_ERROR_HANDLER 		(1 << 0)
#define _ZCLI_ERROR_HANDLER 	(1 << 1)
#define _ZWEB_ERROR_HANDLER 	(1 << 2)

#if PHP_VERSION_ID < 80400
#define Z_PHP_ACC_PUBLIC(x)         (x & ZEND_ACC_PUBLIC)
#define Z_PHP_ACC_PROTECTED(x)      (x & ZEND_ACC_PROTECTED)
#define Z_PHP_ACC_PRIVATE(x)        (x & ZEND_ACC_PRIVATE)
#else
#define Z_PHP_ACC_PUBLIC(x)         (x & ZEND_ACC_PUBLIC) && !(x & (ZEND_ACC_PROTECTED_SET|ZEND_ACC_PRIVATE_SET))
#define Z_PHP_ACC_PROTECTED(x)      (x & (ZEND_ACC_PROTECTED|ZEND_ACC_PROTECTED_SET))
#define Z_PHP_ACC_PRIVATE(x)        (x & (ZEND_ACC_PRIVATE|ZEND_ACC_PRIVATE_SET))
#endif 

bool inline __attribute__ ((always_inline))
property_name_compare_offset_x( zend_string* source,  zend_string* target )
{
    char* target_src = target->val;
    size_t target_len = target->len;
    size_t i = 0, b = 0;

    while( i < source->len )
    {
        char c = source->val[i++];
        if( c == target_src[b++] )
            continue;
        b = 0;
    }

    return b == target_len;
}

#define ZPHEUR_GET_OBJECT(class_entry, o) (class_entry *)((char *)o - XtOffsetOf(class_entry, std))
#define ZPHEUR_ZVAL_GET_OBJECT(class_entry, z) ZPHEUR_GET_OBJECT(class_entry, Z_OBJ_P(z))

#define zend_this_update_property( name, value ) \
    zend_update_property(Z_OBJCE_P(getThis()), Z_OBJ_P(getThis()), name, sizeof(name) - 1, value)

#define zend_this_read_property( name ) \
    zend_read_property(Z_OBJCE_P(getThis()), Z_OBJ_P(getThis()), name, sizeof(name) - 1, 0, NULL)

#define zend_this_unset_property( name ) \
    zend_unset_property(Z_OBJCE_P(getThis()), Z_OBJ_P(getThis()), name, sizeof(name) - 1)

#define zend_target_read_property_ex( ce, obj, name ) \
    zend_read_property(ce, obj, name, sizeof(name) - 1, 0, NULL)

#define zpheur_hash_str_find( src, name ) \
    zend_hash_str_find(src, name, sizeof(name) - 1)
    
#define zend_static_read_property( ce, name ) \
    zend_read_static_property(ce, name, sizeof(name) - 1, 0)

#endif