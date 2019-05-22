/*!
 * \file log.h
 *
 * \par Copyright
 * Copyright (C) 1999-2019 scratchmud.org
 * All rights reserved.
 *
 * \author Geoffrey Davis <gdavis@scratchmud.org>
 * \addtogroup log
 */
#ifndef _SCRATCH_LOG_H_
#define _SCRATCH_LOG_H_

#include <scratch/scratch.h>

/*!
 * Emits a log message.
 * \addtogroup log
 */
#define Log(...) \
  do { \
    LogReal(__FILE__, __LINE__, __VA_ARGS__); \
  } while (0)

/*!
 * Emits a log message.
 * \addtogroup log
 * \param fname the filename to write to the log file
 * \param lineNumber the line number to write to the log file
 * \param format the prinft-style format specifier
 */
void LogReal(
	const char *fname,
	const int lineNumber,
	const char *format, ...);

#endif /* _SCRATCH_LOG_H_ */
