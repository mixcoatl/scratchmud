/*!
 * \file random.c
 *
 * \par Copyright
 * Copyright (C) 1999-2023 scratchmud.org
 * All rights reserved.
 *
 * \author Geoffrey Davis <gdavis@scratchmud.org>
 * \addtogroup random
 */
#define _SCRATCH_RANDOM_C_

#include <scratch/log.h>
#include <scratch/memory.h>
#include <scratch/random.h>
#include <scratch/scratch.h>
#include <scratch/time.h>

/*!
 * A shared global RNG state.
 * \addtogroup random
 */
Random g_random;

/*!
 * Constructs a new RNG state.
 * \addtogroup random
 * \return the new RNG state or NULL
 * \sa RandomFree(Random*)
 * \sa RandomFreeV(void*)
 */
Random *RandomAlloc(void) {
  Random *rng;
  MemoryCreate(rng, Random, 1);
  RandomReseedTime(rng, NULL);
  return (rng);
}

/*!
 * Copies a RNG state.
 * \addtogroup random
 * \param toRng the location of the copied RNG state
 * \param fromRng the RNG state to copy
 */
void RandomCopy(
	Random *toRng,
	const Random *fromRng) {
  if (!toRng) {
    Log(L_ASSERT, "Invalid `toRng` Random.");
  } else if (!fromRng) {
    Log(L_ASSERT, "Invalid `fromRng` Random.");
  } else if (toRng != fromRng) {
    toRng->seed = fromRng->seed;
  }
}

/*!
 * Frees a RNG state.
 * \addtogroup random
 * \param rng the RNG state to free
 * \sa RandomAlloc()
 * \sa RandomFreeV(void*)
 */
void RandomFree(Random *rng) {
  MemoryFree(rng);
}

/*!
 * Frees a RNG state.
 * \addtogroup random
 * \param rng the RNG state to free
 * \sa RandomAlloc()
 * \sa RandomFree(Random*)
 */
void RandomFreeV(void *rng) {
  RandomFree(rng);
}

/*
 * This program is public domain and was written by William S. England
 * (Oct 1988).  It is based on an article by:
 *
 * Stephen K. Park and Keith W. Miller. RANDOM NUMBER GENERATORS:
 * GOOD ONES ARE HARD TO FIND. Communications of the ACM, New York,
 * NY, October 1988 p.1192
 *
 * The following is a portable c program for generating random numbers.
 * The modulus and multipilier have been extensively tested and should
 * not be changed except by someone who is a professional Lehmer
 * generator writer.  THIS GENERATOR REPRESENTS THE MINIMUM STANDARD
 * AGAINST WHICH OTHER GENERATORS SHOULD BE JUDGED. ("Quote from the
 * referenced article's authors. WSE" )
 */

/*!
 * Returns a random value.
 * \addtogroup random
 * \param rng the RNG state
 * \return a random value
 */
uint32_t RandomNext(Random *rng) {
  register uint32_t result = 0;
  if (!rng) {
    Log(L_ASSERT, "Invalid `rng` Random.");
  } else {
    /* Lehmer LCG parameters */
    const uint32_t m = 2147483647;
    const uint32_t a = 48271;
    const uint32_t q = m / a;
    const uint32_t r = m % a;

    /* Calculate low and high values */
    const uint32_t high = rng->seed / q;
    const uint32_t low  = rng->seed % q;

    /* Calculate test value */
    const int32_t test = a * low - r * high;

    /* Calculate new seed value */
    if (test > 0) {
      result = rng->seed = test;
    } else {
      result = rng->seed = test + m;
    }
  }
  return (result);
}

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
	const int32_t maximum) {
  register int32_t result = 0;
  if (!rng) {
    Log(L_ASSERT, "Invalid `rng` Random.");
  } else {
    /* Calculate range */
    const int32_t range = maximum - minimum + 1;

    /* Generate random value within range */
    result = RandomNext(rng) % range + minimum;
  }
  return (result);
}

/*!
 * Reseeds a RNG state.
 * \addtogroup random
 * \param rng the RNG state to reseed
 * \param value the new seed value
 */
void RandomReseed(
	Random *rng,
	const uint32_t value) {
  if (!rng) {
    Log(L_ASSERT, "Invalid `rng` Random.");
  } else {
    /* Assign new RNG seed */
    rng->seed = value ^ 0xAAAAAAAA;
  }
}

/*!
 * Reseeds a RNG state.
 * \addtogroup random
 * \param rng the RNG state to reseed
 * \param timestamp the timestamp to use to calculate the new
 *     seed value for the specified RNG state
 */
void RandomReseedTime(
	Random *rng,
	const Time *timestamp) {
  if (!rng) {
    Log(L_ASSERT, "Invalid `rng` Random.");
  } else {
    Time seed;
    if (timestamp) {
      TimeCopy(&seed, timestamp);
    } else {
      /* Use current time */
      TimeCurrent(&seed);
    }
    rng->seed = seed.tv_sec ^ seed.tv_usec;
  }
}
