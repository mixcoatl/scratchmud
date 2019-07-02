/*!
 * \file block.c
 *
 * \par Copyright
 * Copyright (C) 1999-2019 scratchmud.org
 * All rights reserved.
 *
 * \author Geoffrey Davis <gdavis@scratchmud.org>
 * \addtogroup block
 */
#define _SCRATCH_BLOCK_C_

#include <scratch/bitvector.h>
#include <scratch/block.h>
#include <scratch/log.h>
#include <scratch/scratch.h>
#include <scratch/string.h>

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
	const Bitvector bits) {
  if (!out && outlen) {
    Log("invalid `out` buffer");
  } else if (!block) {
    Log("invalid `block` string block");
  } else if (!bits) {
    strlcpy(out, "NoBits", outlen);
  } else {
    register size_t outpos = 0;
    register size_t bitN;
    for (bitN = 0; *block[bitN] != '\n'; ++bitN) {
      BPrintf(out, outlen, outpos, "%s%s",
		BitCheckN(bits, bitN) && outpos ? " " : "",
		BitCheckN(bits, bitN) ? block[bitN] : "");
    }
  }
}

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
	const int type) {
  if (!out && outlen) {
    Log("invalid `out` buffer");
  } else if (!block) {
    Log("invalid `block` string block");
  } else {
    register size_t bitN;
    for (bitN = 0; *block[bitN] != '\n'; ++bitN) {
      if (bitN == type) {
	strlcpy(out, block[bitN], outlen);
	break;
      }
    }
    if (*block[bitN] == '\n')
      strlcpy(out, "*Undefined*", outlen);
  }
}

/*!
 * Searches a string block.
 * \addtogroup block
 * \param block the string block to search
 * \param str the string for which to search
 * \return the zero-based index of the specified string or -1
 */
ssize_t BlockSearch(
	const char **block,
	const char *str) {
  if (!block) {
    Log("invalid `block` string block");
  } else {
    register size_t blockN;
    for (blockN = 0; *block[blockN] != '\n'; ++blockN) {
      if (StringCmpCI(block[blockN], str) == 0)
	return (blockN);
    }
  }
  return (-1);
}

/*!
 * Returns the size of a string block.
 * \addtogroup block
 * \param block the string block whose size to return
 * \return the size of the specified string block
 */
size_t BlockSize(const char **block) {
  register size_t size = 0;
  if (!block) {
    Log("invalid `block` string block");
  } else {
    while (*block[size] != '\n')
      ++size;
  }
  return (size);
}
