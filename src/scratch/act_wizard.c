/*!
 * \file act_wizard.c
 *
 * \par Copyright
 * Copyright (C) 1999-2019 scratchmud.org
 * All rights reserved.
 *
 * \author Geoffrey Davis <gdavis@scratchmud.org>
 * \addtogroup act
 */
#define _SCRATCH_ACT_WIZARD_C_

#include <scratch/action.h>
#include <scratch/block.h>
#include <scratch/client.h>
#include <scratch/color.h>
#include <scratch/command.h>
#include <scratch/constant.h>
#include <scratch/game.h>
#include <scratch/gender.h>
#include <scratch/log.h>
#include <scratch/object.h>
#include <scratch/rbtree.h>
#include <scratch/scratch.h>
#include <scratch/server.h>
#include <scratch/state.h>
#include <scratch/string.h>

/* Local functions */
COMMAND(ActClient);
COMMAND(ActClientClose);
COMMAND(ActClientShow);
COMMAND(ActClientShowAll);
COMMAND(ActShutdown);
COMMAND(ActShutdownDie);
COMMAND(ActShutdownReboot);

/* Client command usage */
#define ACT_CLIENT_USAGE \
  "Usage: Client Close <Client>\r\n" \
  "       Client Show All\r\n" \
  "       Client Show <Client>\r\n"

/*!
 * The CLIENT command.
 * \addtogroup act
 * \param actor the game object that performed the command
 * \param command the command the specified game object performed
 * \param input the command line input to the command
 */
COMMAND(ActClient) {
  ObjectPrint(actor, "%s", ACT_CLIENT_USAGE);
}

/*!
 * The CLIENT CLOSE command.
 * \addtogroup act
 * \param actor the game object that performed the command
 * \param command the command the specified game object performed
 * \param input the command line input to the command
 */
COMMAND(ActClientClose) {
  /* Read the client name */
  char name[MAXLEN_INPUT] = {'\0'};
  input = StringOneWord(name, sizeof(name), input);

  Client *client;
  if (*name == '\0') {
    Act(CX_FAILED, TO_ACTOR, "Close which client?", actor, NULL, NULL);
  } else if ((client = ClientByName(actor->game->server, name)) == NULL) {
    Act(CX_FAILED, TO_ACTOR, "There's no client named {D.Text}.", actor, name, NULL);
  } else if (client == actor->client) {
    Act(CX_FAILED, TO_ACTOR, "You can't close your own client.", actor, NULL, NULL);
  } else {
    /* Disconnect the player */
    if (client->player->client == client) {
      client->player->client = NULL;
      client->player = NULL;
    }

    /* Close */
    ClientClose(client);

    Log("Client %s disconnected by %s.", client->name, ObjectGetName(actor));
    ObjectPrint(actor, "Closed client %s.\r\n", client->name);
  }
}

/*!
 * The CLIENT SHOW command.
 * \addtogroup act
 * \param actor the game object that performed the command
 * \param command the command the specified game object performed
 * \param input the command line input to the command
 */
COMMAND(ActClientShow) {
  /* Read the client name */
  char name[MAXLEN_INPUT] = {'\0'};
  input = StringOneWord(name, sizeof(name), input);

  Client *client = NULL;
  if (*name == '\0') {
    Act(CX_FAILED, TO_ACTOR, "Show which client?", actor, NULL, NULL);
  } else if ((client = ClientByName(actor->game->server, name)) == NULL) {
    Act(CX_FAILED, TO_ACTOR, "There's no client named {D.Text}.", actor, name, NULL);
  } else {
    /* Output buffer */
    char out[MAXLEN_STRING] = {'\0'};
    register size_t outpos = 0;

    /* Client bits */
    char clientBits[MAXLEN_INPUT] = {'\0'};
    BlockPrintBits(clientBits, sizeof(clientBits), _G_clientBits, client->flags);

    /* State bits */
    char stateBits[MAXLEN_INPUT] = {'\0'};
    BlockPrintBits(stateBits, sizeof(stateBits), _G_stateBits,
	client->state ? client->state->flags : 0);

    /* Window size */
    char window[MAXLEN_INPUT] = {'\0'};
    if (client->nawsWidth && client->nawsHeight)
      snprintf(window, sizeof(window), "%hux%hu", client->nawsWidth, client->nawsHeight);

    BPrintf(out, sizeof(out), outpos,
	"%s--- %sClient %s %s---%s\r\n"
	"%sClient Bits %s............. : %s%s%s\r\n"
	"%sClient Hostname %s......... : %s%s%s\r\n"
	"%sClient Window Size %s...... : %s%s%s\r\n"
	"%sConnection State %s........ : %s%s%s\r\n"
	"%sConnection State Bits %s... : %s%s%s\r\n"
	"%sPlayer Name %s............. : %s%s%s\r\n",
	
	/* Client Name */
	QQ_WHITE, QQ_GREEN, *StringBlank(client->name) != '\0' ?
			     StringBlank(client->name) : "<Unknown>",
			     QQ_WHITE, QQ_NORMAL,

	/* Client Bits */
	QQ_GREEN, QQ_WHITE, QQ_CYAN, clientBits, QQ_NORMAL,


	/* Client Hostname */
	QQ_GREEN, QQ_WHITE, QQ_YELLOW,
		*StringBlank(client->hostname) != '\0' ?
		 StringBlank(client->hostname) : "<Unknown>",
		 QQ_NORMAL,

	/* Client Window Size */
	QQ_GREEN, QQ_WHITE, QQ_YELLOW,
		*window != '\0' ? window : "<Undefined>", QQ_NORMAL,

	/* Connection State */
	QQ_GREEN, QQ_WHITE, QQ_YELLOW,
		client->state ? *StringBlank(client->state->name) != '\0' ?
				 StringBlank(client->state->name) :
				 "<Undefined>" : "<None>", QQ_NORMAL,

	/* Connection State Bits */
	QQ_GREEN, QQ_WHITE, QQ_CYAN, stateBits, QQ_NORMAL,

	/* Player Name */
	QQ_GREEN, QQ_WHITE, QQ_MAGENTA,
		*ObjectGetName(client->player) != '\0' ?
		 ObjectGetName(client->player) :
		 "<None>", QQ_NORMAL);

    ObjectPrint(actor, "%s", out);
  }
}

