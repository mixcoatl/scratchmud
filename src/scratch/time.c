/*!
 * \file time.c
 *
 * \par Copyright
 * Copyright (C) 1999-2019 scratchmud.org
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
	const Time *addend) {
  if (!sum) {
    Log("invalid `sum` Time");
  } else if (!augend) {
    Log("invalid `augend` Time");
    sum = NULL;
  } else if (!addend) {
    Log("invalid `addend` Time");
    sum = NULL;
  } else {
    TimeCopy(sum, augend);
    sum->tv_sec += addend->tv_sec;
    sum->tv_usec += addend->tv_usec;
    while (sum->tv_usec >= 1000000) {
      sum->tv_usec -= 1000000;
      sum->tv_sec++;
    }
  }
  return (sum);
}

/*!
 * Constructs a new time.
 * \addtogroup time
 * \return a new time or NULL
 * \sa TimeFree(Time*)
 */
Time *TimeAlloc(void) {
  Time *time = NULL;
  MemoryCreate(time, Time, 1);
  time->tv_sec = 0;
  time->tv_usec = 0;
  return (time);
}

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
	const Time *yTime) {
  if (xTime && yTime) {
    if (xTime->tv_sec != yTime->tv_sec) {
      return (xTime->tv_sec < yTime->tv_sec ? -1 : +1);
    } else if (xTime->tv_usec != yTime->tv_usec) {
      return (xTime->tv_usec < yTime->tv_usec ? -1 : +1);
    }
  } else {
    return (xTime ? +1 : -1);
  }
  return (0);
}

/*!
 * Copies a time.
 * \addtogroup time
 * \param toTime the time into which to copy 
 * \param fromTime the time to copy
 * \return the time indicated by the specified `toTime` or NULL
 */
Time *TimeCopy(
	Time *toTime,
	const Time *fromTime) {
  if (!toTime) {
    Log("invalid `toTime` Time");
  } else if (!fromTime) {
    Log("invalid `fromTime` Time");
    toTime = NULL;
  } else if (toTime != fromTime) {
    toTime->tv_sec  = fromTime->tv_sec;
    toTime->tv_usec = fromTime->tv_usec;
  }
  return (toTime);
}

/*!
 * Retrieves the current time.
 * \addtogroup time
 * \param toTime the time into which to write the current time
 * \return the time indicated by the specified `toTime` or NULL
 */
Time *TimeCurrent(Time *toTime) {
  if (!toTime) {
    Log("invalid `toTime` Time");
  } else if (gettimeofday(toTime, 0) < 0) {
    Log("gettimeofday() failed: errno=%d", errno);
    abort(); /* Required operation */
  }
  return (toTime);
}

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
	const Time *yTime) {
  return TimeCmp(xTime, yTime) == 0;
}

/*!
 * Frees a time.
 * \addtogroup time
 * \param time the time to free
 */
void TimeFree(Time *time) {
  MemoryFree(time);
}

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
	const suseconds_t microseconds) {
  if (!toTime) {
    Log("invalid `toTime` Time");
  } else {
    toTime->tv_sec = seconds;
    toTime->tv_usec = microseconds;
    while (toTime->tv_usec >= 1000000) {
      toTime->tv_usec -= 1000000;
      toTime->tv_sec++;
    }
  }
  return (toTime);
}

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
	const Time *subtrahend) {
  if (!difference) {
    Log("invalid `difference` Time");
  } else if (!minuend) {
    Log("invalid `minuend` Time");
    difference = NULL;
  } else if (!subtrahend) {
    Log("invalid `subtrahend` Time");
    difference = NULL;
  } else if (TimeCmp(minuend, subtrahend) < 0) {
    TimeSet(difference, 0, 0);
  } else {
    TimeCopy(difference, minuend);
    difference->tv_sec -= subtrahend->tv_sec;
    difference->tv_usec -= subtrahend->tv_usec;
    while (difference->tv_usec < 0) {
      difference->tv_usec += 1000000;
      difference->tv_sec--;
    }
  }
  return (difference);
}

/*!
 * Returns whether a time is zeroed out.
 * \addtogroup time
 * \param time the time to be tested
 * \return true if the seconds and microseconds components of
 *     the specified time have zero values
 */
bool TimeZero(const Time *time) {
  return !time || (!time->tv_sec && !time->tv_usec);
}
