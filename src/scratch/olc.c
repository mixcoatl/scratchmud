/*!
 * \file olc.c
 *
 * \par Copyright
 * Copyright (C) 1999-2019 scratchmud.org
 * All rights reserved.
 *
 * \author Geoffrey Davis <gdavis@scratchmud.org>
 * \addtogroup olc
 */
#define _SCRATCH_OLC_C_

#include <scratch/client.h>
#include <scratch/log.h>
#include <scratch/memory.h>
#include <scratch/olc.h>
#include <scratch/scratch.h>
#include <scratch/string.h>

/*!
 * Constructs an OLC state.
 * \addtogroup olc
 * \return the new OLC state or NULL
 * \sa OlcFree(OlcData*)
 */
OlcData *OlcAlloc(void) {
  OlcData *olc;
  MemoryCreate(olc, OlcData, 1);
  olc->changed = false;
  olc->lastState = NULL;
  olc->lastStateName = NULL;
  olc->name = NULL;
  return (olc);
}

/*!
 * Performs OLC state cleanup.
 * \addtogroup olc
 * \param client the client whose OLC state to clean up
 */
void OlcCleanup(Client *client) {
  if (!client) {
    Log("invalid `client` Client");
  } else if (!client->olc)  {
    Log("client %s is missing OLC state", client->name);
  } else {
    if (client->olc->lastState) {
      ClientStateChange(client, client->olc->lastState);
    } else if (*StringBlank(client->olc->lastStateName) != '\0') {
      ClientStateChangeByName(client, client->olc->lastStateName);
    }
    OlcFree(client->olc);
    client->olc = NULL;
  }
}

/*!
 * Frees an OLC state.
 * \addtogroup olc
 * \param olc the OLC state to free
 * \sa OlcAlloc()
 */
void OlcFree(OlcData *olc) {
  if (olc) {
    StringFree(olc->name);
    StringFree(olc->lastStateName);
    MemoryFree(olc);
  }
}
