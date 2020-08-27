/*!
 * \file command.h
 *
 * \par Copyright
 * Copyright (C) 1999-2019 scratchmud.org
 * All rights reserved.
 *
 * \author Geoffrey Davis <gdavis@scratchmud.org>
 * \addtogroup command
 */
#ifndef _SCRATCH_COMMAND_H_
#define _SCRATCH_COMMAND_H_

#include <scratch/scratch.h>

/* Forward type declarations */
typedef struct _Command    Command;
typedef struct _Data       Data;
typedef struct _Game       Game;
typedef struct _Object     Object;
typedef struct _RBTree     RBTree;
typedef struct _SocialData SocialData;

/*!
 * Declares a command function.
 * \addtogroup command
 * \param name the name of the function to declare
 */
#define COMMAND(name) \
  void name(Object *actor, Command *command, const char *input)

/*! The type of a command function. */
typedef COMMAND((*CommandProc));

/*!
 * Command.
 * \addtogroup command
 * \{
 */
struct _Command {
  RBTree       *children;       /*!< The child command index */
  CommandProc   function;       /*!< The command function */
  char         *functionName;   /*!< The command function name */
  char        **keywordList;    /*!< The keyword list */
  size_t        keywordListN;   /*!< The length of the keyword list */
  char         *name;           /*!< The command name */
  Command      *parent;         /*!< The parent command or NULL */
  size_t        refcount;       /*!< The reference count */
  SocialData   *socialData;     /*!< The command social data */
  int           trust;          /*!< The minimum trust: TRUST_x */
};
/*! \} */

/*!
 * Command social special data.
 * \addtogroup command
 * \{
 */
struct _SocialData {
  char         *found;          /*!< The message when target is not you */
  char         *foundAuto;      /*!< The message when target is you */
  bool          hide;           /*!< The social is hidden */
  char         *noArgument;     /*!< The message when no argument specified */
};
/*! \} */

/*!
 * Constructs a new command.
 * \addtogroup command
 * \return the new command or NULL
 * \sa CommandFree(Command*)
 */
Command *CommandAlloc(void);

/*!
 * Retrieves a command.
 * \addtogroup command
 * \param game the game state
 * \param commandName the command name of the command to return
 * \return the command indicated by the specified command name or NULL
 */
Command *CommandByName(
	Game *game,
	const char *commandName);

/*!
 * Retrieves a child command.
 * \addtogroup command
 * \param parent the parent command
 * \param childName the command name of the child command to return
 * \return the child command indicated by the specified command name or NULL
 */
Command *CommandChildByName(
	const Command *parent,
	const char *childName);

/*!
 * Deletes a child command.
 * \addtogroup command
 * \param parent the parent command
 * \param childName the command name of the child command to delete
 * \return true if the child command indicated by the specified command
 *     name was successfully deleted
 */
bool CommandChildDelete(
	Command *parent,
	const char *childName);

/*!
 * Recreates the child command keyword index.
 * \addtogroup command
 * \param parent the command whose child commands to reindex
 * \return a copy of the specified child command or NULL
 */
void CommandChildReindexKeywords(Command *parent);

/*!
 * Stores a copy of a child command.
 * \addtogroup command
 * \param parent the parent command
 * \param child the child command to store
 * \return a copy of the specified child command or NULL
 */
Command *CommandChildStore(
	Command *parent,
	const Command *child);

/*!
 * Returns the size of a command in bytes.
 * \addtogroup command
 * \param command the command whose size to return
 * \return the size of the specified command in bytes
 */
size_t CommandCountBytes(const Command *command);

/*!
 * Copies a command.
 * \addtogroup command
 * \param toCommand the location of the copied command
 * \param fromCommand the command to copy
 */
void CommandCopy(
	Command *toCommand,
	const Command *fromCommand);

/*!
 * Deletes a command.
 * \addtogroup command
 * \param game the game state
 * \param commandName the command name of the command to delete
 * \return true if the command indicated by the specified command name
 *     was successfully deleted
 */
bool CommandDelete(
	Game *game,
	const char *commandName);

/*!
 * Emits a command.
 * \addtogroup command
 * \param toData the data to which to write
 * \param fromCommand the command to emit
 * \sa CommandParse(Data*, Command*)
 */
void CommandEmit(
	Data *toData,
	const Command *fromCommand);

/*!
 * Frees a command.
 * \addtogroup command
 * \param command the command to free
 * \sa CommandAlloc()
 */
void CommandFree(Command *command);

/*!
 * Generates a default function name.
 * \addtogroup command
 * \param out the output buffer
 * \param outlen the length of the specified buffer
 * \param command the command whose function name to generate
 * \return true if a default function name for the specified
 *     command was successfully generated
 */
bool CommandGetDefaultFunctionName(
	char *out, const size_t outlen,
	const Command *command);

/*!
 * Generates a command filename.
 * \addtogroup command
 * \param fname the command filename buffer
 * \param fnamelen the length of the specified buffer.
 * \param commandName the command name
 * \return true if a command filename was successfully generated
 */
bool CommandGetFileName(
	char *fname, const size_t fnamelen,
	const char *commandName);

/*!
 * Interprets command input.
 * \addtogroup command
 * \param actor the game object that performed the command
 * \param input the command input to interpret
 */
void CommandInterpret(
	Object *actor,
	const char *input);

/*!
 * Loads a command.
 * \addtogroup command
 * \param game the game state
 * \param commandName the command name of the command to load
 * \return true if the command indicated by the specified
 *     command name was successfully loaded
 * \sa CommandSave(Game*, Command*)
 */
bool CommandLoad(
	Game *game,
	const char *commandName);

/*!
 * Loads the command index.
 * \param game the game state
 * \addtogroup command
 * \sa CommandSaveIndex(Game*)
 */
void CommandLoadIndex(Game *game);

/*!
 * Parses a command.
 * \addtogroup command
 * \param fromData the data to parse
 * \param toCommand the location of the parsed command
 * \sa CommandEmit(Data*, const Command*)
 */
void CommandParse(
	Data *fromData,
	Command *toCommand);

/*!
 * Recreates the command keyword index.
 * \addtogroup command
 * \param game the game state
 */
void CommandReindexKeywords(Game *game);

/*!
 * Saves a command.
 * \addtogroup command
 * \param game the game state
 * \param command the command to save
 * \return true if the specified command was successfully saved
 * \sa CommandLoad(Game*, const char*)
 */
bool CommandSave(
	Game *game,
	Command *command);

/*!
 * Saves the command index.
 * \param game the game state
 * \addtogroup command
 * \sa CommandLoadIndex(Game*)
 */
void CommandSaveIndex(Game *game);

/*!
 * Stores a copy of a command.
 * \addtogroup command
 * \param game the game state
 * \param command the command to store
 * \return a copy of the specified command or NULL
 */
Command *CommandStore(
	Game *game,
	const Command *command);

#endif /* _SCRATCH_COMMAND_H_ */
