/*!
 * \file string.c
 *
 * \par Copyright
 * Copyright (C) 1999-2023 scratchmud.org
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
 * Compares strings for order.
 * \addtogroup string
 * \param left the first string to compare
 * \param right the second string to compare
 * \return < 0 if the first string is less than the second; or,
 *         > 0 if the first string is greater than the second; or,
 *           0 if the specific strings are equal
 * \sa StringCaseCompareV(const void*, const void*)
 */
int StringCaseCompare(
	const char *left,
	const char *right) {
  /* Coalesce nulls to empty strings */
  left  = left && *left != '\0' ? left : "";
  right = right && *right != '\0' ? right : "";

  /* Compare strings */
  for (; tolower(*left) == tolower(*right); ++left, ++right) {
    if (*left == '\0' || *right == '\0')
      break;
  }
  return tolower(*left) - tolower(*right);
}

/*!
 * Compares strings for order.
 * \addtogroup string
 * \param left the first string to compare
 * \param right the second string to compare
 * \return < 0 if the first string is less than the second; or,
 *         > 0 if the first string is greater than the second; or,
 *           0 if the specific strings are equal
 * \sa StringCaseCompare(const char*, const char*)
 */
int StringCaseCompareV(
	const void *left,
	const void *right) {
  return StringCaseCompare(left, right);
}

/*!
 * Compares strings for order.
 * \addtogroup string
 * \param left the first string to compare
 * \param right the second string to compare
 * \return < 0 if the first string is less than the second; or,
 *         > 0 if the first string is greater than the second; or,
 *           0 if the specified strings are equal
 * \sa StringCompareV(const void*, const void*)
 */
int StringCompare(
	const char *left,
	const char *right) {
  return strcmp(
	left && *left != '\0' ? left : "",
	right && *right != '\0' ? right : "");
}

/*!
 * Compares strings for order.
 * \addtogroup string
 * \param left the first string to compare
 * \param right the second string to compare
 * \return < 0 if the first string is less than the second; or,
 *         > 0 if the first string is greater than the second; or,
 *           0 if the specified strings are equal
 * \sa StringCompare(const char*, const char*)
 */
int StringCompareV(
	const void *left,
	const void *right) {
  return StringCompare(left, right);
}

/*!
 * Formats a string for display.
 * \param out the output buffer
 * \param outlen the length of the specified buffer
 * \param pageWidth the width of the client window in characters
 * \param indent indicates whether to indent new paragraphs
 * \param str the string to format
 * \return the number of bytes written to the specified buffer
 */
