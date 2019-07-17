/*!
 * \file string.c
 *
 * \par Copyright
 * Copyright (C) 1999-2019 scratchmud.org
 * All rights reserved.
 *
 * \author Geoffrey Davis <gdavis@scratchmud.org>
 * \addtogroup string
 */
#define _SCRATCH_STRING_C_

#include <scratch/log.h>
#include <scratch/memory.h>
#include <scratch/scratch.h>
#include <scratch/string.h>

/*!
 * Compares strings with normalized whitespace.
 * \addtogroup string
 * \param xString the first string to compare
 * \param yString the second string to compare
 * \return < 0 if the first string is less than the second; or,
 *         > 0 if the first string is greater than the second; or,
 *           0 if the strings are equal
 */
int StringCmp(
	const char *xString,
	const char *yString) {
  register int result = 0;

  /* Compare empty strings instead of NULLs */
  xString = StringBlank(xString);
  yString = StringBlank(yString);

  /* Loop until the strings do not match */
  while (!result && *xString != '\0' && *yString != '\0') {
    /* Some word buffers */
    char xWord[MAXLEN_INPUT] = {'\0'};
    char yWord[MAXLEN_INPUT] = {'\0'};

    /* Read words from each string */
    xString = StringOneWord(xWord, sizeof(xWord), xString);
    yString = StringOneWord(yWord, sizeof(yWord), yString);

    /* Compare the words for order */
    result = strcmp(xWord, yWord);
  }
  /* The string whose end was found first sorts first */
  if (!result && (*xString != '\0' || *yString != '\0')) {
    result = *xString == '\0' ? -1 :
	     *yString == '\0' ? +1 : 0;
  }
  return (result);
}

/*!
 * Compares strings with normalized whitespace.
 * \note This function is case-insensitive.
 * \addtogroup string
 * \param xString the first string to compare
 * \param yString the second string to compare
 * \return < 0 if the first string is less than the second; or,
 *         > 0 if the first string is greater than the second; or,
 *           0 if the strings are equal
 */
int StringCmpCI(
	const char *xString,
	const char *yString) {
  register int result = 0;

  /* Compare empty strings instead of NULLs */
  xString = StringBlank(xString);
  yString = StringBlank(yString);

  /* Loop until the strings do not match */
  while (!result && *xString != '\0' && *yString != '\0') {
    /* Some word buffers */
    char xWord[MAXLEN_INPUT] = {'\0'};
    char yWord[MAXLEN_INPUT] = {'\0'};

    /* Read words from each string */
    xString = StringOneWord(xWord, sizeof(xWord), xString);
    yString = StringOneWord(yWord, sizeof(yWord), yString);

    /* Compare the words for order */
    result = strcasecmp(xWord, yWord);
  }
  /* The string whose end was found first sorts first */
  if (!result && (*xString != '\0' || *yString != '\0')) {
    result = *xString == '\0' ? -1 :
	     *yString == '\0' ? +1 : 0;
  }
  return (result);
}

/*!
 * Creates a string.
 * \addtogroup string
 * \param format the printf-style format specifier string
 * \return the new string or NULL
 */
char *StringCreate(const char *format, ...) {
  char *str = NULL;
  if (format) {
    /* The message buffer */
    char messg[MAXLEN_STRING] = {'\0'};

    /* Variadic arguments */
    va_list args;
    va_start(args, format);
    if (vsnprintf(messg, sizeof(messg), format, args) < 0) {
      Log("vsnprint() failed: errno=%d", errno);
    } else {
      str = strdup(messg);
    }
    va_end(args);
  }
  return (str);
}

/*!
 * Frees a string.
 * \addtogroup string
 * \param str the string to free
 */
void StringFree(char *str) {
  MemoryFree(str);
}

/*!
 * Normalizes whitespace.
 * \addtogroup string
 * \param str the string to normalize
 * \return the specified string or NULL
 */
