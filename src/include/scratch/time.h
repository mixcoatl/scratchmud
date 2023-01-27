/*!
 * \file time.h
 *
 * \par Copyright
 * Copyright (C) 1999-2023 scratchmud.org
 * All rights reserved.
 *
 * \author Geoffrey Davis <gdavis@scratchmud.org>
 * \addtogroup time
 */
#ifndef _SCRATCH_TIME_H_
#define _SCRATCH_TIME_H_

#include <scratch/scratch.h>

/* Forward type declarations */
typedef struct timeval Time;

/*!
 * Adds times.
 * \addtogroup time
 * \param toTime the result
 * \param left the first time to add
 * \param right the second time to add
 * \sa TimeSubtract(Time*, const Time*, const Time*)
 */
void TimeAdd(
	Time *toTime,
	const Time *left,
	const Time *right);

/*!
 * Constructs a new time.
 * \addtogroup time
 * \return the new time or NULL
 * \sa TimeFree(Time*)
 * \sa TimeFreeV(void*)
 */
Time *TimeAlloc(void);

/*!
 * Compares times for order.
 * \addtogroup time
 * \param left the first time to compare
 * \param right the second time to compare
 * \return < 0 if the first time is less than the second; or
 *         > 0 if the first time is greater than the second; or
 *           0 if the specified times are equal
 */
int TimeCompare(
	const Time *left,
	const Time *right);

/*!
 * Compares times for order.
 * \addtogroup time
 * \param left the first time to compare
 * \param right the second time to compare
 * \return < 0 if the first time is less than the second; or
 *         > 0 if the first time is greater than the second; or
 *           0 if the specified times are equal
 */
int TimeCompareV(
	const void *left,
	const void *right);

/*!
 * Copies a time.
 * \addtogroup time
 * \param toTime the location of the copied time
 * \param fromTime the time to copy
 */
void TimeCopy(
	Time *toTime,
	const Time *fromTime);

/*!
 * Retrieves the current time.
 * \addtogroup time
 * \param toTime the time destination
 */
void TimeCurrent(Time *toTime);

/*!
 * Frees a time.
 * \addtogroup time
 * \param time the time to free
 * \sa TimeAlloc()
 * \sa TimeFreeV(void*)
 */
void TimeFree(Time *time);

/*!
 * Frees a time.
 * \addtogroup time
 * \param time the time to free
 * \sa TimeAlloc()
 * \sa TimeFree(Time*)
 */
void TimeFreeV(void *time);

/*!
 * Sets a time.
 * \addtogroup time
 * \param toTime the time destination
 * \param seconds the seconds component of the time
 * \param microseconds the microsecond component of the time
 */
void TimeSet(
	Time *toTime,
	const time_t seconds,
	const suseconds_t microseconds);

/*!
 * Subtracts times.
 * \addtogroup time
 * \param toTime the result
 * \param left the first time to subtract
 * \param right the second time to subtract
 * \sa TimeAdd(Time*, const Time*, const Time*)
 */
void TimeSubtract(
	Time *toTime,
	const Time *left,
	const Time *right);

#endif /* _SCRATCH_TIME_H_ */
