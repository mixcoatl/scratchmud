/*!
 * \file main.c
 *
 * \par Copyright
 * Copyright (C) 1999-2023 scratchmud.org
 * All rights reserved.
 *
 * \author Geoffrey Davis <gdavis@scratchmud.org>
 * \addtogroup main
 */
#include <scratch/game.h>
#include <scratch/log.h>
#include <scratch/scratch.h>

/* Local functions. */
int main(int argc, const char *argv[]);

/*!
 * Program entry point.
 * \addtogroup main
 * \param argc the number of command line arguments
 * \param argv an array containing the command line arguments
 * \return zero for normal program termination, non-zero otherwise
 */
int main(int argc, const char *argv[]) {
  /* Run game */
  Log(L_MAIN, "Starting game.");
  Game *game = GameAlloc();
  if (GameParseArguments(game, argv, argc)) {
    GameRun(game);
  }
  GameFree(game);

  /* Exit */
  Log(L_MAIN, "Exiting.");
  return (EXIT_SUCCESS);
}
