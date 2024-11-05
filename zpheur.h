
#ifndef H_INCLUDE_ZPHEUR
#define H_INCLUDE_ZPHEUR

#define _PHP_ERROR_HANDLER 		1 << 0
#define _ZCLI_ERROR_HANDLER 	1 << 1
#define _ZWEB_ERROR_HANDLER 	1 << 2

#define zend_this_update_property( name, value ) \
    zend_update_property(Z_OBJCE_P(getThis()), Z_OBJ_P(getThis()), name, sizeof(name) - 1, value)

#define zend_this_read_property( name ) \
    zend_read_property(Z_OBJCE_P(getThis()), Z_OBJ_P(getThis()), name, sizeof(name) - 1, 0, NULL)

#define zend_target_read_property_ex( ce, obj, name ) \
    zend_read_property(ce, obj, name, sizeof(name) - 1, 0, NULL)

#define zpheur_hash_str_find( src, name ) \
    zend_hash_str_find(src, name, sizeof(name) - 1)

#define ARRAY_SHIFT( stack ) \
	zval *val; \
	uint32_t idx; \
	\
	if (HT_IS_PACKED(Z_ARRVAL_P(stack))) { \
		uint32_t k = 0; \
		\
		idx = 0; \
		while (1) { \
			if (idx == Z_ARRVAL_P(stack)->nNumUsed) { \
				return; \
			} \
			val = Z_ARRVAL_P(stack)->arPacked + idx; \
			if (Z_TYPE_P(val) != IS_UNDEF) { \
				break; \
			} \
			idx++; \
		} \
		RETVAL_COPY_DEREF(val); \
		\
		zend_hash_packed_del_val(Z_ARRVAL_P(stack), val); \
		\
		if (EXPECTED(!HT_HAS_ITERATORS(Z_ARRVAL_P(stack)))) { \
			for (idx = 0; idx < Z_ARRVAL_P(stack)->nNumUsed; idx++) { \
				val = Z_ARRVAL_P(stack)->arPacked + idx; \
				if (Z_TYPE_P(val) == IS_UNDEF) continue; \
				if (idx != k) { \
					zval *q = Z_ARRVAL_P(stack)->arPacked + k; \
					ZVAL_COPY_VALUE(q, val); \
					ZVAL_UNDEF(val); \
				} \
				k++; \
			} \
		} else { \
			uint32_t iter_pos = zend_hash_iterators_lower_pos(Z_ARRVAL_P(stack), 0); \
	\
			for (idx = 0; idx < Z_ARRVAL_P(stack)->nNumUsed; idx++) { \
				val = Z_ARRVAL_P(stack)->arPacked + idx; \
				if (Z_TYPE_P(val) == IS_UNDEF) continue; \
				if (idx != k) { \
					zval *q = Z_ARRVAL_P(stack)->arPacked + k; \
					ZVAL_COPY_VALUE(q, val); \
					ZVAL_UNDEF(val); \
					if (idx == iter_pos) { \
						zend_hash_iterators_update(Z_ARRVAL_P(stack), idx, k); \
						iter_pos = zend_hash_iterators_lower_pos(Z_ARRVAL_P(stack), iter_pos + 1); \
					} \
				} \
				k++; \
			} \
		} \
		Z_ARRVAL_P(stack)->nNumUsed = k; \
		Z_ARRVAL_P(stack)->nNextFreeElement = k; \
	} else { \
		uint32_t k = 0; \
		int should_rehash = 0; \
		Bucket *p; \
		\
		idx = 0; \
		while (1) { \
			if (idx == Z_ARRVAL_P(stack)->nNumUsed) { \
				return; \
			} \
			p = Z_ARRVAL_P(stack)->arData + idx; \
			val = &p->val; \
			if (Z_TYPE_P(val) != IS_UNDEF) { \
				break; \
			} \
			idx++; \
		} \
		RETVAL_COPY_DEREF(val); \
		\
		zend_hash_del_bucket(Z_ARRVAL_P(stack), p); \
		\
		for (idx = 0; idx < Z_ARRVAL_P(stack)->nNumUsed; idx++) { \
			p = Z_ARRVAL_P(stack)->arData + idx; \
			if (Z_TYPE(p->val) == IS_UNDEF) continue; \
			if (p->key == NULL) { \
				if (p->h != k) { \
					p->h = k++; \
					should_rehash = 1; \
				} else { \
					k++; \
				} \
			} \
		} \
		Z_ARRVAL_P(stack)->nNextFreeElement = k; \
		if (should_rehash) { \
			zend_hash_rehash(Z_ARRVAL_P(stack)); \
		} \
	} \
	\
	zend_hash_internal_pointer_reset(Z_ARRVAL_P(stack)); \


#define ARRAY_POP( stack ) \
    zval *val; \
	uint32_t idx; \
 \
	if (HT_IS_PACKED(Z_ARRVAL_P(stack))) { \
		idx = Z_ARRVAL_P(stack)->nNumUsed; \
		while (1) { \
			if (idx == 0) { \
				return; \
			} \
			idx--; \
			val = Z_ARRVAL_P(stack)->arPacked + idx; \
			if (Z_TYPE_P(val) != IS_UNDEF) { \
				break; \
			} \
		} \
		RETVAL_COPY_DEREF(val); \
 \
		if (idx == (Z_ARRVAL_P(stack)->nNextFreeElement - 1)) { \
			Z_ARRVAL_P(stack)->nNextFreeElement = Z_ARRVAL_P(stack)->nNextFreeElement - 1; \
		} \
 \
		zend_hash_packed_del_val(Z_ARRVAL_P(stack), val); \
	} else { \
		Bucket *p; \
 \
		idx = Z_ARRVAL_P(stack)->nNumUsed; \
		while (1) { \
			if (idx == 0) { \
				return; \
			} \
			idx--; \
			p = Z_ARRVAL_P(stack)->arData + idx; \
			val = &p->val; \
			if (Z_TYPE_P(val) != IS_UNDEF) { \
				break; \
			} \
		} \
		RETVAL_COPY_DEREF(val); \
 \
		if (!p->key && (zend_long)p->h == (Z_ARRVAL_P(stack)->nNextFreeElement - 1)) { \
			Z_ARRVAL_P(stack)->nNextFreeElement = Z_ARRVAL_P(stack)->nNextFreeElement - 1; \
		} \
 \
		zend_hash_del_bucket(Z_ARRVAL_P(stack), p); \
	} \
	zend_hash_internal_pointer_reset(Z_ARRVAL_P(stack));  \



#endif