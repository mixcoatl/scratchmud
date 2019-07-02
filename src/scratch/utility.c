/*!
 * \file utility.c
 *
 * \par Copyright
 * Copyright (C) 1999-2019 scratchmud.org
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
 * Encodes a number in base 36.
 * \addtogroup utility
 * \param out the base 36 output buffer
 * \param outlen the length of the specified buffer
 * \param value the number to encode
 * \return the specified base 36 output buffer or NULL
 */
char *UtilityBase36(
	char *out, const size_t outlen,
	const uint32_t value) {
  if (!out) {
    Log("invalid `out` buffer");
  } else if (outlen < 14) {
    Log("invalid `outlen` value %zu", outlen);
    out = NULL;
  } else {
    register size_t outpos = 0;
    register unsigned long rValue = value;
    do {
      const int digit = rValue % 36;
      if (digit >= 0 && digit <= 9) {
        BPrintf(out, outlen, outpos, "%c", '0' + digit);
      } else {
        BPrintf(out, outlen, outpos, "%c", 'A' + digit - 10);
      }
    } while (rValue /= 36);
  }
  /* Reverse the order of the characters */
  if (out && *out != '\0') {
    register char *p = out;
    register char *q = out + strlen(out) - 1;
    for (; p < q; ++p, --q) {
      const char ch = *p;
      *p = *q;
      *q = ch;
    }
  }
  return (out);
}

/*!
 * Hashes a plaintext message.
 * \addtogroup utility
 * \param hash the hash buffer
 * \param hashlen the length of the specified buffer
 * \param plaintext the plaintext message to hash
 * \return true if the message was successfully hashed
 */
bool UtilityCrypt(
	char *hash, const size_t hashlen,
	const char *plaintext) {
  register bool result = false;
  if (!hash && hashlen) {
    Log("invalid `hash` buffer");
  } else {
#ifdef HAVE_LIBCRYPT
    /* Salt buffer */
    char salt[PATH_MAX] = {'\0'};
    register size_t saltlen = 0;

    /* Generate salt */
    BPrintf(salt, sizeof(salt), saltlen, "$1$");
    UtilityNameGenerate(salt + saltlen, sizeof(salt) - saltlen);

    /* Encrypt the password */
    result = snprintf(hash, hashlen, "%s", crypt(plaintext, salt)) > 0;
#else
    /* No crypt(): use plaintext */
    result = snprintf(hash, hashlen, "%s", plaintext) > 0;
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
  if (!passwd) {
    Log("invalid `passwd` string");
  } else if (!plaintext) {
    Log("invalid `plaintext` string");
  } else {
#ifdef HAVE_LIBCRYPT
    /* Compare MD5-hashed passwords */
    if (strcmp(crypt(plaintext, passwd), passwd) == 0)
      return (true);
#else
    if (strcmp(plaintext, passwd) == 0)
      return (true);
#endif /* HAVE_LIBCRYPT */
  }
  return (false);
}

/*!
 * Generate a filename.
 * \addtogroup utility
 * \param fname the filename buffer
 * \param fnamelen the length of the specified buffer
 * \param prefix the top-level directory
 * \param extension the filename extension
 * \param name the name of the object
 * \return true if a filename was successfully generated
 */
bool UtilityGetFileName(
	char *fname, const size_t fnamelen,
	const char *prefix,
	const char *extension,
	const char *name) {
  register bool retval = false;
  if (!fname && fnamelen) {
    Log("invalid `fname` buffer");
  } else if (*StringBlank(name) == '\0') {
    Log("invalid `name` string");
  } else {
    /* Filename buffer position */
    register size_t fnamepos = 0;

    /* Make a defensive copy */
    char nameCopy[PATH_MAX] = {'\0'};
    strlcpy(nameCopy, name, sizeof(nameCopy));

    /* Lowercase */
    if (*nameCopy != '\0') {
      register char *p = NULL;
      for (p = nameCopy; *p != '\0'; ++p)
        *p = isalnum(*p) ? tolower(*p) : '_';
    }

    /* Path prefix */
    if (*StringBlank(prefix) != '\0')
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
	*StringBlank(extension) != '\0' &&
	*StringBlank(extension) != '.' ? "." : "",
	 StringBlank(extension));

    /* Check success */
    if (*fname != '\0')
      retval = true;
  }
  return (retval);
}

/*!
 * Compares names for order.
 * \addtogroup utility
 * \param xName the first name to compare
 * \param yName the second name to compare
 * \return < 0 if the first name is less than the second; or,
 *         > 0 if the first name is greater than the second; or,
 *           0 if the specified names are equal
 */
int UtilityNameCmp(
	const char **xName,
	const char **yName) {
  return StringCmpCI(
	StringBlank(xName ? *xName : ""),
	StringBlank(yName ? *yName : ""));
}

/*!
 * Generates a random name.
 * \addtogroup utility
 * \param name the name buffer
 * \param namelen the length of the specified buffer
 * \return the specified name buffer or NULL
 */
char *UtilityNameGenerate(
	char *name,
	const size_t namelen) {
  if (!name && namelen) {
    Log("invalid `name` buffer");
  } else {
    const uint32_t value = RandomNext(&_G_random);
    if (UtilityBase36(name, namelen, value) != name) {
      Log("Couldn't BASE-36 encode value %u", value);
      abort();
    }
  }
  return (name);
}

/*!
 * \brief Returns whether a name is valid.
 * A name is considered valid if it is not NULL, it is not an
 * empty string (""), and contains only letters, digits, and the
 * underscore and dollar sign characters.
 * \addtogroup utility
 * \param name the name whose validity to return
 * \return true if the specified name is valid
 */
bool UtilityNameValid(const char *name) {
  register bool result = false;
  if (*StringBlank(name) != '\0') {
    register const char *p = name;
    for (result = true; result && *p != '\0'; ++p) {
      if (!isalnum(*p) && strchr("$_", *p) == NULL)
        result = false;
    }
  }
  return (result);
}

/*!
 * Returns a function address.
 * \addtogroup utility
 * \param procName the procedure name of the function to return
 * \return the address of the function indicated by the specified
 *     procedure name or NULL
 */
void *UtilityProcByName(const char *procName) {
  register void *proc = NULL;
  if (*StringBlank(procName) != '\0') {
#ifdef WIN32
    HMODULE m = GetModuleHandle(NULL);
    if (!m) {
      const DWORD dwError = GetLastError();
      Log("GetModuleHandle() failed: dwError=0x%u", dwError);
    } else {
      proc = GetProcAddress(m, procName);
      if (!proc) {
	const DWORD dwError = GetLastError();
	if (dwError != ERROR_PROC_NOT_FOUND)
	  Log("GetProcAddress() failed: dwError=0x%x", dwError);
      }
    }
#else /* WIN32 */
    void *m = dlopen(NULL, RTLD_LAZY | RTLD_GLOBAL);
    if (!m) {
      Log("dlopen() failed: errno=%d", errno);
    } else {
      proc = dlsym(m, procName);
      if (!proc && errno != 0)
	Log("dlsym() failed: errno=%d", errno);
      if (dlclose(m) < 0)
	Log("dlclose() failed: errno=%d", errno);
    }
#endif /* WIN32 */
  }
  return (proc);
}
