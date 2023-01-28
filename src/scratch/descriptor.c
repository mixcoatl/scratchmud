/*!
 * \file descriptor.c
 *
 * \par Copyright
 * Copyright (C) 1999-2023 scratchmud.org
 * All rights reserved.
 *
 * \author Geoffrey Davis <gdavis@scratchmud.org>
 * \addtogroup descriptor
 */
#define _SCRATCH_DESCRIPTOR_C_

#define TELCMDS
#define TELOPTS

#include <scratch/color.h>
#include <scratch/descriptor.h>
#include <scratch/creator.h>
#include <scratch/editor.h>
#include <scratch/game.h>
#include <scratch/log.h>
#include <scratch/memory.h>
#include <scratch/scratch.h>
#include <scratch/socket.h>
#include <scratch/state.h>
#include <scratch/string.h>
#include <scratch/tree.h>
#include <scratch/utility.h>

/* Local functions */
STATE(PlayingOnReceived);

/*!
 * Constructs a new descriptor.
 * \addtogroup descriptor
 * \param game the game state
 * \return a new descriptor or NULL
 * \sa DescriptorFree(Descriptor*)
 * \sa DescriptorFreeV(void*)
 */
Descriptor *DescriptorAlloc(Game *game) {
  register Descriptor *d;
  if (!game) {
    Log(L_ASSERT, "Invalid `game` Game.");
  } else {
    /* Descriptor */
    MemoryCreate(d, Descriptor, 1);
    MemoryZero(d->input, char, sizeof(d->input));
    MemoryZero(d->sb, char, sizeof(d->sb));
    d->bits.color = false;
    d->bits.prompt = false;
    d->bits.sb = false;
    d->creator = NULL;
    d->editor = NULL;
    d->game = game;
    d->hostname = NULL;
    d->inputN = 0;
    d->name = NULL;
    d->sbN = 0;
    d->socket = NULL;
    d->state = NULL;
    d->telnetCommand = 0;
    d->telnetOption = 0;
    d->windowHeight = /* default */ 25;
    d->windowWidth = /* default */ 80;

    /* Hostname is unknown */
    d->hostname = strdup("*Unknown*");

    while (!d->name || *d->name == '\0') {
      /* Pick descriptor name */
      char name[MAXLEN_INPUT] = {'\0'};
      UtilityNameGenerate(name, sizeof(name));

      /* Search for descriptor using name */
      if (!DescriptorByName(game, name)) {
	StringFree(d->name);
	d->name = strdup(name);
      }
    }
  }
  return (d);
}

/*!
 * Searches for a descriptor.
 * \addtogroup descriptor
 * \param game the game state
 * \param descriptorName the descriptor name of the descriptor to return
 * \return the descriptor indicated by the specified descriptor name or NULL
 */
Descriptor *DescriptorByName(
	Game *game,
	const char *descriptorName) {
  register Descriptor *d = NULL;
  if (!game) {
    Log(L_ASSERT, "Invalid `game` Game.");
  } else if (!descriptorName || *descriptorName == '\0') {
    Log(L_ASSERT, "Invalid `descriptorName` string.");
  } else {
    d = TreeGetValue(game->descriptors, &descriptorName, NULL);
  }
  return (d);
}

/*!
 * Closes a descriptor.
 * \addtogroup descriptor
 * \param d the descriptor to close
 * \sa DescriptorClosed(const Descriptor*)
 */
void DescriptorClose(Descriptor *d) {
  if (!d) {
    Log(L_ASSERT, "Invalid `d` Descriptor.");
  } else {
    /* Abort string editor */
    if (d->editor)
      EditorAbort(d);

    /* Detach creator */
    if (d->creator) {
      CreatorFree(d->creator);
      d->creator = NULL;
    }

    /* Detach descriptor state */
    StateChange(d, NULL);

    /* Socket cleanup */
    SocketFree(d->socket);
    d->socket = NULL;
  }
}

/*!
 * Returns whether a descriptor is closed.
 * \addtogroup descriptor
 * \param d the descriptor whose closed state to return
 * \return true if the specified descriptor is closed
 * \sa DescriptorClose(Descriptor*)
 */
bool DescriptorClosed(const Descriptor *d) {
  return !d || SocketClosed(d->socket);
}

/*!
 * Flushes buffered output.
 * \addtogroup descriptor
 * \param d the descriptor to flush
 */
