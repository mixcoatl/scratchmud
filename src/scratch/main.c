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
#include <scratch/random.h>
#include <scratch/scratch.h>
#include <scratch/socket.h>

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
  /* Configure RNG state */
  Log(L_MAIN, "Seeding shared RNG state.");
  RandomReseedTime(&g_random, NULL);

  /* OS socket library */
  Log(L_MAIN, "Starting OS socket library.");
  SocketStartup();

  /* Run game */
  Log(L_MAIN, "Starting game.");
  Game *game = GameAlloc();
  if (GameParseArguments(game, argv, argc)) {
    GameRun(game);
  }
  GameFree(game);

  /* OS socket library again */
  Log(L_MAIN, "Terminating OS socket library.");
  SocketCleanup();

  /* Exit */
  Log(L_MAIN, "Exiting.");
  return (EXIT_SUCCESS);
}
