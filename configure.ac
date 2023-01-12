AC_INIT([scratch],[1.0],[bugs@scratchmud.com])
AM_INIT_AUTOMAKE
AM_PROG_AR
AC_PROG_CC
AC_LANG(C)
LT_INIT
AX_COMPILER_FLAGS
AC_ENABLE_SHARED
AC_CHECK_HEADER(arpa/inet.h,[AC_DEFINE([HAVE_ARPA_INET_H],[1],[Define to 1 if you have the <arpa/inet.h> header file.])])
AC_CHECK_HEADER(arpa/telnet.h,[AC_DEFINE([HAVE_ARPA_TELNET_H],[1],[Define to 1 if you have the <arpa/telnet.h> header file.])])
AC_CHECK_HEADER(crypt.h,[AC_DEFINE([HAVE_CRYPT_H],[1],[Define to 1 if you have the <crypt.h> header file.])])
AC_CHECK_HEADER(ctype.h,[AC_DEFINE([HAVE_CTYPE_H],[1],[Define to 1 if you have the <ctype.h> header file.])])
AC_CHECK_HEADER(dirent.h,[AC_DEFINE([HAVE_DIRENT_H],[1],[Define to 1 if you have the <dirent.h> header file.])])
AC_CHECK_HEADER(dlfcn.h,[AC_DEFINE([HAVE_DLFCN_H],[1],[Define to 1 if you have the <dlfcn.h> header file.])])
AC_CHECK_HEADER(errno.h,[AC_DEFINE([HAVE_ERRNO_H],[1],[Define to 1 if you have the <errno.h> header file.])])
AC_CHECK_HEADER(fcntl.h,[AC_DEFINE([HAVE_FCNTL_H],[1],[Define to 1 if you have the <fcntl.h> header file.])])
AC_CHECK_HEADER(limits.h,[AC_DEFINE([HAVE_LIMITS_H],[1],[Define to 1 if you have the <limits.h> header file.])])
AC_CHECK_HEADER(math.h,[AC_DEFINE([HAVE_MATH_H],[1],[Define to 1 if you have the <math.h> header file.])])
AC_CHECK_HEADER(netdb.h,[AC_DEFINE([HAVE_NETDB_H],[1],[Define to 1 if you have the <netdb.h> header file.])])
AC_CHECK_HEADER(netinet/in.h,[AC_DEFINE([HAVE_NETINET_IN_H],[1],[Define to 1 if you have the <netinet/in.h> header file.])])
AC_CHECK_HEADER(stdarg.h,[AC_DEFINE([HAVE_STDARG_H],[1],[Define to 1 if you have the <stdarg.h> header file.])])
AC_CHECK_HEADER(stdbool.h,[AC_DEFINE([HAVE_STDBOOL_H],[1],[Define to 1 if you have the <stdbool.h> header file.])])
AC_CHECK_HEADER(stddef.h,[AC_DEFINE([HAVE_STDDEF_H],[1],[Define to 1 if you have the <stddef.h> header file.])])
AC_CHECK_HEADER(stdlib.h,[AC_DEFINE([HAVE_STDLIB_H],[1],[Define to 1 if you have the <stdlib.h> header file.])])
AC_CHECK_HEADER(string.h,[AC_DEFINE([HAVE_STRING_H],[1],[Define to 1 if you have the <string.h> header file.])])
AC_CHECK_HEADER(sys/socket.h,[AC_DEFINE([HAVE_SYS_SOCKET_H],[1],[Define to 1 if you have the <sys/socket.h> header file.])])
AC_CHECK_HEADER(sys/time.h,[AC_DEFINE([HAVE_SYS_TIME_H],[1],[Define to 1 if you have the <sys/time.h> header file.])])
AC_CHECK_HEADER(sys/types.h,[AC_DEFINE([HAVE_SYS_TYPES_H],[1],[Define to 1 if you have the <sys/types.h> header file.])])
AC_CHECK_HEADER(time.h,[AC_DEFINE([HAVE_TIME_H],[1],[Define to 1 if you have the <time.h> header file.])])
AC_CHECK_HEADER(unistd.h,[AC_DEFINE([HAVE_UNISTD_H],[1],[Define to 1 if you have the <unistd.h> header file.])])
AC_CHECK_HEADER(windows.h,[AC_DEFINE([HAVE_WINDOWS_H],[1],[Define to 1 if you have the <windows.h> header file.])])
AC_CHECK_LIB(c, printf)
AC_CHECK_LIB(crypt, crypt)
AC_CHECK_LIB(m, sqrt)
AC_CHECK_LIB(dl, dlsym)
AC_CHECK_FUNCS(calloc fprintf free gettimeofday malloc strdup strlcpy snprintf vsnprintf)
AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_HEADERS([src/include/conf.h])
AC_CONFIG_FILES([Makefile src/Makefile src/scratch/Makefile])
AC_SUBST([CFLAGS], ${WARN_CFLAGS})
AC_SUBST([LDFLAGS], ${WARN_LDFLAGS})
LT_INIT
AC_OUTPUT
