/*!
 * \file command.c
 *
 * \par Copyright
 * Copyright (C) 1999-2019 scratchmud.org
 * All rights reserved.
 *
 * \author Geoffrey Davis <gdavis@scratchmud.org>
 * \addtogroup command
 */
#define _SCRATCH_COMMAND_C_

#include <scratch/action.h>
#include <scratch/color.h>
#include <scratch/command.h>
#include <scratch/constant.h>
#include <scratch/data.h>
#include <scratch/game.h>
#include <scratch/list.h>
#include <scratch/log.h>
#include <scratch/memory.h>
#include <scratch/object.h>
#include <scratch/player.h>
#include <scratch/rbtree.h>
#include <scratch/scratch.h>
#include <scratch/string.h>
#include <scratch/utility.h>

/*!
 * Constructs a new command.
 * \addtogroup command
 * \return the new command or NULL
 * \sa CommandFree(Command*)
 */
Command *CommandAlloc(void) {
  register Command *command = NULL;
  MemoryCreate(command, Command, 1);
  command->children = RBTreeAlloc(
	(RBTreeCompareProc) UtilityNameCmp,
	(RBTreeFreeProc)    CommandFree);
  command->function = NULL;
  command->functionName = NULL;
  command->keywordList = NULL;
  command->keywordListN = 0;
  command->name = NULL;
  command->refcount = 1;
  command->trust = TRUST_NONE;
  return (command);
}

/*!
 * Retrieves a command.
 * \addtogroup command
 * \param game the game state
 * \param commandName the command name of the command to return
 * \return the command indicated by the specified command name or NULL
 */
Command *CommandByName(
	Game *game,
	const char *commandName) {
  register Command *command = NULL;
  if (!game) {
    Log("invalid `game` Game");
  } else if (*StringBlank(commandName) == '\0') {
    Log("invalid `commandName` string");
  } else {
    command = RBTreeGetValue(game->commands, (RBTreeKey) &commandName, NULL);
  }
  return (command);
}

/*!
 * Retrieves a child command.
 * \addtogroup command
 * \param parent the parent command
 * \param childName the command name of the child command to return
 * \return the child command indicated by the specified command name or NULL
 */
Command *CommandChildByName(
	const Command *parent,
	const char *childName) {
  register Command *child = NULL;
  if (!parent) {
    Log("invalid `parent` Command");
  } else if (*StringBlank(childName) == '\0') {
    Log("invalid `childName` string");
  } else {
    child = RBTreeGetValue(parent->children, (RBTreeKey) &childName, NULL);
  }
  return (child);
}

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
	const char *childName) {
  register bool result = false;
  if (!parent) {
    Log("invalid `parent` Command");
  } else {
    result = RBTreeDelete(parent->children, (RBTreeKey) &childName);
  }
  return (result);
}

/*!
 * Recreates the child command keyword index.
 * \addtogroup command
 * \param parent the command whose child commands to reindex
 * \return a copy of the specified child command or NULL
 */
void CommandChildReindexKeywords(Command *parent) {
  if (!parent) {
    Log("invalid `parent` Command");
  } else {
    /* Temporary command list */
    List *commands = ListAlloc(
	(ListCompareProc)	NULL,
	(ListFreeProc)		CommandFree);

    /* Build temporary command list */
    RBTreeForEach(parent->children, tChildNode) {
      /* Iterator variable */
      Command *tChild = tChildNode->value;
      char **tChildKey = tChildNode->key;

      /* Check for command names and keywords */
      if (StringCmpCI(tChild->name, *tChildKey) == 0) {
	ListPushBack(commands, tChild);
	tChild->refcount++;
      }
    }
    RBTreeForEachEnd();

    /* Clear the keyword index */
    RBTreeClear(parent->children);

    /* Add child keywords back to child index */
    ListForEach(commands, tChildNode) {
      /* Iterator variable */
      Command *tChild = tChildNode->value;

      /* Index child command keywords */
      if (tChild->keywordListN) {
	register size_t keywordN = 0;
	for (; keywordN < tChild->keywordListN; ++keywordN) {
	  RBTreeInsert(parent->children, &tChild->keywordList[keywordN], tChild);
	  tChild->refcount++;
	}
      }
    }
    ListForEachEnd();

    /* Add child names back to child index */
    ListForEach(commands, tChildNode) {
      /* Iterator variable */
      Command *tChild = tChildNode->value;

      /* Index child command name last */
      RBTreeInsert(parent->children, &tChild->name, tChild);
      tChild->refcount++;
    }
    ListForEachEnd();

    /* Cleanup */
    ListFree(commands);
  }
}

/*!
 * Stores a copy of a child command.
 * \addtogroup command
 * \param parent the parent command
 * \param child the child command to store
 * \return a copy of the specified child command or NULL
 */
