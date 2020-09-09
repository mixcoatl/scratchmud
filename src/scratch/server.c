/*!
 * \file server.c
 *
 * \par Copyright
 * Copyright (C) 1999-2019 scratchmud.org
 * All rights reserved.
 *
 * \author Geoffrey Davis <gdavis@scratchmud.org>
 * \addtogroup server
 */
#define _SCRATCH_SERVER_C_

#include <scratch/bitvector.h>
#include <scratch/client.h>
#include <scratch/game.h>
#include <scratch/list.h>
#include <scratch/log.h>
#include <scratch/memory.h>
#include <scratch/rbtree.h>
#include <scratch/scratch.h>
#include <scratch/server.h>
#include <scratch/socket.h>
#include <scratch/string.h>
#include <scratch/time.h>
#include <scratch/utility.h>

/*!
 * Accepts an incoming connection.
 * \addtogroup server
 * \param server the server that has an incoming connection
 */
void ServerAccept(Server *server) {
  if (!server) {
    Log("invalid `server` Server");
  } else if (ServerClosed(server)) {
    Log("server is already closed");
  } else {
    /* Accept the connection */
    Client *client = ClientAlloc(server);
    client->socket = SocketAccept(server->socket);

    if (!client->socket) {
      Log("Couldn't accept incoming socket connection");
      ClientFree(client);
    } else {
      /* Shortcuts */
      SOCKADDR *peer = &client->socket->address;
      socklen_t peerSZ = sizeof(SOCKADDR);

      /* Get the socket name */
      char name[INET6_ADDRSTRLEN] = {'\0'};
      if (getnameinfo(peer, peerSZ, name, sizeof(name), 0, 0, 0) < 0) {
	Log("getnameinfo() failed: errno=%d", errno);
	strlcpy(name, "*Unknown*", sizeof(name));
      }

      StringSet(&client->hostname, name);
      if (!RBTreeInsert(server->clients, (RBTreeKey) &client->name, client)) {
	Log("Couldn't add client %s to the client index", client->name);
	ClientFree(client), client = NULL;
      }

      if (client) {
	Log("Accepted client %s from %s", client->name, client->hostname);

	/* Setup default TELNET options */
	ClientPutCommand(client, DO, TELOPT_ECHO);
	ClientPutCommand(client, DO, TELOPT_NAWS);
	ClientPutCommand(client, WONT, TELOPT_ECHO);
      }
    }
  }
}

/*!
 * Constructs a new server.
 * \addtogroup server
 * \param game the game state
 * \return the new server or NULL
 * \sa ServerFree(Server*)
 */
Server *ServerAlloc(Game *game) {
  register Server *server;
  if (!game) {
    Log("invalid `game` Game");
  } else {
    MemoryCreate(server, Server, 1);
    server->clients = RBTreeAlloc(
	(RBTreeCompareProc) UtilityNameCmp,
	(RBTreeFreeProc)    ClientFree);
    server->game    = game;
    server->socket  = NULL;
  }
  return (server);
}

/*!
 * Closes a server.
 * \addtogroup server
 * \param server the server to close
 * \sa ServerClosed(const Server*)
 */
void ServerClose(Server *server) {
  if (!server) {
    Log("invalid `server` Server");
  } else {
    SocketFree(server->socket);
    server->socket = NULL;
  }
}

/*!
 * Frees a server.
 * \addtogroup server
 * \param server the server to free
 */
void ServerFree(Server *server) {
  if (server) {
    /* Dettach the server from the game */
    if (server->game && server->game->server == server)
      server->game = NULL;

    /* Close control socket */
    ServerClose(server);

    /* Cleanup */
    RBTreeFree(server->clients);
    MemoryFree(server);
  }
}

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
	const uint16_t port) {
  register bool result = false;
  if (!server) {
    Log("invalid `server` Server");
  } else if (server->socket) {
    Log("server control socket already open");
  } else {
    server->socket = SocketAlloc();
    if (!SocketOpen(server->socket, address, port)) {
      Log("Couldn't open server using address '%s', port %hu", StringBlank(address), port);
      ServerClose(server);
    } else if (*StringBlank(address) != '\0') {
      Log("Opened server on %s, port %hu.", address, port);
      result = true;
    } else {
      Log("Opened server on port %hu.", port);
      result = true;
    }
  }
  return (result);
}

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
	Time *timeout) {
  if (!server) {
    Log("invalid `server` Server");
  } else {
    /* Reader sockets */
    fd_set readers;
    FD_ZERO(&readers);

    /* Writer sockets */
    fd_set writers;
    FD_ZERO(&writers);

    /* The highest valued handle */
    SOCKET topHandle = INVALID_SOCKET;

    /* The control socket */
    if (server->socket) {
      FD_SET(server->socket->handle, &readers);
      topHandle = server->socket->handle;
    }

    /* Configure our read and write sets */
    RBTreeForEach(server->clients, tClientNode) {
      /* Iterator variable */
      Client *tClient = tClientNode->value;

      /* Reader membership */
      if (tClient->socket)
	FD_SET(tClient->socket->handle, &readers);
      if (tClient->socket && BitCheckN(tClient->flags, CLIENT_PROMPT))
	FD_SET(tClient->socket->handle, &writers);

      /* Writer membership */
      if (tClient->socket && tClient->outputN)
	FD_SET(tClient->socket->handle, &writers);

      /* Maximum socket handle value */
      if (tClient->socket && topHandle < tClient->socket->handle)
	topHandle = tClient->socket->handle;
    }
    RBTreeForEachEnd();

    /* Wait for network events */
    const ssize_t retcode = select(
	topHandle + 1,
	&readers, &writers, NULL,
	timeout);

    if (retcode < 0 && errno != EINTR) {
      Log("select() failed: errno=%d", errno);
    } else {
      /* Dispatch client network events */
      List *removed = ListAlloc(0, 0);
      RBTreeForEach(server->clients, tClientNode) {
	/* Iterator variable */
	Client *tClient = tClientNode->value;

	/* Check for readers */
	if (tClient->socket && FD_ISSET(tClient->socket->handle, &readers))
	  ClientReceive(tClient);

	/* Check for writers */
	if (tClient->socket && FD_ISSET(tClient->socket->handle, &writers))
	  ClientFlush(tClient);

	/* Check for closed clients */
	if (!tClient->socket)
	  ListPushBack(removed, tClient);
      }
      RBTreeForEachEnd();

      /* Removed any closed clients */
      ListForEach(removed, tClientNode) {
	Client *tClient = tClientNode->value;
	RBTreeDelete(server->clients, (RBTreeKey) &tClient->name);
      }
      ListForEachEnd();
      ListFree(removed);

      /* Control socket network events */
      if (server->socket && FD_ISSET(server->socket->handle, &readers))
	ServerAccept(server);
    }
  }
}
