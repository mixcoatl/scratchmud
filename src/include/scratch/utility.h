/*!
 * \file utility.h
 *
 * \par Copyright
 * Copyright (C) 1999-2023 scratchmud.org
 * All rights reserved.
 *
 * \author Geoffrey Davis <gdavis@scratchmud.org>
 * \addtogroup utility
 */
#ifndef _SCRATCH_UTILITY_H_
#define _SCRATCH_UTILITY_H_

#include <scratch/scratch.h>

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
	const uint64_t value);

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
	const char *plaintext);

/*!
 * Matches a password hash.
 * \addtogroup utility
 * \param passwd the password hash to match
 * \param plaintext the plaintext password
 * \return true if the passwords match
 */
bool UtilityCryptMatch(
	const char *passwd,
	const char *plaintext);

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
	const char *name);

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
	const char **right);

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
	const void *right);

/*!
 * Generates a random name.
 * \addtogroup utility
 * \param out the name output buffer
 * \param outlen the length of the specified buffer
 * \return the number of bytes written to the specified buffer
 */
size_t UtilityNameGenerate(
	char *out, const size_t outlen);

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
bool UtilityNameValid(const char *name);

/*!
 * Resolves a procedure name.
 * \addtogroup utility
 * \param procName the procedure name of the function to resolve
 * \return the address of the function indicated by the specified
 *     procedure name or NULL
 */
void *UtilityProcByName(const char *procName);

#endif /* _SCRATCH_UTILITY_H_ */