void DescriptorFlush(Descriptor *d) {
  if (!d) {
    Log(L_ASSERT, "Invalid `d` Descriptor.");
  } else if (DescriptorClosed(d)) {
    Log(L_ASSERT, "Descriptor %s is already closed.", d->name);
  } else {
    /* Send prompt */
    if (d->bits.prompt) {
      if (d->state && d->state->bits.prompt)
	DescriptorPutPrompt(d);
      d->bits.prompt = false;
    }

    /* Write buffered output */
    const ssize_t nBytes = SocketWrite(
	d->socket,
	d->output, d->outputN);

    if (nBytes < 0) {
      Log(L_NETWORK, "Losing descriptor %s.", d->name);
      DescriptorClose(d);
    } else if (nBytes > 0) {
      /* Erase flushed output */
      MemoryCopy(
	d->output,
	d->output + d->outputN,
	uint8_t,
	d->outputN - nBytes);

      /* Adjust output size */
      d->outputN -= nBytes;
    }
  }
}

/*!
 * Frees a descriptor.
 * \addtogroup descriptor
 * \param d the descriptor to free
 * \sa DescriptorAlloc(Game*)
 * \sa DescriptorFreeV(void*)
 */
void DescriptorFree(Descriptor *d) {
  if (d) {
    DescriptorClose(d);
    StringFree(d->hostname);
    StringFree(d->name);
    MemoryFree(d);
  }
}

/*!
 * Frees a descriptor.
 * \addtogroup descriptor
 * \param d the descriptor to free
 * \sa DescriptorAlloc(Game*)
 * \sa DescriptorFree(Descriptor*)
 */
void DescriptorFreeV(void *d) {
  DescriptorFree(d);
}

/*!
 * Prints a message to a descriptor.
 * \addtogroup descriptor
 * \param d the descriptor to which to print
 * \param format the printf-style format specifier
 */
void DescriptorPrint(
	Descriptor *d,
	const char *format, ...) {
  if (!d) {
    Log(L_ASSERT, "Invalid `d` Descriptor.");
  } else if (format && *format != '\0') {
    /* Format message */
    va_list args;
    va_start(args, format);
    char messg[MAXLEN_STRING] = {'\0'};
    const int messglen = vsnprintf(messg, sizeof(messg), format, args);
    va_end(args);

    /* Check for format failure */
    if (messglen < 0) {
      Log(L_SYSTEM, "vsnprintf() failed: errno=%d.", errno);
      DescriptorClose(d);
    /* Check for output buffer overflow */
    } else if (sizeof(d->output) < d->outputN + messglen) {
      Log(L_NETWORK, "Output overflow on descriptor %s.", d->name);
      DescriptorClose(d);
    /* Must have been OK then */
    } else {
      /* Interrupt */
      if (!d->bits.prompt && !d->inputN) {
	if (d->state && d->state->bits.prompt)
	  BPrintf(d->output, sizeof(d->output), d->outputN, "\r\n");
      }

      /* Process output */
      register const char *p = messg;
      for (; p && *p != '\0'; ++p) {
	d->output[d->outputN++] = *p;
	if (strchr("\r\n", *p) != NULL) {
	  d->lineLength = 0;
	  if (*p == '\n')
	    d->bits.prompt = true;
	} else if (*p == '\x1b' && p[1] == '[') {
	  if (strncmp(p, "\x1b[2J", 5) == 0)
	    d->lineLength = 0;
	  while (p && *p != '\0' && !isalpha((int) *p))
	    d->output[d->outputN++] = *p++;
	  d->output[d->outputN++] = *p;
	} else if (strchr("\b\x7f", *p) != NULL) {
	  if (d->lineLength)
	    d->lineLength -= 1;
	} else if (*p == '\t') {
	  d->lineLength += 8;
	} else if (isprint((int) *p)) {
	  d->lineLength++;
	}
      }
    }
  }
}

/*!
 * Sends a TELNET command.
 * \addtogroup descriptor
 * \param d the descriptor to which to send
 * \param telnetCommand the TELNET command: DO, DONT, WILL, WONT, etc.
 * \param telnetOption the TELNET option: TELOPT_x
 */
void DescriptorPutCommand(
	Descriptor *d,
	const uint8_t telnetCommand,
	const uint8_t telnetOption) {
  if (!d) {
    Log(L_ASSERT, "Invalid `d` Descriptor.");
  } else if (DescriptorClosed(d)) {
    Log(L_ASSERT, "Descriptor %s is already closed.", d->name);
  } else if (sizeof(d->output) < d->outputN + 3) {
    Log(L_NETWORK, "Output overflow on descriptor %s.", d->name);
    DescriptorClose(d);
  } else {
    d->output[d->outputN++] = IAC;
    d->output[d->outputN++] = telnetCommand;
    d->output[d->outputN++] = telnetOption;
    Log(L_NETWORK, "Descriptor %s sent IAC %s %s.", d->name, TELCMD(telnetCommand), TELOPT(telnetOption));
  }
}

