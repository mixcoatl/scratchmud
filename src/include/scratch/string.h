/*!
 * \file string.h
 *
 * \par Copyright
 * Copyright (C) 1999-2023 scratchmud.org
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
  do { \
    if (bufpos < buflen - 1) { \
      const int r_ = snprintf( \
	buf + bufpos, \
	buflen - bufpos, \
	__VA_ARGS__); \
      if (r_ > 0) { \
	const size_t rSize_ = (size_t) r_; \
	if (rSize_ < buflen - bufpos) \
	  bufpos += rSize_; \
      } \
      buf[bufpos] = '\0'; \
    } \
  } while (0)

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
	const char *right);

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
	const void *right);

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
	const char *right);

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
	const void *right);

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
	const char *str);

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
 */
void StringNormalize(char *str);

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
	const char *str);

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
	const char *str);

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
	const char *replacement);

/*!
 * Assigns a string.
 * \addtogroup string
 * \param ptr the address of the string to assign
 * \param value the string value to assign
 * \sa StringSetFormatted(char**, const char*, ...)
 */
void StringSet(
	char **ptr,
	const char *value);

/*!
 * Assigns a string.
 * \addtogroup string
 * \param ptr the address of the string to assign
 * \param format the printf-style format specifier
 * \sa StringSet(char**, const char*)
 */
void StringSetFormatted(
	char **ptr,
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

#endif /* _SCRATCH_STRING_H_ */
