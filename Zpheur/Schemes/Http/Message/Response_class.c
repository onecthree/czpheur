#include <php.h>
#include <ext/standard/info.h>
#include <php_zpheur.h>

#include <string.h>
#include <zpheur.h>
#include <include/runtime.h>
#include <include/onecstr.h>
#include "HeaderTrait_arginfo.h"
#include "Response_arginfo.h"


void free_response_object(zend_object* object)
{
    response_object* instance = ZPHEUR_GET_OBJECT(response_object, object);

    zend_object_std_dtor(&instance->std);
    if( instance->common )
    {
        if( instance->common->headers )
        {
            zend_hash_destroy(instance->common->headers);
            FREE_HASHTABLE(instance->common->headers);
        } 

        if( instance->common->output_buffer )
        {
            onec_string_release(instance->common->output_buffer);
        }

        efree(instance->common);       
    }
}

zend_object* create_response_object( zend_class_entry* ce )
{
    response_object* object = 
        ecalloc(1, sizeof(response_object) + zend_object_properties_size(ce));

    zend_object_std_init(&object->std, ce);
    object_properties_init(&object->std, ce);

    memcpy(&response_object_handlers, zend_get_std_object_handlers(), sizeof(response_object_handlers));
    response_object_handlers.offset = XtOffsetOf(response_object, std);
    response_object_handlers.free_obj = free_response_object;
    object->std.handlers = &response_object_handlers;

    object->common = ecalloc(1, sizeof(response_common_t));

    HashTable* headers;
    ALLOC_HASHTABLE(headers);
    zend_hash_init(headers, 0, NULL, ZVAL_PTR_DTOR, 0);
    object->common->write_header = true;
    object->common->headers = headers;
    object->common->output_buffer = onec_string_initd("", sizeof("") - 1);

    return &object->std;
}

void static _set_mime_type( zend_ulong utype )
{
    if( utype )
    {       
        sapi_header_line ctr = {0};

        switch( utype )
        {
            case 1: // application/json
                ctr.line = "Content-Type: application/json";
                ctr.line_len = sizeof("Content-Type: application/json") - 1;       
            break;
            case 2: // plain/text
                ctr.line = "Content-Type: text/plain";
                ctr.line_len = sizeof("Content-Type: text/plain") - 1;       
            break;
            default: // not found
                goto end;
            break;
        }

        sapi_header_op(SAPI_HEADER_REPLACE, &ctr);
    }

    end: {}
}

PHP_METHOD(Response, __construct)
{
    bool write_header = true;

    ZEND_PARSE_PARAMETERS_START(0, 1)
        Z_PARAM_OPTIONAL
        Z_PARAM_BOOL(write_header)
    ZEND_PARSE_PARAMETERS_END();

    response_object* instance = 
        ZPHEUR_ZVAL_GET_OBJECT(response_object, getThis());

    instance->common->write_header = write_header;
}

PHP_METHOD(Response, send)
{
    char*       output_buffer_src = NULL;
    size_t      output_buffer_len = 0;

    ZEND_PARSE_PARAMETERS_START(1, 1)
        Z_PARAM_STRING(output_buffer_src, output_buffer_len)
    ZEND_PARSE_PARAMETERS_END();

    response_object* instance = 
        ZPHEUR_ZVAL_GET_OBJECT(response_object, getThis());

    if( instance->common->output_buffer != NULL )
    {
        onec_string_release(instance->common->output_buffer);
        instance->common->output_buffer = NULL;
    }
    instance->common->output_buffer = onec_string_initd(output_buffer_src, output_buffer_len);

    RETURN_ZVAL(getThis(), 1, 0);
}

PHP_METHOD(Response, statusCode)
{
    zend_long response_code = 200;

    ZEND_PARSE_PARAMETERS_START(1, 1)
        Z_PARAM_LONG(response_code)
    ZEND_PARSE_PARAMETERS_END();

    response_object* instance = 
        ZPHEUR_ZVAL_GET_OBJECT(response_object, getThis());

    if( response_code >= 100 && response_code <= 599 )
    {
        if( SG(headers_sent) && !SG(request_info).no_headers && instance->common->write_header )
        {
            const char *output_start_filename = php_output_get_start_filename();
            int output_start_lineno = php_output_get_start_lineno();

            if (output_start_filename) {
                php_error_docref(NULL, E_ERROR, "Cannot set response code - headers already sent "
                    "(output started at %s:%d)", output_start_filename, output_start_lineno);
            } else {
                php_error_docref(NULL, E_ERROR, "Cannot set response code - headers already sent");
            }
        }

        if( instance->common->write_header )
        {
            zend_long old_response_code;

            old_response_code = SG(sapi_headers).http_response_code;
            SG(sapi_headers).http_response_code = (int)response_code;
        }

        zval new_response_code; ZVAL_LONG(&new_response_code, response_code);
        zend_this_update_property("code", &new_response_code);
    }
    else
    {
        if( response_code < 100 || response_code > 599 )
            zend_error(E_ERROR, "Invalid HTTP status code range given");

        zend_error(E_ERROR, "Non-standard HTTP status code given");
    }

    RETURN_ZVAL(getThis(), 1, 0);
}

