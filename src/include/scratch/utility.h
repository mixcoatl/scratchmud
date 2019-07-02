/*!
 * \file utility.h
 *
 * \par Copyright
 * Copyright (C) 1999-2019 scratchmud.org
 * All rights reserved.
 *
 * \author Geoffrey Davis <gdavis@scratchmud.org>
 * \addtogroup utility
 */
#ifndef _SCRATCH_UTILITY_H_
#define _SCRATCH_UTILITY_H_

#include <scratch/scratch.h>

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
	const uint32_t value);

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
	const char *name);

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
	const char **yName);

/*!
 * Generates a random name.
 * \addtogroup utility
 * \param name the name buffer
 * \param namelen the length of the specified buffer
 * \return the specified name buffer or NULL
 */
char *UtilityNameGenerate(
	char *name,
	const size_t namelen);

/*!
 * \brief Returns whether a name is valid.
 * A name is considered valid if it is not NULL, it is not an
 * empty string (""), and contains only letters, digits, and the
 * underscore and dollar sign characters.
 * \addtogroup utility
 * \param name the name whose validity to return
 * \return true if the specified name is valid
 */
bool UtilityNameValid(const char *name);

/*!
 * Returns a function address.
 * \addtogroup utility
 * \param procName the procedure name of the function to return
 * \return the address of the function indicated by the specified
 *     procedure name or NULL
 */
void *UtilityProcByName(const char *procName);

#endif /* _SCRATCH_UTILITY_H_ */
