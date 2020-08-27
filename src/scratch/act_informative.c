/*!
 * \file act_informative.c
 *
 * \par Copyright
 * Copyright (C) 1999-2019 scratchmud.org
 * All rights reserved.
 *
 * \author Geoffrey Davis <gdavis@scratchmud.org>
 * \addtogroup act
 */
#define _SCRATCH_ACT_INFORMATIVE_C_

#include <scratch/action.h>
#include <scratch/block.h>
#include <scratch/client.h>
#include <scratch/color.h>
#include <scratch/command.h>
#include <scratch/constant.h>
#include <scratch/editor.h>
#include <scratch/game.h>
#include <scratch/gender.h>
#include <scratch/log.h>
#include <scratch/object.h>
#include <scratch/player.h>
#include <scratch/rbtree.h>
#include <scratch/scratch.h>
#include <scratch/state.h>
#include <scratch/string.h>

/* Local functions */
COMMAND(ActCommands);
COMMAND(ActDescribe);
EDITOR(ActDescribeEditorAborted);
EDITOR(ActDescribeEditorFinished);
COMMAND(ActWho);

#define ACT_COMMANDS_OVERFLOW (32)

/*!
 * The COMMANDS command.
 * \addtogroup act
 * \param actor the game object that performed the command
 * \param command the command the specified game object performed
 * \param input the command line input to the command
 */
COMMAND(ActCommands) {
  /* Output buffer */
  char out[MAXLEN_STRING] = {'\0'};
  register size_t outpos = 0;

  /* Find length of longest command name */
  register size_t width = 0;
  RBTreeForEach(actor->game->commands, tCommandNode) {
    /* Iterator variable */
    const Command *tCommand = tCommandNode->value;

    /* Skip commands we can't perform */
    if (GetPlayerTrust(actor) < tCommand->trust)
      continue;

    /* Compute new longest command name */
    if (strlen(tCommand->name) > width)
      width = strlen(tCommand->name);
  }
  RBTreeForEachEnd();

  /* How many columns should we show? */
  const size_t nColumns =
	(actor->client ? actor->client->nawsWidth : 80) /
	(width + 1);

  /* List commands */
  register size_t count = 1;
  RBTreeForEach(actor->game->commands, tCommandNode) {
    /* Iterator variable */
    const Command *tCommand = tCommandNode->value;
    const char **tCommandKey = tCommandNode->key;

    /* Remember where we are in the output buffer */
    const size_t lastOutpos = outpos;

    /* Skip commands we can't perform */
    if (GetPlayerTrust(actor) < tCommand->trust)
      continue;

    /* Skip command keywords */
    if (StringCmpCI(tCommand->name, *tCommandKey) != 0)
      continue;

    /* Command name color */
    const char *tColor = QQ_GREEN;
    if (tCommand->socialData) tColor = QQ_MAGENTA;
    if (tCommand->trust >= TRUST_WIZARD) tColor = QQ_CYAN;

    /* Print the command */
    BPrintf(out, sizeof(out) - ACT_COMMANDS_OVERFLOW, outpos,
	"%s%-*.*s%s%s",

	/* Command name */
	tColor, (int) width, (int) width, tCommand->name,

	/* Ending */
	count % nColumns == 0 ? QQ_NORMAL : "",
	count % nColumns == 0 ? "\r\n" : " ");

    if (outpos == lastOutpos) {
      BPrintf(out, sizeof(out), outpos, "%s*OVERFLOW*%s\r\n", QQ_RED, QQ_NORMAL);
      goto ActCommandsAbort;
    }

    /* Bump the count */
    count++;
  }
  RBTreeForEachEnd();

  if (count % nColumns != 1)
    BPrintf(out, sizeof(out), outpos, "%s\r\n", QQ_NORMAL);

ActCommandsAbort:

  if (!outpos) {
    ObjectPrint(actor, "No commands found.\r\n");
  } else {
    ObjectPrint(actor, "%s", out);
  }
}

/*!
 * The DESCRIBE command.
 * \addtogroup act
 * \param actor the game object that performed the command
 * \param command the command the specified game object performed
 * \param input the command line input to the command
 */
