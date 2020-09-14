/*!
 * \file olc_player.h
 *
 * \par Copyright
 * Copyright (C) 1999-2019 scratchmud.org
 * All rights reserved.
 *
 * \author Geoffrey Davis <gdavis@scratchmud.org>
 * \addtogroup build
 */
#ifndef _SCRATCH_OLC_PLAYER_H_
#define _SCRATCH_OLC_PLAYER_H_

#include <scratch/scratch.h>

/* Forward type declarations */
typedef struct _Client Client;
typedef struct _Object Object;

/*!
 * Starts the player editor.
 * \addtogroup build
 * \addtogroup player
 * \param client the client that is editing the player
 * \param player the player object to edit, or NULL
 */
void OlcPlayerStart(
	Client *client,
	const Object *player);

#endif /* _SCRATCH_OLC_PLAYER_H_ */