#define ACT_CLIENT_SHOW_OVERFLOW (32)

/*!
 * The CLIENT SHOW ALL command.
 * \addtogroup act
 * \param actor the game object that performed the command
 * \param command the command the specified game object performed
 * \param input the command line input to the command
 */
COMMAND(ActClientShowAll) {
  /* Output buffer */
  char out[MAXLEN_STRING] = {'\0'};
  register size_t outpos = 0;

  register size_t count = 0;
  RBTreeForEach(actor->game->server->clients, tClientNode) {
    /* Iterator variable */
    const Client *tClient = tClientNode->value;

    /* Remember where we are in the output buffer */
    const size_t lastOutpos = outpos;

    if (!outpos) {
      BPrintf(out, sizeof(out) - ACT_CLIENT_SHOW_OVERFLOW, outpos,
	"%sNum    %sName         %sPlayer          %sState %sHost%s\r\n"
	"%s--- ------- -------------- -------------- --------------------------------------%s\r\n",
	QQ_WHITE, QQ_YELLOW, QQ_GREEN, QQ_CYAN, QQ_WHITE, QQ_NORMAL,
	QQ_WHITE, QQ_NORMAL);
    }

    BPrintf(out, sizeof(out), outpos,
	"%s%3zu %s%7s %s%14s %s%14s %s%-38.38s%s\r\n",

	/* Number */
	QQ_WHITE, ++count,

	/* Client name */
	QQ_YELLOW, *StringBlank(tClient->name) != '\0' ?
		    StringBlank(tClient->name) :
		    "<None>",

	/* Connected player name */
	QQ_GREEN, *ObjectGetName(tClient->player) != '\0' ?
		   ObjectGetName(tClient->player) :
		   "<None>",

	/* Client state name */
	QQ_CYAN, tClient->state ?
		*StringBlank(tClient->state->name) != '\0' ?
		 StringBlank(tClient->state->name) :
		 "<Unknown>" :
		 "<None>",

	/* Client hostname */
	QQ_WHITE, *StringBlank(tClient->hostname) != '\0' ?
		   StringBlank(tClient->hostname) : "<Unknown>",

	QQ_NORMAL);

    if (outpos == lastOutpos) {
      BPrintf(out, sizeof(out), outpos, "%s*OVERFLOW*%s\r\n", QQ_RED, QQ_NORMAL);
      goto ActClientShowAbort;
    }
  }
  RBTreeForEachEnd();

ActClientShowAbort:

  if (!outpos) {
    ObjectPrint(actor, "No clients found.\r\n");
  } else {
    ObjectPrint(actor, "%s", out);
  }
}

#define ACT_SHUTDOWN_USAGE \
  "Usage: Shutdown Die    - Shutdown and stop autorun.\r\n" \
  "       Shutdown Reboot - Shutdown and restart in a few moments.\r\n"

/*!
 * The SHUTDOWN command.
 * \addtogroup act
 * \param actor the game object that performed the command
 * \param command the command the specified game object performed
 * \param input the command line input to the command
 */
COMMAND(ActShutdown) {
  ObjectPrint(actor, "%s", ACT_SHUTDOWN_USAGE);
}

/*!
 * The SHUTDOWN DIE command.
 * \addtogroup act
 * \param actor the game object that performed the command
 * \param command the command the specified game object performed
 * \param input the command line input to the command
 */
COMMAND(ActShutdownDie) {
  Log("Shutdown (Die!) initiated by %s.", ObjectGetName(actor));
  actor->game->shutdown = true;

  /* Touch the kill file */
  FILE *stream;
  if ((stream = fopen(".autorun.kill", "at")) == NULL) {
    Log("couldn't touch kill file: errno=%d", errno);
  } else if (fclose(stream) < 0) {
    Log("fclose() failed: errno=%d", errno);
  }
}

/*!
 * The SHUTDOWN REBOOT command.
 * \addtogroup act
 * \param actor the game object that performed the command
 * \param command the command the specified game object performed
 * \param input the command line input to the command
 */
COMMAND(ActShutdownReboot) {
  Log("Shutdown initiated by %s.", ObjectGetName(actor));
  actor->game->shutdown = true;
}
