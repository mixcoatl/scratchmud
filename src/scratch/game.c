/*!
 * \file game.c
 *
 * \par Copyright
 * Copyright (C) 1999-2019 scratchmud.org
 * All rights reserved.
 *
 * \author Geoffrey Davis <gdavis@scratchmud.org>
 * \addtogroup game
 */
#define _SCRATCH_GAME_C_

#include <scratch/game.h>
#include <scratch/gender.h>
#include <scratch/log.h>
#include <scratch/memory.h>
#include <scratch/rbtree.h>
#include <scratch/scratch.h>
#include <scratch/server.h>
#include <scratch/state.h>
#include <scratch/time.h>
#include <scratch/utility.h>

/*!
 * Constructs a game state.
 * \addtogroup game
 * \return the new game state or NULL
 * \sa GameFree(Game*)
 */
Game *GameAlloc(void) {
  Game *game;
  MemoryCreate(game, Game, 1);
  game->genders = RBTreeAlloc(
	(RBTreeCompareProc) UtilityNameCmp,
	(RBTreeFreeProc)    GenderFree);
  game->server = ServerAlloc(game);
  game->shutdown = false;
  game->states = RBTreeAlloc(
	(RBTreeCompareProc) UtilityNameCmp,
	(RBTreeFreeProc)    StateFree);

  return (game);
}

/*!
 * Frees a game state.
 * \addtogroup game
 * \param game the game state to free
 * \sa GameAlloc()
 */
void GameFree(Game *game) {
  if (game) {
    RBTreeFree(game->genders);
    ServerFree(game->server);
    RBTreeFree(game->states);
    MemoryFree(game);
  }
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
    Log("invalid `game` Game");
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
    Log("invalid `game` Game");
  } else {
    /* Load client states */
    StateLoadIndex(game);

    /* Load genders */
    GenderLoadIndex(game);

    /* Open the server port */
    if (!ServerOpen(game->server, "", 6767))
      return;

    Log("Starting game loop");

    /* Run the game loop */
    while (!game->shutdown) {
      Time timeout;
      TimeSet(&timeout, 60, 0);

      /* Poll for network activity */
      ServerPoll(game->server, &timeout);
    }
    Log("Game loop finished");
  }
}
