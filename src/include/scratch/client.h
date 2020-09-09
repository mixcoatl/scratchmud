/*!
 * \file client.h
 *
 * \par Copyright
 * Copyright (C) 1999-2019 scratchmud.org
 * All rights reserved.
 *
 * \author Geoffrey Davis <gdavis@scratchmud.org>
 * \addtogroup client
 */
#ifndef _SCRATCH_CLIENT_H_
#define _SCRATCH_CLIENT_H_

#include <scratch/bitvector.h>
#include <scratch/scratch.h>

/* Forward type declarations */
typedef struct _Client Client;
typedef struct _Server Server;
typedef struct _Socket Socket;

/*!
 * Client flags.
 * \addtogroup client
 * \{
 */
#define CLIENT_COLOR            (0)     /*!< Client has color enabled */
#define CLIENT_PROMPT           (1)     /*!< Client needs prompt */
/*! \} */

/*! How many CLIENT_x flags? */
#define MAX_CLIENT_BITS \
  (CLIENT_PROMPT - CLIENT_COLOR + 1)

/*! The length of an I/O buffer. */
#define MAXLEN_IOBUF		(12288)

/*!
 * A network client.
 * \addtogroup client
 * \{
 */
struct _Client {
  Bitvector     flags;          /*!< The client flags: CLIENT_x */
  char         *hostname;       /*!< The remote name */
  uint8_t       input[MAXLEN_INPUT];  /*!< The input buffer */
  size_t        inputN;         /*!< The input buffer used */
  uint16_t      lineLength;     /*!< The output line length */
  char         *name;           /*!< The client name */
  uint8_t       output[MAXLEN_IOBUF]; /*!< The output buffer */
  size_t        outputN;        /*!< The output buffer used */
  Server       *server;         /*!< The server */
  Socket       *socket;         /*!< The client socket */
};
/*! \} */

/*!
 * Constructs a new client.
 * \addtogroup client
 * \param server the server to which the client belongs
 * \return the new client or NULL
 * \sa ClientFree(Client*)
 */
Client *ClientAlloc(Server *server);

/*!
 * Searches for a client.
 * \addtogroup client
 * \param server the server to which the client belongs
 * \param clientName the client name of the client to return
 * \return the client indicated by the specified client name
 *     or NULL
 */
Client *ClientByName(
	Server *server,
	const char *clientName);

/*!
 * Closes a client.
 * \addtogroup client
 * \param client the client to close
 * \sa ClientClosed(client)
 */
void ClientClose(Client *client);

/*!
 * Returns whether a client is closed.
 * \addtogroup client
 * \param client the client whose closed state to return
 * \return true if the specified client is closed
 * \sa ClientClose(Client*)
 */
bool ClientClosed(const Client *client);

/*!
 * Flushes buffered output.
 * \addtogroup client
 * \param client the client to flush
 */
void ClientFlush(Client *client);

/*!
 * Frees a client.
 * \addtogroup client
 * \param client the client to free
 * \sa ClientAlloc(Server*)
 */
void ClientFree(Client *client);

/*!
 * Returns whether a client prints a prompt.
 * \addtogroup client
 * \param client the client for which to return whether to print a prompt
 * \return true if the specified client must print a prompt
 */
bool ClientNeedsPrompt(const Client *client);

/*!
 * Prints a message to a client.
 * \addtogroup client
 * \param client the client to which to print a message
 * \param format the printf-style format specifier string
 */
void ClientPrint(
	Client *client,
	const char *format, ...);

/*!
 * Sends a prompt.
 * \addtogroup client
 * \param client the client to which to send a prompt
 */
void ClientPutPrompt(Client *client);

/*!
 * Reads and processes input.
 * \addtogroup client
 * \param client the client upon which to receive
 */
void ClientReceive(Client *client);

#endif /* _SCRATCH_CLIENT_H_ */
