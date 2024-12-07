PHP_ARG_ENABLE([zpheur],
  [whether to enable zpheur support],
  [AS_HELP_STRING([--enable-zpheur],
    [Enable zpheur support])],
  [no])

TARGET_CFLAG="-fcommon -lpcre2-8"

if test "$PHP_ZPHEUR" != "no"; then
  dnl AC_DEFINE(HAVE_ZPHEUR, 1, [ Have zpheur support ])

  AC_MSG_CHECKING([for PHP version])
    tmp_version=$PHP_VERSION
    if test -z "$tmp_version"; then
      if test -z "$PHP_CONFIG"; then
        AC_MSG_ERROR([php-config not found])
      fi
      php_version=`$PHP_CONFIG --version 2>/dev/null|head -n 1|sed -e 's#\([0-9]\.[0-9]*\.[0-9]*\)\(.*\)#\1#'`
    else
      php_version=`echo "$tmp_version"|sed -e 's#\([0-9]\.[0-9]*\.[0-9]*\)\(.*\)#\1#'`
    fi

    if test -z "$php_version"; then
      AC_MSG_ERROR([Failed to detect PHP version, please report])
    fi

    ac_IFS=$IFS
    IFS="."
    set $php_version
    IFS=$ac_IFS
    yaf_php_version=`expr [$]1 \* 1000000 + [$]2 \* 1000 + [$]3`

    if test "$yaf_php_version" -le "8002001"; then
      AC_MSG_ERROR([Zpheur extension only support PHP v8.2.1 or greater. PHP $php_version found])
    else
      AC_MSG_RESULT([$php_version, ok])
    fi

  AC_MSG_CHECKING([for PCRE2])
    pcre2_version=`ls /usr/include/ | grep pcre2.h`
    if test -z "$pcre2_version"; then
      AC_MSG_ERROR([require PCRE2 for the extension])
    else
      dnl AC_MSG_RESULT([$pcre2_version, ok])
      AC_MSG_RESULT([ found, ok ])
    fi

  AC_MSG_CHECKING([for OpenSSL])
    openssl_version=`ls /usr/include/openssl/ | grep ssl.h`
    if test -z "$openssl_version"; then
      AC_MSG_ERROR([require OpenSSL for the extension])
    else
      dnl AC_MSG_RESULT([$openssl_version, ok])
      AC_MSG_RESULT([ found, ok ])
    fi

  AC_MSG_CHECKING([for PHP MongoDB extension])
    mongodb_version=`php --ri mongodb | grep "extension version" | sed "s/MongoDB extension version => //g"`
    if test -z "$mongodb_version"; then
      AC_MSG_RESULT([not installed, skip Voile include])
    else
      TARGET_CFLAG="$TARGET_CFLAG -DZPHEUR_REQUIRE_VOILE"
      AC_MSG_RESULT([$mongodb_version, added Voile include])
    fi

  CFLAGS="$CFLAGS $TARGET_CFLAG"

  PHP_NEW_EXTENSION(
    [zpheur], 
    [                                                                   \
      Zpheur/DataTransforms/Dotenv/Env_function.c                       \
      Zpheur/Functions/httpverb_function.c                              \
      Zpheur/Functions/is_appns_function.c                              \
      Zpheur/Functions/clfile_function.c                                \
      Zpheur/Functions/filetoclass_function.c                           \
      Zpheur/Functions/zpheur_version_function.c                        \
                                                                        \
      Zpheur/Actions/Console/DefaultAction_class.c                      \
      Zpheur/Actions/Http/DefaultAction_class.c                         \
      Zpheur/Actions/Middleware/Next_class.c                            \
      Zpheur/Actions/Middleware_class.c                                 \
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
      zpheur_hem.c                                                      \
      zpheur.c                                                          \
    ],                                                         
  [$ext_shared], ,
  dnl [-fcommon -lpcre2-8])
  ["$TARGET_CFLAG"])
fi
