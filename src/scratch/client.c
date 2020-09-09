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

#define TELCMDS
#define TELOPTS

#include <scratch/bitvector.h>
#include <scratch/client.h>
#include <scratch/color.h>
#include <scratch/editor.h>
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
    client->nawsHeight = /* default */ 25;
    client->nawsWidth  = /* default */ 80;
    client->server     = server;
    client->socket     = NULL;
    client->telstate   = CLIENT_TELSTATE_DATA;

    /* Initial client flags */
    BitSetN(client->flags, CLIENT_COLOR);
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
    /* Editor cleanup */
    if (client->editor)
      EditorAbort(client);

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
 * Sends a TELNET command.
 * \addtogroup client
 * \param client the client to which to send a TELNET command
 * \param telcmd the TELNET command: DO, DONT, WILL, WONT, etc.
 * \param telopt the TELNET option: TELOPT_x
 */
void ClientPutCommand(
	Client *client,
	const uint8_t telcmd,
	const uint8_t telopt) {
  if (!client) {
    Log("invalid `client` Client");
  } else {
    if (sizeof(client->output) < client->outputN + 3) {
      Log("Output overflow on client %s", client->name);
      ClientClose(client);
    } else {
      client->output[client->outputN++] = IAC;
      client->output[client->outputN++] = telcmd;
      client->output[client->outputN++] = telopt;

      Log("Client %s sent IAC %s %s",
	client->name, TELCMD(telcmd), TELOPT(telopt));
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
    ClientPrint(client, "%s:ScratchMUD:%s> ", Q_RED, Q_NORMAL);
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
	switch (client->telstate) {
	case CLIENT_TELSTATE_DATA:
	  if (*p == IAC) {
	    client->telstate = CLIENT_TELSTATE_IAC;
	  } else if (strchr("\b\x7f", *p) != NULL) {
	    if (client->inputN)
	      client->input[--client->inputN] = '\0';
	  } else if (*p == '\n') {
	    BitSetN(client->flags, CLIENT_PROMPT);
	    if (client->editor) {
	      EditorAdd(client, client->input);
	    } else {
	      ClientPrint(client, "%s\r\n", client->input);
	    }
	    MemoryZero(client->input, char, client->inputN);
	    client->inputN = 0;
	  } else if (client->inputN >= sizeof(client->input) - 1) {
	    Log("Input overflow on client %s", client->name);
	    ClientClose(client);
	  } else if (isprint(*p)) {
	    BPrintf(client->input, sizeof(client->input), client->inputN, "%c", *p);
	  }
	  break;
	case CLIENT_TELSTATE_IAC:
	  switch (*p) {
	  case EC:
	    if (client->inputN)
	      client->input[--client->inputN] = '\0';
	    if (client->lineLength)
	      client->lineLength--;
	    break;
	  case EL:
	    while (client->inputN)
	      client->input[--client->inputN] = '\0';
	    client->lineLength = 0;
	    break;
	  case DO:
	  case DONT:
	  case SB:
	  case WILL:
	  case WONT:
	    client->telcmd = *p;
	    client->telstate = CLIENT_TELSTATE_TELCMD;
	    break;
	  default:
	    client->telstate = CLIENT_TELSTATE_DATA;
	    if (*p == IAC)
	      BPrintf(client->input, sizeof(client->input), client->inputN, "%c", IAC);
	    break;
	  }
	  break;
	case CLIENT_TELSTATE_SB:
	  if (*p == IAC) {
	    client->telstate = CLIENT_TELSTATE_SB_IAC;
	  } else if (sizeof(client->subneg) > client->subnegN) {
	    client->subneg[client->subnegN++] = *p;
	  }
	  break;
	case CLIENT_TELSTATE_SB_IAC:
	  if (*p == SE) {
	    client->telstate = CLIENT_TELSTATE_DATA;
	    ClientReceiveTelnet(client);
	  } else {
	    client->telstate = CLIENT_TELSTATE_SB;
	    if (sizeof(client->subneg) > client->subnegN)
	      client->subneg[client->subnegN++] = *p;
	  }
	  break;
	case CLIENT_TELSTATE_TELCMD:
	  client->telopt = *p;
	  if (client->telcmd == SB) {
	    MemoryZero(client->subneg, uint8_t, sizeof(client->subneg));
	    client->subnegN = 0;
	    client->telstate = CLIENT_TELSTATE_SB;
	  } else {
	    client->telstate = CLIENT_TELSTATE_DATA;
	    ClientReceiveTelnet(client);
	  }
	  break;
	}

      }
    }
  }
}

/*!
 * Processes a TELNET command.
 * \addtogroup client
 * \param client the client for which to process a TELNET command
 */
void ClientReceiveTelnet(Client *client) {
  if (!client) {
    Log("invalid `client` Client");
  } else if (ClientClosed(client)) {
    Log("client %s already closed", client->name);
  } else {
    switch (client->telcmd) {
    case DO:
    case DONT:
    case WILL:
    case WONT:
      Log("Client %s received IAC %s %s",
		client->name,
		TELCMD(client->telcmd),
		TELOPT(client->telopt));
      break;
    case SB:
      switch (client->telopt) {
      case TELOPT_NAWS:
	if (client->subnegN == 4) {
	  client->nawsWidth  = ntohs(*((uint16_t*)(client->subneg + 0)));
	  client->nawsHeight = ntohs(*((uint16_t*)(client->subneg + 2)));
	  Log("Client %s has window size %hu x %hu",
		client->name,
		client->nawsWidth,
		client->nawsHeight);
	}
	break;
      }
      break;
    }
  }
}