Command *CommandChildStore(
	Command *parent,
	const Command *child) {
  register Command *copied = NULL;
  if (!parent) {
    Log("invalid `parent` Command");
  } else if (!child) {
    Log("invalid `child` Command");
  } else {
    copied = CommandChildByName(parent, child->name);
    if (!copied) {
      copied = CommandAlloc();
      copied->parent = parent;
      StringSet(&copied->name, child->name);
      if (!RBTreeInsert(parent->children, (RBTreeKey) &copied->name, copied)) {
	Log("Couldn't add child command `%s` to command `%s`", copied->name, parent->name);
	CommandFree(copied), copied = NULL;
      }
    }
    if (copied) {
      CommandCopy(copied, child);
      CommandChildReindexKeywords(parent);
    }
  }
  return (copied);
}

/*!
 * Returns the size of a command in bytes.
 * \addtogroup command
 * \param command the command whose size to return
 * \return the size of the specified command in bytes
 */
size_t CommandCountBytes(const Command *command) {
  register size_t nBytes = 0;
  if (command) {
    nBytes += sizeof(Command);
    if (command->children) {
      nBytes += sizeof(RBTree);
      RBTreeForEach(command->children, tChildNode) {
	/* Iterator variable */
	const Command *tChild = tChildNode->value;

	/* Update byte count */
	nBytes += sizeof(RBTreeNode);
	nBytes += CommandCountBytes(tChild);
      }
      RBTreeForEachEnd();
    }
    if (command->functionName)
      nBytes += strlen(command->functionName) + 1;
    if (command->keywordList) {
      register size_t keywordN;
      for (keywordN = 0; keywordN < command->keywordListN; ++keywordN) {
	if (command->keywordList[keywordN])
	  nBytes += strlen(command->keywordList[keywordN]) + 1;
      }
      nBytes += sizeof(char*) * command->keywordListN;
    }
    if (command->name)
      nBytes += strlen(command->name) + 1;
    if (command->socialData) {
      nBytes += sizeof(SocialData);
      if (command->socialData->found)
	nBytes += strlen(command->socialData->found) + 1;
      if (command->socialData->foundAuto)
	nBytes += strlen(command->socialData->foundAuto) + 1;
      if (command->socialData->noArgument)
	nBytes += strlen(command->socialData->noArgument) + 1;
    }
  }
  return (nBytes);
}

/*!
 * Copies a command.
 * \addtogroup command
 * \param toCommand the location of the copied command
 * \param fromCommand the command to copy
 */
static void CommandCopyChildList(
	Command *toCommand,
	const Command *fromCommand) {
  if (!toCommand) {
    Log("invalid `toCommand` Command");
  } else if (!fromCommand) {
    Log("invalid `fromCommand` Command");
  } else if (toCommand != fromCommand) {
    /* Copy child commands to destination */
    RBTreeForEach(fromCommand->children, tChildNode) {
      /* Iterator variable */
      const Command *tChild = tChildNode->value;

      /* Store child command */
      CommandChildStore(toCommand, tChild);
    }
    RBTreeForEachEnd();

    /* Search for deleted child commands */
    List *deleted = ListAlloc(NULL, NULL);
    RBTreeForEach(toCommand->children, tChildNode) {
      /* Iterator variable */
      Command *tChild = tChildNode->value;

      /* Check whether the child command was removed */
      if (CommandChildByName(fromCommand, tChild->name) == NULL)
	ListPushBack(deleted, tChild);
    }
    RBTreeForEachEnd();

    /* Remove deleted child commands */
    ListForEach(deleted, tChildNode) {
      /* Iterator variable */
      Command *tChild = tChildNode->value;

      /* Remove the child command */
      CommandChildDelete(toCommand, tChild->name);
    }
    ListForEachEnd();
    ListFree(deleted);
  }
}

/*!
 * Copies a command.
 * \addtogroup command
 * \param toCommand the location of the copied command
 * \param fromCommand the command to copy
 */
static void CommandCopyFunction(
	Command *toCommand,
	const Command *fromCommand) {
  if (!toCommand) {
    Log("invalid `toCommand` Command");
  } else if (!fromCommand) {
    Log("invalid `fromCommand` Command");
  } else if (toCommand != fromCommand) {
    StringSet(&toCommand->functionName, fromCommand->functionName);
    toCommand->function = fromCommand->function;
  }
}

/*!
 * Copies a command.
 * \addtogroup command
 * \param toCommand the location of the copied command
 * \param fromCommand the command to copy
 */
