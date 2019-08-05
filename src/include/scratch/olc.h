/*!
 * \file olc.h
 *
 * \par Copyright
 * Copyright (C) 1999-2019 scratchmud.org
 * All rights reserved.
 *
 * \author Geoffrey Davis <gdavis@scratchmud.org>
 * \addtogroup olc
 */
#ifndef _SCRATCH_OLC_H_
#define _SCRATCH_OLC_H_

#include <scratch/scratch.h>

/* Forward type declarations */
typedef struct _OlcData OlcData;
typedef struct _Client  Client;
typedef struct _State   State;

/*!
 * Client OLC state.
 * \addtogroup olc
 * \{
 */
struct _OlcData {
  bool          changed;        /*!< Indicates if the thing was modified */
  State        *lastState;      /*!< The previous state */
  char         *lastStateName;  /*!< The previous state name */
  char         *name;           /*!< The canonical name or NULL */
};
/*! \} */

/*!
 * Constructs an OLC state.
 * \addtogroup olc
 * \return the new OLC state or NULL
 * \sa OlcFree(OlcData*)
 */
OlcData *OlcAlloc(void);

/*!
 * Performs OLC state cleanup.
 * \addtogroup olc
 * \param client the client whose OLC state to clean up
 */
void OlcCleanup(Client *client);

/*!
 * Frees an OLC state.
 * \addtogroup olc
 * \param build the OLC state to free
 * \sa OlcAlloc()
 */
void OlcFree(OlcData *Olc);

#endif /* _SCRATCH_OLC_H_ */
