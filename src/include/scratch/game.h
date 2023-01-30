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
typedef struct Socket Socket;
typedef struct Tree Tree;

/*!
 * The game state.
 * \addtogroup game
 * \{
 */
struct Game {
  Tree                 *descriptors;    /*!< The descriptor index */
  bool                  shutdown;       /*!< The shutdown flag */
  Socket               *socket;         /*!< The control socket */
  Tree                 *states;         /*!< The state index */
};
/*! \} */

/*!
 * Accepts a descriptor.
 * \addtogroup game
 * \param game the game state
 */
void GameAccept(Game *game);

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
 * Opens the server socket.
 * \addtogroup game
 * \param game the game state
 * \param address the local endpoint address
 * \param port the local endpoint port
 */
void GameOpen(
	Game *game,
	const char *address,
	const uint16_t port);

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
 * Polls for network events.
 * \addtogroup game
 * \param game the game state
 * \param timeout the interval for which this function should
 *     block waiting for the control socket or one of its descriptors
 *     to become ready
 */
void GamePoll(
	Game *game,
	Time *timeout);

/*!
 * Runs the game until it finishes.
 * \addtogroup game
 * \param game the game state
 */
void GameRun(Game *game);

#endif /* _SCRATCH_GAME_H_ */