static void CommandCopyKeywordList(
	Command *toCommand,
	const Command *fromCommand) {
  if (!toCommand) {
    Log("invalid `toCommand` Command");
  } else if (!fromCommand) {
    Log("invalid `fromCommand` Command");
  } else if (toCommand != fromCommand) {
    /* Resize if we need more command keywords */
    if (toCommand->keywordListN < fromCommand->keywordListN) {
      MemoryRecreate(toCommand->keywordList, char*, fromCommand->keywordListN);
      while (toCommand->keywordListN < fromCommand->keywordListN)
	toCommand->keywordList[toCommand->keywordListN++] = NULL;
    }
    /* Resize if we need fewer command keywords */
    if (toCommand->keywordListN > fromCommand->keywordListN) {
      while (toCommand->keywordListN > fromCommand->keywordListN)
	StringFree(toCommand->keywordList[--toCommand->keywordListN]);
      MemoryRecreate(toCommand->keywordList, char*, toCommand->keywordListN);
    }
    /* Copy command keywords */
    register size_t keywordN;
    for (keywordN = 0; keywordN < toCommand->keywordListN; ++keywordN) {
      StringSet(&toCommand->keywordList[keywordN], fromCommand->keywordList[keywordN]);
    }
  }
}

/*!
 * Copies a command.
 * \addtogroup command
 * \param toCommand the location of the copied command
 * \param fromCommand the command to copy
 */
static void CommandCopyName(
	Command *toCommand,
	const Command *fromCommand) {
  if (!toCommand) {
    Log("invalid `toCommand` Command");
  } else if (!fromCommand) {
    Log("invalid `fromCommand` Command");
  } else if (toCommand != fromCommand) {
    StringSet(&toCommand->name, fromCommand->name);
  }
}

/*!
 * Copies a command.
 * \addtogroup command
 * \param toCommand the location of the copied command
 * \param fromCommand the command to copy
 */
static void CommandCopySocialData(
	Command *toCommand,
	const Command *fromCommand) {
  if (!toCommand) {
    Log("invalid `toCommand` Command");
  } else if (!fromCommand) {
    Log("invalid `fromCommand` Command");
  } else if (toCommand != fromCommand) {
    /* If needed, free social special data */
    if (toCommand->socialData && !fromCommand->socialData) {
      StringFree(toCommand->socialData->found);
      StringFree(toCommand->socialData->foundAuto);
      StringFree(toCommand->socialData->noArgument);
      MemoryFree(toCommand->socialData);
    }
    /* If needed, create social special data */
    if (!toCommand->socialData && fromCommand->socialData) {
      MemoryCreate(toCommand->socialData, SocialData, 1);
      toCommand->socialData->hide = false;
      toCommand->socialData->found = NULL;
      toCommand->socialData->foundAuto = NULL;
      toCommand->socialData->noArgument = NULL;
    }
    /* Copy social special data */
    if (toCommand->socialData && fromCommand->socialData) {
      StringSet(&toCommand->socialData->found, fromCommand->socialData->found);
      StringSet(&toCommand->socialData->foundAuto, fromCommand->socialData->foundAuto);
      StringSet(&toCommand->socialData->noArgument, fromCommand->socialData->noArgument);
      toCommand->socialData->hide = fromCommand->socialData->hide;
    }
  }
}

/*!
 * Copies a command.
 * \addtogroup command
 * \param toCommand the location of the copied command
 * \param fromCommand the command to copy
 */
static void CommandCopyTrust(
	Command *toCommand,
	const Command *fromCommand) {
  if (!toCommand) {
    Log("invalid `toCommand` Command");
  } else if (!fromCommand) {
    Log("invalid `fromCommand` Command");
  } else if (toCommand != fromCommand) {
    toCommand->trust = fromCommand->trust;
  }
}

/*!
 * Copies a command.
 * \addtogroup command
 * \param toCommand the location of the copied command
 * \param fromCommand the command to copy
 */
void CommandCopy(
	Command *toCommand,
	const Command *fromCommand) {
  if (!toCommand) {
    Log("invalid `toCommand` Command");
  } else if (!fromCommand) {
    Log("invalid `fromCommand` Command");
  } else if (toCommand != fromCommand) {
    CommandCopyChildList(toCommand, fromCommand);
    CommandCopyFunction(toCommand, fromCommand);
    CommandCopyKeywordList(toCommand, fromCommand);
    CommandCopyName(toCommand, fromCommand);
    CommandCopySocialData(toCommand, fromCommand);
    CommandCopyTrust(toCommand, fromCommand);
    CommandChildReindexKeywords(toCommand);
  }
}

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
	const char *commandName) {
  register bool result = false;
  if (!game) {
    Log("invalid `game` Game");
  } else if (*StringBlank(commandName) == '\0') {
    Log("invalid `commandName` string");
  } else {
    /* Remove command index */
    result = RBTreeDelete(game->commands, (RBTreeKey) &commandName);

    /* Remove command file */
    char fname[PATH_MAX] = {'\0'};
    if (!CommandGetFileName(fname, sizeof(fname), commandName)) {
      Log("Couldn't create filename for command `%s`", commandName);
    } else if (unlink(fname) < 0 && errno != ENOENT) {
      Log("unlink() failed: fname=%s, errno=%d", fname, errno);
    }
  }
  return (result);
}

