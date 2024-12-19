srcdir = /home/onecthree/Projects/czpheur
builddir = /home/onecthree/Projects/czpheur
top_srcdir = /home/onecthree/Projects/czpheur
top_builddir = /home/onecthree/Projects/czpheur
EGREP = /usr/bin/grep -E
SED = /usr/bin/sed
CONFIGURE_COMMAND = './configure'
CONFIGURE_OPTIONS =
SHLIB_SUFFIX_NAME = so
SHLIB_DL_SUFFIX_NAME = so
AWK = nawk
shared_objects_zpheur = Zpheur/DataTransforms/Dotenv/Env_function.lo Zpheur/Functions/httpverb_function.lo Zpheur/Functions/is_appns_function.lo Zpheur/Functions/clfile_function.lo Zpheur/Functions/filetoclass_function.lo Zpheur/Functions/zpheur_version_function.lo Zpheur/Actions/Console/DefaultAction_class.lo Zpheur/Actions/Http/DefaultAction_class.lo Zpheur/Actions/WebSocket/DefaultAction_class.lo Zpheur/Actions/Middleware/Next_class.lo Zpheur/Actions/Middleware_class.lo Zpheur/Actions/Reflection/ActionResolver_class.lo Zpheur/Actions/Reflection/ArgumentResolver_class.lo Zpheur/Caches/Adapter/Filesystem/ItemTagHandler_class.lo Zpheur/Caches/Adapter/Filesystem_class.lo Zpheur/Consoles/Input/InputArgument_class.lo Zpheur/Consoles/Runtime/Application_class.lo Zpheur/DataTransforms/Dotenv/Dotenv_class.lo Zpheur/Dependencies/ServiceLocator/Container_class.lo Zpheur/Schemes/Http/Foundation/ParameterBag_class.lo Zpheur/Schemes/Http/Foundation/HeaderBag_class.lo Zpheur/Schemes/Http/Foundation/InputBag_class.lo Zpheur/Schemes/Http/Foundation/Kernel_class.lo Zpheur/Schemes/Http/Foundation/Request_class.lo Zpheur/Schemes/Http/Foundation/ServerBag_class.lo Zpheur/Schemes/Http/Message/HeaderInterface_class.lo Zpheur/Schemes/Http/Message/HeaderTrait_class.lo Zpheur/Schemes/Http/Message/Response_class.lo Zpheur/Schemes/Http/Responder/ErrorRequest_class.lo Zpheur/Schemes/Http/Routing/Dispatcher/DispatchItem_class.lo Zpheur/Schemes/Http/Routing/Dispatcher_class.lo Zpheur/Schemes/Http/Routing/Route_class.lo Zpheur/Files/Mime_class.lo Zpheur/Databases/Voile_class.lo Zpheur/Databases/Voile/Model_class.lo Zpheur/Databases/Voile/Collection_class.lo Zpheur/Databases/Voile/Schema/Attributes_class.lo Zpheur/Databases/Voile/Schema/AbstractionType_class.lo Zpheur/Databases/Phase_class.lo Zpheur/Databases/Phase/Model_class.lo Zpheur/Databases/Phase/Schema/AbstractionType_class.lo Zpheur/Databases/Phase/Schema/Attributes_class.lo Zpheur/Logs/StreamLog_class.lo Zpheur/Logs/StreamLog/StreamHandler_class.lo Zpheur/Logs/StreamLog/Level_class.lo Zpheur/Strings/contains_function.lo Zpheur/Encryptions/Random/Generator/csrf_token_function.lo include/libstr.lo include/onecstr.lo include/runtime.lo include/regexp.lo include/dotenv.lo include/furouter.lo include/zerror.lo zpheur_hem.lo zpheur.lo
PHP_PECL_EXTENSION = zpheur
PHP_MODULES = $(phplibdir)/zpheur.la
PHP_ZEND_EX =
all_targets = $(PHP_MODULES) $(PHP_ZEND_EX)
install_targets = install-modules install-headers
prefix = /usr
exec_prefix = $(prefix)
libdir = ${exec_prefix}/lib
prefix = /usr
phplibdir = /home/onecthree/Projects/czpheur/modules
phpincludedir = /usr/include/php/20220829
CC = cc
CFLAGS = -g -O2 -fcommon -lpcre2-8 -DZPHEUR_REQUIRE_VOILE
CFLAGS_CLEAN = $(CFLAGS) -D_GNU_SOURCE
CPP = cc -E
CPPFLAGS = -DHAVE_CONFIG_H
CXX =
CXXFLAGS =
CXXFLAGS_CLEAN = $(CXXFLAGS)
EXTENSION_DIR = /usr/lib/php/20220829
PHP_EXECUTABLE = /usr/bin/php8.2
EXTRA_LDFLAGS =
EXTRA_LIBS =
INCLUDES = -I/usr/include/php/20220829 -I/usr/include/php/20220829/main -I/usr/include/php/20220829/TSRM -I/usr/include/php/20220829/Zend -I/usr/include/php/20220829/ext -I/usr/include/php/20220829/ext/date/lib
LFLAGS =
LDFLAGS =
SHARED_LIBTOOL =
LIBTOOL = $(SHELL) $(top_builddir)/libtool
SHELL = /bin/bash
INSTALL_HEADERS =
BUILD_CC = cc
mkinstalldirs = $(top_srcdir)/build/shtool mkdir -p
INSTALL = $(top_srcdir)/build/shtool install -c
INSTALL_DATA = $(INSTALL) -m 644

DEFS = -I$(top_builddir)/include -I$(top_builddir)/main -I$(top_srcdir)
COMMON_FLAGS = $(DEFS) $(INCLUDES) $(EXTRA_INCLUDES) $(CPPFLAGS) $(PHP_FRAMEWORKPATH)

all: $(all_targets)
	@echo
	@echo "Build complete."
	@echo "Don't forget to run 'make test'."
	@echo

build-modules: $(PHP_MODULES) $(PHP_ZEND_EX)

build-binaries: $(PHP_BINARIES)

libphp.la: $(PHP_GLOBAL_OBJS) $(PHP_SAPI_OBJS)
	$(LIBTOOL) --tag=CC --mode=link $(CC) $(LIBPHP_CFLAGS) $(CFLAGS) $(EXTRA_CFLAGS) -rpath $(phptempdir) $(EXTRA_LDFLAGS) $(LDFLAGS) $(PHP_RPATHS) $(PHP_GLOBAL_OBJS) $(PHP_SAPI_OBJS) $(EXTRA_LIBS) $(ZEND_EXTRA_LIBS) -o $@
	-@$(LIBTOOL) --silent --tag=CC --mode=install cp $@ $(phptempdir)/$@ >/dev/null 2>&1

libs/libphp.bundle: $(PHP_GLOBAL_OBJS) $(PHP_SAPI_OBJS)
	$(CC) $(MH_BUNDLE_FLAGS) $(CFLAGS_CLEAN) $(EXTRA_CFLAGS) $(LDFLAGS) $(EXTRA_LDFLAGS) $(PHP_GLOBAL_OBJS:.lo=.o) $(PHP_SAPI_OBJS:.lo=.o) $(PHP_FRAMEWORKS) $(EXTRA_LIBS) $(ZEND_EXTRA_LIBS) -o $@ && cp $@ libs/libphp.so

install: $(all_targets) $(install_targets)

install-sapi: $(OVERALL_TARGET)
	@echo "Installing PHP SAPI module:       $(PHP_SAPI)"
	-@$(mkinstalldirs) $(INSTALL_ROOT)$(bindir)
	-@if test ! -r $(phptempdir)/libphp.$(SHLIB_DL_SUFFIX_NAME); then \
		for i in 0.0.0 0.0 0; do \
			if test -r $(phptempdir)/libphp.$(SHLIB_DL_SUFFIX_NAME).$$i; then \
				$(LN_S) $(phptempdir)/libphp.$(SHLIB_DL_SUFFIX_NAME).$$i $(phptempdir)/libphp.$(SHLIB_DL_SUFFIX_NAME); \
				break; \
			fi; \
		done; \
	fi
	@$(INSTALL_IT)

install-binaries: build-binaries $(install_binary_targets)

