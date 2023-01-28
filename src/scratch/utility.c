/*!
 * \file utility.c
 *
 * \par Copyright
 * Copyright (C) 1999-2023 scratchmud.org
 * All rights reserved.
 *
 * \author Geoffrey Davis <gdavis@scratchmud.org>
 * \addtogroup utility
 */
#define _SCRATCH_UTILITY_C_

#include <scratch/log.h>
#include <scratch/memory.h>
#include <scratch/random.h>
#include <scratch/scratch.h>
#include <scratch/string.h>
#include <scratch/utility.h>

/*!
 * Encodes a number as a base-36 string.
 * \addtogroup utility
 * \param out the output buffer
 * \param outlen the length of the specified buffer
 * \param value the number to encode
 * \return the number of bytes written to the specified buffer
 */
size_t UtilityBase36(
	char *out, const size_t outlen,
	const uint64_t value) {
  register size_t outpos = 0;
  if (!out) {
    Log(L_ASSERT, "Invalid `out` buffer.");
  } else if (outlen < 14) {
    Log(L_ASSERT, "Invalid `outlen` value %zu.", outlen);
  } else {
    register uint64_t rValue = value;
    do {
      const int digit = rValue % 36;
      if (digit >= 0 && digit <= 9) {
        BPrintf(out, outlen, outpos, "%c", '0' + digit);
      } else {
        BPrintf(out, outlen, outpos, "%c", 'a' + digit - 10);
      }
    } while (rValue /= 36);
  }

  /* Reverse  order of characters */
  if (out && *out != '\0') {
    register char *p = out;
    register char *q = out + strlen(out) - 1;
    for (; p < q; ++p, --q) {
      const char ch = *p;
      *p = *q;
      *q = ch;
    }
  }
  return (outpos);
}

/*!
 * Hashes a plaintext message.
 * \addtogroup utility
 * \param out the output buffer
 * \param outlen the length of the specified buffer
 * \param plaintext the plaintext message to hash
 * \return the number of bytes written to the specified buffer
 */
size_t UtilityCrypt(
	char *out, const size_t outlen,
	const char *plaintext) {
  register size_t result = 0;
  if (!out && outlen) {
    Log(L_ASSERT, "Invalid `out` buffer.");
  } else if (!plaintext) {
    Log(L_ASSERT, "Invalid `plaintext` string.");
  } else {
#ifdef HAVE_LIBCRYPT
    /* Salt buffer */
    char salt[PATH_MAX] = {'\0'};
    register size_t saltlen = 0;

    /* Generate salt */
    BPrintf(salt, sizeof(salt), saltlen, "$1$");
    UtilityNameGenerate(salt + saltlen, sizeof(salt) - saltlen);

    /* Encrypt password */
    result = strlcpy(out, crypt(plaintext, salt), outlen);
#else
    /* No crypt(): use plaintext */
    result = strlcpy(out, plaintext, outlen);
#endif // HAVE_LIBCRYPT
  }
  return (result);
}

/*!
 * Matches a password hash.
 * \addtogroup utility
 * \param passwd the password hash to match
 * \param plaintext the plaintext password
 * \return true if the passwords match
 */
