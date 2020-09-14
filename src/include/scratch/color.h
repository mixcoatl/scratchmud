/*!
 * \file color.h
 *
 * \par Copyright
 * Copyright (C) 1999-2019 scratchmud.org
 * All rights reserved.
 *
 * \author Geoffrey Davis <gdavis@scratchmud.org>
 * \addtogroup color
 */
#ifndef _SCRATCH_COLOR_H_
#define _SCRATCH_COLOR_H_

#include <scratch/scratch.h>

/*!
 * Colors.
 * \addtogroup color
 * {
 */
#define C_UNDEFINED     (-1)    /*!< No color information */
#define C_BLACK         (0)     /*!< Color is black */
#define C_RED           (1)     /*!< Color is red */
#define C_GREEN         (2)     /*!< Color is green */
#define C_YELLOW        (3)     /*!< Color is yellow */
#define C_BLUE          (4)     /*!< Color is blue */
#define C_MAGENTA       (5)     /*!< Color is magenta */
#define C_CYAN          (6)     /*!< Color is cyan */
#define C_WHITE         (7)     /*!< Color is white */
#define C_NORMAL        (8)     /*!< Color is normal */
/*! \} */

/*! How many C_x types? */
#define MAX_C_TYPES \
  (C_NORMAL - C_BLACK + 1)

/*!
 * Shortcut client macros.
 * \addtogroup color
 * \{
 */
#define Q_BLACK         ColorGet(client, C_BLACK)
#define Q_RED           ColorGet(client, C_RED)
#define Q_GREEN         ColorGet(client, C_GREEN)
#define Q_YELLOW        ColorGet(client, C_YELLOW)
#define Q_BLUE          ColorGet(client, C_BLUE)
#define Q_MAGENTA       ColorGet(client, C_MAGENTA)
#define Q_CYAN          ColorGet(client, C_CYAN)
#define Q_WHITE         ColorGet(client, C_WHITE)
#define Q_NORMAL        ColorGet(client, C_NORMAL)
/*! \} */

/*!
 * Shortcut actor macros.
 * \addtogroup color
 * \{
 */
#define QQ_BLACK	ColorGet(actor->client, C_BLACK)
#define QQ_RED		ColorGet(actor->client, C_RED)
#define QQ_GREEN	ColorGet(actor->client, C_GREEN)
#define QQ_YELLOW	ColorGet(actor->client, C_YELLOW)
#define QQ_BLUE		ColorGet(actor->client, C_BLUE)
#define QQ_MAGENTA	ColorGet(actor->client, C_MAGENTA)
#define QQ_CYAN		ColorGet(actor->client, C_CYAN)
#define QQ_WHITE	ColorGet(actor->client, C_WHITE)
#define QQ_NORMAL	ColorGet(actor->client, C_NORMAL)
/*! \} */

/* Forward type declarations */
typedef struct _Client Client;

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
	const int color);

/*!
 * Removes ANSI codes from a string.
 * \addtogroup color
 * \param str the string to strip
 */
void ColorStrip(char *str);

/*!
 * Returns the length of a string without ANSI codes.
 * \addtogroup color
 * \param str the string whose length to return
 * \return the length of the specified string without ANSI codes
 */
size_t ColorStrlen(const char *str);

#endif /* _SCRATCH_COLOR_H_ */