/*!
 * Sends the descriptor prompt.
 * \addtogroup descriptor
 * \param d the descriptor to which to send the descriptor prompt
 */
void DescriptorPutPrompt(Descriptor *d) {
  if (!d) {
    Log(L_ASSERT, "Invalid `d` Descriptor.");
  } else if (DescriptorClosed(d)) {
    Log(L_ASSERT, "Descriptor %s is already closed.", d->name);
  } else {
    DescriptorPrint(d, "%s:%sScratchMUD%s:> %s", Q_GRAY, Q_PINK, Q_GRAY, Q_NORMAL);
  }
}

/*! Descriptor helper function. */
static void DescriptorReceiveTelnetSubNegotiation(Descriptor *d) {
  if (!d) {
    Log(L_ASSERT, "Invalid `d` Descriptor.");
  } else if (DescriptorClosed(d)) {
    Log(L_ASSERT, "Descriptor %s is already closed.", d->name);
  } else {
    switch (d->telnetOption) {
    case TELOPT_NAWS:
      if (d->sbN != 4) {
	Log(L_NETWORK, "Descriptor %s received malformed NAWS subnegotiation.", d->name);
      } else {
	d->windowWidth  = ntohs(*((uint16_t*)(d->sb + 0)));
	d->windowHeight = ntohs(*((uint16_t*)(d->sb + 2)));
	Log(L_NETWORK, "Descriptor %s has window size %hu x %hu", d->name, d->windowWidth, d->windowHeight);
      }
      break;
    case TELOPT_TTYPE:
      Log(L_NETWORK, "Descriptor %s has terminal-type %s.", d->name, d->sbN ? d->sb : "<None>");
      break;
    default:
      Log(L_NETWORK, "Descriptor %s received unsupported %s subnegotiation.", d->name, TELOPT(d->telnetOption));
      break;
    }
  }
}

/*! Descriptor helper function. */
static void DescriptorReceiveTelnet(Descriptor *d) {
  if (!d) {
    Log(L_ASSERT, "Invalid `d` Descriptor.");
  } else if (DescriptorClosed(d)) {
    Log(L_ASSERT, "Descriptor %s is already closed.", d->name);
  } else {
    /* Some telnet commands have telnet options */
    if (d->telnetCommand != DO && d->telnetCommand != DONT &&
	d->telnetCommand != WILL && d->telnetCommand != WONT &&
	d->telnetCommand != SB) {
      Log(L_NETWORK, "Descriptor %s received IAC %s.", d->name,
	TELCMD(d->telnetCommand));
    } else {
      Log(L_NETWORK, "Descriptor %s received IAC %s %s.", d->name,
	TELCMD(d->telnetCommand),
	TELOPT(d->telnetOption));
    }

    /* Do telnet command */
    switch (d->telnetCommand) {
    case EC:
      if (d->inputN)
	d->input[--d->inputN] = '\0';
      break;
    case EL:
      MemoryZero(d->input, char, d->inputN);
      d->inputN = 0;
      break;
    case SB:
      d->bits.sb = true;
      MemoryZero(d->sb, char, d->sbN);
      d->sbN = 0;
      break;
    case SE:
      DescriptorReceiveTelnetSubNegotiation(d);
      d->bits.sb = false;
      break;
    default:
      /* Ignore telnet command */
      break;
    }
    d->telnetCommand = /* None */ 0;
  }
}

/*! Descriptor helper function. */
static void DescriptorReceiveInput(Descriptor *d) {
  if (!d) {
    Log(L_ASSERT, "Invalid `d` Descriptor.");
  } else if (DescriptorClosed(d)) {
    Log(L_ASSERT, "Descriptor %s is already closed.", d->name);
  } else {
    /* Send new-line in quiet mode */
    if (d->state && d->state->bits.quiet)
      DescriptorPrint(d, "\r\n");

    /* Handle received input */
    if (d->editor)
      EditorAdd(d, d->input);
    else if (d->state && d->state->received)
      d->state->received(d, d->game, d->input);

    d->bits.prompt = true;
  }
}

