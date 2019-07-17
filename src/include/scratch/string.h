/*!
 * \file string.h
 *
 * \par Copyright
 * Copyright (C) 1999-2019 scratchmud.org
 * All rights reserved.
 *
 * \author Geoffrey Davis <gdavis@scratchmud.org>
 * \addtogroup string
 */
#ifndef _SCRATCH_STRING_H_
#define _SCRATCH_STRING_H_

#include <scratch/scratch.h>

/*!
 * Prints to a static buffer.
 * \addtogroup string
 * \param buf the buffer
 * \param buflen the length of the specified buffer
 * \param bufpos the current offset into the specified buffer
 */
#define BPrintf(buf, buflen, bufpos, ...) \
  if (bufpos < buflen - 1) { \
    const ssize_t _r = snprintf( \
        buf + bufpos, \
        buflen - bufpos, \
        __VA_ARGS__); \
    if (_r >= 0 && _r < buflen - bufpos) { \
      bufpos += _r; \
    } \
    buf[bufpos] = '\0'; \
  }

/*!
 * Returns blank if a string is null.
 * \addtogroup string
 * \param str the string to check
 * \return the specified string or the empty string ("")
 */
#define StringBlank(str) \
  ((str) && *(str) != '\0' ? (str) : "")

/*!
 * Returns <Undefined> if a string is blank or null.
 * \addtogroup string
 * \param str the string to check
 * \return the specified string or the <Undefined> string
 */
#define StringUndefined(str) \
  ((str) && *(str) != '\0' ? (str) : "<Undefined>")

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
	const char *yString);

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
	const char *yString);

/*!
 * Creates a string.
 * \addtogroup string
 * \param format the printf-style format specifier string
 * \return the new string or NULL
 */
char *StringCreate(const char *format, ...);

/*!
 * Frees a string.
 * \addtogroup string
 * \param str the string to free
 */
void StringFree(char *str);

/*!
 * Normalizes whitespace.
 * \addtogroup string
 * \param str the string to normalize
 * \return the specified string or NULL
 */
char *StringNormalize(char *str);

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
	const char *str);

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
	const char *substr);

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
	const char *substr);

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
	const char *replacement);

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
	const char *value);

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
	const char *format, ...);

/*!
 * Skips leading whitespace.
 * \addtogroup string
 * \param str the string to process
 * \return a pointer to the first non-whitespace character
 *     in the specified string, or NULL if an error occurs
 *     while processing the specified string
 */
const char *StringSkipSpaces(const char *str);

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
	const char *substr);

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
	const char *substr);

#endif /* _SCRATCH_STRING_H_ */
