/*!
 * \file time.h
 *
 * \par Copyright
 * Copyright (C) 1999-2019 scratchmud.org
 * All rights reserved.
 *
 * \author Geoffrey Davis <gdavis@scratchmud.org>
 * \addtogroup time
 */
#ifndef _SCRATCH_TIME_H_
#define _SCRATCH_TIME_H_

#include <scratch/scratch.h>

/*!
 * An alias for struct timeval.
 * \addtogroup time
 */
typedef struct timeval Time;

/*!
 * Adds two times together.
 * \addtogroup time
 * \param sum the time to contain the result
 * \param augend the first time (augend) to add
 * \param addend the second time (addend) to add
 * \return the time indicated by the specified `sum` or NULL;
 * \sa TimeSubtract(Time*, const Time*, const Time*)
 */
Time *TimeAdd(
	Time *sum,
	const Time *augend,
	const Time *addend);

/*!
 * Constructs a new time.
 * \addtogroup time
 * \return a new time or NULL
 * \sa TimeFree(Time*)
 */
Time *TimeAlloc(void);

/*!
 * Compares two times for order.
 * \addtogroup time
 * \param xTime the first time to compare
 * \param yTime the second time to compare
 * \return < 0 if the first time is less than the second; or
 *         > 0 if the first time is greater than the second; or
 *           0 if the specified times are equal
 * \sa TimeEquals(const Time*, const Time*)
 */
int TimeCmp(
	const Time *xTime,
	const Time *yTime);

/*!
 * Copies a time.
 * \addtogroup time
 * \param toTime the time into which to copy 
 * \param fromTime the time to copy
 * \return the time indicated by the specified `toTime` or NULL
 */
Time *TimeCopy(
	Time *toTime,
	const Time *fromTime);

/*!
 * Retrieves the current time.
 * \addtogroup time
 * \param toTime the time into which to write the current time
 * \return the time indicated by the specified `toTime` or NULL
 */
Time *TimeCurrent(Time *toTime);

/*!
 * Compares two times for equality.
 * \addtogroup time
 * \param xTime the first time to compare
 * \param yTime the second time to compare
 * \return true if the specified times are equal
 * \sa TimeCmp(const Time*, const Time*)
 */
bool TimeEquals(
	const Time *xTime,
	const Time *yTime);

/*!
 * Frees a time.
 * \addtogroup time
 * \param time the time to free
 */
void TimeFree(Time *time);

/*!
 * Assigns a value to a time.
 * \addtogroup time
 * \param toTime the time to which to assign a value
 * \param seconds the seconds component of the time value
 * \param microseconds the microsecond component of the time value
 * \return the time indicated by the specified `toTime` or NULL
 */
Time *TimeSet(
	Time *toTime,
	const time_t seconds,
	const suseconds_t microseconds);

/*!
 * Subtracts two times.
 * \addtogroup time
 * \param difference the time to contain to result
 * \param minuend the first time (minuend) to subtract
 * \param subtrahend the second time (subtrahend) to subtract
 * \return the time indicated by the specified `difference` or NULL
 * \sa TimeAdd(Time*, const Time*, const Time*)
 */
Time *TimeSubtract(
	Time *difference,
	const Time *minuend,
	const Time *subtrahend);

/*!
 * Returns whether a time is zeroed out.
 * \addtogroup time
 * \param time the time to be tested
 * \return true if the seconds and microseconds components of
 *     the specified time have zero values
 */
bool TimeZero(const Time *time);

#endif /* _SCRATCH_TIME_H_ */
