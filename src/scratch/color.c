/*!
 * \file color.c
 *
 * \par Copyright
 * Copyright (C) 1999-2023 scratchmud.org
 * All rights reserved.
 *
 * \author Geoffrey Davis <gdavis@scratchmud.org>
 * \addtogroup color
 */
#define _SCRATCH_COLOR_C_

#include <scratch/color.h>
#include <scratch/descriptor.h>
#include <scratch/log.h>
#include <scratch/scratch.h>

/*!
 * Returns an ANSI escape sequence.
 * \addtogroup color
 * \param d the descriptor instance
 * \param color the C_x color enumeration value
 * \return the ANSI escape sequence for the specified color
 *     or the empty string ("").  Never returns NULL
 */
const char *ColorGet(
	const Descriptor *d,
	const int color) {
  if (d && d->bits.color) {
    switch (color) {
    case C_AQUA:    return ("\x1b[1;36m");
    case C_BLACK:   return ("\x1b[0;30m");
    case C_BLUE:    return ("\x1b[1;34m");
    case C_CYAN:    return ("\x1b[0;36m");
    case C_GOLD:    return ("\x1b[0;33m");
    case C_GRAY:    return ("\x1b[1;30m");
    case C_GREEN:   return ("\x1b[0;32m");
    case C_LIME:    return ("\x1b[1;32m");
    case C_PINK:    return ("\x1b[1;31m");
    case C_PURPLE:  return ("\x1b[1;35m");
    case C_RED:     return ("\x1b[0;31m");
    case C_SILVER:  return ("\x1b[0;37m");
    case C_YELLOW:  return ("\x1b[1;33m");
    case C_NAVY:    return ("\x1b[0;34m");
    case C_NORMAL:  return ("\x1b[0m");
    case C_MAGENTA: return ("\x1b[1;35m");
    case C_WHITE:   return ("\x1b[1;37m");
    default:        return ("");
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
      while (*inptr != '\0' && !isalpha((int) *inptr))
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
  register size_t length = 0;
  while (str && *str != '\0') {
    if (*str == '\x1b') {
      while (*str != '\0' && !isalpha((int) *str))
	++str;
    } else
      length++;
  }
  return (length);
}
