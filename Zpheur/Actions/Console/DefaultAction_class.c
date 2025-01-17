
#include <php.h>
#include <zpheur.h>
#include <ext/standard/info.h>
#include <php_zpheur.h>
#include <include/onecstr.h>
#include "DefaultAction_arginfo.h"


zend_object_handlers console_default_action_object_handlers;

typedef struct _console_default_action_common_t
{
    size_t exit_code;
} console_default_action_common_t;

typedef struct _console_default_action_object
{
    console_default_action_common_t* common;
    zend_object std;
} console_default_action_object;

void free_console_default_action_object(zend_object *object)
{
    console_default_action_object* instance = ZPHEUR_GET_OBJECT(console_default_action_object, object);

    zend_object_std_dtor(&instance->std);
    if( instance->common )
    {
        efree(instance->common);       
    }
}

zend_object* create_console_default_action_object( zend_class_entry* ce )
{
    console_default_action_object* object = 
        ecalloc(1, sizeof(console_default_action_object) + zend_object_properties_size(ce));

    zend_object_std_init(&object->std, ce);
    object_properties_init(&object->std, ce);

    memcpy(&console_default_action_object_handlers, zend_get_std_object_handlers(), sizeof(console_default_action_object_handlers));
    console_default_action_object_handlers.offset = XtOffsetOf(console_default_action_object, std);
    console_default_action_object_handlers.free_obj = free_console_default_action_object;
    object->std.handlers = &console_default_action_object_handlers;

    object->common = emalloc(sizeof(console_default_action_common_t));
    object->common->exit_code = 0;

    return &object->std;
}

PHP_METHOD(Console_DefaultAction, __construct)
{
    ZEND_PARSE_PARAMETERS_NONE();
}

ZEND_MINIT_FUNCTION(Zpheur_Actions_Console_DefaultAction)
{
    zend_class_entry ce;
    INIT_NS_CLASS_ENTRY(ce, "Zpheur\\Actions\\Console", "DefaultAction", zpheur_actions_console_defaultaction_class_method);
    zpheur_actions_console_defaultaction_class_entry = zend_register_internal_class(&ce);
    zpheur_actions_console_defaultaction_class_entry->ce_flags |= ZEND_ACC_NO_DYNAMIC_PROPERTIES;
    zpheur_actions_console_defaultaction_class_entry->create_object = create_console_default_action_object;

    return SUCCESS;
}