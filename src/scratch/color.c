/*!
 * \file color.c
 *
 * \par Copyright
 * Copyright (C) 1999-2019 scratchmud.org
 * All rights reserved.
 *
 * \author Geoffrey Davis <gdavis@scratchmud.org>
 * \addtogroup color
 */
#define _SCRATCH_COLOR_C_

#include <scratch/bitvector.h>
#include <scratch/client.h>
#include <scratch/color.h>
#include <scratch/log.h>
#include <scratch/scratch.h>

/*!
 * Returns an ANSI escape sequence.
 * \addtogroup color
 * \param client the client for which to return an ANSI escape sequence
 * \param color the C_x color enumeration value
 * \return the ANSI escape sequence for the specified color
 *     or the empty string (""). Never returns NULL
 */
const char *ColorGet(
	const Client *client,
	const int color) {
  if (client && BitCheckN(client->flags, CLIENT_COLOR)) {
    switch (color) {
    case C_BLACK:   return ("\x1b[0;30m");
    case C_RED:     return ("\x1b[0;31m");
    case C_GREEN:   return ("\x1b[0;32m");
    case C_YELLOW:  return ("\x1b[0;33m");
    case C_BLUE:    return ("\x1b[0;34m");
    case C_MAGENTA: return ("\x1b[0;35m");
    case C_CYAN:    return ("\x1b[0;36m");
    case C_WHITE:   return ("\x1b[0;37m");
    case C_NORMAL:  return ("\x1b[0m");
    }
  }
  return ("");
}

/*!
 * Removes ANSI codes from a string.
 * \addtogroup color
 * \param str the string to strip
 */
void ColorStrip(char *str) {
  register char *inptr = str;
  register char *outptr = str;
  while (inptr && *inptr != '\0') {
    if (*inptr == '\x1b') {
      while (*inptr != '\0' && !isalpha(*inptr))
	++inptr;
    } else {
      *outptr++ = *inptr++;
    }
  }
  if (outptr)
    *outptr = '\0';
}

/*!
 * Returns the length of a string without ANSI codes.
 * \addtogroup color
 * \param str the string whose length to return
 * \return the length of the specified string without ANSI codes
 */
size_t ColorStrlen(const char *str) {
  register size_t len = 0;
  while (str && *str != '\0') {
    if (*str == '\x1b') {
      while (*str != '\0' && !isalpha(*str))
	++str;
    } else
      len++;
  }
  return (len);
}
