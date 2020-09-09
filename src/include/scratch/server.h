/*!
 * \file server.h
 *
 * \par Copyright
 * Copyright (C) 1999-2019 scratchmud.org
 * All rights reserved.
 *
 * \author Geoffrey Davis <gdavis@scratchmud.org>
 * \addtogroup server
 */
#ifndef _SCRATCH_SERVER_H_
#define _SCRATCH_SERVER_H_

#include <scratch/scratch.h>
#include <scratch/time.h>

/* Forward type declarations */
typedef struct _Client Client;
typedef struct _Game   Game;
typedef struct _RBTree RBTree;
typedef struct _Server Server;
typedef struct _Socket Socket;

/*!
 * A network server.
 * \addtogroup server
 * \{
 */
struct _Server {
  RBTree       *clients;        /*!< The clients */
  Game         *game;           /*!< The game state */
  Socket       *socket;         /*!< The control socket */
};
/*! \} */

/*!
 * Accepts an incoming connection.
 * \addtogroup server
 * \param server the server that has an incoming connection
 */
void ServerAccept(Server *server);

/*!
 * Constructs a new server.
 * \addtogroup server
 * \param game the game state
 * \return the new server or NULL
 * \sa ServerFree(Server*)
 */
Server *ServerAlloc(Game *game);

/*!
 * Closes a server.
 * \addtogroup server
 * \param server the server to close
 * \sa ServerClosed(server)
 */
void ServerClose(Server *server);

/*!
 * Returns whether a server is closed.
 * \addtogroup server
 * \param server the server whose closed state to return
 * \return true if the specified server is closed
 * \sa ServerClose(Server*)
 */
#define ServerClosed(server) \
  (!(server) || !(server)->socket)

/*!
 * Frees a server.
 * \addtogroup server
 * \param server the server to free
 */
void ServerFree(Server *server);

/*!
 * Opens a server.
 * \addtogroup server
 * \param server the server to open
 * \param address the local endpoint address
 * \param port the local endpoint port
 * \return true if the specified server was successfully opened
 *     using the specified address and port
 */
bool ServerOpen(
	Server *server,
	const char *address,
	const uint16_t port);

/*!
 * Polls for network events.
 * \addtogroup server
 * \param server the server to poll for network events
 * \param timeout the interval this function should block
 *     waiting for the server or one of its clients to
 *     become ready
 */
void ServerPoll(
	Server *server,	
	Time *timeout);

#endif /* _SCRATCH_SERVER_H_ */
