/*!
 * \file random.h
 *
 * \par Copyright
 * Copyright (C) 1999-2023 scratchmud.org
 * All rights reserved.
 *
 * \author Geoffrey Davis <gdavis@scratchmud.org>
 * \addtogroup random
 */
#ifndef _SCRATCH_RANDOM_H_
#define _SCRATCH_RANDOM_H_

#include <scratch/scratch.h>

/* Forward type declarations */
typedef struct Random Random;

/*!
 * The RNG state.
 * \addtogroup random
 * \{
 */
struct Random {
  uint32_t              seed;           /*! The RNG seed value */
};
/*! \} */

/*!
 * A shared global RNG state.
 * \addtogroup random
 */
extern Random g_random;

/*!
 * Constructs a new RNG state.
 * \addtogroup random
 * \return the new RNG state or NULL
 * \sa RandomFree(Random*)
 * \sa RandomFreeV(void*)
 */
Random *RandomAlloc(void);

/*!
 * Copies a RNG state.
 * \addtogroup random
 * \param toRng the location of the copied RNG state
 * \param fromRng the RNG state to copy
 */
void RandomCopy(
	Random *toRng,
	const Random *fromRng);

/*!
 * Frees a RNG state.
 * \addtogroup random
 * \param rng the RNG state to free
 * \sa RandomAlloc()
 * \sa RandomFreeV(void*)
 */
void RandomFree(Random *rng);

/*!
 * Frees a RNG state.
 * \addtogroup random
 * \param rng the RNG state to free
 * \sa RandomAlloc()
 * \sa RandomFree(Random*)
 */
void RandomFreeV(void *rng);

/*!
 * Returns a random value.
 * \addtogroup random
 * \param rng the RNG state
 * \return a random value
 */
uint32_t RandomNext(Random *rng);

/*!
 * Returns a random value.
 * \addtogroup random
 * \param rng the RNG state
 * \param minimum the minimum value to return
 * \param maximum the maximum value to return
 * \return a random value
 */
int32_t RandomNextInt(
	Random *rng,
	const int32_t minimum,
	const int32_t maximum);

/*!
 * Reseeds a RNG state.
 * \addtogroup random
 * \param rng the RNG state to reseed
 * \param value the new seed value
 */
void RandomReseed(
	Random *rng,
	const uint32_t value);

/*!
 * Reseeds a RNG state.
 * \addtogroup random
 * \param rng the RNG state to reseed
 * \param timestamp the timestamp to use to calculate the new
 *     seed value for the specified RNG state
 */
void RandomReseedTime(
	Random *rng,
	const Time *timestamp);

#endif /* _SCRATCH_RANDOM_H_ */
