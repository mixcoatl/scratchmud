/*!
 * \file random.h
 *
 * \par Copyright
 * Copyright (C) 1999-2019 scratchmud.org
 * All rights reserved.
 *
 * \addtogroup random
 * \author Geoffrey Davis <gdavis@scratchmud.org>
 */

#ifndef _SCRATCH_RANDOM_H_
#define _SCRATCH_RANDOM_H_

#include <scratch/scratch.h>
#include <scratch/time.h>

/* Forward type declarations */
typedef struct _Random Random;

/*!
 * The RNG state.
 * \addtogroup random
 * \{
 */
struct _Random {
  uint32_t      seed;           /*! The RNG seed value */
};
/*! \} */

/*!
 * A shared global RNG state.
 * \addtogroup random
 */
extern Random _G_random;

/*!
 * Constructs a new RNG state.
 * \addtogroup random
 * \return a new RNG state or NULL
 * \sa RandomFree(Random*)
 */
Random *RandomAlloc(void);

/*!
 * Copies a RNG state.
 * \addtogroup random
 * \param toRng the RNG state into which to copy
 * \param fromRng the RNG state to copy
 * \return the RNG state indicated by `toRng` or NULL
 */
Random *RandomCopy(
	Random *toRng,
	const Random *fromRng);

/*!
 * Frees a RNG state.
 * \addtogroup random
 * \param rng the RNG state to free
 * \sa RandomAlloc()
 */
void RandomFree(Random *rng);

/*!
 * Generates a pseudo-random value.
 * \addtogroup random
 * \param rng the RNG state
 * \return the pseudo-random value or zero
 */
uint32_t RandomNext(Random *rng);

/*!
 * Generates a pseudo-random value.
 * \addtogroup random
 * \param rng the RNG state
 * \return the pseudo-random value or zero
 */
float RandomNextFloat(Random *rng);

/*!
 * Generates a pseudo-random value.
 * \addtogroup random
 * \param rng the RNG state
 * \param minValue the minimum value to return
 * \param maxValue the maximum value to return
 * \return the pseudo-random value or zero
 */
int32_t RandomNextInt(
	Random *rng,
	const int32_t minValue,
	const int32_t maxValue);

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