/*!
 * Emits a command.
 * \addtogroup command
 * \param toData the data to which to write
 * \param fromCommand the command to emit
 */
static void CommandEmitChildList(
	Data *toData,
	const Command *fromCommand) {
  if (!toData) {
    Log("invalid `toData` Data");
  } else if (!fromCommand) {
    Log("invalid `fromCommand` Command");
  } else {
    Data *xChildList = DataAlloc();
    RBTreeForEach(fromCommand->children, tChildNode) {
      /* Iterator variable */
      const Command *tChild = tChildNode->value;

      /* Emit child command information */
      CommandEmit(xChildList, tChild);
    }
    RBTreeForEachEnd();

    if (DataSize(xChildList) == 0) {
      DataFree(xChildList);
    } else {
      DataPut(toData, "Children", xChildList);
      DataSort(xChildList);
    }
  }
}

/*!
 * Emits a command.
 * \addtogroup command
 * \param toData the data to which to write
 * \param fromCommand the command to emit
 */
static void CommandEmitFunction(
	Data *toData,
	const Command *fromCommand) {
  if (!toData) {
    Log("invalid `toData` Data");
  } else if (!fromCommand) {
    Log("invalid `fromCommand` Command");
  } else {
    /* Default command function name */
    char procName[MAXLEN_INPUT] = {'\0'};
    CommandGetDefaultFunctionName(procName, sizeof(procName), fromCommand);

    if (*StringBlank(fromCommand->functionName) != '\0' &&
	 StringCmp(fromCommand->functionName, procName) != 0)
      DataPutString(toData, "Function", fromCommand->functionName);
  }
}

/*!
 * Emits a command.
 * \addtogroup command
 * \param toData the data to which to write
 * \param fromCommand the command to emit
 */
static void CommandEmitKeywordList(
	Data *toData,
	const Command *fromCommand) {
  if (!toData) {
    Log("invalid `toData` Data");
  } else if (!fromCommand) {
    Log("invalid `fromCommand` Command");
  } else {
    Data *xKeywordList = DataAlloc();

    register size_t keywordN;
    for (keywordN = 0; keywordN < fromCommand->keywordListN; ++keywordN) {
      DataPutString(xKeywordList, "%", fromCommand->keywordList[keywordN]);
    }
    if (DataSize(xKeywordList) == 0) {
      DataFree(xKeywordList);
    } else {
      DataPut(toData, "Keywords", xKeywordList);
    }
  }
}

/*!
 * Emits a command.
 * \addtogroup command
 * \param toData the data to which to write
 * \param fromCommand the command to emit
 */
static void CommandEmitName(
	Data *toData,
	const Command *fromCommand) {
  if (!toData) {
    Log("invalid `toData` Data");
  } else if (!fromCommand) {
    Log("invalid `fromCommand` Command");
  } else {
    if (*StringBlank(fromCommand->name) != '\0')
      DataPutString(toData, "Name", fromCommand->name);
  }
}

/*!
 * Emits a command.
 * \addtogroup command
 * \param toData the data to which to write
 * \param fromCommand the command to emit
 */
static void CommandEmitSocialData(
	Data *toData,
	const Command *fromCommand) {
  if (!toData) {
    Log("invalid `toData` Data");
  } else if (!fromCommand) {
    Log("invalid `fromCommand` Command");
  } else if (fromCommand->socialData) {
    Data *xSocialData = DataAlloc();
    if (*StringBlank(fromCommand->socialData->found) != '\0')
      DataPutString(xSocialData, "Found", fromCommand->socialData->found);
    if (*StringBlank(fromCommand->socialData->foundAuto) != '\0')
      DataPutString(xSocialData, "FoundAuto", fromCommand->socialData->foundAuto);
    if (*StringBlank(fromCommand->socialData->noArgument) != '\0')
      DataPutString(xSocialData, "NoArgument", fromCommand->socialData->noArgument);
    if (fromCommand->socialData->hide)
      DataPutYesNo(xSocialData, "Hide", fromCommand->socialData->hide);

    if (DataSize(xSocialData) == 0) {
      DataFree(xSocialData);
    } else {
      DataPut(toData, "SocialData", xSocialData);
      DataSort(xSocialData);
    }
  }
}

/*!
 * Emits a command.
 * \addtogroup command
 * \param toData the data to which to write
 * \param fromCommand the command to emit
 */
static void CommandEmitTrust(
	Data *toData,
	const Command *fromCommand) {
  if (!toData) {
    Log("invalid `toData` Data");
  } else if (!fromCommand) {
    Log("invalid `fromCommand` Command");
  } else {
    if (fromCommand->trust != TRUST_NONE)
      DataPutType(toData, "Trust", _G_trustTypes, fromCommand->trust);
  }
}

/*!
 * Emits a command.
 * \addtogroup command
 * \param toData the data to which to write
 * \param fromCommand the command to emit
 * \sa CommandParse(Data*, Command*)
 */