char *StringNormalize(char *str) {
  if (str && *str != '\0') {
    register bool flag = false; /* Indicates if we're writing */
    register char *in  = str;   /* The current read position */
    register char *out = str;   /* The current write position */

    /* Iterate over the string and modify it in place */
    while (*in != '\0') {
      if (flag) {
	/* Check for whitespace */
	if (isspace(*in)) {
	  flag = false;
	} else {
	  /* Write the character back to the string */
	  *out++ = *in;
	}
      } else {
	/* Look for non-whitespace characters */
	if (!isspace(*in)) {
	  /* One space between words */
	  if (out != str)
	    *out++ = ' ';

	  /* Write the character back to the string */
	  *out++ = *in;

	  /* We're in write mode now. */
	  flag = true;
	}
      }
      /* Advance one character. */
      ++in;
    }
    *out = '\0';
  }
  return (str);
}

/*!
 * Reads the first word from a string.
 * \addtogroup string
 * \param word the buffer to contain the first word
 * \param wordlen the length of the specified buffer
 * \param str the string from to read which the first word
 * \return a pointer to the character immediately following
 *     the first word in the specified string, or NULL if an
 *     error occurs while processing the string
 */
const char *StringOneWord(
	char *word, const size_t wordlen,
	const char *str) {
  register const char *ptr = NULL;
  if (!word && wordlen) {
    Log("invalid `word` buffer");
  } else if (!str) {
    Log("invalid `str` string");
  } else {
    register size_t wordpos = 0;
    for (ptr = StringSkipSpaces(str);
	*ptr != '\0' && !isspace(*ptr); ++ptr) {
      if (word && wordpos < wordlen - 1)
	word[wordpos++] = *ptr;
    }
    if (word && wordpos < wordlen)
      word[wordpos] = '\0';
  }
  return (ptr);
}


/*!
 * Returns whether a string starts with a substring.
 * \addtogroup string
 * \param str the string to test
 * \param substr the substring for which to test
 * \return true if the specified string starts with the
 *     specified substring
 */
bool StringPrefix(
	const char *str,
	const char *substr) {
  register bool retval = false;
  if (!str) {
    Log("invalid `str` string");
  } else if (!substr) {
    Log("invalid `substr` string");
  } else {
    const size_t strN = strlen(str);       /* Length of string */
    const size_t substrN = strlen(substr); /* Length of substring */
    if (strN >= substrN && strncmp(str, substr, substrN) == 0)
      retval = true;
  }
  return (retval);
}


/*!
 * Returns whether a string starts with a substring.
 * \note This function is case-insensitive.
 * \addtogroup string
 * \param str the string to test
 * \param substr the substring for which to test
 * \return true if the specified string starts with the
 *     specified substring
 */
bool StringPrefixCI(
	const char *str,
	const char *substr) {
  register bool retval = false;
  if (!str) {
    Log("invalid `str` string");
  } else if (!substr) {
    Log("invalid `substr` string");
  } else {
    const size_t strN = strlen(str);       /* Length of string */
    const size_t substrN = strlen(substr); /* Length of substring */
    if (strN >= substrN && strncasecmp(str, substr, substrN) == 0)
      retval = true;
  }
  return (retval);
}

/*!
 * Inline string replacement.
 * \addtogroup string
 * \param out the output buffer
 * \param outlen the length of the specified buffer
 * \param input the input string
 * \param token the token to be replaced
 * \param replacement the string to replace the token with
 * \return the number of characters written the specified buffer
 */
