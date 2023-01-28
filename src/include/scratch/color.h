/*!
 * \file color.h
 *
 * \par Copyright
 * Copyright (C) 1999-2023 scratchmud.org
 * All rights reserved.
 *
 * \author Geoffrey Davis <gdavis@scratchmud.org>
 * \addtogroup color
 */
#ifndef _SCRATCH_COLOR_H_
#define _SCRATCH_COLOR_H_

#include <scratch/scratch.h>

/* Forward type declarations */
typedef struct Descriptor Descriptor;

/*!
 * The colors.
 * \addtogroup color
 * {
 */
#define C_UNDEFINED	(-1)	/*!< No color information */
#define C_AQUA		(0)	/*!< Color is aqua */
#define C_BLACK		(1)	/*!< Color is black */
#define C_BLUE		(2)	/*!< Color is blue */
#define C_CYAN		(3)	/*!< Color is cyan */
#define C_GOLD		(4)	/*!< Color is gold */
#define C_GRAY		(5)	/*!< Color is gray */
#define C_GREEN		(6)	/*!< Color is green */
#define C_LIME		(7)	/*!< Color is lime */
#define C_MAGENTA	(8)	/*!< Color is magenta */
#define C_NAVY		(9)	/*!< Color is navy */
#define C_NORMAL	(10)	/*!< Color is normal */
#define C_PINK		(11)	/*!< Color is pink */
#define C_PURPLE	(12)	/*!< Color is purple */
#define C_RED		(13)	/*!< Color is red */
#define C_SILVER	(14)	/*!< Color is silver */
#define C_YELLOW	(15)	/*!< Color is yellow */
#define C_WHITE		(16)	/*!< Color is white */
/*! \} */

/*! How many C_x types? */
#define MAX_C_TYPES	(C_WHITE - C_AQUA + 1)

/*!
 * The message colors.
 * \addtogroup color
 * \{
 */
#define CX_EMPHASIS		(C_LIME)
#define CX_FAILED		(C_RED)
#define CX_KEY			(C_RED)
#define CX_NUMBER		(C_CYAN)
#define CX_OKAY			(C_CYAN)
#define CX_ORDINAL		(C_AQUA)
#define CX_PERCENT		(C_RED)
#define CX_PROMPT		(C_GREEN)
#define CX_PUNCTUATION		(C_GRAY)
#define CX_TEXT			(C_YELLOW)
#define CX_TYPECODE		(C_CYAN)
#define CX_YESNO		(C_CYAN)
/*! \} */

/*!
 * Shortcut descriptor macros.
 * \addtogroup color
 * \{
 */
#define Q_AQUA			ColorGet(d, C_AQUA)
#define Q_BLACK			ColorGet(d, C_BLACK)
#define Q_BLUE			ColorGet(d, C_BLUE)
#define Q_CYAN			ColorGet(d, C_CYAN)
#define Q_GOLD			ColorGet(d, C_GOLD)
#define Q_GRAY			ColorGet(d, C_GRAY)
#define Q_GREEN			ColorGet(d, C_GREEN)
#define Q_LIME			ColorGet(d, C_LIME)
#define Q_MAGENTA		ColorGet(d, C_MAGENTA)
#define Q_NAVY			ColorGet(d, C_NAVY)
#define Q_NORMAL		ColorGet(d, C_NORMAL)
#define Q_PINK			ColorGet(d, C_PINK)
#define Q_PURPLE		ColorGet(d, C_PURPLE)
#define Q_RED			ColorGet(d, C_RED)
#define Q_SILVER		ColorGet(d, C_SILVER)
#define Q_YELLOW		ColorGet(d, C_YELLOW)
#define Q_WHITE			ColorGet(d, C_WHITE)
/*! \} */

/*!
 * Shortcut message color descriptor macros.
 * \addtogroup color
 * \{
 */
#define QX_EMPHASIS		ColorGet(d, CX_EMPHASIS)
#define QX_FAILED		ColorGet(d, CX_FAILED)
#define QX_KEY			ColorGet(d, CX_KEY)
#define QX_NUMBER		ColorGet(d, CX_NUMBER)
#define QX_OKAY			ColorGet(d, CX_OKAY)
#define QX_ORDINAL		ColorGet(d, CX_ORDINAL)
#define QX_PERCENT		ColorGet(d, CX_PERCENT)
#define QX_PROMPT		ColorGet(d, CX_PROMPT)
#define QX_PUNCTUATION		ColorGet(d, CX_PUNCTUATION)
#define QX_TEXT			ColorGet(d, CX_TEXT)
#define QX_TYPECODE		ColorGet(d, CX_TYPECODE)
#define QX_YESNO		ColorGet(d, CX_YESNO)
/*! \} */

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