void CommandEmit(
	Data *toData,
	const Command *fromCommand) {
  if (!toData) {
    Log("invalid `toData` Data");
  } else if (!fromCommand) {
    Log("invalid `fromCommand` Command");
  } else {
    Data *xCommand = DataAlloc();
    CommandEmitChildList(xCommand, fromCommand);
    CommandEmitFunction(xCommand, fromCommand);
    CommandEmitKeywordList(xCommand, fromCommand);
    if (!fromCommand->parent)
      CommandEmitName(xCommand, fromCommand);
    CommandEmitSocialData(xCommand, fromCommand);
    CommandEmitTrust(xCommand, fromCommand);

    if (DataSize(xCommand) == 0) {
      if (fromCommand->parent)
	DataPutString(toData, fromCommand->name, "");
      DataFree(xCommand);
    } else {
      DataPut(toData, fromCommand->parent ? fromCommand->name : "Command", xCommand);
      DataSort(xCommand);
    }
  }
}

/*!
 * Frees a command.
 * \addtogroup command
 * \param command the command to free
 * \sa CommandAlloc()
 */
void CommandFree(Command *command) {
  if (command && command->refcount)
    command->refcount--;
  if (command && !command->refcount) {
    RBTreeFree(command->children);
    StringFree(command->functionName);
    StringFree(command->name);
    if (command->keywordList) {
      register size_t keywordN;
      for (keywordN = 0; keywordN < command->keywordListN; ++keywordN)
	StringFree(command->keywordList[keywordN]);
    }
    MemoryFree(command->keywordList);
    if (command->socialData) {
      StringFree(command->socialData->found);
      StringFree(command->socialData->foundAuto);
      StringFree(command->socialData->noArgument);
    }
    MemoryFree(command->socialData);
    MemoryFree(command);
  }
}

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
	const Command *command) {
  register bool result = false;
  if (!out && outlen) {
    Log("invalid `out` buffer");
  } else if (!command) {
    Log("invalid `command` Command");
  } else {
    register size_t outpos = 0;
    BPrintf(out, outlen, outpos, "Act");
    if (command->socialData) {
      BPrintf(out, outlen, outpos, "Social");
      result =  (outpos == 9);
    } else {
      /* Keep track of command length */
      register size_t expected = 3;

      /* Parent command queue */
      List *queue = ListAlloc(NULL, NULL);

      /* Push parent commands */
      register const Command *parent;
      for (parent = command; parent; parent = parent->parent) {
	if (*StringBlank(parent->name) != '\0')
	  expected += strlen(parent->name);
	ListPushFront(queue, (ListValue) parent);
      }

      /* Build the default command function name */
      ListForEach(queue, tCommandNode) {
	/* Iterator variable */
	const Command *tCommand = tCommandNode->value;

	/* Append default command function name */
	if (*StringBlank(tCommand->name) != '\0') {
	  BPrintf(out, outlen, outpos, "%s", tCommand->name);
	} else {
	  BPrintf(out, outlen, outpos, "Blank");
	}
      }
      ListForEachEnd();

      /* Cleanup */
      ListFree(queue);

      /* Make sure we got what we expected */
      result = (outpos == expected);
    }
    if (!result)
      MemoryZero(out, char, outlen);
  }
  return (result);
}

#define COMMAND_DIRECTORY	"data/command"
#define COMMAND_FILE_EXTENSION	"dat"

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
	const char *commandName) {
  register bool result = false;
  if (!fname && fnamelen) {
    Log("invalid `fname` buffer");
  } else if (*StringBlank(commandName) == '\0') {
    Log("invalid `commandName` string");
  } else {
    result = UtilityGetFileName(
	fname, fnamelen,
	COMMAND_DIRECTORY,
	COMMAND_FILE_EXTENSION,
	commandName);
  }
  return (result);
}

/*!
 * Interprets command input.
 * \addtogroup command
 * \param actor the game object that performed the command
 * \param input the command input to interpret
 */
