/*!
 * \file time.c
 *
 * \par Copyright
 * Copyright (C) 1999-2023 scratchmud.org
 * All rights reserved.
 *
 * \author Geoffrey Davis <gdavis@scratchmud.org>
 * \addtogroup time
 */
#define _SCRATCH_TIME_C_

#include <scratch/log.h>
#include <scratch/memory.h>
#include <scratch/scratch.h>
#include <scratch/time.h>

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
	const Time *right) {
  if (!toTime) {
    Log(L_ASSERT, "Invalid `toTime` Time.");
  } else if (!left) {
    Log(L_ASSERT, "Invalid `left` Time.");
  } else if (!right) {
    Log(L_ASSERT, "Invalid `right` Time.");
  } else {
    TimeCopy(toTime, left);
    toTime->tv_sec  += right->tv_sec;
    toTime->tv_usec += right->tv_usec;

    while (toTime->tv_usec >= 1000000) {
      toTime->tv_usec -= 1000000;
      toTime->tv_sec++;
    }
  }
}

/*!
 * Constructs a new time.
 * \addtogroup time
 * \return the new time or NULL
 * \sa TimeFree(Time*)
 * \sa TimeFreeV(void*)
 */
Time *TimeAlloc(void) {
  Time *time;
  MemoryCreate(time, Time, 1);
  time->tv_sec = 0;
  time->tv_usec = 0;
  return (time);
}

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
	const Time *right) {
  if (left && right) {
    if (left->tv_sec != right->tv_sec)
      return (left->tv_sec < right->tv_sec ? -1 : +1);
    if (left->tv_usec != right->tv_usec)
      return (left->tv_usec < right->tv_usec ? -1 : +1);
  } else {
    return (left ? +1 : -1);
  }
  return (0);
}

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
	const void *right) {
  return TimeCompare(left, right);
}

/*!
 * Copies a time.
 * \addtogroup time
 * \param toTime the time destination
 * \param fromTime the time to copy
 */
void TimeCopy(
	Time *toTime,
	const Time *fromTime) {
  if (!toTime) {
    Log(L_ASSERT, "Invalid `toTime` Time.");
  } else if (!fromTime) {
    Log(L_ASSERT, "Invalid `fromTime` Time.");
  } else if (toTime != fromTime) {
    MemoryZero(toTime, Time, 1);
    toTime->tv_sec = fromTime->tv_sec;
    toTime->tv_usec = fromTime->tv_usec;
  }
}

/*!
 * Retrieves the current time.
 * \addtogroup time
 * \param toTime the time destination
 */
void TimeCurrent(Time *toTime) {
  if (!toTime) {
    Log(L_ASSERT, "Invalid `toTime` Time.");
  } else if (gettimeofday(toTime, 0) < 0) {
    Log(L_SYSTEM, "gettimeofday() failed: errno=%d.", errno);
    abort(); /* Required operation */
  }
}

/*!
 * Frees a time.
 * \addtogroup time
 * \param time the time to free
 * \sa TimeAlloc()
 * \sa TimeFreeV(void*)
 */
void TimeFree(Time *time) {
  MemoryFree(time);
}

/*!
 * Frees a time.
 * \addtogroup time
 * \param time the time to free
 * \sa TimeAlloc()
 * \sa TimeFree(Time*)
 */
void TimeFreeV(void *time) {
  TimeFree(time);
}

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
	const suseconds_t microseconds) {
  if (!toTime) {
    Log(L_ASSERT, "Invalid `toTime` Time.");
  } else {
    MemoryZero(toTime, Time, 1);
    toTime->tv_sec  = seconds;
    toTime->tv_usec = microseconds;

    while (toTime->tv_usec >= 1000000) {
      toTime->tv_usec -= 1000000;
      toTime->tv_sec++;
    }
  }
}

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
	const Time *right) {
  if (!toTime) {
    Log(L_ASSERT, "Invalid `toTime` Time.");
  } else if (!left) {
    Log(L_ASSERT, "Invalid `left` Time.");
  } else if (!right) {
    Log(L_ASSERT, "Invalid `right` Time.");
  } else if (TimeCompare(left, right) < 0) {
    TimeSet(toTime, 0, 0);
  } else {
    TimeCopy(toTime, left);
    toTime->tv_sec  -= right->tv_sec;
    toTime->tv_usec -= right->tv_usec;

    while (toTime->tv_usec < 0) {
      toTime->tv_usec += 1000000;
      toTime->tv_sec--;
    }
  }
}
