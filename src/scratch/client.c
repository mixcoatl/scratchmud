/*!
 * \file client.c
 *
 * \par Copyright
 * Copyright (C) 1999-2019 scratchmud.org
 * All rights reserved.
 *
 * \author Geoffrey Davis <gdavis@scratchmud.org>
 * \addtogroup client
 */
#define _SCRATCH_CLIENT_C_

#include <scratch/bitvector.h>
#include <scratch/client.h>
#include <scratch/game.h>
#include <scratch/log.h>
#include <scratch/memory.h>
#include <scratch/rbtree.h>
#include <scratch/server.h>
#include <scratch/scratch.h>
#include <scratch/socket.h>
#include <scratch/string.h>
#include <scratch/utility.h>

/*!
 * Constructs a new client.
 * \addtogroup client
 * \param server the server to which the client belongs
 * \return the new client or NULL
 * \sa ClientFree(Client*)
 */
Client *ClientAlloc(Server *server) {
  register Client *client;
  if (!server) {
    Log("invalid `server` Server");
  } else {
    MemoryCreate(client, Client, 1);
    client->server = server;
    client->socket = NULL;

    /* Initial client flags */
    BitSetN(client->flags, CLIENT_PROMPT);

    StringSet(&client->hostname, "*Unknown*");
    while (*StringBlank(client->name) == '\0') {
      /* Pick a client name */
      char name[MAXLEN_INPUT] = {'\0'};
      UtilityNameGenerate(name, sizeof(name));

      /* Search for a client with this name */
      if (ClientByName(server, name) == NULL)
	StringSet(&client->name, name);
    }
  }
  return (client);
}

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
        const char *clientName) {
  register Client *client = NULL;
  if (!server) {
    Log("invalid `server` Server");
  } else if (*StringBlank(clientName) == '\0') {
    Log("invalid `clientName` string");
  } else {
    client = RBTreeGetValue(server->clients, (RBTreeKey) &clientName, NULL);
  }
  return (client);
}

/*!
 * Closes a client.
 * \addtogroup client
 * \param client the client to close
 */
void ClientClose(Client *client) {
  if (!client) {
    Log("invalid `client` Client");
  } else {
    /* Socket cleanup */
    SocketFree(client->socket);
    client->socket = NULL;
  }
}

/*!
 * Returns whether a client is closed.
 * \addtogroup client
 * \param client the client whose closed state to return
 * \return true if the specified client is closed
 * \sa ClientClose(Client*)
 */
bool ClientClosed(const Client *client) {
  return !client || !client->socket;
}

/*!
 * Flushes buffered output.
 * \addtogroup client
 * \param client the client to flush
 */
void ClientFlush(Client *client) {
  if (!client) {
    Log("invalid `client` Client");
  } else if (ClientClosed(client)) {
    Log("client %s is already closed", client->name);
  } else {
    /* Send prompt */
    if (ClientNeedsPrompt(client))
      ClientPutPrompt(client);

    /* We don't need this anymore */
    BitRemoveN(client->flags, CLIENT_PROMPT);

    /* Write buffered output */
    const ssize_t nBytes = SocketWrite(
	client->socket,
	client->output, client->outputN);

    if (nBytes < 0) {
      Log("Losing client %s", client->name);
      ClientClose(client);
    } else if (nBytes > 0) {
      /* Erase flushed output */
      MemoryCopy(
	client->output,
	client->output + client->outputN,
	uint8_t,
	client->outputN - nBytes);

      /* Adjust output size */
      client->outputN -= nBytes;
    }
  }
}

/*!
 * Frees a client.
 * \addtogroup client
 * \param client the client to free
 * \sa ClientAlloc(Server*)
 */
void ClientFree(Client *client) {
  if (client) {
    ClientClose(client);
    StringFree(client->hostname);
    StringFree(client->name);
    MemoryFree(client);
  }
}

/*!
 * Returns whether a client prints a prompt.
 * \addtogroup client
 * \param client the client for which to return whether to print a prompt
 * \return true if the specified client must print a prompt
 */
bool ClientNeedsPrompt(const Client *client) {
  register bool result = false;
  if (!client) {
    Log("invalid `client` Client");
  } else {
    result = BitCheckN(client->flags, CLIENT_PROMPT);
  }
  return (result);
}