void CommandInterpret(
	Object *actor,
	const char *input) {
  if (!actor) {
    Log("invalid `actor` Object");
  } else if (*StringBlank(input) != '\0') {
    /* Command line buffers */
    char argument[MAXLEN_INPUT] = {'\0'};
    char name[MAXLEN_INPUT] = {'\0'};

    Command *command = NULL;
    while (true) {
      /* Read command name */
      const char *ptr = StringOneWord(name, sizeof(name), input);

      /* Resolve command keyword */
      register Command *maybeCommand = NULL;
      RBTreeForEach(command ? command->children : actor->game->commands, tCommandNode) {
	/* Iterator variable */
	Command *tCommand = tCommandNode->value;
	char **tCommandKey = tCommandNode->key;

	/* Check player trust */
	if (GetPlayerTrust(actor) < tCommand->trust)
	  continue;

	/* Search command keywords */
	if (*name == '\0' && StringCmp(*tCommandKey, name) == 0)
	  maybeCommand = tCommand; /* ? */
	else if (*name != '\0' && StringPrefixCI(*tCommandKey, name))
	  maybeCommand = tCommand; /* ? */

	if (maybeCommand)
	  break;
      }
      RBTreeForEachEnd();

      /* We found the command! */
      if (!maybeCommand)
	break;

      /* Advance */
      command = maybeCommand;
      input = ptr;
    }

    /* Might have meant to say the input instead*/
    if (BitCheckN(GetPlayerPreferenceBits(actor), PREF_AUTOSAY)) {
      if (!command && (command = CommandByName(actor->game, "say")) != NULL) {
	strlcpy(argument, StringSkipSpaces(input), sizeof(argument));
	strlcpy(name, command->name, sizeof(name));
      }
    }

    if (!command) {
      Act(CX_FAILED, TO_ACTOR, "There is no such command.", actor, NULL, NULL);
    } else if (GetPlayerTrust(actor) < command->trust) {
      Act(CX_FAILED, TO_ACTOR, "You can't use that command.", actor, NULL, NULL);
    } else if (!command->function) {
      Act(CX_FAILED, TO_ACTOR, "That command isn't implemented.", actor, NULL, NULL);
    } else {
      strlcpy(argument, StringSkipSpaces(input), sizeof(argument));
      command->function(actor, command, argument);
    }
  }
}

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
	const char *commandName) {
  register bool result = false;
  if (!game) {
    Log("invalid `game` Game");
  } else if (*StringBlank(commandName) == '\0') {
    Log("invalid `commandName` string");
  } else {
    char fname[PATH_MAX] = {'\0'};
    if (!CommandGetFileName(fname, sizeof(fname), commandName)) {
      Log("Couldn't create filename for command `%s`", commandName);
    } else {
      Data *fromData = DataLoadFile(fname);
      if (!fromData) {
	Log("Couldn't load command file `%s`", fname);
      } else {
	/* Parse command data */
	Command *command = CommandAlloc();
	CommandParse(fromData, command);

	/* Store command */
	result = CommandStore(game, command) != NULL;

	/* Cleanup */
	CommandFree(command);
	DataFree(fromData);
      }
    }
  }
  return (result);
}

#define COMMAND_INDEX_FILE	"data/command/index"

/*!
 * Loads the command index.
 * \param game the game state
 * \addtogroup command
 * \sa CommandSaveIndex(Game*)
 */
void CommandLoadIndex(Game *game) {
  if (!game) {
    Log("invalid `game` Game");
  } else {
    FILE *stream = fopen(COMMAND_INDEX_FILE, "rt");
    if (!stream) {
      Log("Couldn't open command index file `%s` for reading", COMMAND_INDEX_FILE);
    } else {
      /* Read lines from the file */
      char line[MAXLEN_INPUT] = {'\0'};
      while (fgets(line, sizeof(line), stream) != NULL) {
	/* Load command */
	StringNormalize(line);
	CommandLoad(game, line);
      }
      fclose(stream);
    }
    /* Reindex keywords */
    CommandReindexKeywords(game);

    /* Calculate some metrics */
    register size_t nCommands = 0;
    register size_t nCommandBytes = 0;
    RBTreeForEach(game->commands, tCommandNode) {
      /* Iterator variable */
      const Command *tCommand = tCommandNode->value;

      /* Update byte and record counts */
      nCommandBytes += CommandCountBytes(tCommand);
      nCommands++;
    }
    RBTreeForEachEnd();
    Log("Loaded %zu command(s), %zu byte(s).", nCommands, nCommandBytes);
  }
}

/*!
 * Parses a command.
 * \addtogroup command
 * \param fromData the data to parse
 * \param toCommand the location of the parsed command
 */
static void CommandParseChildList(
	Data *fromData,
	Command *toCommand) {
  if (!fromData) {
    Log("invalid `fromData` Data");
  } else if (!toCommand) {
    Log("invalid `toCommand` Command");
  } else {
    Data *xChildList = DataGet(fromData, "Children");
    DataForEach(xChildList, xChildName, xChild) {
      Command *toChildCommand = CommandAlloc();
      toChildCommand->name = strdup(xChildName);
      toChildCommand->parent = toCommand;
      CommandParse(xChild, toChildCommand);
      if (*xChildName != '\0' && !UtilityNameValid(xChildName)) {
	Log("Invalid child command name `%s` for command `%s`", xChildName, toCommand->name);
      } else if (!CommandChildStore(toCommand, toChildCommand)) {
	Log("Couldn't add child command `%s` to command `%s`", xChildName, toCommand->name);
      }
      CommandFree(toChildCommand);
    }
    DataForEachEnd();
  }
}


/*!
 * Parses a command.
 * \addtogroup command
 * \param fromData the data to parse
 * \param toCommand the location of the parsed command
 */
