/*!
 * \file block.h
 *
 * \par Copyright
 * Copyright (C) 1999-2019 scratchmud.org
 * All rights reserved.
 *
 * \author Geoffrey Davis <gdavis@scratchmud.org>
 * \addtogroup block
 */
#ifndef _SCRATCH_BLOCK_H_
#define _SCRATCH_BLOCK_H_

#include <scratch/bitvector.h>
#include <scratch/scratch.h>

/*!
 * Prints a bitvector value.
 * \addtogroup block
 * \param out the output buffer
 * \param outlen the length of the specified buffer
 * \param block the string block that contains the bit strings
 * \param bits the bitvector to print out
 * \sa BlockPrintType(char*, const size_t, const char**, const int)
 */
void BlockPrintBits(
	char *out, const size_t outlen,
	const char **block,
	const Bitvector bits);

/*!
 * Prints a type value.
 * \addtogroup block
 * \param out the output buffer
 * \param outlen the length of the specified buffer
 * \param block the string block that contains the type strings
 * \param type the type to print out
 * \sa BlockPrintBits(char*, const size_t, const char**, const Bitvector)
 */
void BlockPrintType(
	char *out, const size_t outlen,
	const char **block,
	const int type);

/*!
 * Searches a string block.
 * \addtogroup block
 * \param block the string block to search
 * \param str the string for which to search
 * \return the zero-based index of the specified string or -1
 */
ssize_t BlockSearch(
	const char **block,
	const char *str);

/*!
 * Returns the size of a string block.
 * \addtogroup block
 * \param block the string block whose size to return
 * \return the size of the specified string block
 */
size_t BlockSize(const char **block);

#endif /* _SCRATCH_BLOCK_H_ */
