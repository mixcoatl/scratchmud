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
typedef struct _Editor Editor;
typedef struct _Server Server;
typedef struct _Socket Socket;
typedef struct _State  State;

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

/*!
 * Client TELNET states.
 * \addtogroup client
 * \{
 */
#define CLIENT_TELSTATE_DATA    (0)     /*!< Client handling plaintext */
#define CLIENT_TELSTATE_IAC     (1)     /*!< Client got IAC */
#define CLIENT_TELSTATE_SB      (2)     /*!< Client got SB */
#define CLIENT_TELSTATE_SB_IAC  (3)     /*!< Client got IAC inside SB */
#define CLIENT_TELSTATE_TELCMD  (4)     /*!< Client got a TELNET command */
#define CLIENT_TELSTATE_TELOPT  (5)     /*!< Client got a TELNET option */
/*! \} */

/*! How many CLIENT_TELSTATE_x constants? */
#define MAX_CLIENT_TELSTATE_TYPES \
  (CLIENT_TELSTATE_TELOPT - CLIENT_TELSTATE_DATA + 1)

/*! The length of an I/O buffer. */
#define MAXLEN_IOBUF		(12288)

/*!
 * A network client.
 * \addtogroup client
 * \{
 */
struct _Client {
  Editor       *editor;         /*!< The string editor session */
  Bitvector     flags;          /*!< The client flags: CLIENT_x */
  char         *hostname;       /*!< The remote name */
  uint8_t       input[MAXLEN_INPUT];  /*!< The input buffer */
  size_t        inputN;         /*!< The input buffer used */
  uint16_t      lineLength;     /*!< The output line length */
  char         *name;           /*!< The client name */
  uint16_t      nawsHeight;     /*!< The client window height */
  uint16_t      nawsWidth;      /*!< The client window width */
  uint8_t       output[MAXLEN_IOBUF]; /*!< The output buffer */
  size_t        outputN;        /*!< The output buffer used */
  uint8_t       subneg[MAXLEN_IOBUF]; /*!< The sub-negotiation buffer */
  size_t        subnegN;        /*!< The sub-negotiation buffer used */
  Server       *server;         /*!< The server */
  Socket       *socket;         /*!< The client socket */
  State        *state;          /*!< The state of connectedness */
  uint8_t       telcmd;         /*!< The TELNET command: DO, DONT, etc. */
  int           telstate;       /*!< The TELNET state: CLIENT_TELSTATE_x */
  uint8_t       telopt;         /*!< The TELNET option */
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
 * Sends a TELNET command.
 * \addtogroup client
 * \param client the client to which to send a TELNET command
 * \param telcmd the TELNET command: DO, DONT, WILL, WONT, etc.
 * \param telopt the TELNET option: TELOPT_x
 */
void ClientPutCommand(
	Client *client,
	const uint8_t telcmd,
	const uint8_t telopt);

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

/*!
 * Processes a TELNET command.
 * \addtogroup client
 * \param client the client for which to process a TELNET command
 */
void ClientReceiveTelnet(Client *client);

/*!
 * Changes to another state.
 * \addtogroup client
 * \param client the client whose state to change
 * \param state the state to which to change
 * \sa ClientStateChangeByName(Client*, const char*)
 */
void ClientStateChange(
	Client *client,
	State *state);

/*!
 * Changes to another state.
 * \addtogroup client
 * \param client the client whose state to change
 * \param stateName the state name of the state to which to change
 * \sa ClientStateChange(Client*, State*)
 */
void ClientStateChangeByName(
	Client *client,
	const char *stateName);

#endif /* _SCRATCH_CLIENT_H_ */