static void CommandParseFunction(
	Data *fromData,
	Command *toCommand) {
  if (!fromData) {
    Log("invalid `fromData` Data");
  } else if (!toCommand) {
    Log("invalid `toCommand` Command");
  } else {
    /* Default command function name */
    char procName[MAXLEN_INPUT] = {'\0'};
    CommandGetDefaultFunctionName(procName, sizeof(procName), toCommand);

    /* Command function */
    StringFree(toCommand->functionName);
    toCommand->functionName = DataGetStringCopy(fromData, "Function", NULL);

    /* Resolve command function */
    toCommand->function = NULL;
    if (*StringBlank(toCommand->functionName) != '\0') {
      toCommand->function = UtilityProcByName(toCommand->functionName);
    } else if ((toCommand->function = UtilityProcByName(procName)) != NULL) {
      StringSet(&toCommand->functionName, procName);
    }
  }
}

/*!
 * Parses a command.
 * \addtogroup command
 * \param fromData the data to parse
 * \param toCommand the location of the parsed command
 */
static void CommandParseKeywordList(
	Data *fromData,
	Command *toCommand) {
  if (!fromData) {
    Log("invalid `fromData` Data");
  } else if (!toCommand) {
    Log("invalid `toCommand` Command");
  } else {
    if (toCommand->keywordList) {
      register size_t keywordN;
      for (keywordN = 0; keywordN < toCommand->keywordListN; ++keywordN) {
	StringFree(toCommand->keywordList[keywordN]);
      }
      MemoryFree(toCommand->keywordList);
      toCommand->keywordListN = 0;
    }
    Data *xKeywordList = DataGet(fromData, "Keywords");
    DataForEach(xKeywordList, xKeywordKey, xKeyword) {
      MemoryRecreate(toCommand->keywordList, char*, toCommand->keywordListN + 1);
      toCommand->keywordList[toCommand->keywordListN] = DataGetStringCopy(xKeyword, "", "");
      toCommand->keywordListN++;
    }
    DataForEachEnd();
  }
}

/*!
 * Parses a command.
 * \addtogroup command
 * \param fromData the data to parse
 * \param toCommand the location of the parsed command
 */
static void CommandParseName(
	Data *fromData,
	Command *toCommand) {
  if (!fromData) {
    Log("invalid `fromData` Data");
  } else if (!toCommand) {
    Log("invalid `toCommand` Command");
  } else if (DataGet(fromData, "Name") != NULL) {
    StringFree(toCommand->name);
    toCommand->name = DataGetStringCopy(fromData, "Name", NULL);
  }
}

/*!
 * Parses a command.
 * \addtogroup command
 * \param fromData the data to parse
 * \param toCommand the location of the parsed command
 */
static void CommandParseSocialData(
	Data *fromData,
	Command *toCommand) {
  if (!fromData) {
    Log("invalid `fromData` Data");
  } else if (!toCommand) {
    Log("invalid `toCommand` Command");
  } else {
    /* Check for social special data */
    Data *xSocialData = DataGet(fromData, "SocialData");

    /* Create social special data */
    if (!toCommand->socialData && xSocialData) {
      MemoryCreate(toCommand->socialData, SocialData, 1);
      toCommand->socialData->hide = false;
      toCommand->socialData->found = NULL;
      toCommand->socialData->foundAuto = NULL;
      toCommand->socialData->noArgument = NULL;
    }

    /* Free social special data */
    if (toCommand->socialData && !xSocialData) {
      StringFree(toCommand->socialData->found);
      StringFree(toCommand->socialData->foundAuto);
      StringFree(toCommand->socialData->noArgument);
      MemoryFree(toCommand->socialData);
    }

    if (toCommand->socialData) {
      /* Found message */
      StringFree(toCommand->socialData->found);
      toCommand->socialData->found = DataGetStringCopy(xSocialData, "Found", NULL);

      /* Found auto message */
      StringFree(toCommand->socialData->foundAuto);
      toCommand->socialData->foundAuto = DataGetStringCopy(xSocialData, "FoundAuto", NULL);

      /* Hide */
      toCommand->socialData->hide = DataGetYesNo(xSocialData, "Hide", false);

      /* No argument message */
      StringFree(toCommand->socialData->noArgument);
      toCommand->socialData->noArgument = DataGetStringCopy(xSocialData, "NoArgument", NULL);
    }
  }
}

/*!
 * Parses a command.
 * \addtogroup command
 * \param fromData the data to parse
 * \param toCommand the location of the parsed command
 */
static void CommandParseTrust(
	Data *fromData,
	Command *toCommand) {
  if (!fromData) {
    Log("invalid `fromData` Data");
  } else if (!toCommand) {
    Log("invalid `toCommand` Command");
  } else {
    toCommand->trust = DataGetType(fromData, "Trust", _G_trustTypes, TRUST_NONE);
  }
}

/*!
 * Parses a command.
 * \addtogroup command
 * \param fromData the data to parse
 * \param toCommand the location of the parsed command
 * \sa CommandEmit(Data*, const Command*)
 */