PHP_METHOD(Response, set)
{
    zval*        header = 0;

    char*        value_src = NULL;
    size_t       value_len = 0;

    ZEND_PARSE_PARAMETERS_START(1, 2)
        Z_PARAM_ZVAL(header)
        Z_PARAM_OPTIONAL
        Z_PARAM_STRING(value_src, value_len)
    ZEND_PARSE_PARAMETERS_END();

    response_object* instance = 
        ZPHEUR_ZVAL_GET_OBJECT(response_object, getThis());

    if( Z_TYPE_P(header) == IS_ARRAY )
    {
        ZEND_HASH_FOREACH_STR_KEY_VAL(Z_ARR_P(header), zend_string* _header, zval* _value )
        {
            zend_string* __value = zval_get_string(_value);
            zend_hash_update(instance->common->headers, _header, _value);

            if( instance->common->write_header )
                local_set_header(_header->val, _header->len, __value->val, __value->len);
        }
        ZEND_HASH_FOREACH_END();
    }
    else
    {
        if(! value_src )
        {
            zend_error(E_ERROR, "value_src must be filled, with use inline header");
        }       
        zend_string* __header_key = zval_get_string(header);
        zend_string* __header_value = zend_string_init(value_src, value_len, 0);
        zval ___header_value; ZVAL_STR(&___header_value, __header_value);

        zend_hash_update(instance->common->headers, __header_key, &___header_value);

        if( instance->common->write_header )
            local_set_header(__header_key->val, __header_key->len, __header_value->val, __header_value->len);

        zend_string_release(__header_key);
        // zend_string_release(__header_value);
    }

    RETURN_ZVAL(getThis(), 1, 0);
}

PHP_METHOD(Response, redirect)
{
    char*        url_src = NULL;
    size_t       url_len = 0;

    zval*        options = NULL;

    ZEND_PARSE_PARAMETERS_START(1, 2)
        Z_PARAM_STRING(url_src, url_len)
        Z_PARAM_OPTIONAL
        Z_PARAM_ZVAL(options)
    ZEND_PARSE_PARAMETERS_END();

    response_object* instance = 
        ZPHEUR_ZVAL_GET_OBJECT(response_object, getThis());

    sapi_header_line ctr = {0};

    zval code;
    if( options && Z_TYPE_P(options) == IS_LONG )
    {
        switch( zval_get_long(options) )
        {
            case 301: // permanent | 301
                ZVAL_LONG(&code, 301);
                ctr.line = "HTTP/1.1 301 Moved Permanently";
                ctr.line_len = sizeof("HTTP/1.1 301 Moved Permanently") - 1;
            break;
            case 307: // temporary | 307
                ZVAL_LONG(&code, 307);
                ctr.line = "HTTP/1.1 307 Temporary Redirect";
                ctr.line_len = sizeof("HTTP/1.1 307 Temporary Redirect") - 1;
            break;
        }

    }
    else
    {
        ZVAL_LONG(&code, 302);
        ctr.line = "HTTP/1.1 302 Found";
        ctr.line_len = sizeof("HTTP/1.1 302 Found") - 1;
    }
    if( instance->common->write_header )
        sapi_header_op(SAPI_HEADER_REPLACE, &ctr);
    zend_this_update_property("code", &code);

    // url_src[url_len] = '\0';
    // size_t redirect_target_len = snprintf(NULL, 0, "Location: %s", url_src);
    // char redirect_target_src[redirect_target_len];
    // php_sprintf(redirect_target_src, "Location: %s", url_src);
    onec_stringlc redirect_target;
    onec_string_initlc(redirect_target);

    onec_string_appendlc(redirect_target, 2, "Location: ", url_src);
    onec_string_trimlc(redirect_target);

    // ctr.line = redirect_target_src;
    // ctr.line_len = redirect_target_len;
    ctr.line = redirect_target.val;
    ctr.line_len = redirect_target.len;

    if( instance->common->write_header )
        sapi_header_op(SAPI_HEADER_REPLACE, &ctr);

    zend_string* redirect = zend_string_init(url_src, url_len, 0);
    zval _redirect; ZVAL_STR(&_redirect, redirect);
    zend_this_update_property("redirect", &_redirect);
    zend_string_release(redirect);

    RETURN_ZVAL(getThis(), 1, 0);
}

