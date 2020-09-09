/*!
 * \file main.c
 *
 * \par Copyright
 * Copyright (C) 1999-2019 scratchmud.org
 * All rights reserved.
 *
 * \author Geoffrey Davis <gdavis@scratchmud.org>
 * \addtogroup main
 */
#include <main.h>
#include <scratch/game.h>
#include <scratch/log.h>
#include <scratch/random.h>
#include <scratch/scratch.h>
#include <scratch/socket.h>

/*!
 * Program entry point.
 * \addtogroup main
 * \param argc the number of command line arguments
 * \param argv an array containing the command line arguments
 * \return zero for normal program termination, non-zero otherwise
 */
int main(int argc, const char *argv[]) {
  Log("Package " PACKAGE_STRING "");

  /* RNG state */
  Log("Seeding shared RNG state");
  RandomReseedTime(&_G_random, NULL);

  /* OS socket library */
  Log("Starting OS socket library");
  SocketStartup();

  /* Run the game */
  Game *game = GameAlloc();
  if (GameParseArguments(game, argv, argc))
    GameRun(game);
  GameFree(game);

  /* OS socket library again */
  Log("Terminating OS socket library");
  SocketCleanup();

  return (EXIT_SUCCESS);
}
