PHP_ARG_ENABLE([zpheur],
  [whether to enable zpheur support],
  [AS_HELP_STRING([--enable-zpheur],
    [Enable zpheur support])],
  [no])

if test "$PHP_ZPHEUR" != "no"; then
  AC_DEFINE(HAVE_ZPHEUR, 1, [ Have zpheur support ])

  PHP_NEW_EXTENSION(
    [zpheur], 
    [                                                                   \
      Zpheur/DataTransforms/Dotenv/Env_function.c                       \
      Zpheur/Functions/httpverb_function.c                              \
      Zpheur/Functions/is_appns_function.c                              \
      Zpheur/Functions/clfile_function.c                                \
      Zpheur/Functions/filetoclass_function.c                           \
                                                                        \
      Zpheur/Actions/Http/DefaultAction_class.c                         \
      Zpheur/Actions/Reflection/ActionResolver_class.c                  \
      Zpheur/Actions/Reflection/ArgumentResolver_class.c                \
      Zpheur/Caches/Adapter/Filesystem/ItemTagHandler_class.c           \
      Zpheur/Caches/Adapter/Filesystem_class.c                          \
      Zpheur/Consoles/Input/InputArgument_class.c                       \
      Zpheur/Consoles/Runtime/Application_class.c                       \
      Zpheur/DataTransforms/Dotenv/Dotenv_class.c                       \
      Zpheur/Dependencies/ServiceLocator/Container_class.c              \
      Zpheur/Schemes/Http/Foundation/ParameterBag_class.c               \
      Zpheur/Schemes/Http/Foundation/HeaderBag_class.c                  \
      Zpheur/Schemes/Http/Foundation/InputBag_class.c                   \
      Zpheur/Schemes/Http/Foundation/Kernel_class.c                     \
      Zpheur/Schemes/Http/Foundation/Request_class.c                    \
      Zpheur/Schemes/Http/Foundation/ServerBag_class.c                  \
      Zpheur/Schemes/Http/Message/HeaderInterface_class.c               \
      Zpheur/Schemes/Http/Message/HeaderTrait_class.c                   \
      Zpheur/Schemes/Http/Message/Response_class.c                      \
      Zpheur/Schemes/Http/Middleware/Next_class.c                       \
      Zpheur/Schemes/Http/Middleware_class.c                            \
      Zpheur/Schemes/Http/Responder/ErrorRequest_class.c                \
      Zpheur/Schemes/Http/Routing/Dispatcher/DispatchItem_class.c       \
      Zpheur/Schemes/Http/Routing/Dispatcher_class.c                    \
      Zpheur/Schemes/Http/Routing/Route_class.c                         \
      Zpheur/Files/Mime_class.c                                         \
      Zpheur/Databases/Voile_class.c                                    \
      Zpheur/Databases/Voile/Model_class.c                              \
      Zpheur/Databases/Voile/Collection_class.c                         \
      Zpheur/Databases/Voile/Schema/Attributes_class.c                  \
      Zpheur/Databases/Voile/Schema/AbstractionType_class.c             \
      Zpheur/Databases/Phase_class.c                                    \
      Zpheur/Databases/Phase/Model_class.c                              \
      Zpheur/Databases/Phase/Schema/AbstractionType_class.c             \
      Zpheur/Databases/Phase/Schema/Attributes_class.c                  \
      Zpheur/Logs/StreamLog_class.c                                     \
      Zpheur/Logs/StreamLog/StreamHandler_class.c                       \
      Zpheur/Logs/StreamLog/Level_class.c                               \
      Zpheur/Strings/contains_function.c                                \
      Zpheur/Encryptions/Random/Generator/csrf_token_function.c         \
      include/libstr.c                                                  \
      include/onecstr.c                                                 \
      include/runtime.c                                                 \
      include/regexp.c                                                  \
      include/dotenv.c                                                  \
      include/furouter.c                                                \
      include/zerror.c                                                  \
      zpheur.c                                                          \
    ],                                                         
  [$ext_shared], ,
  [-fcommon -lpcre2-8])
fi