COMMAND(ActDescribe) {
  /* Read target name */
  char name[MAXLEN_INPUT] = {'\0'};
  input = StringOneWord(name, sizeof(name), StringBlank(input));

  Object *target = actor;
  if (*name != '\0' && (target = ObjectResolve(actor, name)) == NULL) {
    Act(CX_FAILED, TO_ACTOR, "There's nobody here named {D.Text}.", actor, name, NULL);
  } else if (!target->playerData) {
    Act(CX_FAILED, TO_ACTOR, "You can't describe an object that is not a player.", actor, NULL, NULL);
  } else if (target != actor && GetPlayerTrust(actor) < TRUST_WIZARD) {
    Act(CX_FAILED, TO_ACTOR, "You don't have permission to describe other players.", actor, NULL, NULL);
  } else if (target != actor && GetPlayerTrust(actor) < GetPlayerTrust(target)) {
    Act(CX_FAILED, TO_ACTOR, "You don't have permission to describe {D.Name}.", actor, target, NULL);
  } else {
    EditorStart(actor->client,
	target->description, 1024,
	ActDescribeEditorAborted,
	ActDescribeEditorFinished,
	target);
  }
}

/*!
 * Editor callback.
 * \param client the client
 * \param text the editor text
 * \param userData the user-specified data
 */
EDITOR(ActDescribeEditorAborted) {
  ClientPrint(client, "%sDescription aborted!%s\r\n", Q_RED, Q_NORMAL);
}

/*!
 * Editor callback.
 * \param client the client
 * \param text the editor text
 * \param userData the user-specified data
 */
EDITOR(ActDescribeEditorFinished) {
  Object *target = NULL;
  RBTreeForEach(client->player->game->objects, tObjectNode) {
    /* Iterator variable */
    Object *tObject = tObjectNode->value;

    /* Check for the object we're editing */
    if (tObject == userData)
      target = tObject;
  }
  RBTreeForEachEnd();

  if (!target) {
    ClientPrint(client, "%sYour target is gone!%s\r\n", Q_RED, Q_NORMAL);
  } else {
    StringSet(&target->description, text);
  }
}


#define ACT_WHO_OVERFLOW (32)

/*!
 * The WHO command.
 * \addtogroup act
 * \param actor the game object that performed the command
 * \param command the command the specified game object performed
 * \param input the command line input to the command
 */
COMMAND(ActWho) {
  /* Output buffer */
  char out[MAXLEN_STRING] = {'\0'};
  register size_t outpos = 0;

  register size_t count = 0;
  RBTreeForEach(actor->game->objects, tObjectNode) {
    /* Iterator variable */
    const Object *tObject = tObjectNode->value;

    /* Remember where we are in the output buffer */
    const size_t lastOutpos = outpos;

    /* Skip non-player objects */
    if (!tObject->playerData)
      continue;

    if (!outpos) {
      BPrintf(out, sizeof(out) - ACT_WHO_OVERFLOW, outpos,
		" %sNum %sGender  %sTrust %sName%s\r\n"
		"%s---- ------ ------ -------------------------------------------------------------%s\r\n",
		QQ_WHITE, QQ_MAGENTA, QQ_YELLOW, QQ_GREEN, QQ_NORMAL,
		QQ_WHITE, QQ_NORMAL);
    }

    /* Print trust level */
    char trustName[MAXLEN_INPUT] = {'\0'};
    BlockPrintType(trustName, sizeof(trustName), _G_trustTypes, tObject->playerData->trust);

    /* Print player */
    BPrintf(out, sizeof(out) - ACT_WHO_OVERFLOW, outpos,
	"%s%4zu %s%6.6s %s%6s %s%-61.61s%s\r\n",
	QQ_WHITE, ++count,
	QQ_MAGENTA, tObject->gender && tObject->gender->name ?
		    tObject->gender->name : "None",
	QQ_YELLOW, trustName,
	QQ_GREEN, ObjectGetName(tObject),
	QQ_NORMAL);

    if (outpos == lastOutpos) {
      BPrintf(out, sizeof(out), outpos, "%s*OVERFLOW*%s\r\n", QQ_RED, QQ_NORMAL);
      goto ActWhoAbort;
    }
  }
  RBTreeForEachEnd();

ActWhoAbort:

  if (!outpos) {
    ObjectPrint(actor, "No players found.\r\n");
  } else {
    ObjectPrint(actor, "%s", out);
  }
}
