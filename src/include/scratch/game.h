/*!
 * \file game.h
 *
 * \par Copyright
 * Copyright (C) 1999-2023 scratchmud.org
 * All rights reserved.
 *
 * \author Geoffrey Davis <gdavis@scratchmud.org>
 * \addtogroup game
 */
#ifndef _SCRATCH_GAME_H_
#define _SCRATCH_GAME_H_

#include <scratch/scratch.h>

/* Forward type declarations */
typedef struct Game Game;

/*!
 * The game state.
 * \addtogroup game
 * \{
 */
struct Game {
  bool                  shutdown;       /*!< The shutdown flag */
};
/*! \} */

/*!
 * Constructs a new game state.
 * \addtogroup game
 * \return the new game state or NULL
 * \sa GameFree(Game*)
 */
Game *GameAlloc(void);

/*!
 * Frees a game state.
 * \addtogroup game
 * \param game the game state to free
 * \sa GameAlloc()
 */
void GameFree(Game *game);

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
	const int argc);

/*!
 * Runs the game until it finishes.
 * \addtogroup game
 * \param game the game state
 */
void GameRun(Game *game);

#endif /* _SCRATCH_GAME_H_ */