/*! Descriptor helper function. */
static void DescriptorReceiveInputByte(
	Descriptor *d,
	const uint8_t byteReceived) {
  if (!d) {
    Log(L_ASSERT, "Invalid `d` Descriptor.");
  } else if (DescriptorClosed(d)) {
    Log(L_ASSERT, "Descriptor %s is already closed.", d->name);
  } else if (strchr("\b\x7f", byteReceived) != NULL) {
    if (d->inputN)
      d->input[--d->inputN] = '\0';
  } else if (byteReceived == '\n') {
    DescriptorReceiveInput(d);
    MemoryZero(d->input, char, d->inputN);
    d->inputN = 0;
  } else if (d->inputN >= sizeof(d->input) - 1) {
    Log(L_NETWORK, "Input overflow on descriptor %s.", d->name);
    DescriptorClose(d);
  } else if (isprint((int) byteReceived) && byteReceived != '\r') {
    BPrintf(d->input, sizeof(d->input), d->inputN, "%c", byteReceived);
  }
}

/*! Descriptor helper function. */
static void DescriptorReceiveByte(
	Descriptor *d,
	const uint8_t byteReceived) {
  if (!d) {
    Log(L_ASSERT, "Invalid `d` Descriptor.");
  } else if (DescriptorClosed(d)) {
    Log(L_ASSERT, "Descriptor %s is already closed.", d->name);
  } else {
    switch (d->telnetCommand) {
    case /* None */ 0:
      if (byteReceived == IAC) {
	d->telnetCommand = IAC;
      } else if (d->bits.sb) {
	BPrintf(d->sb, sizeof(d->sb), d->sbN, "%c", byteReceived);
      } else {
	DescriptorReceiveInputByte(d, byteReceived);
      }
      break;
    case IAC:
      if (byteReceived == IAC) {
	if (d->bits.sb) {
	  BPrintf(d->sb, sizeof(d->sb), d->sbN, "%c", byteReceived);
	  d->telnetCommand = SB;
	} else {
	  DescriptorReceiveInputByte(d, byteReceived);
	  d->telnetCommand = /* None */ 0;
	}
      } else {
	d->telnetCommand = byteReceived;
	if (byteReceived != DO   && byteReceived != DONT && byteReceived != SB &&
	    byteReceived != WILL && byteReceived != WONT)
	  DescriptorReceiveTelnet(d);
      }
      break;
    case DO:
    case DONT:
    case SB:
    case WILL:
    case WONT:
      d->telnetOption = byteReceived;
      DescriptorReceiveTelnet(d);
      break;
    default:
      Log(L_NETWORK, "Descriptor %s has unknown state IAC %s.", d->name, TELCMD(d->telnetCommand));
      d->telnetCommand = /* None */ 0;
      break;
    }
  }
}

/*!
 * Reads and processes input.
 * \addtogroup descriptor
 * \param d the descriptor upon which to receive
 */
void DescriptorReceive(Descriptor *d) {
  if (!d) {
    Log(L_ASSERT, "Invalid `d` Descriptor.");
  } else if (DescriptorClosed(d)) {
    Log(L_ASSERT, "Descriptor %s is already closed.", d->name);
  } else {
    /* Try to read socket */
    uint8_t messg[MAXLEN_INPUT] = {'\0'};
    const ssize_t nBytes = SocketRead(d->socket, messg, sizeof(messg));

    /* Socket read error */
    if (nBytes < 0) {
      Log(L_NETWORK, "Couldn't read descriptor %s.", d->name);
      DescriptorClose(d);
    /* Remote host closed descriptor */
    } else if (!nBytes) {
      Log(L_NETWORK, "EOF read on descriptor %s.", d->name);
      DescriptorClose(d);
    } else {
      /* Telnet protocol */
      register size_t messgN = 0;
      for (; !DescriptorClosed(d) && messgN < (size_t) nBytes; ++messgN) {
	DescriptorReceiveByte(d, messg[messgN]);
      }
    }
  }
}

/*! Descriptor state function. */
STATE(PlayingOnReceived) {
  if (!StringCaseCompare("quit", input)) {
    DescriptorClose(d);
  } else {
    TreeForEach(game->descriptors, tDescNode) {
      Descriptor *tDesc = tDescNode->mappingValue;
      DescriptorPrint(tDesc, "%sFrom %s%s%s: %s%s%s\r\n",
		QX_PROMPT, QX_EMPHASIS, d->name, QX_PUNCTUATION,
		QX_PROMPT, input, Q_NORMAL);
    }
  }
  return (true);
}
