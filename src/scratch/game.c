/*!
 * \file game.c
 *
 * \par Copyright
 * Copyright (C) 1999-2023 scratchmud.org
 * All rights reserved.
 *
 * \author Geoffrey Davis <gdavis@scratchmud.org>
 * \addtogroup game
 */
#define _SCRATCH_GAME_C_

#include <scratch/game.h>
#include <scratch/log.h>
#include <scratch/memory.h>
#include <scratch/scratch.h>

/*!
 * Constructs a new game state.
 * \addtogroup game
 * \return the new game state or NULL
 * \sa GameFree(Game*)
 */
Game *GameAlloc(void) {
  Game *game;
  MemoryCreate(game, Game, 1);
  game->shutdown = false;
  return (game);
}

/*!
 * Frees a game state.
 * \addtogroup game
 * \param game the game state to free
 * \sa GameAlloc()
 */
void GameFree(Game *game) {
  MemoryFree(game);
}

/*!
 * Parses command line arguments.
 * \addtogroup game
 * \param game the game state
 * \param argv an array of command line arguments
 * \param argc the number of command line arguments
 * \return true if the command line arguments specified by \p argv
 *     and \p argc were successfully parsed
 */
bool GameParseArguments(
	Game *game,
	const char **argv,
	const int argc) {
  register bool result = false;
  if (!game) {
    Log(L_ASSERT, "Invalid `game` Game.");
  } else {
    /* Nothing yet */
    result = true;
  }
  return (result);
}

/*!
 * Runs the game until it finishes.
 * \addtogroup game
 * \param game the game state
 */
void GameRun(Game *game) {
  if (!game) {
    Log(L_ASSERT, "Invalid `game` Game.");
  } else {
    /* Nothing yet */
  }
}