install-modules: build-modules
	@test -d modules && \
	$(mkinstalldirs) $(INSTALL_ROOT)$(EXTENSION_DIR)
	@echo "Installing shared extensions:     $(INSTALL_ROOT)$(EXTENSION_DIR)/"
	@rm -f modules/*.la >/dev/null 2>&1
	@$(INSTALL) modules/* $(INSTALL_ROOT)$(EXTENSION_DIR)

install-headers:
	-@if test "$(INSTALL_HEADERS)"; then \
		for i in `echo $(INSTALL_HEADERS)`; do \
			i=`$(top_srcdir)/build/shtool path -d $$i`; \
			paths="$$paths $(INSTALL_ROOT)$(phpincludedir)/$$i"; \
		done; \
		$(mkinstalldirs) $$paths && \
		echo "Installing header files:          $(INSTALL_ROOT)$(phpincludedir)/" && \
		for i in `echo $(INSTALL_HEADERS)`; do \
			if test "$(PHP_PECL_EXTENSION)"; then \
				src=`echo $$i | $(SED) -e "s#ext/$(PHP_PECL_EXTENSION)/##g"`; \
			else \
				src=$$i; \
			fi; \
			if test -f "$(top_srcdir)/$$src"; then \
				$(INSTALL_DATA) $(top_srcdir)/$$src $(INSTALL_ROOT)$(phpincludedir)/$$i; \
			elif test -f "$(top_builddir)/$$src"; then \
				$(INSTALL_DATA) $(top_builddir)/$$src $(INSTALL_ROOT)$(phpincludedir)/$$i; \
			else \
				(cd $(top_srcdir)/$$src && $(INSTALL_DATA) *.h $(INSTALL_ROOT)$(phpincludedir)/$$i; \
				cd $(top_builddir)/$$src && $(INSTALL_DATA) *.h $(INSTALL_ROOT)$(phpincludedir)/$$i) 2>/dev/null || true; \
			fi \
		done; \
	fi

PHP_TEST_SETTINGS = -d 'open_basedir=' -d 'output_buffering=0' -d 'memory_limit=-1'
PHP_TEST_SHARED_EXTENSIONS =  ` \
	if test "x$(PHP_MODULES)" != "x"; then \
		for i in $(PHP_MODULES)""; do \
			. $$i; \
			if test "x$$dlname" != "xdl_test.so"; then \
				$(top_srcdir)/build/shtool echo -n -- " -d extension=$$dlname"; \
			fi; \
		done; \
	fi; \
	if test "x$(PHP_ZEND_EX)" != "x"; then \
		for i in $(PHP_ZEND_EX)""; do \
			. $$i; $(top_srcdir)/build/shtool echo -n -- " -d zend_extension=$(top_builddir)/modules/$$dlname"; \
		done; \
	fi`
PHP_DEPRECATED_DIRECTIVES_REGEX = '^(magic_quotes_(gpc|runtime|sybase)?|(zend_)?extension(_debug)?(_ts)?)[\t\ ]*='

test: all
	@if test ! -z "$(PHP_EXECUTABLE)" && test -x "$(PHP_EXECUTABLE)"; then \
		INI_FILE=`$(PHP_EXECUTABLE) -d 'display_errors=stderr' -r 'echo php_ini_loaded_file();' 2> /dev/null`; \
		if test "$$INI_FILE"; then \
			$(EGREP) -h -v $(PHP_DEPRECATED_DIRECTIVES_REGEX) "$$INI_FILE" > $(top_builddir)/tmp-php.ini; \
		else \
			echo > $(top_builddir)/tmp-php.ini; \
		fi; \
		INI_SCANNED_PATH=`$(PHP_EXECUTABLE) -d 'display_errors=stderr' -r '$$a = explode(",\n", trim(php_ini_scanned_files())); echo $$a[0];' 2> /dev/null`; \
		if test "$$INI_SCANNED_PATH"; then \
			INI_SCANNED_PATH=`$(top_srcdir)/build/shtool path -d $$INI_SCANNED_PATH`; \
			$(EGREP) -h -v $(PHP_DEPRECATED_DIRECTIVES_REGEX) "$$INI_SCANNED_PATH"/*.ini >> $(top_builddir)/tmp-php.ini; \
		fi; \
		TEST_PHP_EXECUTABLE=$(PHP_EXECUTABLE) \
		TEST_PHP_SRCDIR=$(top_srcdir) \
		CC="$(CC)" \
			$(PHP_EXECUTABLE) -n -c $(top_builddir)/tmp-php.ini $(PHP_TEST_SETTINGS) $(top_srcdir)/run-tests.php -n -c $(top_builddir)/tmp-php.ini -d extension_dir=$(top_builddir)/modules/ $(PHP_TEST_SHARED_EXTENSIONS) $(TESTS); \
		TEST_RESULT_EXIT_CODE=$$?; \
		rm $(top_builddir)/tmp-php.ini; \
		exit $$TEST_RESULT_EXIT_CODE; \
	else \
		echo "ERROR: Cannot run tests without CLI sapi."; \
	fi

clean:
	find . -name \*.gcno -o -name \*.gcda | xargs rm -f
	find . -name \*.lo -o -name \*.o -o -name \*.dep | xargs rm -f
	find . -name \*.la -o -name \*.a | xargs rm -f
	find . -name \*.so | xargs rm -f
	find . -name .libs -a -type d|xargs rm -rf
	rm -f libphp.la $(SAPI_CLI_PATH) $(SAPI_CGI_PATH) $(SAPI_LITESPEED_PATH) $(SAPI_FPM_PATH) $(OVERALL_TARGET) modules/* libs/*
	rm -f ext/opcache/jit/zend_jit_x86.c
	rm -f ext/opcache/jit/zend_jit_arm64.c
	rm -f ext/opcache/minilua

distclean: clean
	rm -f Makefile config.cache config.log config.status Makefile.objects Makefile.fragments libtool main/php_config.h main/internal_functions_cli.c main/internal_functions.c Zend/zend_dtrace_gen.h Zend/zend_dtrace_gen.h.bak Zend/zend_config.h
	rm -f main/build-defs.h scripts/phpize
	rm -f ext/date/lib/timelib_config.h ext/mbstring/libmbfl/config.h ext/oci8/oci8_dtrace_gen.h ext/oci8/oci8_dtrace_gen.h.bak
	rm -f scripts/man1/phpize.1 scripts/php-config scripts/man1/php-config.1 sapi/cli/php.1 sapi/cgi/php-cgi.1 sapi/phpdbg/phpdbg.1 ext/phar/phar.1 ext/phar/phar.phar.1
	rm -f sapi/fpm/php-fpm.conf sapi/fpm/init.d.php-fpm sapi/fpm/php-fpm.service sapi/fpm/php-fpm.8 sapi/fpm/status.html
	rm -f ext/phar/phar.phar ext/phar/phar.php
	if test "$(srcdir)" != "$(builddir)"; then \
	  rm -f ext/phar/phar/phar.inc; \
	fi
	$(EGREP) define'.*include/php' $(top_srcdir)/configure | $(SED) 's/.*>//'|xargs rm -f

prof-gen:
	CCACHE_DISABLE=1 $(MAKE) PROF_FLAGS=-fprofile-generate all
	find . -name \*.gcda | xargs rm -f

prof-clean:
	find . -name \*.lo -o -name \*.o | xargs rm -f
	find . -name \*.la -o -name \*.a | xargs rm -f
	find . -name \*.so | xargs rm -f
	rm -f libphp.la $(SAPI_CLI_PATH) $(SAPI_CGI_PATH) $(SAPI_LITESPEED_PATH) $(SAPI_FPM_PATH) $(OVERALL_TARGET) modules/* libs/*

prof-use:
	CCACHE_DISABLE=1 $(MAKE) PROF_FLAGS=-fprofile-use all

%_arginfo.h: %.stub.php
	@if test -e "$(top_srcdir)/build/gen_stub.php"; then \
		if test ! -z "$(PHP)"; then \
			echo Parse $< to generate $@;\
			$(PHP) $(top_srcdir)/build/gen_stub.php $<; \
		elif test ! -z "$(PHP_EXECUTABLE)" && test -x "$(PHP_EXECUTABLE)"; then \
			echo Parse $< to generate $@;\
			$(PHP_EXECUTABLE) $(top_srcdir)/build/gen_stub.php $<; \
		fi; \
	fi;

.PHONY: all clean install distclean test prof-gen prof-clean prof-use
.NOEXPORT:
-include Zpheur/DataTransforms/Dotenv/Env_function.dep
Zpheur/DataTransforms/Dotenv/Env_function.lo: /home/onecthree/Projects/czpheur/Zpheur/DataTransforms/Dotenv/Env_function.c
	$(LIBTOOL) --tag=CC --mode=compile $(CC) -I. -I/home/onecthree/Projects/czpheur $(COMMON_FLAGS) $(CFLAGS_CLEAN) $(EXTRA_CFLAGS)    -fcommon -lpcre2-8 -DZPHEUR_REQUIRE_VOILE -DZEND_COMPILE_DL_EXT=1 -c /home/onecthree/Projects/czpheur/Zpheur/DataTransforms/Dotenv/Env_function.c -o Zpheur/DataTransforms/Dotenv/Env_function.lo  -MMD -MF Zpheur/DataTransforms/Dotenv/Env_function.dep -MT Zpheur/DataTransforms/Dotenv/Env_function.lo
-include Zpheur/Functions/httpverb_function.dep
Zpheur/Functions/httpverb_function.lo: /home/onecthree/Projects/czpheur/Zpheur/Functions/httpverb_function.c
	$(LIBTOOL) --tag=CC --mode=compile $(CC) -I. -I/home/onecthree/Projects/czpheur $(COMMON_FLAGS) $(CFLAGS_CLEAN) $(EXTRA_CFLAGS)    -fcommon -lpcre2-8 -DZPHEUR_REQUIRE_VOILE -DZEND_COMPILE_DL_EXT=1 -c /home/onecthree/Projects/czpheur/Zpheur/Functions/httpverb_function.c -o Zpheur/Functions/httpverb_function.lo  -MMD -MF Zpheur/Functions/httpverb_function.dep -MT Zpheur/Functions/httpverb_function.lo
-include Zpheur/Functions/is_appns_function.dep
Zpheur/Functions/is_appns_function.lo: /home/onecthree/Projects/czpheur/Zpheur/Functions/is_appns_function.c
	$(LIBTOOL) --tag=CC --mode=compile $(CC) -I. -I/home/onecthree/Projects/czpheur $(COMMON_FLAGS) $(CFLAGS_CLEAN) $(EXTRA_CFLAGS)    -fcommon -lpcre2-8 -DZPHEUR_REQUIRE_VOILE -DZEND_COMPILE_DL_EXT=1 -c /home/onecthree/Projects/czpheur/Zpheur/Functions/is_appns_function.c -o Zpheur/Functions/is_appns_function.lo  -MMD -MF Zpheur/Functions/is_appns_function.dep -MT Zpheur/Functions/is_appns_function.lo
-include Zpheur/Functions/clfile_function.dep
Zpheur/Functions/clfile_function.lo: /home/onecthree/Projects/czpheur/Zpheur/Functions/clfile_function.c
	$(LIBTOOL) --tag=CC --mode=compile $(CC) -I. -I/home/onecthree/Projects/czpheur $(COMMON_FLAGS) $(CFLAGS_CLEAN) $(EXTRA_CFLAGS)    -fcommon -lpcre2-8 -DZPHEUR_REQUIRE_VOILE -DZEND_COMPILE_DL_EXT=1 -c /home/onecthree/Projects/czpheur/Zpheur/Functions/clfile_function.c -o Zpheur/Functions/clfile_function.lo  -MMD -MF Zpheur/Functions/clfile_function.dep -MT Zpheur/Functions/clfile_function.lo
-include Zpheur/Functions/filetoclass_function.dep
Zpheur/Functions/filetoclass_function.lo: /home/onecthree/Projects/czpheur/Zpheur/Functions/filetoclass_function.c
	$(LIBTOOL) --tag=CC --mode=compile $(CC) -I. -I/home/onecthree/Projects/czpheur $(COMMON_FLAGS) $(CFLAGS_CLEAN) $(EXTRA_CFLAGS)    -fcommon -lpcre2-8 -DZPHEUR_REQUIRE_VOILE -DZEND_COMPILE_DL_EXT=1 -c /home/onecthree/Projects/czpheur/Zpheur/Functions/filetoclass_function.c -o Zpheur/Functions/filetoclass_function.lo  -MMD -MF Zpheur/Functions/filetoclass_function.dep -MT Zpheur/Functions/filetoclass_function.lo
-include Zpheur/Functions/zpheur_version_function.dep
Zpheur/Functions/zpheur_version_function.lo: /home/onecthree/Projects/czpheur/Zpheur/Functions/zpheur_version_function.c
	$(LIBTOOL) --tag=CC --mode=compile $(CC) -I. -I/home/onecthree/Projects/czpheur $(COMMON_FLAGS) $(CFLAGS_CLEAN) $(EXTRA_CFLAGS)    -fcommon -lpcre2-8 -DZPHEUR_REQUIRE_VOILE -DZEND_COMPILE_DL_EXT=1 -c /home/onecthree/Projects/czpheur/Zpheur/Functions/zpheur_version_function.c -o Zpheur/Functions/zpheur_version_function.lo  -MMD -MF Zpheur/Functions/zpheur_version_function.dep -MT Zpheur/Functions/zpheur_version_function.lo
-include Zpheur/Actions/Console/DefaultAction_class.dep
Zpheur/Actions/Console/DefaultAction_class.lo: /home/onecthree/Projects/czpheur/Zpheur/Actions/Console/DefaultAction_class.c
	$(LIBTOOL) --tag=CC --mode=compile $(CC) -I. -I/home/onecthree/Projects/czpheur $(COMMON_FLAGS) $(CFLAGS_CLEAN) $(EXTRA_CFLAGS)    -fcommon -lpcre2-8 -DZPHEUR_REQUIRE_VOILE -DZEND_COMPILE_DL_EXT=1 -c /home/onecthree/Projects/czpheur/Zpheur/Actions/Console/DefaultAction_class.c -o Zpheur/Actions/Console/DefaultAction_class.lo  -MMD -MF Zpheur/Actions/Console/DefaultAction_class.dep -MT Zpheur/Actions/Console/DefaultAction_class.lo
-include Zpheur/Actions/Http/DefaultAction_class.dep
Zpheur/Actions/Http/DefaultAction_class.lo: /home/onecthree/Projects/czpheur/Zpheur/Actions/Http/DefaultAction_class.c
	$(LIBTOOL) --tag=CC --mode=compile $(CC) -I. -I/home/onecthree/Projects/czpheur $(COMMON_FLAGS) $(CFLAGS_CLEAN) $(EXTRA_CFLAGS)    -fcommon -lpcre2-8 -DZPHEUR_REQUIRE_VOILE -DZEND_COMPILE_DL_EXT=1 -c /home/onecthree/Projects/czpheur/Zpheur/Actions/Http/DefaultAction_class.c -o Zpheur/Actions/Http/DefaultAction_class.lo  -MMD -MF Zpheur/Actions/Http/DefaultAction_class.dep -MT Zpheur/Actions/Http/DefaultAction_class.lo
-include Zpheur/Actions/WebSocket/DefaultAction_class.dep
Zpheur/Actions/WebSocket/DefaultAction_class.lo: /home/onecthree/Projects/czpheur/Zpheur/Actions/WebSocket/DefaultAction_class.c
	$(LIBTOOL) --tag=CC --mode=compile $(CC) -I. -I/home/onecthree/Projects/czpheur $(COMMON_FLAGS) $(CFLAGS_CLEAN) $(EXTRA_CFLAGS)    -fcommon -lpcre2-8 -DZPHEUR_REQUIRE_VOILE -DZEND_COMPILE_DL_EXT=1 -c /home/onecthree/Projects/czpheur/Zpheur/Actions/WebSocket/DefaultAction_class.c -o Zpheur/Actions/WebSocket/DefaultAction_class.lo  -MMD -MF Zpheur/Actions/WebSocket/DefaultAction_class.dep -MT Zpheur/Actions/WebSocket/DefaultAction_class.lo
-include Zpheur/Actions/Middleware/Next_class.dep
Zpheur/Actions/Middleware/Next_class.lo: /home/onecthree/Projects/czpheur/Zpheur/Actions/Middleware/Next_class.c
	$(LIBTOOL) --tag=CC --mode=compile $(CC) -I. -I/home/onecthree/Projects/czpheur $(COMMON_FLAGS) $(CFLAGS_CLEAN) $(EXTRA_CFLAGS)    -fcommon -lpcre2-8 -DZPHEUR_REQUIRE_VOILE -DZEND_COMPILE_DL_EXT=1 -c /home/onecthree/Projects/czpheur/Zpheur/Actions/Middleware/Next_class.c -o Zpheur/Actions/Middleware/Next_class.lo  -MMD -MF Zpheur/Actions/Middleware/Next_class.dep -MT Zpheur/Actions/Middleware/Next_class.lo
-include Zpheur/Actions/Middleware_class.dep
Zpheur/Actions/Middleware_class.lo: /home/onecthree/Projects/czpheur/Zpheur/Actions/Middleware_class.c
	$(LIBTOOL) --tag=CC --mode=compile $(CC) -I. -I/home/onecthree/Projects/czpheur $(COMMON_FLAGS) $(CFLAGS_CLEAN) $(EXTRA_CFLAGS)    -fcommon -lpcre2-8 -DZPHEUR_REQUIRE_VOILE -DZEND_COMPILE_DL_EXT=1 -c /home/onecthree/Projects/czpheur/Zpheur/Actions/Middleware_class.c -o Zpheur/Actions/Middleware_class.lo  -MMD -MF Zpheur/Actions/Middleware_class.dep -MT Zpheur/Actions/Middleware_class.lo
-include Zpheur/Actions/Reflection/ActionResolver_class.dep
Zpheur/Actions/Reflection/ActionResolver_class.lo: /home/onecthree/Projects/czpheur/Zpheur/Actions/Reflection/ActionResolver_class.c
	$(LIBTOOL) --tag=CC --mode=compile $(CC) -I. -I/home/onecthree/Projects/czpheur $(COMMON_FLAGS) $(CFLAGS_CLEAN) $(EXTRA_CFLAGS)    -fcommon -lpcre2-8 -DZPHEUR_REQUIRE_VOILE -DZEND_COMPILE_DL_EXT=1 -c /home/onecthree/Projects/czpheur/Zpheur/Actions/Reflection/ActionResolver_class.c -o Zpheur/Actions/Reflection/ActionResolver_class.lo  -MMD -MF Zpheur/Actions/Reflection/ActionResolver_class.dep -MT Zpheur/Actions/Reflection/ActionResolver_class.lo
-include Zpheur/Actions/Reflection/ArgumentResolver_class.dep
Zpheur/Actions/Reflection/ArgumentResolver_class.lo: /home/onecthree/Projects/czpheur/Zpheur/Actions/Reflection/ArgumentResolver_class.c
	$(LIBTOOL) --tag=CC --mode=compile $(CC) -I. -I/home/onecthree/Projects/czpheur $(COMMON_FLAGS) $(CFLAGS_CLEAN) $(EXTRA_CFLAGS)    -fcommon -lpcre2-8 -DZPHEUR_REQUIRE_VOILE -DZEND_COMPILE_DL_EXT=1 -c /home/onecthree/Projects/czpheur/Zpheur/Actions/Reflection/ArgumentResolver_class.c -o Zpheur/Actions/Reflection/ArgumentResolver_class.lo  -MMD -MF Zpheur/Actions/Reflection/ArgumentResolver_class.dep -MT Zpheur/Actions/Reflection/ArgumentResolver_class.lo
-include Zpheur/Caches/Adapter/Filesystem/ItemTagHandler_class.dep
Zpheur/Caches/Adapter/Filesystem/ItemTagHandler_class.lo: /home/onecthree/Projects/czpheur/Zpheur/Caches/Adapter/Filesystem/ItemTagHandler_class.c
	$(LIBTOOL) --tag=CC --mode=compile $(CC) -I. -I/home/onecthree/Projects/czpheur $(COMMON_FLAGS) $(CFLAGS_CLEAN) $(EXTRA_CFLAGS)    -fcommon -lpcre2-8 -DZPHEUR_REQUIRE_VOILE -DZEND_COMPILE_DL_EXT=1 -c /home/onecthree/Projects/czpheur/Zpheur/Caches/Adapter/Filesystem/ItemTagHandler_class.c -o Zpheur/Caches/Adapter/Filesystem/ItemTagHandler_class.lo  -MMD -MF Zpheur/Caches/Adapter/Filesystem/ItemTagHandler_class.dep -MT Zpheur/Caches/Adapter/Filesystem/ItemTagHandler_class.lo
-include Zpheur/Caches/Adapter/Filesystem_class.dep
Zpheur/Caches/Adapter/Filesystem_class.lo: /home/onecthree/Projects/czpheur/Zpheur/Caches/Adapter/Filesystem_class.c
	$(LIBTOOL) --tag=CC --mode=compile $(CC) -I. -I/home/onecthree/Projects/czpheur $(COMMON_FLAGS) $(CFLAGS_CLEAN) $(EXTRA_CFLAGS)    -fcommon -lpcre2-8 -DZPHEUR_REQUIRE_VOILE -DZEND_COMPILE_DL_EXT=1 -c /home/onecthree/Projects/czpheur/Zpheur/Caches/Adapter/Filesystem_class.c -o Zpheur/Caches/Adapter/Filesystem_class.lo  -MMD -MF Zpheur/Caches/Adapter/Filesystem_class.dep -MT Zpheur/Caches/Adapter/Filesystem_class.lo
-include Zpheur/Consoles/Input/InputArgument_class.dep
Zpheur/Consoles/Input/InputArgument_class.lo: /home/onecthree/Projects/czpheur/Zpheur/Consoles/Input/InputArgument_class.c
	$(LIBTOOL) --tag=CC --mode=compile $(CC) -I. -I/home/onecthree/Projects/czpheur $(COMMON_FLAGS) $(CFLAGS_CLEAN) $(EXTRA_CFLAGS)    -fcommon -lpcre2-8 -DZPHEUR_REQUIRE_VOILE -DZEND_COMPILE_DL_EXT=1 -c /home/onecthree/Projects/czpheur/Zpheur/Consoles/Input/InputArgument_class.c -o Zpheur/Consoles/Input/InputArgument_class.lo  -MMD -MF Zpheur/Consoles/Input/InputArgument_class.dep -MT Zpheur/Consoles/Input/InputArgument_class.lo
-include Zpheur/Consoles/Runtime/Application_class.dep
Zpheur/Consoles/Runtime/Application_class.lo: /home/onecthree/Projects/czpheur/Zpheur/Consoles/Runtime/Application_class.c
	$(LIBTOOL) --tag=CC --mode=compile $(CC) -I. -I/home/onecthree/Projects/czpheur $(COMMON_FLAGS) $(CFLAGS_CLEAN) $(EXTRA_CFLAGS)    -fcommon -lpcre2-8 -DZPHEUR_REQUIRE_VOILE -DZEND_COMPILE_DL_EXT=1 -c /home/onecthree/Projects/czpheur/Zpheur/Consoles/Runtime/Application_class.c -o Zpheur/Consoles/Runtime/Application_class.lo  -MMD -MF Zpheur/Consoles/Runtime/Application_class.dep -MT Zpheur/Consoles/Runtime/Application_class.lo
-include Zpheur/DataTransforms/Dotenv/Dotenv_class.dep
Zpheur/DataTransforms/Dotenv/Dotenv_class.lo: /home/onecthree/Projects/czpheur/Zpheur/DataTransforms/Dotenv/Dotenv_class.c
	$(LIBTOOL) --tag=CC --mode=compile $(CC) -I. -I/home/onecthree/Projects/czpheur $(COMMON_FLAGS) $(CFLAGS_CLEAN) $(EXTRA_CFLAGS)    -fcommon -lpcre2-8 -DZPHEUR_REQUIRE_VOILE -DZEND_COMPILE_DL_EXT=1 -c /home/onecthree/Projects/czpheur/Zpheur/DataTransforms/Dotenv/Dotenv_class.c -o Zpheur/DataTransforms/Dotenv/Dotenv_class.lo  -MMD -MF Zpheur/DataTransforms/Dotenv/Dotenv_class.dep -MT Zpheur/DataTransforms/Dotenv/Dotenv_class.lo
-include Zpheur/Dependencies/ServiceLocator/Container_class.dep
Zpheur/Dependencies/ServiceLocator/Container_class.lo: /home/onecthree/Projects/czpheur/Zpheur/Dependencies/ServiceLocator/Container_class.c
	$(LIBTOOL) --tag=CC --mode=compile $(CC) -I. -I/home/onecthree/Projects/czpheur $(COMMON_FLAGS) $(CFLAGS_CLEAN) $(EXTRA_CFLAGS)    -fcommon -lpcre2-8 -DZPHEUR_REQUIRE_VOILE -DZEND_COMPILE_DL_EXT=1 -c /home/onecthree/Projects/czpheur/Zpheur/Dependencies/ServiceLocator/Container_class.c -o Zpheur/Dependencies/ServiceLocator/Container_class.lo  -MMD -MF Zpheur/Dependencies/ServiceLocator/Container_class.dep -MT Zpheur/Dependencies/ServiceLocator/Container_class.lo
-include Zpheur/Schemes/Http/Foundation/ParameterBag_class.dep
Zpheur/Schemes/Http/Foundation/ParameterBag_class.lo: /home/onecthree/Projects/czpheur/Zpheur/Schemes/Http/Foundation/ParameterBag_class.c
	$(LIBTOOL) --tag=CC --mode=compile $(CC) -I. -I/home/onecthree/Projects/czpheur $(COMMON_FLAGS) $(CFLAGS_CLEAN) $(EXTRA_CFLAGS)    -fcommon -lpcre2-8 -DZPHEUR_REQUIRE_VOILE -DZEND_COMPILE_DL_EXT=1 -c /home/onecthree/Projects/czpheur/Zpheur/Schemes/Http/Foundation/ParameterBag_class.c -o Zpheur/Schemes/Http/Foundation/ParameterBag_class.lo  -MMD -MF Zpheur/Schemes/Http/Foundation/ParameterBag_class.dep -MT Zpheur/Schemes/Http/Foundation/ParameterBag_class.lo
-include Zpheur/Schemes/Http/Foundation/HeaderBag_class.dep
Zpheur/Schemes/Http/Foundation/HeaderBag_class.lo: /home/onecthree/Projects/czpheur/Zpheur/Schemes/Http/Foundation/HeaderBag_class.c
	$(LIBTOOL) --tag=CC --mode=compile $(CC) -I. -I/home/onecthree/Projects/czpheur $(COMMON_FLAGS) $(CFLAGS_CLEAN) $(EXTRA_CFLAGS)    -fcommon -lpcre2-8 -DZPHEUR_REQUIRE_VOILE -DZEND_COMPILE_DL_EXT=1 -c /home/onecthree/Projects/czpheur/Zpheur/Schemes/Http/Foundation/HeaderBag_class.c -o Zpheur/Schemes/Http/Foundation/HeaderBag_class.lo  -MMD -MF Zpheur/Schemes/Http/Foundation/HeaderBag_class.dep -MT Zpheur/Schemes/Http/Foundation/HeaderBag_class.lo
-include Zpheur/Schemes/Http/Foundation/InputBag_class.dep
Zpheur/Schemes/Http/Foundation/InputBag_class.lo: /home/onecthree/Projects/czpheur/Zpheur/Schemes/Http/Foundation/InputBag_class.c
	$(LIBTOOL) --tag=CC --mode=compile $(CC) -I. -I/home/onecthree/Projects/czpheur $(COMMON_FLAGS) $(CFLAGS_CLEAN) $(EXTRA_CFLAGS)    -fcommon -lpcre2-8 -DZPHEUR_REQUIRE_VOILE -DZEND_COMPILE_DL_EXT=1 -c /home/onecthree/Projects/czpheur/Zpheur/Schemes/Http/Foundation/InputBag_class.c -o Zpheur/Schemes/Http/Foundation/InputBag_class.lo  -MMD -MF Zpheur/Schemes/Http/Foundation/InputBag_class.dep -MT Zpheur/Schemes/Http/Foundation/InputBag_class.lo
-include Zpheur/Schemes/Http/Foundation/Kernel_class.dep
Zpheur/Schemes/Http/Foundation/Kernel_class.lo: /home/onecthree/Projects/czpheur/Zpheur/Schemes/Http/Foundation/Kernel_class.c
	$(LIBTOOL) --tag=CC --mode=compile $(CC) -I. -I/home/onecthree/Projects/czpheur $(COMMON_FLAGS) $(CFLAGS_CLEAN) $(EXTRA_CFLAGS)    -fcommon -lpcre2-8 -DZPHEUR_REQUIRE_VOILE -DZEND_COMPILE_DL_EXT=1 -c /home/onecthree/Projects/czpheur/Zpheur/Schemes/Http/Foundation/Kernel_class.c -o Zpheur/Schemes/Http/Foundation/Kernel_class.lo  -MMD -MF Zpheur/Schemes/Http/Foundation/Kernel_class.dep -MT Zpheur/Schemes/Http/Foundation/Kernel_class.lo
-include Zpheur/Schemes/Http/Foundation/Request_class.dep
Zpheur/Schemes/Http/Foundation/Request_class.lo: /home/onecthree/Projects/czpheur/Zpheur/Schemes/Http/Foundation/Request_class.c
	$(LIBTOOL) --tag=CC --mode=compile $(CC) -I. -I/home/onecthree/Projects/czpheur $(COMMON_FLAGS) $(CFLAGS_CLEAN) $(EXTRA_CFLAGS)    -fcommon -lpcre2-8 -DZPHEUR_REQUIRE_VOILE -DZEND_COMPILE_DL_EXT=1 -c /home/onecthree/Projects/czpheur/Zpheur/Schemes/Http/Foundation/Request_class.c -o Zpheur/Schemes/Http/Foundation/Request_class.lo  -MMD -MF Zpheur/Schemes/Http/Foundation/Request_class.dep -MT Zpheur/Schemes/Http/Foundation/Request_class.lo
-include Zpheur/Schemes/Http/Foundation/ServerBag_class.dep
Zpheur/Schemes/Http/Foundation/ServerBag_class.lo: /home/onecthree/Projects/czpheur/Zpheur/Schemes/Http/Foundation/ServerBag_class.c
	$(LIBTOOL) --tag=CC --mode=compile $(CC) -I. -I/home/onecthree/Projects/czpheur $(COMMON_FLAGS) $(CFLAGS_CLEAN) $(EXTRA_CFLAGS)    -fcommon -lpcre2-8 -DZPHEUR_REQUIRE_VOILE -DZEND_COMPILE_DL_EXT=1 -c /home/onecthree/Projects/czpheur/Zpheur/Schemes/Http/Foundation/ServerBag_class.c -o Zpheur/Schemes/Http/Foundation/ServerBag_class.lo  -MMD -MF Zpheur/Schemes/Http/Foundation/ServerBag_class.dep -MT Zpheur/Schemes/Http/Foundation/ServerBag_class.lo
-include Zpheur/Schemes/Http/Message/HeaderInterface_class.dep
Zpheur/Schemes/Http/Message/HeaderInterface_class.lo: /home/onecthree/Projects/czpheur/Zpheur/Schemes/Http/Message/HeaderInterface_class.c
	$(LIBTOOL) --tag=CC --mode=compile $(CC) -I. -I/home/onecthree/Projects/czpheur $(COMMON_FLAGS) $(CFLAGS_CLEAN) $(EXTRA_CFLAGS)    -fcommon -lpcre2-8 -DZPHEUR_REQUIRE_VOILE -DZEND_COMPILE_DL_EXT=1 -c /home/onecthree/Projects/czpheur/Zpheur/Schemes/Http/Message/HeaderInterface_class.c -o Zpheur/Schemes/Http/Message/HeaderInterface_class.lo  -MMD -MF Zpheur/Schemes/Http/Message/HeaderInterface_class.dep -MT Zpheur/Schemes/Http/Message/HeaderInterface_class.lo
-include Zpheur/Schemes/Http/Message/HeaderTrait_class.dep
Zpheur/Schemes/Http/Message/HeaderTrait_class.lo: /home/onecthree/Projects/czpheur/Zpheur/Schemes/Http/Message/HeaderTrait_class.c
	$(LIBTOOL) --tag=CC --mode=compile $(CC) -I. -I/home/onecthree/Projects/czpheur $(COMMON_FLAGS) $(CFLAGS_CLEAN) $(EXTRA_CFLAGS)    -fcommon -lpcre2-8 -DZPHEUR_REQUIRE_VOILE -DZEND_COMPILE_DL_EXT=1 -c /home/onecthree/Projects/czpheur/Zpheur/Schemes/Http/Message/HeaderTrait_class.c -o Zpheur/Schemes/Http/Message/HeaderTrait_class.lo  -MMD -MF Zpheur/Schemes/Http/Message/HeaderTrait_class.dep -MT Zpheur/Schemes/Http/Message/HeaderTrait_class.lo
-include Zpheur/Schemes/Http/Message/Response_class.dep
Zpheur/Schemes/Http/Message/Response_class.lo: /home/onecthree/Projects/czpheur/Zpheur/Schemes/Http/Message/Response_class.c
	$(LIBTOOL) --tag=CC --mode=compile $(CC) -I. -I/home/onecthree/Projects/czpheur $(COMMON_FLAGS) $(CFLAGS_CLEAN) $(EXTRA_CFLAGS)    -fcommon -lpcre2-8 -DZPHEUR_REQUIRE_VOILE -DZEND_COMPILE_DL_EXT=1 -c /home/onecthree/Projects/czpheur/Zpheur/Schemes/Http/Message/Response_class.c -o Zpheur/Schemes/Http/Message/Response_class.lo  -MMD -MF Zpheur/Schemes/Http/Message/Response_class.dep -MT Zpheur/Schemes/Http/Message/Response_class.lo
-include Zpheur/Schemes/Http/Responder/ErrorRequest_class.dep
Zpheur/Schemes/Http/Responder/ErrorRequest_class.lo: /home/onecthree/Projects/czpheur/Zpheur/Schemes/Http/Responder/ErrorRequest_class.c
	$(LIBTOOL) --tag=CC --mode=compile $(CC) -I. -I/home/onecthree/Projects/czpheur $(COMMON_FLAGS) $(CFLAGS_CLEAN) $(EXTRA_CFLAGS)    -fcommon -lpcre2-8 -DZPHEUR_REQUIRE_VOILE -DZEND_COMPILE_DL_EXT=1 -c /home/onecthree/Projects/czpheur/Zpheur/Schemes/Http/Responder/ErrorRequest_class.c -o Zpheur/Schemes/Http/Responder/ErrorRequest_class.lo  -MMD -MF Zpheur/Schemes/Http/Responder/ErrorRequest_class.dep -MT Zpheur/Schemes/Http/Responder/ErrorRequest_class.lo
-include Zpheur/Schemes/Http/Routing/Dispatcher/DispatchItem_class.dep
Zpheur/Schemes/Http/Routing/Dispatcher/DispatchItem_class.lo: /home/onecthree/Projects/czpheur/Zpheur/Schemes/Http/Routing/Dispatcher/DispatchItem_class.c
	$(LIBTOOL) --tag=CC --mode=compile $(CC) -I. -I/home/onecthree/Projects/czpheur $(COMMON_FLAGS) $(CFLAGS_CLEAN) $(EXTRA_CFLAGS)    -fcommon -lpcre2-8 -DZPHEUR_REQUIRE_VOILE -DZEND_COMPILE_DL_EXT=1 -c /home/onecthree/Projects/czpheur/Zpheur/Schemes/Http/Routing/Dispatcher/DispatchItem_class.c -o Zpheur/Schemes/Http/Routing/Dispatcher/DispatchItem_class.lo  -MMD -MF Zpheur/Schemes/Http/Routing/Dispatcher/DispatchItem_class.dep -MT Zpheur/Schemes/Http/Routing/Dispatcher/DispatchItem_class.lo
-include Zpheur/Schemes/Http/Routing/Dispatcher_class.dep
Zpheur/Schemes/Http/Routing/Dispatcher_class.lo: /home/onecthree/Projects/czpheur/Zpheur/Schemes/Http/Routing/Dispatcher_class.c
	$(LIBTOOL) --tag=CC --mode=compile $(CC) -I. -I/home/onecthree/Projects/czpheur $(COMMON_FLAGS) $(CFLAGS_CLEAN) $(EXTRA_CFLAGS)    -fcommon -lpcre2-8 -DZPHEUR_REQUIRE_VOILE -DZEND_COMPILE_DL_EXT=1 -c /home/onecthree/Projects/czpheur/Zpheur/Schemes/Http/Routing/Dispatcher_class.c -o Zpheur/Schemes/Http/Routing/Dispatcher_class.lo  -MMD -MF Zpheur/Schemes/Http/Routing/Dispatcher_class.dep -MT Zpheur/Schemes/Http/Routing/Dispatcher_class.lo
-include Zpheur/Schemes/Http/Routing/Route_class.dep
Zpheur/Schemes/Http/Routing/Route_class.lo: /home/onecthree/Projects/czpheur/Zpheur/Schemes/Http/Routing/Route_class.c
	$(LIBTOOL) --tag=CC --mode=compile $(CC) -I. -I/home/onecthree/Projects/czpheur $(COMMON_FLAGS) $(CFLAGS_CLEAN) $(EXTRA_CFLAGS)    -fcommon -lpcre2-8 -DZPHEUR_REQUIRE_VOILE -DZEND_COMPILE_DL_EXT=1 -c /home/onecthree/Projects/czpheur/Zpheur/Schemes/Http/Routing/Route_class.c -o Zpheur/Schemes/Http/Routing/Route_class.lo  -MMD -MF Zpheur/Schemes/Http/Routing/Route_class.dep -MT Zpheur/Schemes/Http/Routing/Route_class.lo
-include Zpheur/Files/Mime_class.dep
Zpheur/Files/Mime_class.lo: /home/onecthree/Projects/czpheur/Zpheur/Files/Mime_class.c
	$(LIBTOOL) --tag=CC --mode=compile $(CC) -I. -I/home/onecthree/Projects/czpheur $(COMMON_FLAGS) $(CFLAGS_CLEAN) $(EXTRA_CFLAGS)    -fcommon -lpcre2-8 -DZPHEUR_REQUIRE_VOILE -DZEND_COMPILE_DL_EXT=1 -c /home/onecthree/Projects/czpheur/Zpheur/Files/Mime_class.c -o Zpheur/Files/Mime_class.lo  -MMD -MF Zpheur/Files/Mime_class.dep -MT Zpheur/Files/Mime_class.lo
-include Zpheur/Databases/Voile_class.dep
Zpheur/Databases/Voile_class.lo: /home/onecthree/Projects/czpheur/Zpheur/Databases/Voile_class.c
	$(LIBTOOL) --tag=CC --mode=compile $(CC) -I. -I/home/onecthree/Projects/czpheur $(COMMON_FLAGS) $(CFLAGS_CLEAN) $(EXTRA_CFLAGS)    -fcommon -lpcre2-8 -DZPHEUR_REQUIRE_VOILE -DZEND_COMPILE_DL_EXT=1 -c /home/onecthree/Projects/czpheur/Zpheur/Databases/Voile_class.c -o Zpheur/Databases/Voile_class.lo  -MMD -MF Zpheur/Databases/Voile_class.dep -MT Zpheur/Databases/Voile_class.lo
-include Zpheur/Databases/Voile/Model_class.dep
Zpheur/Databases/Voile/Model_class.lo: /home/onecthree/Projects/czpheur/Zpheur/Databases/Voile/Model_class.c
	$(LIBTOOL) --tag=CC --mode=compile $(CC) -I. -I/home/onecthree/Projects/czpheur $(COMMON_FLAGS) $(CFLAGS_CLEAN) $(EXTRA_CFLAGS)    -fcommon -lpcre2-8 -DZPHEUR_REQUIRE_VOILE -DZEND_COMPILE_DL_EXT=1 -c /home/onecthree/Projects/czpheur/Zpheur/Databases/Voile/Model_class.c -o Zpheur/Databases/Voile/Model_class.lo  -MMD -MF Zpheur/Databases/Voile/Model_class.dep -MT Zpheur/Databases/Voile/Model_class.lo
-include Zpheur/Databases/Voile/Collection_class.dep
Zpheur/Databases/Voile/Collection_class.lo: /home/onecthree/Projects/czpheur/Zpheur/Databases/Voile/Collection_class.c
	$(LIBTOOL) --tag=CC --mode=compile $(CC) -I. -I/home/onecthree/Projects/czpheur $(COMMON_FLAGS) $(CFLAGS_CLEAN) $(EXTRA_CFLAGS)    -fcommon -lpcre2-8 -DZPHEUR_REQUIRE_VOILE -DZEND_COMPILE_DL_EXT=1 -c /home/onecthree/Projects/czpheur/Zpheur/Databases/Voile/Collection_class.c -o Zpheur/Databases/Voile/Collection_class.lo  -MMD -MF Zpheur/Databases/Voile/Collection_class.dep -MT Zpheur/Databases/Voile/Collection_class.lo
-include Zpheur/Databases/Voile/Schema/Attributes_class.dep
Zpheur/Databases/Voile/Schema/Attributes_class.lo: /home/onecthree/Projects/czpheur/Zpheur/Databases/Voile/Schema/Attributes_class.c
	$(LIBTOOL) --tag=CC --mode=compile $(CC) -I. -I/home/onecthree/Projects/czpheur $(COMMON_FLAGS) $(CFLAGS_CLEAN) $(EXTRA_CFLAGS)    -fcommon -lpcre2-8 -DZPHEUR_REQUIRE_VOILE -DZEND_COMPILE_DL_EXT=1 -c /home/onecthree/Projects/czpheur/Zpheur/Databases/Voile/Schema/Attributes_class.c -o Zpheur/Databases/Voile/Schema/Attributes_class.lo  -MMD -MF Zpheur/Databases/Voile/Schema/Attributes_class.dep -MT Zpheur/Databases/Voile/Schema/Attributes_class.lo
-include Zpheur/Databases/Voile/Schema/AbstractionType_class.dep
Zpheur/Databases/Voile/Schema/AbstractionType_class.lo: /home/onecthree/Projects/czpheur/Zpheur/Databases/Voile/Schema/AbstractionType_class.c
	$(LIBTOOL) --tag=CC --mode=compile $(CC) -I. -I/home/onecthree/Projects/czpheur $(COMMON_FLAGS) $(CFLAGS_CLEAN) $(EXTRA_CFLAGS)    -fcommon -lpcre2-8 -DZPHEUR_REQUIRE_VOILE -DZEND_COMPILE_DL_EXT=1 -c /home/onecthree/Projects/czpheur/Zpheur/Databases/Voile/Schema/AbstractionType_class.c -o Zpheur/Databases/Voile/Schema/AbstractionType_class.lo  -MMD -MF Zpheur/Databases/Voile/Schema/AbstractionType_class.dep -MT Zpheur/Databases/Voile/Schema/AbstractionType_class.lo
-include Zpheur/Databases/Phase_class.dep
Zpheur/Databases/Phase_class.lo: /home/onecthree/Projects/czpheur/Zpheur/Databases/Phase_class.c
	$(LIBTOOL) --tag=CC --mode=compile $(CC) -I. -I/home/onecthree/Projects/czpheur $(COMMON_FLAGS) $(CFLAGS_CLEAN) $(EXTRA_CFLAGS)    -fcommon -lpcre2-8 -DZPHEUR_REQUIRE_VOILE -DZEND_COMPILE_DL_EXT=1 -c /home/onecthree/Projects/czpheur/Zpheur/Databases/Phase_class.c -o Zpheur/Databases/Phase_class.lo  -MMD -MF Zpheur/Databases/Phase_class.dep -MT Zpheur/Databases/Phase_class.lo
-include Zpheur/Databases/Phase/Model_class.dep
Zpheur/Databases/Phase/Model_class.lo: /home/onecthree/Projects/czpheur/Zpheur/Databases/Phase/Model_class.c
	$(LIBTOOL) --tag=CC --mode=compile $(CC) -I. -I/home/onecthree/Projects/czpheur $(COMMON_FLAGS) $(CFLAGS_CLEAN) $(EXTRA_CFLAGS)    -fcommon -lpcre2-8 -DZPHEUR_REQUIRE_VOILE -DZEND_COMPILE_DL_EXT=1 -c /home/onecthree/Projects/czpheur/Zpheur/Databases/Phase/Model_class.c -o Zpheur/Databases/Phase/Model_class.lo  -MMD -MF Zpheur/Databases/Phase/Model_class.dep -MT Zpheur/Databases/Phase/Model_class.lo
-include Zpheur/Databases/Phase/Schema/AbstractionType_class.dep
Zpheur/Databases/Phase/Schema/AbstractionType_class.lo: /home/onecthree/Projects/czpheur/Zpheur/Databases/Phase/Schema/AbstractionType_class.c
	$(LIBTOOL) --tag=CC --mode=compile $(CC) -I. -I/home/onecthree/Projects/czpheur $(COMMON_FLAGS) $(CFLAGS_CLEAN) $(EXTRA_CFLAGS)    -fcommon -lpcre2-8 -DZPHEUR_REQUIRE_VOILE -DZEND_COMPILE_DL_EXT=1 -c /home/onecthree/Projects/czpheur/Zpheur/Databases/Phase/Schema/AbstractionType_class.c -o Zpheur/Databases/Phase/Schema/AbstractionType_class.lo  -MMD -MF Zpheur/Databases/Phase/Schema/AbstractionType_class.dep -MT Zpheur/Databases/Phase/Schema/AbstractionType_class.lo
-include Zpheur/Databases/Phase/Schema/Attributes_class.dep
Zpheur/Databases/Phase/Schema/Attributes_class.lo: /home/onecthree/Projects/czpheur/Zpheur/Databases/Phase/Schema/Attributes_class.c
	$(LIBTOOL) --tag=CC --mode=compile $(CC) -I. -I/home/onecthree/Projects/czpheur $(COMMON_FLAGS) $(CFLAGS_CLEAN) $(EXTRA_CFLAGS)    -fcommon -lpcre2-8 -DZPHEUR_REQUIRE_VOILE -DZEND_COMPILE_DL_EXT=1 -c /home/onecthree/Projects/czpheur/Zpheur/Databases/Phase/Schema/Attributes_class.c -o Zpheur/Databases/Phase/Schema/Attributes_class.lo  -MMD -MF Zpheur/Databases/Phase/Schema/Attributes_class.dep -MT Zpheur/Databases/Phase/Schema/Attributes_class.lo
-include Zpheur/Logs/StreamLog_class.dep
Zpheur/Logs/StreamLog_class.lo: /home/onecthree/Projects/czpheur/Zpheur/Logs/StreamLog_class.c
	$(LIBTOOL) --tag=CC --mode=compile $(CC) -I. -I/home/onecthree/Projects/czpheur $(COMMON_FLAGS) $(CFLAGS_CLEAN) $(EXTRA_CFLAGS)    -fcommon -lpcre2-8 -DZPHEUR_REQUIRE_VOILE -DZEND_COMPILE_DL_EXT=1 -c /home/onecthree/Projects/czpheur/Zpheur/Logs/StreamLog_class.c -o Zpheur/Logs/StreamLog_class.lo  -MMD -MF Zpheur/Logs/StreamLog_class.dep -MT Zpheur/Logs/StreamLog_class.lo
-include Zpheur/Logs/StreamLog/StreamHandler_class.dep
Zpheur/Logs/StreamLog/StreamHandler_class.lo: /home/onecthree/Projects/czpheur/Zpheur/Logs/StreamLog/StreamHandler_class.c
	$(LIBTOOL) --tag=CC --mode=compile $(CC) -I. -I/home/onecthree/Projects/czpheur $(COMMON_FLAGS) $(CFLAGS_CLEAN) $(EXTRA_CFLAGS)    -fcommon -lpcre2-8 -DZPHEUR_REQUIRE_VOILE -DZEND_COMPILE_DL_EXT=1 -c /home/onecthree/Projects/czpheur/Zpheur/Logs/StreamLog/StreamHandler_class.c -o Zpheur/Logs/StreamLog/StreamHandler_class.lo  -MMD -MF Zpheur/Logs/StreamLog/StreamHandler_class.dep -MT Zpheur/Logs/StreamLog/StreamHandler_class.lo
-include Zpheur/Logs/StreamLog/Level_class.dep
Zpheur/Logs/StreamLog/Level_class.lo: /home/onecthree/Projects/czpheur/Zpheur/Logs/StreamLog/Level_class.c
	$(LIBTOOL) --tag=CC --mode=compile $(CC) -I. -I/home/onecthree/Projects/czpheur $(COMMON_FLAGS) $(CFLAGS_CLEAN) $(EXTRA_CFLAGS)    -fcommon -lpcre2-8 -DZPHEUR_REQUIRE_VOILE -DZEND_COMPILE_DL_EXT=1 -c /home/onecthree/Projects/czpheur/Zpheur/Logs/StreamLog/Level_class.c -o Zpheur/Logs/StreamLog/Level_class.lo  -MMD -MF Zpheur/Logs/StreamLog/Level_class.dep -MT Zpheur/Logs/StreamLog/Level_class.lo
-include Zpheur/Strings/contains_function.dep
Zpheur/Strings/contains_function.lo: /home/onecthree/Projects/czpheur/Zpheur/Strings/contains_function.c
	$(LIBTOOL) --tag=CC --mode=compile $(CC) -I. -I/home/onecthree/Projects/czpheur $(COMMON_FLAGS) $(CFLAGS_CLEAN) $(EXTRA_CFLAGS)    -fcommon -lpcre2-8 -DZPHEUR_REQUIRE_VOILE -DZEND_COMPILE_DL_EXT=1 -c /home/onecthree/Projects/czpheur/Zpheur/Strings/contains_function.c -o Zpheur/Strings/contains_function.lo  -MMD -MF Zpheur/Strings/contains_function.dep -MT Zpheur/Strings/contains_function.lo
-include Zpheur/Encryptions/Random/Generator/csrf_token_function.dep
Zpheur/Encryptions/Random/Generator/csrf_token_function.lo: /home/onecthree/Projects/czpheur/Zpheur/Encryptions/Random/Generator/csrf_token_function.c
	$(LIBTOOL) --tag=CC --mode=compile $(CC) -I. -I/home/onecthree/Projects/czpheur $(COMMON_FLAGS) $(CFLAGS_CLEAN) $(EXTRA_CFLAGS)    -fcommon -lpcre2-8 -DZPHEUR_REQUIRE_VOILE -DZEND_COMPILE_DL_EXT=1 -c /home/onecthree/Projects/czpheur/Zpheur/Encryptions/Random/Generator/csrf_token_function.c -o Zpheur/Encryptions/Random/Generator/csrf_token_function.lo  -MMD -MF Zpheur/Encryptions/Random/Generator/csrf_token_function.dep -MT Zpheur/Encryptions/Random/Generator/csrf_token_function.lo
-include include/libstr.dep
include/libstr.lo: /home/onecthree/Projects/czpheur/include/libstr.c
	$(LIBTOOL) --tag=CC --mode=compile $(CC) -I. -I/home/onecthree/Projects/czpheur $(COMMON_FLAGS) $(CFLAGS_CLEAN) $(EXTRA_CFLAGS)    -fcommon -lpcre2-8 -DZPHEUR_REQUIRE_VOILE -DZEND_COMPILE_DL_EXT=1 -c /home/onecthree/Projects/czpheur/include/libstr.c -o include/libstr.lo  -MMD -MF include/libstr.dep -MT include/libstr.lo
-include include/onecstr.dep
include/onecstr.lo: /home/onecthree/Projects/czpheur/include/onecstr.c
	$(LIBTOOL) --tag=CC --mode=compile $(CC) -I. -I/home/onecthree/Projects/czpheur $(COMMON_FLAGS) $(CFLAGS_CLEAN) $(EXTRA_CFLAGS)    -fcommon -lpcre2-8 -DZPHEUR_REQUIRE_VOILE -DZEND_COMPILE_DL_EXT=1 -c /home/onecthree/Projects/czpheur/include/onecstr.c -o include/onecstr.lo  -MMD -MF include/onecstr.dep -MT include/onecstr.lo
-include include/runtime.dep
include/runtime.lo: /home/onecthree/Projects/czpheur/include/runtime.c
	$(LIBTOOL) --tag=CC --mode=compile $(CC) -I. -I/home/onecthree/Projects/czpheur $(COMMON_FLAGS) $(CFLAGS_CLEAN) $(EXTRA_CFLAGS)    -fcommon -lpcre2-8 -DZPHEUR_REQUIRE_VOILE -DZEND_COMPILE_DL_EXT=1 -c /home/onecthree/Projects/czpheur/include/runtime.c -o include/runtime.lo  -MMD -MF include/runtime.dep -MT include/runtime.lo
-include include/regexp.dep
include/regexp.lo: /home/onecthree/Projects/czpheur/include/regexp.c
	$(LIBTOOL) --tag=CC --mode=compile $(CC) -I. -I/home/onecthree/Projects/czpheur $(COMMON_FLAGS) $(CFLAGS_CLEAN) $(EXTRA_CFLAGS)    -fcommon -lpcre2-8 -DZPHEUR_REQUIRE_VOILE -DZEND_COMPILE_DL_EXT=1 -c /home/onecthree/Projects/czpheur/include/regexp.c -o include/regexp.lo  -MMD -MF include/regexp.dep -MT include/regexp.lo
-include include/dotenv.dep
include/dotenv.lo: /home/onecthree/Projects/czpheur/include/dotenv.c
	$(LIBTOOL) --tag=CC --mode=compile $(CC) -I. -I/home/onecthree/Projects/czpheur $(COMMON_FLAGS) $(CFLAGS_CLEAN) $(EXTRA_CFLAGS)    -fcommon -lpcre2-8 -DZPHEUR_REQUIRE_VOILE -DZEND_COMPILE_DL_EXT=1 -c /home/onecthree/Projects/czpheur/include/dotenv.c -o include/dotenv.lo  -MMD -MF include/dotenv.dep -MT include/dotenv.lo
-include include/furouter.dep
include/furouter.lo: /home/onecthree/Projects/czpheur/include/furouter.c
	$(LIBTOOL) --tag=CC --mode=compile $(CC) -I. -I/home/onecthree/Projects/czpheur $(COMMON_FLAGS) $(CFLAGS_CLEAN) $(EXTRA_CFLAGS)    -fcommon -lpcre2-8 -DZPHEUR_REQUIRE_VOILE -DZEND_COMPILE_DL_EXT=1 -c /home/onecthree/Projects/czpheur/include/furouter.c -o include/furouter.lo  -MMD -MF include/furouter.dep -MT include/furouter.lo
-include include/zerror.dep
include/zerror.lo: /home/onecthree/Projects/czpheur/include/zerror.c
	$(LIBTOOL) --tag=CC --mode=compile $(CC) -I. -I/home/onecthree/Projects/czpheur $(COMMON_FLAGS) $(CFLAGS_CLEAN) $(EXTRA_CFLAGS)    -fcommon -lpcre2-8 -DZPHEUR_REQUIRE_VOILE -DZEND_COMPILE_DL_EXT=1 -c /home/onecthree/Projects/czpheur/include/zerror.c -o include/zerror.lo  -MMD -MF include/zerror.dep -MT include/zerror.lo
-include zpheur_hem.dep
zpheur_hem.lo: /home/onecthree/Projects/czpheur/zpheur_hem.c
	$(LIBTOOL) --tag=CC --mode=compile $(CC) -I. -I/home/onecthree/Projects/czpheur $(COMMON_FLAGS) $(CFLAGS_CLEAN) $(EXTRA_CFLAGS)    -fcommon -lpcre2-8 -DZPHEUR_REQUIRE_VOILE -DZEND_COMPILE_DL_EXT=1 -c /home/onecthree/Projects/czpheur/zpheur_hem.c -o zpheur_hem.lo  -MMD -MF zpheur_hem.dep -MT zpheur_hem.lo
-include zpheur.dep
zpheur.lo: /home/onecthree/Projects/czpheur/zpheur.c
	$(LIBTOOL) --tag=CC --mode=compile $(CC) -I. -I/home/onecthree/Projects/czpheur $(COMMON_FLAGS) $(CFLAGS_CLEAN) $(EXTRA_CFLAGS)    -fcommon -lpcre2-8 -DZPHEUR_REQUIRE_VOILE -DZEND_COMPILE_DL_EXT=1 -c /home/onecthree/Projects/czpheur/zpheur.c -o zpheur.lo  -MMD -MF zpheur.dep -MT zpheur.lo
$(phplibdir)/zpheur.la: ./zpheur.la
	$(LIBTOOL) --tag=CC --mode=install cp ./zpheur.la $(phplibdir)

./zpheur.la: $(shared_objects_zpheur) $(ZPHEUR_SHARED_DEPENDENCIES)
	$(LIBTOOL) --tag=CC --mode=link $(CC) -shared $(COMMON_FLAGS) $(CFLAGS_CLEAN) $(EXTRA_CFLAGS) $(LDFLAGS)  -o $@ -export-dynamic -avoid-version -prefer-pic -module -rpath $(phplibdir) $(EXTRA_LDFLAGS) $(shared_objects_zpheur) $(ZPHEUR_SHARED_LIBADD)

