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

#include <scratch/descriptor.h>
#include <scratch/game.h>
#include <scratch/list.h>
#include <scratch/log.h>
#include <scratch/memory.h>
#include <scratch/scratch.h>
#include <scratch/socket.h>
#include <scratch/time.h>
#include <scratch/tree.h>
#include <scratch/utility.h>

/*!
 * Accepts a descriptor.
 * \addtogroup game
 * \param game the game state
 */
void GameAccept(Game *game) {
  if (!game) {
    Log(L_ASSERT, "Invalid `game` Game.");
  } else if (SocketClosed(game->socket)) {
    Log(L_ASSERT, "Server is already closed.");
  } else {
    /* Accept connection */
    Descriptor *d = DescriptorAlloc(game);
    d->socket = SocketAccept(game->socket);

    if (DescriptorClosed(d)) {
      Log(L_NETWORK, "Couldn't accept incoming connection.");
      DescriptorFree(d);
    } else {
      /* Peer address */
      SOCKADDR *peer = &d->socket->address;
      socklen_t peerSZ = sizeof(SOCKADDR);

      /* Get socket name */
      char name[INET6_ADDRSTRLEN] = {'\0'};
      if (getnameinfo(peer, peerSZ, name, sizeof(name), 0, 0, 0) < 0) {
	Log(L_SYSTEM, "getnameinfo() failed: errno=%d.", errno);
	strlcpy(name, "*Unknown*", sizeof(name));
      }

      /* Set hostname */
      MemoryFree(d->hostname);
      d->hostname = strdup(name);

      /* Add descriptor to descriptor index */
      if (!TreeInsert(game->descriptors, &d->name, d)) {
	Log(L_NETWORK, "Couldn't add descriptor %s to descriptor index.", d->name);
	DescriptorFree(d), d = NULL;
      }

      if (d) {
	Log(L_NETWORK, "Accepted descriptor %s from %s.", d->name, d->hostname);
	DescriptorPutCommand(d, DO, TELOPT_ECHO);   /* Remote echo */
	DescriptorPutCommand(d, WONT, TELOPT_ECHO); /* Local won't echo */
	DescriptorPutCommand(d, DO, TELOPT_NAWS);   /* Remote NAWS */
      }
    }
  }
}

/*!
 * Constructs a new game state.
 * \addtogroup game
 * \return the new game state or NULL
 * \sa GameFree(Game*)
 */
Game *GameAlloc(void) {
  Game *game;
  MemoryCreate(game, Game, 1);
  game->descriptors = TreeAlloc(UtilityNameCompareV, NULL, DescriptorFreeV);
  game->shutdown = false;
  game->socket = NULL;
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
    TreeFree(game->descriptors);
    SocketClose(game->socket);
    MemoryFree(game);
  }
}

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
	const uint16_t port) {
  if (!game) {
    Log(L_ASSERT, "Invalid `game` Game.");
  } else if (!SocketClosed(game->socket)) {
    Log(L_ASSERT, "Server control socket already open.");
  } else {
    game->socket = SocketAlloc();
    if (!SocketOpen(game->socket, address, port)) {
      Log(L_NETWORK, "Couldn't open server using address '%s', port %hu.", address && *address != '\0' ? address : "<Blank>", port);
      SocketClose(game->socket), game->socket = NULL;
    } else if (address && *address != '\0') {
      Log(L_NETWORK, "Opened server on %s, port %hu.", address, port);
    } else {
      Log(L_NETWORK, "Opened server on port %hu.", port);
    }
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
    Log(L_ASSERT, "Invalid `game` Game.");
  } else {
    /* Nothing yet */
    result = true;
  }
  return (result);
}

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
	Time *timeout) {
  if (!game) {
    Log(L_ASSERT, "Invalid `game` Game.");
  } else {
    /* Reader sockets */
    fd_set readers;
    FD_ZERO(&readers);

    /* Writer sockets */
    fd_set writers;
    FD_ZERO(&writers);

    /* Highest valued handle */
    SOCKET topHandle = INVALID_SOCKET;

    /* Control socket */
    if (!SocketClosed(game->socket)) {
      FD_SET(game->socket->handle, &readers);
      topHandle = game->socket->handle;
    }

    /* Configure read and write sets */
    TreeForEach(game->descriptors, tDescNode) {
      /* Iterator variable */
      Descriptor *tDesc = tDescNode->mappingValue;

      /* Skip closed descriptors */
      if (DescriptorClosed(tDesc))
	continue;

      /* Writer membership */
      if (tDesc->bits.prompt || tDesc->outputN)
	FD_SET(tDesc->socket->handle, &writers);

      /* Reader membership */
      FD_SET(tDesc->socket->handle, &readers);

      /* Maximum socket handle value */
      if (tDesc->socket->handle > topHandle)
	topHandle = tDesc->socket->handle;
    }

    /* Wait for network events */
    const int howMany = select(
	topHandle + 1, &readers, &writers, NULL, timeout);

    if (howMany < 0 && errno != EINTR) {
      Log(L_SYSTEM, "select() failed: errno=%d.", errno);
    } else {
      /* Check read and write sets */
      List *closed = ListAlloc(NULL, NULL);
      TreeForEach(game->descriptors, tDescNode) {
	/* Iterator variable */
	Descriptor *tDesc = tDescNode->mappingValue;

	/* Check readers */
	if (SocketCheck(tDesc->socket, &readers))
	  DescriptorReceive(tDesc);

	/* Check writers */
	if (SocketCheck(tDesc->socket, &writers))
	  DescriptorFlush(tDesc);

	/* Check closed descriptors */
	if (DescriptorClosed(tDesc))
	  ListPushBack(closed, tDesc);
      }

      /* Control socket network events */
      if (SocketCheck(game->socket, &readers))
	GameAccept(game);

      /* Delete closed descriptors */
      ListForEach(closed, tDescNode) {
	Descriptor *tDesc = tDescNode->value;
	TreeDelete(game->descriptors, &tDesc->name);
      }
      ListFree(closed);
    }
  }
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
    /* Open server */
    GameOpen(game, "", 6767);
    if (SocketClosed(game->socket))
      return;

    /* Run game loop */
    Log(L_NETWORK, "Starting game loop.");
    while (!game->shutdown) {
      /* Poll timeout */
      Time timeout;
      TimeSet(&timeout, 60, 0);

      /* Poll for network events */
      GamePoll(game, &timeout);
    }
    Log(L_NETWORK, "Game loop finished.");

    /* Close server */
    SocketClose(game->socket);
  }
}