ssize_t StringReplace(
	char *out, const size_t outlen,
	const char *input,
	const char *token,
	const char *replacement) {
  register size_t outpos = 0;
  if (!out && outlen) {
    Log("invalid `out` buffer");
  } else if (!input) {
    Log("invalid `input` string");
  } else if (*StringBlank(token) == '\0') {
    Log("invalid `token` string");
  } else if (!replacement) {
    Log("invalid `replacement` string");
  } else {
    /* The temporary buffer */
    char temp[MAXLEN_STRING] = {'\0'};
    register size_t temppos = 0;

    if (*input != '\0') {
      const size_t tokenN = strlen(token);
      register const char *p = input;
      while (p && *p != '\0') {
	register const char *q = strstr(p, token);
	for (; p && p != q && *p != '\0'; ++p)
	  BPrintf(temp, sizeof(temp), temppos, "%c", *p);
	if (q) {
	  BPrintf(temp, sizeof(temp), temppos, "%s", replacement);
	  p += tokenN;
	}
      }
    }

    /* Copy the temporary buffer to output */
    BPrintf(out, outlen, outpos, "%s", temp);
  }
  return (outpos);
}

/*!
 * Assigns a string.
 * \addtogroup string
 * \param sptr the address of the string to assign
 * \param value the string value to assign
 * \return the assigned string or NULL
 * \sa StringSetFormatted(char**, const char*, ...)
 */
char *StringSet(
	char **sptr,
	const char *value) {
  register char *str = NULL;
  if (!sptr) {
    Log("invalid `sptr` string pointer");
  } else {
    if (value) {
      if ((str = strdup(value)) == NULL)
        Log("strdup() failed: errno=%d", errno);
    }
    if (*sptr)
      StringFree(*sptr);

    *sptr = str;
  }
  return (str);
}

/*!
 * Assigns a string.
 * \addtogroup string
 * \param sptr the address of the string to assign
 * \param format the printf-style format specifier
 * \return the assigned string or NULL
 * \sa StringSetFormatted(char**, const char*)
 */
char *StringSetFormatted(
	char **sptr,
	const char *format, ...) {
  register char *str = NULL;
  if (!sptr) {
    Log("invalid `sptr` string pointer");
  } else {
    char messg[MAXLEN_STRING] = {'\0'};
    va_list args;
    va_start(args, format);
    if (vsnprintf(messg, sizeof(messg), StringBlank(format), args) < 0) {
      Log("vsnprintf() failed: errno=%d", errno);
    } else {
      str = StringSet(sptr, messg);
    }
    va_end(args);
  }
  return (str);
}

/*!
 * Skips leading whitespace.
 * \addtogroup string
 * \param str the string to process
 * \return a pointer to the first non-whitespace character
 *     in the specified string, or NULL if an error occurs
 *     while processing the specified string
 */
const char *StringSkipSpaces(const char *str) {
  if (!str) {
    Log("invalid `str` string");
  } else {
    while (*str != '\0' && isspace(*str))
      ++str;
  }
  return (str);
}

/*!
 * Returns whether a string ends with a substring.
 * \addtogroup string
 * \param str the string to test
 * \param substr the substring for which to test
 * \return true if the specified string ends with the
 *     specified substring
 */
bool StringSuffix(
	const char *str,
	const char *substr) {
  register bool retval = false;
  if (!str) {
    Log("invalid `str` string");
  } else if (!substr) {
    Log("invalid `substr` string");
  } else {
    const size_t strN = strlen(str);       /* Length of string */
    const size_t substrN = strlen(substr); /* Length of substring */
    if (strN >= substrN && strcmp(str + strN - substrN, substr) == 0)
      retval = true;
  }
  return (retval);
}

/*!
 * Returns whether a string ends with a substring.
 * \note This function is case-insensitive.
 * \addtogroup string
 * \param str the string to test
 * \param substr the substring for which to test
 * \return true if the specified string ends with the
 *     specified substring
 */
bool StringSuffixCI(
	const char *str,
	const char *substr) {
  register bool retval = false;
  if (!str) {
    Log("invalid `str` string");
  } else if (!substr) {
    Log("invalid `substr` string");
  } else {
    const size_t strN = strlen(str);       /* Length of string */
    const size_t substrN = strlen(substr); /* Length of substring */
    if (strN >= substrN && strcasecmp(str + strN - substrN, substr) == 0)
      retval = true;
  }
  return (retval);
}

