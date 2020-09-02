/*!
 * \file bitvector.h
 *
 * \par Copyright
 * Copyright (C) 1999-2019 scratchmud.org
 * All rights reserved.
 *
 * \author Geoffrey Davis <gdavis@scratchmud.org>
 * \addtogroup bitvector
 */
#ifndef _SCRATCH_BITVECTOR_H_
#define _SCRATCH_BITVECTOR_H_

#include <scratch/scratch.h>

/*!
 * Returns the bit value for a bit index.
 * \addtogroup bitvector
 * \param bitno the zero-based index of the bit
 * \return the bit-shifted value of the specified bit index
 */
#define Bit(bitno) \
  (1 << (bitno))

/*!
 * Clears a bit value.
 * \addtogroup bitvector
 * \param bitv the bitvector
 * \param bitvalue the bit values
 */
#define BitRemove(bitv, bitvalue) \
  ((bitv) &= ~(bitvalue))

/*!
 * Removes a bit by bit index.
 * \addtogroup bitvector
 * \param bitv the bitvector
 * \param bitno the zero-based index of the bit
 */
#define BitRemoveN(bitv, bitno) \
  BitRemove(bitv, Bit(bitno))

/*!
 * Sets a bit value.
 * \addtogroup bitvector
 * \param bitv the bitvector
 * \param bitvalue the bit values
 */
#define BitSet(bitv, bitvalue) \
  ((bitv) |= (bitvalue))

/*!
 * Sets a bit by bit index.
 * \addtogroup bitvector
 * \param bitv the bitvector
 * \param bitno the zero-based index of the bit
 */
#define BitSetN(bitv, bitno) \
  BitSet(bitv, Bit(bitno))

/*!
 * Checks a bit value.
 * \addtogroup bitvector
 * \param bitv the bitvector to test
 * \param bitvalue the bit values
 * \return true if any of the bits indicated by the specified bit
 *     value are set in the specified bitvector
 */
#define BitCheck(bitv, bitvalue) \
  (((bitv) & (bitvalue)) != 0)

/*!
 * Checks a bit by bit index.
 * \addtogroup bitvector
 * \param bitv the bitvector to test
 * \param bitno the zero-based index of the bit
 * \return true if the bit value indicated by the specified
 *     bit index is set in the specified bitvector
 */
#define BitCheckN(bitv, bitno) \
  BitCheck(bitv, Bit(bitno))

/*!
 * Toggles a bit value.
 * \addtogroup bitvector
 * \param bitv the bitvector
 * \param bitvalue the bit values
 */
#define BitToggle(bitv, bitvalue) \
  ((bitv) ^= (bitvalue))

/*!
 * Toggles a bit by bit index.
 * \addtogroup bitvector
 * \param bitv the bitvector
 * \param bitno the zero-based index of the bit
 */
#define BitToggleN(bitv, bitno) \
  BitToggle(bitv, Bit(bitno))

/*!
 * The bitvector type.
 * \addtogroup bitvector
 */
typedef uint64_t Bitvector;

#endif /* _SCRATCH_BITVECTOR_H_ */