bool UtilityCryptMatch(
	const char *passwd,
	const char *plaintext) {
  register bool result = false;
  if (!passwd) {
    Log(L_ASSERT, "Invalid `passwd` string.");
  } else if (!plaintext) {
    Log(L_ASSERT, "Invalid `plaintext` string.");
  } else {
#ifdef HAVE_LIBCRYPT
    /* Compare hashed passwords */
    if (strcmp(crypt(plaintext, passwd), passwd) == 0) {
#else
    /* Compare plaintext passwords */
    if (strcmp(plaintext, passwd) == 0) {
#endif /* HAVE_LIBCRYPT */
      result = true;
    }
  }
  return (result);
}

/*!
 * Generates a filename.
 * \addtogroup utility
 * \param fname the filename output buffer
 * \param fnamelen the length of the specified buffer
 * \param prefix the top-level directory
 * \param extension the filename extension
 * \param name the name of the object
 * \return the number of bytes written to the specified buffer
 */
size_t UtilityGetFileName(
	char *fname, const size_t fnamelen,
	const char *prefix,
	const char *extension,
	const char *name) {
  register size_t fnamepos = 0;
  if (!fname && fnamelen) {
    Log(L_ASSERT, "Invalid `fname` buffer.");
  } else if (!name || *name == '\0') {
    Log(L_ASSERT, "Invalid `name` string.");
  } else {
    /* Defensive copy */
    char nameCopy[PATH_MAX] = {'\0'};
    strlcpy(nameCopy, name, sizeof(nameCopy));

    /* Lowercase */
    for (register char *p = nameCopy; *p != '\0'; ++p) {
      *p = isalnum((int) *p) ? tolower((int) *p) : '_';
    }

    /* Path prefix */
    if (prefix && *prefix != '\0')
      BPrintf(fname, fnamelen, fnamepos, "%s/", prefix);

    /* Middle subdirectory */
    switch (*nameCopy) {
    case 'a':  case 'b':  case 'c':  case 'd':  case 'e':
      BPrintf(fname, fnamelen, fnamepos, "a-e/");
      break;
    case 'f':  case 'g':  case 'h':  case 'i':  case 'j':
      BPrintf(fname, fnamelen, fnamepos, "f-j/");
      break;
    case 'k':  case 'l':  case 'm':  case 'n':  case 'o':
      BPrintf(fname, fnamelen, fnamepos, "k-o/");
      break;
    case 'p':  case 'q':  case 'r':  case 's':  case 't':
      BPrintf(fname, fnamelen, fnamepos, "p-t/");
      break;
    case 'u':  case 'v':  case 'w':  case 'x':  case 'y':  case 'z':
      BPrintf(fname, fnamelen, fnamepos, "u-z/");
      break;
    default:
      BPrintf(fname, fnamelen, fnamepos, "zzz/");
      break;
    }

    /* Filename without extension */
    BPrintf(fname, fnamelen, fnamepos, "%s", nameCopy);

    /* File extension */
    BPrintf(fname, fnamelen, fnamepos, "%s%s",
	extension && *extension != '\0' && *extension != '.' ? "." : "",
	extension);
  }
  return (fnamepos);
}

/*!
 * Compares names for order.
 * \addtogroup utility
 * \param left the first name to compare
 * \param right the second name to compare
 * \return < 0 if the first name is less than the second; or,
 *         > 0 if the first name is greater than the second; or,
 *           0 if the specified names are equal
 * \sa UtilityNameCompareV(const void*, const void*)
 */
int UtilityNameCompare(
	const char **left,
	const char **right) {
  return StringCaseCompare(
	left  && *left  ? *left : "",
	right && *right ? *right : "");
}

/*!
 * Compares names for order.
 * \addtogroup utility
 * \param left the first name to compare
 * \param right the second name to compare
 * \return < 0 if the first name is less than the second; or,
 *         > 0 if the first name is greater than the second; or,
 *           0 if the specified names are equal
 * \sa UtilityNameCompare(const char**, const char**)
 */
int UtilityNameCompareV(
	const void *left,
	const void *right) {
  return UtilityNameCompare(
	(const char**) left,
	(const char**) right);
}

/*!
 * Generates a random name.
 * \addtogroup utility
 * \param out the name output buffer
 * \param outlen the length of the specified buffer
 * \return the number of bytes written to the specified buffer
 */
size_t UtilityNameGenerate(
	char *out, const size_t outlen) {
  register size_t outpos = 0;
  if (!out && outlen) {
    Log(L_ASSERT, "Invalid `out` buffer.");
  } else {
    const uint64_t value = RandomNext(&g_random);
    outpos = UtilityBase36(out, outlen, value);
  }
  return (outpos);
}

/*!
 * \brief Returns whether a name is valid.
 *
 * A name is considered valid if it is not NULL, it is not an
 * empty string (""), and contains only letters, digits, and the
 * underscore and dollar sign characters.
 *
 * \addtogroup utility
 * \param name the name whose validity to return
 * \return true if the specified name is valid
 */
bool UtilityNameValid(const char *name) {
  /* Blank string is invalid */
  if (!name || *name == '\0')
    return (false);

  /* Check for invalid characters */
  for (register const char *p = name; *p != '\0'; ++p) {
    if (!isalnum((int) *p) && !strchr("$_", *p))
      return (false);
  }

  /* Name is valid */
  return (true);
}

/*!
 * Resolves a procedure name.
 * \addtogroup utility
 * \param procName the procedure name of the function to resolve
 * \return the address of the function indicated by the specified
 *     procedure name or NULL
 */
void *UtilityProcByName(const char *procName) {
  register void *proc = NULL;
  if (procName && *procName != '\0') {
#ifdef WIN32
    const HMODULE m = GetModuleHandle(NULL);
    if (!m) {
      const DWORD dwError = GetLastError();
      Log(L_SYSTEM, "GetModuleHandle() failed: dwError=0x%x.", dwError);
    } else {
      proc = GetProcAddress(m, procName);
      if (!proc) {
	const DWORD dwError = GetLastError();
	if (dwError != ERROR_PROC_NOT_FOUND)
	  Log(L_SYSTEM, "GetProcAddress() failed: dwError=0x%x.", dwError);
      }
    }
#else /* WIN32 */
    const void *m = dlopen(NULL, RTLD_LAZY | RTLD_GLOBAL);
    if (!m) {
      Log(L_SYSTEM, "dlopen() failed: errno=%d.", errno);
    } else {
      proc = dlsym(m, procName);
      if (!proc && errno != 0)
	Log(L_SYSTEM, "dlsym() failed: errno=%d.", errno);
      if (dlclose(m) < 0)
	Log(L_SYSTEM, "dlclose() failed: errno=%d.", errno);
    }
#endif /* WIN32 */
  }
  return (proc);
}
