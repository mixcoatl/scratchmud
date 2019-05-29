/*!
 * \file random.c
 *
 * \par Copyright
 * Copyright (C) 1999-2019 scratchmud.org
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
Random _G_random;

/*!
 * Constructs a new RNG state.
 * \addtogroup random
 * \return a new RNG state or NULL
 * \sa RandomFree(Random*)
 */
Random *RandomAlloc(void) {
  Random *rng = NULL;
  MemoryCreate(rng, Random, 1);
  RandomReseedTime(rng, NULL);
  return (rng);
}

/*!
 * Copies a RNG state.
 * \addtogroup random
 * \param toRng the RNG state into which to copy
 * \param fromRng the RNG state to copy
 * \return the RNG state indicated by `toRng` or NULL
 */
Random *RandomCopy(
	Random *toRng,
	const Random *fromRng) {
  if (!toRng) {
    Log("invalid `toRng` Random");
  } else if (!fromRng) {
    Log("invalid `fromRng` Random");
    toRng = NULL;
  } else {
    toRng->seed = fromRng->seed;
  }
  return (toRng);
}

/*!
 * Frees a RNG state.
 * \addtogroup random
 * \param rng the RNG state to free
 * \sa RandomAlloc()
 */
void RandomFree(Random *rng) {
  MemoryFree(rng);
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

#define M (uint32_t) 2147483647
#define Q (uint32_t) 127773
#define A (uint32_t) 16807
#define R (uint32_t) 2836

/*!
 * Generates a pseudo-random value.
 * \addtogroup random
 * \param rng the RNG state
 * \return the pseudo-random value or zero
 */
uint32_t RandomNext(Random *rng) {
  register uint32_t retval = 0;
  if (!rng) {
    Log("invalid `rng` Random");
  } else {
    /*
     * F(z)  = (az)%m
     *       = az-m(az/m)
     *
     * F(z)  = G(z)+mT(z)
     * G(z)  = a(z%q)- r(z/q)
     * T(z)  = (z/q) - (az/m)
     *
     * F(z)  = a(z%q)- rz/q+ m((z/q) - a(z/m))
     *       = a(z%q)- rz/q+ m(z/q) - az
     */

    /* Calculate the "low" and "high" values. */
    const int32_t low  = rng->seed % Q;
    const int32_t high = rng->seed / Q;

    /* Calculate the "test" value. */
    const int32_t test = A * low - R * high;

    /* Calculate the new seed value. */
    rng->seed = retval = test > 0 ? (uint32_t) test : test + M;
  }
  return (retval);
}

/*!
 * Generates a pseudo-random value.
 * \addtogroup random
 * \param rng the RNG state
 * \return the pseudo-random value or zero
 */
float RandomNextFloat(Random *rng) {
  register float retval = 0.0f;
  if (!rng) {
    Log("invalid `rng` Random");
  } else {
    retval = RandomNext(rng) / (float) M;
  }
  return (retval);
}

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
	const int32_t maxValue) {
  register int32_t retval = 0;
  if (!rng) {
    Log("invalid `rng` Random");
  } else {
    /* Calculate the range of the random number */
    const int32_t range = maxValue - minValue + 1;

    /* Generates the next random value within the range */
    retval = RandomNext(rng) % range + minValue;
  }
  return (retval);
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
    Log("invalid `rng` Random");
  } else {
    /* Assign the new random number seed */
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
    Log("invalid `rng` Random");
  } else {
    Time tvSeed;
    if (timestamp) {
      TimeCopy(&tvSeed, timestamp);
    } else {
      /* Use the current time of day */
      TimeCurrent(&tvSeed);
    }
    rng->seed = tvSeed.tv_sec ^ tvSeed.tv_usec;
  }
}
