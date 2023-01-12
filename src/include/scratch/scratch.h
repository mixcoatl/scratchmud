/*!
 * \file scratch.h
 *
 * \par Copyright
 * Copyright (C) 1999-2023 scratchmud.org
 * All rights reserved.
 *
 * \author Geoffrey Davis <gdavis@scratchmud.org>
 * \addtogroup main
 */
#ifndef _SCRATCH_H_
#define _SCRATCH_H_

#include "../conf.h"

#ifdef HAVE_ARPA_INET_H
#include <arpa/inet.h>
#endif /* HAVE_ARPA_INET_H */

#ifdef HAVE_ARPA_TELNET_H
#include <arpa/telnet.h>
#endif /* HAVE_ARPA_TELNET_H */

#ifdef HAVE_CRYPT_H
#include <crypt.h>
#endif /* HAVE_CRYPT_H_ */

#ifdef HAVE_CTYPE_H
#include <ctype.h>
#endif /* HAVE_CTYPE_H */

#ifdef HAVE_DIRENT_H
#include <dirent.h>
#endif /* HAVE_DIRENT_H */

#ifdef HAVE_DLFCN_H
#include <dlfcn.h>
#endif /* HAVE_DLFCN_H */

#ifdef HAVE_ERRNO_H
#include <errno.h>
#endif /* HAVE_ERRNO_H */

#ifdef HAVE_FCNTL_H
#include <fcntl.h>
#endif /* HAVE_FCNTL_H */

#ifdef HAVE_INTTYPES_H
#include <inttypes.h>
#endif /* HAVE_INTTYPES_H */

#ifdef HAVE_LIMITS_H
#include <limits.h>
#endif /* HAVE_LIMITS_H */

#ifdef HAVE_MATH_H
#include <math.h>
#endif /* HAVE_MATH_H */

#ifdef HAVE_MEMORY_H
#include <memory.h>
#endif /* HAVE_MEMORY_H */

#ifdef HAVE_NETDB_H
#include <netdb.h>
#endif /* HAVE_NETDB_H */

#ifdef HAVE_NETINET_IN_H
#include <netinet/in.h>
#endif /* HAVE_NET_INET_H */

#ifdef HAVE_STDARG_H
#include <stdarg.h>
#endif /* HAVE_STDARG_H */

#ifdef HAVE_STDBOOL_H
#include <stdbool.h>
#endif /* HAVE_STDBOOL_H */

#ifdef HAVE_STDINT_H
#include <stdint.h>
#endif /* HAVE_STDINT_H */

#include <stdio.h>

#ifdef HAVE_STDLIB_H
#include <stdlib.h>
#endif /* HAVE_STDLIB_H */

#ifdef HAVE_STRING_H
#include <string.h>
#endif /* HAVE_STRING_H */

#ifdef HAVE_STRINGS_H
#include <strings.h>
#endif /* HAVE_STRING_H */

#ifdef HAVE_SYS_SOCKET_H
#include <sys/socket.h>
#endif /* HAVE_SYS_SOCKET_H */

#ifdef HAVE_SYS_STAT_H
#include <sys/stat.h>
#endif /* HAVE_SYS_STAT_H */

#ifdef HAVE_SYS_TIME_H
#include <sys/time.h>
#endif /* HAVE_SYS_TIME_H */

#ifdef HAVE_SYS_TYPES_H
#include <sys/types.h>
#endif /* HAVE_SYS_TYPES_H */

#ifdef HAVE_TIME_H
#include <time.h>
#endif /* HAVE_TIME_H */

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif /* HAVE_UNISTD_H */

#ifdef HAVE_WINDOWS_H
#include <windows.h>
#endif /* HAVE_WINDOWS_H */

#ifndef HAVE_STRDUP
/* Rudimentary strdup replacement */
static char *strdup(const char *src) {
  const size_t srclen = strlen(src);
  return strcpy(malloc(srclen + 1), src);
}
#endif /* HAVE_STRDUP */

#ifndef HAVE_STRLCPY
/* Rudimentary strlcpy replacement */
static size_t strlcpy(char *dst, const char *src, size_t dstsize) {
  strncpy(dst, src, dstsize - 1);
  dst[dstsize - 1] = '\0';
  return strlen(dst);
}
#endif /* HAVE_STRLCPY */

/*!
 * Static input buffer size.
 * \addtogroup main
 */
#define MAXLEN_INPUT (256)

/*!
 * Static string buffer size.
 * \addtogroup main
 */
#define MAXLEN_STRING (8192)

#endif /* _SCRATCH_H_ */
