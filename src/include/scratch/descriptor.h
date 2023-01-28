/*!
 * \file descriptor.h
 *
 * \par Copyright
 * Copyright (C) 1999-2023 scratchmud.org
 * All rights reserved.
 *
 * \author Geoffrey Davis <gdavis@scratchmud.org>
 * \addtogroup descriptor
 */
#ifndef _SCRATCH_DESCRIPTOR_H_
#define _SCRATCH_DESCRIPTOR_H_

#include <scratch/scratch.h>

/*! The length of a descriptor output buffer. */
#define MAXLEN_OUTPUT		(1024 * 16)

/* Forward type declarations */
typedef struct Creator Creator;
typedef struct Descriptor Descriptor;
typedef struct DescriptorBits DescriptorBits;
typedef struct Editor Editor;
typedef struct Game Game;
typedef struct Socket Socket;
typedef struct State State;

/*!
 * The descriptor bitfield structure.
 * \addtogroup descriptor
 * \{
 */
struct DescriptorBits {
  uint8_t               color: 1;       /*!< Descriptor has color enabled */
  uint8_t               prompt: 1;      /*!< Descriptor needs prompt */
  uint8_t               sb: 1;          /*!< Descriptor received telnet SB */
};
/*! \} */

/*!
 * The descriptor structure.
 * \addtogroup descriptor
 * \{
 */
struct Descriptor {
  DescriptorBits        bits;           /*!< The descriptor bits */
  Creator              *creator;        /*!< The OLC state */
  Editor               *editor;         /*!< The string editor */
  Game                 *game;           /*!< The game state */
  char                 *hostname;       /*!< The remote name */
  char                  input[MAXLEN_INPUT];   /*!< The input buffer */
  size_t                inputN;         /*!< The input buffer used */
  uint16_t              lineLength;     /*!< The output line length */
  char                 *name;           /*!< The descriptor name */
  char                  output[MAXLEN_OUTPUT]; /*!< The output buffer */
  size_t                outputN;        /*!< The output buffer used */
  char                  sb[MAXLEN_INPUT];      /*!< The telnet SB input buffer */
  size_t                sbN;            /*!< The telnet SB input buffer used */
  Socket               *socket;         /*!< The descriptor socket */
  State                *state;          /*!< The state of connectedness */
  uint8_t               telnetCommand;  /*!< The telnet command: DO, DONT, etc. */
  uint8_t               telnetOption;   /*!< The telnet option: TELOPT_x */
  uint16_t              windowHeight;   /*!< The window height: default=25 */
  uint16_t              windowWidth;    /*!< The window width: default=80 */
};
/*! \} */

/*!
 * Constructs a new descriptor.
 * \addtogroup descriptor
 * \param game the game state
 * \return the new descriptor or NULL
 * \sa DescriptorFree(Descriptor*)
 * \sa DescriptorFreeV(void*)
 */
Descriptor *DescriptorAlloc(Game *game);

/*!
 * Searches for a descriptor.
 * \addtogroup descriptor
 * \param game the game state
 * \param descriptorName the descriptor name of the descriptor to return
 * \return the descriptor indicated by the specified descriptor name or NULL
 */
Descriptor *DescriptorByName(
	Game *game,
	const char *descriptorName);

/*!
 * Closes a descriptor.
 * \addtogroup descriptor
 * \param d the descriptor to close
 * \sa DescriptorClosed(const Descriptor*)
 */
void DescriptorClose(Descriptor *d);

/*!
 * Returns whether a descriptor is closed.
 * \addtogroup descriptor
 * \param d the descriptor whose closed state to return
 * \return true if the specified descriptor is closed
 * \sa DescriptorClose(Descriptor*)
 */
bool DescriptorClosed(const Descriptor *d);

/*!
 * Flushes buffered output.
 * \addtogroup descriptor
 * \param d the descriptor to flush
 */
void DescriptorFlush(Descriptor *d);

/*!
 * Frees a descriptor.
 * \addtogroup descriptor
 * \param d the descriptor to free
 * \sa DescriptorAlloc(Game*)
 * \sa DescriptorFreeV(void*)
 */
void DescriptorFree(Descriptor *d);

/*!
 * Frees a descriptor.
 * \addtogroup descriptor
 * \param d the descriptor to free
 * \sa DescriptorAlloc(Game*)
 * \sa DescriptorFree(Descriptor*)
 */
void DescriptorFreeV(void *d);

/*!
 * Prints a message to a descriptor.
 * \addtogroup descriptor
 * \param d the descriptor to which to print
 * \param format the printf-style format specifier
 */
void DescriptorPrint(
	Descriptor *d,
	const char *format, ...)
	__attribute__ ((format (printf, 2, 3)));

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
	const uint8_t telnetOption);

/*!
 * Sends the descriptor prompt.
 * \addtogroup descriptor
 * \param d the descriptor to which to send the descriptor prompt
 */
void DescriptorPutPrompt(Descriptor *d);

/*!
 * Reads and processes input.
 * \addtogroup descriptor
 * \param d the descriptor upon which to receive
 */
void DescriptorReceive(Descriptor *d);

#endif /* _SCRATCH_DESCRIPTOR_H_ */
