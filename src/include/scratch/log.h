/*!
 * \file log.h
 *
 * \par Copyright
 * Copyright (C) 1999-2023 scratchmud.org
 * All rights reserved.
 *
 * \author Geoffrey Davis <gdavis@scratchmud.org>
 * \addtogroup log
 */
#ifndef _SCRATCH_LOG_H_
#define _SCRATCH_LOG_H_

#include <scratch/scratch.h>

/*!
 * The built-in log types.
 * \addtogroup log
 * \{
 */
#define L_ASSERT	"Assert"	/*!< Code assertion. */
#define L_DATA		"Data"		/*!< Data-related messages. */
#define L_MAIN		"Main"		/*!< Program entry point. */
#define L_NETWORK	"Network"	/*!< Network server messages */
#define L_SYSTEM	"System"	/*!< System errors, status, etc. */
/*! \} */

/*!
 * Emits a log message.
 * \addtogroup log
 * \param type the log message type
 */
#define Log(type, ...) \
  RealLog(__FILE__, __LINE__, type, __VA_ARGS__)

/*!
 * Emits a log message.
 * \addtogroup log
 * \param fileName the filename to write to the log file
 * \param fileLine the line number to write to the log file
 * \param type the log message type
 * \param format the prinft-style format specifier
 */
void RealLog(
	const char *fileName,
	const int fileLine,
	const char *type,
	const char *format, ...);

#endif /* _SCRATCH_LOG_H_ */