size_t StringFormat(
	char *out, const size_t outlen,
	const uint8_t pageWidth,
	const bool indent,
	const char *str) {
  register size_t outpos = 0;
  if (!out && outlen) {
    Log(L_ASSERT, "Invalid `out` buffer.");
  } else if (outlen < 3) {
    Log(L_ASSERT, "Invalid `outlen` buffer length %zu.", outlen);
  } else {
    /* Format state information */
    register bool capNext = true, capNextNext = false;
    register size_t charsLine = 0, charsWord = 0;
    register bool indentNext = indent;

    /* Iterate input string */
    register const char *p = str;
    while (p && *p != '\0') {
      /* Try to detect indentation */
      if (indent) {
	register size_t howManyEols = 0;
	register size_t howManySpacesAfterEol = 0;
	for (; *p != '\0' && isspace((int) *p); ++p) {
	  if (*p == '\n') {
	    howManySpacesAfterEol = 0;
	    howManyEols++;
	  } else if (howManyEols && *p != '\r') {
	    howManySpacesAfterEol++;
	  }
	}
	indentNext = (howManyEols > 1) ||
		     (howManyEols && howManySpacesAfterEol);
      }

      /* Read one token */
      char token[MAXLEN_INPUT] = {'\0'};
      p = StringOneWord(token, sizeof(token), p);

      /* Determine token length */
      char *first = NULL, *last = NULL;
      for (register char *q = token; *q != '\0'; ++q) {
	if (q == token)
	  charsWord = /* reset */ 0;
	if (*q == '\x1B' && *(q + 1) == '[') {
	  while (*q != '\0' && !isalpha((int) *q))
	    q++;
	} else {
	  charsWord++;
	  first = first ? first : q;
	  last = q;
	}
      }

      /* Capitalization checks */
      if (capNextNext) {
	capNextNext = false;
	capNext = true;
      }
      if (last && strchr("!?.", *last) != NULL)
	capNextNext = true;

      /* How many preceding spaces */
      register int precedingSpaces = 0;
      if (indentNext) {
	precedingSpaces = 3;
      } else if (charsLine) {
	precedingSpaces = capNext ? 2 : 1;
      }

      /* Break line if token too long */
      /* Minimum length prevents small words at end-of-line */
      const size_t realCharsWord = charsWord > 3 ? charsWord : 3;
      if (charsLine + realCharsWord + precedingSpaces > pageWidth) {
	BPrintf(out, outlen - 2, outpos, "\r\n");
	charsLine = precedingSpaces = 0;
      }

      /* Capitalize token */
      if (capNext && first) {
	*first = toupper((int) *first);
	capNext = false;
      }

      /* Copy token to output */
      const size_t lastOutpos = outpos;
      BPrintf(out, outlen - 2, outpos, "%*.*s%s",
		precedingSpaces, precedingSpaces, "", token);

      /* Handle buffer overflow */
      if (lastOutpos == outpos)
	break;

      /* Token length */
      charsLine += precedingSpaces;
      charsLine += charsWord;
    }

    /* Add closing end-of-line */
    if (charsLine)
      BPrintf(out, outlen, outpos, "\r\n");
  }

  /* Terminate */
  if (out && outlen)
    out[outpos] = '\0';

  return (outpos);
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
 */
void StringNormalize(char *str) {
  if (!str) {
    Log(L_ASSERT, "Invalid `str` string.");
  } else if (*str != '\0') {
    StringNormalizeCopy(str, strlen(str) + 1, str);
  }
}

/*!
 * Normalizes whitespace.
 * \addtogroup string
 * \param out the output buffer
 * \param outlen the length of the specified buffer
 * \param str the string to normalize
 * \return the number of bytes written to the specified buffer
 */
size_t StringNormalizeCopy(
	char *out, const size_t outlen,
	const char *str) {
  register size_t outpos = 0;
  if (!out && outlen) {
    Log(L_ASSERT, "Invalid `out` buffer.");
  } else if (!str) {
    Log(L_ASSERT, "Invalid `str` string.");
  } else if (*str != '\0') {
    /* Iterate input string */
    register const char *p = str;
    while (p && *p != '\0') {
      /* Read one word */
      char word[MAXLEN_INPUT] = {'\0'};
      p = StringOneWord(word, sizeof(word), p);

      /* Copy word to buffer */
      BPrintf(out, outlen, outpos, "%s%s",
	outpos && *word != '\0' ? " " : "",
	word);
    }
  }
  return (outpos);
}

/*!
 * Reads a word from a string.
 * \addtogroup string
 * \param out the output buffer
 * \param outlen the length of the specified buffer
 * \param str the string from to read which a word
 * \return a pointer to the character immediately following
 *     the first word in the specified string, or NULL if an
 *     error occurs while processing the string
 */
const char *StringOneWord(
	char *out, const size_t outlen,
	const char *str) {
  register const char *ptr = NULL;
  if (!out && outlen) {
    Log(L_ASSERT, "Invalid `out` buffer.");
  } else if (!str) {
    Log(L_ASSERT, "Invalid `str` string.");
  } else {
    register size_t outpos = 0;
    for (ptr = StringSkipSpaces(str);
	*ptr != '\0' && !isspace((int) *ptr); ++ptr) {
      BPrintf(out, outlen, outpos, "%c", *ptr);
    }
    if (out && outpos < outlen - 1)
      out[outpos] = '\0';
  }
  return (ptr);
}

/*!
 * Inline string replacement.
 * \addtogroup string
 * \param out the output buffer
 * \param outlen the length of the specified buffer
 * \param str the input string
 * \param substr the token to replace
 * \param replacement the string with which to replace the token
 * \return the number of bytes written to the specified buffer
 */
size_t StringReplace(
	char *out, const size_t outlen,
	const char *str,
	const char *substr,
	const char *replacement) {
  register size_t outpos = 0;
  if (!out && outlen) {
    Log(L_ASSERT, "Invalid `out` buffer.");
  } else if (!str) {
    Log(L_ASSERT, "Invalid `str` string.");
  } else if (!substr || *substr == '\0') {
    Log(L_ASSERT, "Invalid `substr` string.");
  } else if (!replacement) {
    Log(L_ASSERT, "Invalid `replacement` string.");
  } else if (*str != '\0') {
    /* Temporary buffer */
    char temp[MAXLEN_STRING] = {'\0'};
    register size_t temppos = 0;

    /* Replace token occurrences */
    for (register const char *p = str; p && *p != '\0'; ) {
      register const char *q = strstr(p, substr);
      for (; p && p != q && *p != '\0'; ++p) {
	BPrintf(temp, sizeof(temp), temppos, "%c", *p);
      }
      if (q) {
	BPrintf(temp, sizeof(temp), temppos, "%s", replacement);
	p += strlen(substr);
      }
    }

    /* Copy temporary buffer to output */
    BPrintf(out, outlen, outpos, "%s", temp);
  }
  /* Terminate */
  if (out && outlen)
    out[outpos] = '\0';

  return (outpos);
}

/*!
 * Assigns a string.
 * \addtogroup string
 * \param ptr the address of the string to assign
 * \param value the string value to assign
 * \sa StringSetFormatted(char**, const char*, ...)
 */
void StringSet(
	char **ptr,
	const char *value) {
  if (!ptr) {
    Log(L_ASSERT, "Invalid `ptr` string pointer.");
  } else {
    /* Copy value */
    register char *str = NULL;
    if (value && (str = strdup(value)) == NULL)
      Log(L_SYSTEM, "strdup() failed: errno=%d.", errno);

    /* Free previous value */
    if (*ptr)
      StringFree(*ptr);

    /* Now assign */
    *ptr = str;
  }
}

/*!
 * Assigns a string.
 * \addtogroup string
 * \param ptr the address of the string to assign
 * \param format the printf-style format specifier
 * \sa StringSet(char**, const char*)
 */
void StringSetFormatted(
	char **ptr,
	const char *format, ...) {
  if (!ptr) {
    Log(L_ASSERT, "Invalid `ptr` string pointer.");
  } else if (!format) {
    Log(L_ASSERT, "Invalid `format` string.");
  } else {
    va_list args;
    va_start(args, format);
    char formatted[MAXLEN_STRING] = {'\0'};
    if (vsnprintf(formatted, sizeof(formatted), format, args) < 0) {
      Log(L_SYSTEM, "vsnprintf() failed: errno=%d.", errno);
    } else {
      StringSet(ptr, formatted);
    }
    va_end(args);
  }
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
    Log(L_ASSERT, "Invalid `str` string.");
  } else {
    while (*str != '\0' && isspace((int) *str))
      ++str;
  }
  return (str);
}
