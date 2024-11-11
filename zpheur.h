
#ifndef H_INCLUDE_ZPHEUR
#define H_INCLUDE_ZPHEUR

#define _PHP_ERROR_HANDLER 		(1 << 0)
#define _ZCLI_ERROR_HANDLER 	(1 << 1)
#define _ZWEB_ERROR_HANDLER 	(1 << 2)

#define zend_this_update_property( name, value ) \
    zend_update_property(Z_OBJCE_P(getThis()), Z_OBJ_P(getThis()), name, sizeof(name) - 1, value)

#define zend_this_read_property( name ) \
    zend_read_property(Z_OBJCE_P(getThis()), Z_OBJ_P(getThis()), name, sizeof(name) - 1, 0, NULL)

#define zend_target_read_property_ex( ce, obj, name ) \
    zend_read_property(ce, obj, name, sizeof(name) - 1, 0, NULL)

#define zpheur_hash_str_find( src, name ) \
    zend_hash_str_find(src, name, sizeof(name) - 1)

#endif