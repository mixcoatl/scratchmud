/*!
 * \file log.c
 *
 * \par Copyright
 * Copyright (C) 1999-2019 scratchmud.org
 * All rights reserved.
 *
 * \author Geoffrey Davis <gdavis@scratchmud.org>
 * \addtogroup log
 */
#define _SCRATCH_LOG_C_

#include <scratch/log.h>
#include <scratch/scratch.h>

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
	const char *format, ...) {
  if (format && *format != '\0') {
    /* Current time */
    const time_t now = time(0);
    struct tm *nowtm = localtime(&now);

    /* Construct the log filename */
    char logname[PATH_MAX] = {'\0'};
    strftime(logname, sizeof(logname), "log/%m%d.log", nowtm);

    /* Open the log file */
    FILE *stream = fopen(logname, "a+t");
    if (!stream)
      stream = stderr;

    /* Write the timestamp to the log file. */
    strftime(logname, sizeof(logname), "%F %H:%M:%S", nowtm);
    fprintf(stream, "%s :: ", logname);

    /* Print the message out to the log file */
    va_list args;
    va_start(args, format);
    vfprintf(stream, format, args);
    va_end(args);

    /* Write filename and line number */
    fprintf(stream, " {%s:%d}", fname, lineNumber);

    /* Write an EOL character to the log file */
    fprintf(stream, "\n");
    fflush(stream);

    /* Close the log file */
    if (stream != stderr)
      fclose(stream);
  }
}
