/*!
 * \file constant.c
 *
 * \par Copyright
 * Copyright (C) 1999-2019 scratchmud.org
 * All rights reserved.
 *
 * \author Geoffrey Davis <gdavis@scratchmud.org>
 * \addtogroup constant
 */
#define _SCRATCH_CONSTANT_C_

#include <scratch/constant.h>
#include <scratch/scratch.h>

/*!
 * Client bit strings.
 * \addtogroup client
 * \addtogroup constant
 */
const char *_G_clientBits[] = {
  /* 00 */ "Color",
  /* 01 */ "Prompt",
           "\n"
};

/*!
 * Color strings.
 * \addtogroup color
 * \addtogroup constant
 */
const char *_G_colorTypes[] = {
  /* 00 */ "Black",
  /* 01 */ "Red",
  /* 02 */ "Green",
  /* 03 */ "Yellow",
  /* 04 */ "Blue",
  /* 05 */ "Magenta",
  /* 06 */ "Cyan",
  /* 07 */ "White",
  /* 08 */ "Normal",
           "\n"
};

/*!
 * The PREF_x strings.
 * \addtogroup constant
 * \addtogroup player
 */
const char *_G_preferenceBits[] = {
  /* 00 */ "Compact",
           "\n"
};

/*!
 * The STATE_x strings.
 * \addtogroup constant
 * \addtogroup state
 */
const char *_G_stateBits[] = {
  /* 00 */ "Initial",
  /* 01 */ "Prompt",
  /* 02 */ "Quiet",
           "\n"
};

/*!
 * The TRUST_x strings.
 * \addtogroup constant
 * \addtogroup player
 */
const char *_G_trustTypes[] = {
  /* 00 */ "None",
  /* 01 */ "Guest",
  /* 02 */ "Player",
  /* 03 */ "Wizard",
  /* 04 */ "Owner",
           "\n"
};