PHP_METHOD(Response, getHeaders)
{
    ZEND_PARSE_PARAMETERS_NONE();

    response_object* instance = 
        ZPHEUR_ZVAL_GET_OBJECT(response_object, getThis());

    HashTable* headers;
    ALLOC_HASHTABLE(headers);
    zend_hash_init(headers, 0, NULL, ZVAL_PTR_DTOR, 0);
    zend_hash_copy(headers, instance->common->headers, zval_add_ref);

    RETURN_ARR(headers);
}

PHP_METHOD(Response, __service)
{
    ZEND_PARSE_PARAMETERS_NONE();

    RETURN_ZVAL(getThis(), 1, 0);
}

PHP_METHOD(Response, __toString)
{
    ZEND_PARSE_PARAMETERS_NONE();

    response_object* instance = 
        ZPHEUR_ZVAL_GET_OBJECT(response_object, getThis());

    zval asd; ZVAL_STRINGL(&asd, instance->common->output_buffer->val, instance->common->output_buffer->len);
    RETURN_ZVAL(&asd, 1, 0);
    // RETURN_STR(zend_string_init(
    //     instance->common->output_buffer->val, instance->common->output_buffer->len, 0
    // ));
}

ZEND_MINIT_FUNCTION(Zpheur_Schemes_Http_Message_Response)
{
    zend_class_entry ce;

    INIT_NS_CLASS_ENTRY(ce, "Zpheur\\Schemes\\Http\\Message", "Response", zpheur_schemes_http_message_response_class_method);
    zpheur_schemes_http_message_response_class_entry = zend_register_internal_class(&ce);
    zpheur_schemes_http_message_response_class_entry->ce_flags |= ZEND_ACC_NO_DYNAMIC_PROPERTIES;
    zpheur_schemes_http_message_response_class_entry->create_object = create_response_object;


    // zend_declare_property_stringl(zpheur_schemes_http_message_response_class_entry, "output_buffer", sizeof("output_buffer") - 1, "", sizeof("") - 1, ZEND_ACC_PUBLIC);
    // zend_declare_property_null(zpheur_schemes_http_message_response_class_entry, "headers", sizeof("headers") - 1, ZEND_ACC_PUBLIC);
    zend_declare_property_long(zpheur_schemes_http_message_response_class_entry, "code", sizeof("code") - 1, 200, ZEND_ACC_PUBLIC);
    zend_declare_property_null(zpheur_schemes_http_message_response_class_entry, "redirect", sizeof("redirect") - 1, ZEND_ACC_PUBLIC);

    zend_declare_class_constant_long(
        zpheur_schemes_http_message_response_class_entry, "HTTP_CONTINUE", sizeof("HTTP_CONTINUE") - 1, 100
    );
    zend_declare_class_constant_long(
        zpheur_schemes_http_message_response_class_entry, "HTTP_SWITCHING_PROTOCOLS", sizeof("HTTP_SWITCHING_PROTOCOLS") - 1, 101
    );
    zend_declare_class_constant_long(
        zpheur_schemes_http_message_response_class_entry, "HTTP_PROCESSING", sizeof("HTTP_PROCESSING") - 1, 102 // RFC2518
    );
    zend_declare_class_constant_long(
        zpheur_schemes_http_message_response_class_entry, "HTTP_EARLY_HINTS", sizeof("HTTP_EARLY_HINTS") - 1, 103 // RFC8297
    );
    zend_declare_class_constant_long(
        zpheur_schemes_http_message_response_class_entry, "HTTP_OK", sizeof("HTTP_OK") - 1, 200
    );
    zend_declare_class_constant_long(
        zpheur_schemes_http_message_response_class_entry, "HTTP_CREATED", sizeof("HTTP_CREATED") - 1, 201
    );
    zend_declare_class_constant_long(
        zpheur_schemes_http_message_response_class_entry, "HTTP_ACCEPTED", sizeof("HTTP_ACCEPTED") - 1, 202
    );
    zend_declare_class_constant_long(
        zpheur_schemes_http_message_response_class_entry, "HTTP_NON_AUTHORITATIVE_INFORMATION", sizeof("HTTP_NON_AUTHORITATIVE_INFORMATION") - 1, 203
    );
    zend_declare_class_constant_long(
        zpheur_schemes_http_message_response_class_entry, "HTTP_NO_CONTENT", sizeof("HTTP_NO_CONTENT") - 1, 204
    );
    zend_declare_class_constant_long(
        zpheur_schemes_http_message_response_class_entry, "HTTP_RESET_CONTENT", sizeof("HTTP_RESET_CONTENT") - 1, 205
    );
    zend_declare_class_constant_long(
        zpheur_schemes_http_message_response_class_entry, "HTTP_PARTIAL_CONTENT", sizeof("HTTP_PARTIAL_CONTENT") - 1, 206
    );
    zend_declare_class_constant_long(
        zpheur_schemes_http_message_response_class_entry, "HTTP_MULTI_STATUS", sizeof("HTTP_MULTI_STATUS") - 1, 207 // RFC4918
    );
    zend_declare_class_constant_long(
        zpheur_schemes_http_message_response_class_entry, "HTTP_ALREADY_REPORTED", sizeof("HTTP_ALREADY_REPORTED") - 1, 208 // RFC5842
    );
    zend_declare_class_constant_long(
        zpheur_schemes_http_message_response_class_entry, "HTTP_IM_USED", sizeof("HTTP_IM_USED") - 1, 226 // RFC3229
    );
    zend_declare_class_constant_long(
        zpheur_schemes_http_message_response_class_entry, "HTTP_MULTIPLE_CHOICES", sizeof("HTTP_MULTIPLE_CHOICES") - 1, 300
    );
    zend_declare_class_constant_long(
        zpheur_schemes_http_message_response_class_entry, "HTTP_MOVED_PERMANENTLY", sizeof("HTTP_MOVED_PERMANENTLY") - 1, 301
    );
    zend_declare_class_constant_long(
        zpheur_schemes_http_message_response_class_entry, "HTTP_FOUND", sizeof("HTTP_FOUND") - 1, 302
    );
    zend_declare_class_constant_long(
        zpheur_schemes_http_message_response_class_entry, "HTTP_SEE_OTHER", sizeof("HTTP_SEE_OTHER") - 1, 303
    );
    zend_declare_class_constant_long(
        zpheur_schemes_http_message_response_class_entry, "HTTP_NOT_MODIFIED", sizeof("HTTP_NOT_MODIFIED") - 1, 304
    );
    zend_declare_class_constant_long(
        zpheur_schemes_http_message_response_class_entry, "HTTP_USE_PROXY", sizeof("HTTP_USE_PROXY") - 1, 305
    );
    zend_declare_class_constant_long(
        zpheur_schemes_http_message_response_class_entry, "HTTP_RESERVED", sizeof("HTTP_RESERVED") - 1, 306
    );
    zend_declare_class_constant_long(
        zpheur_schemes_http_message_response_class_entry, "HTTP_TEMPORARY_REDIRECT", sizeof("HTTP_TEMPORARY_REDIRECT") - 1, 307
    );
    zend_declare_class_constant_long(
        zpheur_schemes_http_message_response_class_entry, "HTTP_PERMANENTLY_REDIRECT", sizeof("HTTP_PERMANENTLY_REDIRECT") - 1, 308 // RFC7238
    );
    zend_declare_class_constant_long(
        zpheur_schemes_http_message_response_class_entry, "HTTP_BAD_REQUEST", sizeof("HTTP_BAD_REQUEST") - 1, 400
    );
    zend_declare_class_constant_long(
        zpheur_schemes_http_message_response_class_entry, "HTTP_UNAUTHORIZED", sizeof("HTTP_UNAUTHORIZED") - 1, 401
    );
    zend_declare_class_constant_long(
        zpheur_schemes_http_message_response_class_entry, "HTTP_PAYMENT_REQUIRED", sizeof("HTTP_PAYMENT_REQUIRED") - 1, 402
    );
    zend_declare_class_constant_long(
        zpheur_schemes_http_message_response_class_entry, "HTTP_FORBIDDEN", sizeof("HTTP_FORBIDDEN") - 1, 403
    );
    zend_declare_class_constant_long(
        zpheur_schemes_http_message_response_class_entry, "HTTP_NOT_FOUND", sizeof("HTTP_NOT_FOUND") - 1, 404
    );
    zend_declare_class_constant_long(
        zpheur_schemes_http_message_response_class_entry, "HTTP_METHOD_NOT_ALLOWED", sizeof("HTTP_METHOD_NOT_ALLOWED") - 1, 405
    );
    zend_declare_class_constant_long(
        zpheur_schemes_http_message_response_class_entry, "HTTP_NOT_ACCEPTABLE", sizeof("HTTP_NOT_ACCEPTABLE") - 1, 406
    );
    zend_declare_class_constant_long(
        zpheur_schemes_http_message_response_class_entry, "HTTP_PROXY_AUTHENTICATION_REQUIRED", sizeof("HTTP_PROXY_AUTHENTICATION_REQUIRED") - 1, 407
    );
    zend_declare_class_constant_long(
        zpheur_schemes_http_message_response_class_entry, "HTTP_REQUEST_TIMEOUT", sizeof("HTTP_REQUEST_TIMEOUT") - 1, 408
    );
    zend_declare_class_constant_long(
        zpheur_schemes_http_message_response_class_entry, "HTTP_CONFLICT", sizeof("HTTP_CONFLICT") - 1, 409
    );
    zend_declare_class_constant_long(
        zpheur_schemes_http_message_response_class_entry, "HTTP_GONE", sizeof("HTTP_GONE") - 1, 410
    );
    zend_declare_class_constant_long(
        zpheur_schemes_http_message_response_class_entry, "HTTP_LENGTH_REQUIRED", sizeof("HTTP_LENGTH_REQUIRED") - 1, 411
    );
    zend_declare_class_constant_long(
        zpheur_schemes_http_message_response_class_entry, "HTTP_PRECONDITION_FAILED", sizeof("HTTP_PRECONDITION_FAILED") - 1, 412
    );
    zend_declare_class_constant_long(
        zpheur_schemes_http_message_response_class_entry, "HTTP_REQUEST_ENTITY_TOO_LARGE", sizeof("HTTP_REQUEST_ENTITY_TOO_LARGE") - 1, 413
    );
    zend_declare_class_constant_long(
        zpheur_schemes_http_message_response_class_entry, "HTTP_REQUEST_URI_TOO_LONG", sizeof("HTTP_REQUEST_URI_TOO_LONG") - 1, 414
    );
    zend_declare_class_constant_long(
        zpheur_schemes_http_message_response_class_entry, "HTTP_UNSUPPORTED_MEDIA_TYPE", sizeof("HTTP_UNSUPPORTED_MEDIA_TYPE") - 1, 415
    );
    zend_declare_class_constant_long(
        zpheur_schemes_http_message_response_class_entry, "HTTP_REQUESTED_RANGE_NOT_SATISFIABLE", sizeof("HTTP_REQUESTED_RANGE_NOT_SATISFIABLE") - 1, 416
    );
    zend_declare_class_constant_long(
        zpheur_schemes_http_message_response_class_entry, "HTTP_EXPECTATION_FAILED", sizeof("HTTP_EXPECTATION_FAILED") - 1, 417
    );
    zend_declare_class_constant_long(
        zpheur_schemes_http_message_response_class_entry, "HTTP_I_AM_A_TEAPOT", sizeof("HTTP_I_AM_A_TEAPOT") - 1, 418 // RFC2324
    );
    zend_declare_class_constant_long(
        zpheur_schemes_http_message_response_class_entry, "HTTP_MISDIRECTED_REQUEST", sizeof("HTTP_MISDIRECTED_REQUEST") - 1, 421 // RFC7540
    );
    zend_declare_class_constant_long(
        zpheur_schemes_http_message_response_class_entry, "HTTP_UNPROCESSABLE_ENTITY", sizeof("HTTP_UNPROCESSABLE_ENTITY") - 1, 422 // RFC4918
    );
    zend_declare_class_constant_long(
        zpheur_schemes_http_message_response_class_entry, "HTTP_LOCKED", sizeof("HTTP_LOCKED") - 1, 423 // RFC4918
    );
    zend_declare_class_constant_long(
        zpheur_schemes_http_message_response_class_entry, "HTTP_FAILED_DEPENDENCY", sizeof("HTTP_FAILED_DEPENDENCY") - 1, 424 // RFC4918
    );
    zend_declare_class_constant_long(
        zpheur_schemes_http_message_response_class_entry, "HTTP_TOO_EARLY", sizeof("HTTP_TOO_EARLY") - 1, 425 // RFC-ietf-httpbis-replay-04
    );
    zend_declare_class_constant_long(
        zpheur_schemes_http_message_response_class_entry, "HTTP_UPGRADE_REQUIRED", sizeof("HTTP_UPGRADE_REQUIRED") - 1, 426 // RFC2817
    );
    zend_declare_class_constant_long(
        zpheur_schemes_http_message_response_class_entry, "HTTP_PRECONDITION_REQUIRED", sizeof("HTTP_PRECONDITION_REQUIRED") - 1, 428 // RFC6585
    );
    zend_declare_class_constant_long(
        zpheur_schemes_http_message_response_class_entry, "HTTP_TOO_MANY_REQUESTS", sizeof("HTTP_TOO_MANY_REQUESTS") - 1, 429 // RFC6585
    );
    zend_declare_class_constant_long(
        zpheur_schemes_http_message_response_class_entry, "HTTP_REQUEST_HEADER_FIELDS_TOO_LARGE", sizeof("HTTP_REQUEST_HEADER_FIELDS_TOO_LARGE") - 1, 431 // RFC6585
    );
    zend_declare_class_constant_long(
        zpheur_schemes_http_message_response_class_entry, "HTTP_UNAVAILABLE_FOR_LEGAL_REASONS", sizeof("HTTP_UNAVAILABLE_FOR_LEGAL_REASONS") - 1, 451 // RFC7725
    );
    zend_declare_class_constant_long(
        zpheur_schemes_http_message_response_class_entry, "HTTP_INTERNAL_SERVER_ERROR", sizeof("HTTP_INTERNAL_SERVER_ERROR") - 1, 500
    );
    zend_declare_class_constant_long(
        zpheur_schemes_http_message_response_class_entry, "HTTP_NOT_IMPLEMENTED", sizeof("HTTP_NOT_IMPLEMENTED") - 1, 501
    );
    zend_declare_class_constant_long(
        zpheur_schemes_http_message_response_class_entry, "HTTP_BAD_GATEWAY", sizeof("HTTP_BAD_GATEWAY") - 1, 502
    );
    zend_declare_class_constant_long(
        zpheur_schemes_http_message_response_class_entry, "HTTP_SERVICE_UNAVAILABLE", sizeof("HTTP_SERVICE_UNAVAILABLE") - 1, 503
    );
    zend_declare_class_constant_long(
        zpheur_schemes_http_message_response_class_entry, "HTTP_GATEWAY_TIMEOUT", sizeof("HTTP_GATEWAY_TIMEOUT") - 1, 504
    );
    zend_declare_class_constant_long(
        zpheur_schemes_http_message_response_class_entry, "HTTP_VERSION_NOT_SUPPORTED", sizeof("HTTP_VERSION_NOT_SUPPORTED") - 1, 505
    );
    zend_declare_class_constant_long(
        zpheur_schemes_http_message_response_class_entry, "HTTP_VARIANT_ALSO_NEGOTIATES_EXPERIMENTAL", sizeof("HTTP_VARIANT_ALSO_NEGOTIATES_EXPERIMENTAL") - 1, 506 // RFC2295
    );
    zend_declare_class_constant_long(
        zpheur_schemes_http_message_response_class_entry, "HTTP_INSUFFICIENT_STORAGE", sizeof("HTTP_INSUFFICIENT_STORAGE") - 1, 507 // RFC4918
    );
    zend_declare_class_constant_long(
        zpheur_schemes_http_message_response_class_entry, "HTTP_LOOP_DETECTED", sizeof("HTTP_LOOP_DETECTED") - 1, 508 // RFC5842
    );
    zend_declare_class_constant_long(
        zpheur_schemes_http_message_response_class_entry, "HTTP_NOT_EXTENDED", sizeof("HTTP_NOT_EXTENDED") - 1, 510 // RFC2774
    );
    zend_declare_class_constant_long(
        zpheur_schemes_http_message_response_class_entry, "HTTP_NETWORK_AUTHENTICATION_REQUIRED", sizeof("HTTP_NETWORK_AUTHENTICATION_REQUIRED") - 1, 511 // RFC6585
    );

    return SUCCESS;
}