/*!
 * Prints a message to a client.
 * \addtogroup client
 * \param client the client to which to print a message
 * \param format the printf-style format specifier string
 */
void ClientPrint(
	Client *client,
	const char *format, ...) {
  if (!client) {
    Log("invalid `client` Client");
  } else if (*StringBlank(format) != '\0') {
    /* The message buffer */
    char messg[MAXLEN_STRING] = {'\0'};
    register ssize_t messglen;

    /* Format the message */
    va_list args;
    va_start(args, format);
    messglen = vsnprintf(messg, sizeof(messg), format, args);
    va_end(args);

    /* Check for a format failure */
    if (messglen < 0) {
      Log("vsnprintf() failed: errno=%d", errno);
      ClientClose(client);
    /* Check for an output buffer overflow */
    } else if (sizeof(client->output) < client->outputN + messglen) {
      Log("Output overflow on client %s", client->name);
      ClientClose(client);
    /* Must have been OK then */
    } else {
      /* Interrupt */
      if (!ClientNeedsPrompt(client) && !client->inputN)
	BPrintf(client->output, sizeof(client->output), client->outputN, "\r\n");

      /* Process output */
      register const char *p = NULL;
      for (p = messg; p && *p != '\0'; ++p) {
	client->output[client->outputN++] = *p;
	if (strchr("\r\n", *p) != NULL) {
	  client->lineLength = 0;
	  if (*p == '\n')
	    BitSetN(client->flags, CLIENT_PROMPT);
	} else if (*p == '\x1b' && p[1] == '[') {
	  if (strncmp(p, "\x1b[2J", 5) == 0)
	    client->lineLength = 0;
	  while (p && *p != '\0' && !isalpha(*p))
	    client->output[client->outputN++] = *p++;
	  client->output[client->outputN++] = *p;
	} else if (strchr("\b\x7f", *p) != NULL) {
	  if (client->lineLength)
	    client->lineLength -= 1;
	} else if (*p == '\t') {
	  client->lineLength += 8;
	} else if (isprint(*p)) {
	  client->lineLength++;
	}
      }
    }
  }
}

/*!
 * Sends a prompt.
 * \addtogroup client
 * \param client the client to which to send a prompt
 */
void ClientPutPrompt(Client *client) {
  if (!client) {
    Log("invalid `client` Client");
  } else if (ClientClosed(client)) {
    Log("client %s is already closed", client->name);
  } else {
    ClientPrint(client, ":ScratchMUD:> ");
  }
}

/*!
 * Reads and processes input.
 * \addtogroup client
 * \param client the client upon which to receive
 */
void ClientReceive(Client *client) {
  if (!client) {
    Log("invalid `client` Client");
  } else if (ClientClosed(client)) {
    Log("client %s is already closed", client->name);
  } else {
    /* Try to read the socket */
    uint8_t messg[MAXLEN_IOBUF] = {'\0'};
    const ssize_t nBytes = SocketRead(
	client->socket,
	messg, sizeof(messg));

    /* Socket read error */
    if (nBytes < 0) {
      Log("Couldn't read client %s", client->name);
      ClientClose(client);
    /* Remote host closed connection */
    } else if (nBytes == 0) {
      Log("EOF read on client %s", client->name);
      ClientClose(client);
    } else {
      register const uint8_t *p;
      for (p = messg; p < messg + nBytes; ++p) {
	if (strchr("\b\x7f", *p) != NULL) {
	  if (client->inputN)
	    client->input[--client->inputN] = '\0';
	} else if (*p == '\n') {
	  BitSetN(client->flags, CLIENT_PROMPT);
	  ClientPrint(client, "%s\r\n", client->input);
	  MemoryZero(client->input, char, client->inputN);
	  client->inputN = 0;
	} else if (client->inputN >= sizeof(client->input) - 1) {
	  Log("Input overflow on client %s", client->name);
	  ClientClose(client);
	} else if (isprint(*p)) {
	  BPrintf(client->input, sizeof(client->input), client->inputN, "%c", *p);
	}
      }
    }
  }
}