void CommandParse(
	Data *fromData,
	Command *toCommand) {
  if (!fromData) {
    Log("invalid `fromData` Data");
  } else if (!toCommand) {
    Log("invalid `toCommand` Command");
  } else {
    Data *xCommand = fromData;
    if (!toCommand->parent)
      xCommand = DataGet(fromData, "Command");
    if (!toCommand->parent)
      CommandParseName(xCommand, toCommand);
    CommandParseSocialData(xCommand, toCommand);
    CommandParseFunction(xCommand, toCommand);
    CommandParseChildList(xCommand, toCommand);
    CommandParseKeywordList(xCommand, toCommand);
    CommandParseTrust(xCommand, toCommand);
  }
}

/*!
 * Recreates the command keyword index.
 * \addtogroup command
 * \param game the game state
 */
void CommandReindexKeywords(Game *game) {
  if (!game) {
    Log("invalid `game` Game");
  } else {
    /* Temporary command list */
    List *commands = ListAlloc(
        (ListCompareProc)       NULL,
        (ListFreeProc)          CommandFree);

    /* Build temporary command list */
    RBTreeForEach(game->commands, tCommandNode) {
      /* Iterator variable */
      Command *tCommand = tCommandNode->value;
      char **tCommandKey = tCommandNode->key;

      /* Check for command names and keywords */
      if (StringCmpCI(tCommand->name, *tCommandKey) == 0) {
        ListPushBack(commands, tCommand);
        tCommand->refcount++;
      }
    }
    RBTreeForEachEnd();

    /* Clear the keyword index */
    RBTreeClear(game->commands);

    /* Add child keywords back to child index */
    ListForEach(commands, tCommandNode) {
      /* Iterator variable */
      Command *tCommand = tCommandNode->value;

      /* Index child command keywords */
      if (tCommand->keywordListN) {
        register size_t keywordN = 0;
        for (; keywordN < tCommand->keywordListN; ++keywordN) {
          RBTreeInsert(game->commands, &tCommand->keywordList[keywordN], tCommand);
          tCommand->refcount++;
        }
      }
    }
    ListForEachEnd();

    /* Add child names back to child index */
    ListForEach(commands, tCommandNode) {
      /* Iterator variable */
      Command *tCommand = tCommandNode->value;

      /* Index child command name last */
      RBTreeInsert(game->commands, &tCommand->name, tCommand);
      tCommand->refcount++;
    }
    ListForEachEnd();

    /* Cleanup */
    ListFree(commands);
  }
}

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
	Command *command) {
  register bool result = false;
  if (!game) {
    Log("invalid `game` Game");
  } else if (!command) {
    Log("invalid `command` Command");
  } else {
    char fname[PATH_MAX] = {'\0'};
    if (!CommandGetFileName(fname, sizeof(fname), command->name)) {
      Log("Couldn't create filename for command `%s`", command->name);
    } else {
      /* Emit command data */
      Data *toData = DataAlloc();
      CommandEmit(toData, command);

      /* Now save the command file */
      if ((result = DataSaveFile(toData, fname)) == false)
	Log("Couldn't save command file `%s`", fname);

      /* Cleanup */
      DataFree(toData);
    }
  }
  return (result);
}

/*!
 * Saves the command index.
 * \param game the game state
 * \addtogroup command
 * \sa CommandLoadIndex(Game*)
 */
void CommandSaveIndex(Game *game) {
  if (!game) {
    Log("invalid `game` Game");
  } else {
    FILE *stream = fopen(COMMAND_INDEX_FILE, "wt");
    if (!stream) {
      Log("Couldn't open command index file `%s` for writing", COMMAND_INDEX_FILE);
    } else {
      RBTreeForEach(game->commands, tCommandNode) {
	/* Iterator variable */
	const Command *tCommand = tCommandNode->value;

	/* Print command name */
	fprintf(stream, "%s\n", tCommand->name);
      }
      RBTreeForEachEnd();
      fclose(stream);
    }
  }
}

/*!
 * Stores a copy of a command.
 * \addtogroup command
 * \param game the game state
 * \param command the command to store
 * \return a copy of the specified command or NULL
 */
Command *CommandStore(
	Game *game,
	const Command *command) {
  register Command *copied = NULL;
  if (!game) {
    Log("invalid `game` Game");
  } else if (!command) {
    Log("invalid `command` Command");
  } else {
    copied = CommandByName(game, command->name);
    if (!copied) {
      copied = CommandAlloc();
      StringSet(&copied->name, command->name);
      if (!RBTreeInsert(game->commands, (RBTreeKey) &copied->name, copied)) {
	Log("Couldn't add command `%s` to command index", copied->name);
	CommandFree(copied), copied = NULL;
      }
    }
    if (copied)
      CommandCopy(copied, command);
  }
  return (copied);
}
