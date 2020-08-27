/*!
 * \file act_other.c
 *
 * \par Copyright
 * Copyright (C) 1999-2019 scratchmud.org
 * All rights reserved.
 *
 * \author Geoffrey Davis <gdavis@scratchmud.org>
 * \addtogroup act
 */
#define _SCRATCH_ACT_OTHER_C_

#include <scratch/action.h>
#include <scratch/bitvector.h>
#include <scratch/client.h>
#include <scratch/color.h>
#include <scratch/command.h>
#include <scratch/constant.h>
#include <scratch/game.h>
#include <scratch/gender.h>
#include <scratch/log.h>
#include <scratch/object.h>
#include <scratch/player.h>
#include <scratch/rbtree.h>
#include <scratch/scratch.h>
#include <scratch/state.h>
#include <scratch/string.h>
#include <scratch/utility.h>

/* Local functions */
COMMAND(ActClear);
COMMAND(ActPassword);
COMMAND(ActPreferences);
COMMAND(ActPreferencesShow);
COMMAND(ActQuit);
COMMAND(ActSave);
COMMAND(ActSaveAll);

/*!
 * The CLEAR command.
 * \addtogroup act
 * \param actor the game object that performed the command
 * \param command the command the specified game object performed
 * \param input the command line input to the command
 */
COMMAND(ActClear) {
  ObjectPrint(actor, "\x1b[H\x1b[J");
}

/*!
 * The PASSWORD command.
 * \addtogroup act
 * \param actor the game object that performed the command
 * \param command the command the specified game object performed
 * \param input the command line input to the command
 */
COMMAND(ActPassword) {
  if (!actor->client) {
    Log("player `%s` (#%s) missing client", ObjectGetName(actor), actor->name);
  } else {
    ClientStateChangeByName(actor->client, "ChangePasswordCurrent");
  }
}

/*!
 * The PREFERENCES command.
 * \addtogroup act
 * \param actor the game object that performed the command
 * \param command the command the specified game object performed
 * \param input the command line input to the command
 */
COMMAND(ActPreferences) {
  /* Remember our NO REPEAT setting */
  const bool noRepeat = BitCheckN(GetPlayerPreferenceBits(actor), PREF_NO_REPEAT);

  /* The number of preferences changed */
  register size_t nPreferences = 0;

  while (*input != '\0') {
    /* Keep track of what was requested */
    bool optionNo  = false;
    bool optionYes = false;

    /* The current preference we're examining */
    char bitName[MAXLEN_INPUT] = {'\0'};
    input = StringOneWord(bitName, sizeof(bitName), input);

    /* Decode a leading + or - sign */
    if (strchr("+-", *bitName) != NULL) {
      if (*bitName == '-') optionNo  = true;
      if (*bitName == '+') optionYes = true;
      strlcpy(bitName, bitName + 1, sizeof(bitName));
    }

    /* Search for the preference */
    register size_t bitN;
    for (bitN = 0; *_G_preferenceBits[bitN] != '\n'; ++bitN) {
      if (StringPrefixCI(_G_preferenceBits[bitN], bitName))
	break;
    }

    if (*_G_preferenceBits[bitN] == '\n') {
      Act(CX_FAILED, TO_ACTOR, "There is no preference named {D.Text}.", actor, bitName, NULL);
    } else {
      Bitvector preferenceBits = GetPlayerPreferenceBits(actor);
      const bool last = BitCheckN(preferenceBits, bitN);
      if (optionNo) {
	BitRemoveN(actor->playerData->preferenceBits, bitN);
      } else if (optionYes) {
        BitSetN(actor->playerData->preferenceBits, bitN);
      } else {
	BitToggleN(actor->playerData->preferenceBits, bitN);
      }
      preferenceBits = GetPlayerPreferenceBits(actor);
      if (!noRepeat && BitCheckN(preferenceBits, bitN) != last) {
	nPreferences++;
	ObjectPrint(actor, "%sPreference %s%s %sset to %s%s%s.%s\r\n",
		QQ_GREEN, QQ_WHITE, _G_preferenceBits[bitN],
		QQ_GREEN, BitCheckN(preferenceBits, bitN) ? QQ_WHITE : QQ_RED,
			  BitCheckN(preferenceBits, bitN) ? "ON" : "OFF",
		QQ_GREEN, QQ_NORMAL);
      }
    }
  }
  if (!nPreferences)
    Act(CX_OKAY, TO_ACTOR, "No preferences were changed.", actor, NULL, NULL);
}

#define ACT_PREFERENCES_SHOW_OVERFLOW (32)

/*!
 * The PREFERENCES SHOW command.
 * \addtogroup act
 * \param actor the game object that performed the command
 * \param command the command the specified game object performed
 * \param input the command line input to the command
 */
COMMAND(ActPreferencesShow) {
  /* Output buffer */
  char out[MAXLEN_STRING] = {'\0'};
  register size_t outpos = 0;

  /* Find length of preference name */
  register size_t bitN, width = 0;
  for (bitN = 0; *_G_preferenceBits[bitN] != '\n'; ++bitN) {
    if (strlen(_G_preferenceBits[bitN]) > width)
      width = strlen(_G_preferenceBits[bitN]);
  }

  /* How many columns should we show? */
  const size_t nColumns =
	(actor->client ? actor->client->nawsWidth : 80) / (width + 6);

  /* List preferences */
  register size_t count = 1;
  for (bitN = 0; *_G_preferenceBits[bitN] != '\n'; ++bitN) {
    /* Remember where we are in the output buffer */
    const size_t lastOutpos = outpos;

    /* Print the preference name */
    BPrintf(out, sizeof(out) - ACT_PREFERENCES_SHOW_OVERFLOW, outpos,
	"%s%*.*s %s[%s%-3.3s%s]%s%s",

	/* Preference name */
	QQ_GREEN, (int) width, (int) width, _G_preferenceBits[bitN],

	/* Yes/No indicator */
	QQ_WHITE,
	BitCheckN(GetPlayerPreferenceBits(actor), bitN) ? QQ_GREEN : QQ_RED,
	BitCheckN(GetPlayerPreferenceBits(actor), bitN) ? "ON" : "OFF",
	QQ_WHITE,

	/* Ending */
	count % nColumns == 0 ? QQ_NORMAL : "",
	count % nColumns == 0 ? "\r\n" : " ");

    if (outpos == lastOutpos) {
      BPrintf(out, sizeof(out), outpos, "%s*OVERFLOW*%s\r\n", QQ_RED, QQ_NORMAL);
      goto ActPreferencesShowAbort;
    }
    count++;
  }

  /* close with a line terminator */
  if (outpos && count % nColumns != 1)
    BPrintf(out, sizeof(out), outpos, "%s\r\n", QQ_NORMAL);

ActPreferencesShowAbort:

  if (!outpos) {
    ObjectPrint(actor, "%sNo preferences found.%s\r\n", QQ_RED, QQ_NORMAL);
  } else {
    ObjectPrint(actor, "%s", out);
  }
}

/*!
 * The QUIT command.
 * \addtogroup act
 * \param actor the game object that performed the command
 * \param command the command the specified game object performed
 * \param input the command line input to the command
 */
COMMAND(ActQuit) {
  Log("%s has left the game.", ObjectGetName(actor));

  /* Save */
  PlayerSave(actor->game, actor);
  PlayerSaveIndex(actor->game);

  /* Disconnect and cleanup */
  ObjectExtract(actor->game, actor);
  ObjectFree(actor);
}

/*!
 * The SAVE command.
 * \addtogroup act
 * \param actor the game object that performed the command
 * \param command the command the specified game object performed
 * \param input the command line input to the command
 */
COMMAND(ActSave) {
  /* Read target name */
  char name[MAXLEN_INPUT] = {'\0'};
  input = StringOneWord(name, sizeof(name), StringBlank(input));

  Object *target = actor;
  if (*name != '\0' && (target = ObjectResolve(actor, name)) == NULL) {
    Act(CX_FAILED, TO_ACTOR, "There's nobody here named {D.Text}.", actor, name, NULL);
  } else if (!target->playerData) {
    Act(CX_FAILED, TO_ACTOR, "You can't use {I.Text} on {D.Name}.", actor, target, command->name);
  } else if (target != actor && GetPlayerTrust(actor) < TRUST_WIZARD) {
    Act(CX_FAILED, TO_ACTOR, "You can't use {I.Text} on {D.Name}.", actor, target, command->name);
  } else if (target != actor && GetPlayerTrust(actor) < GetPlayerTrust(target)) {
    Act(CX_FAILED, TO_ACTOR, "You can't use {I.Text} on {D.Name}.", actor, target, command->name);
  } else {
    /* Tell the actor and target */
    Act(CX_OKAY, TO_ACTOR | TO_TARGET, "Saving {D.RawName}.", actor, target, NULL);

    /* Save */
    PlayerSave(target->game, target);
    PlayerSaveIndex(target->game);
  }
}

/*!
 * The SAVE ALL command.
 * \addtogroup act
 * \param actor the game object that performed the command
 * \param command the command the specified game object performed
 * \param input the command line input to the command
 */
COMMAND(ActSaveAll) {
  /* Genders */
  GenderSaveIndex(actor->game);

  /* Client states */
  StateSaveIndex(actor->game);

  /* Commands */
  CommandSaveIndex(actor->game);
  RBTreeForEach(actor->game->commands, tCommandNode) {
    /* Iterator variable */
    Command *tCommand = tCommandNode->value;

    /* Save command */
    CommandSave(actor->game, tCommand);
  }
  RBTreeForEachEnd();
  Act(CX_OKAY, TO_ACTOR, "Okay.", actor, NULL, NULL);
}

/*!
 * Connection state callback.
 * \param client the client
 * \param input the line of user input or an empty string ("")
 */
STATE(ChangePasswordAgainOnFocus) {
  ClientPrint(client, "%sEnter NEW password again %s> ", Q_GREEN, Q_NORMAL);
  return (true);
}

/*!
 * Connection state callback.
 * \param client the client
 * \param input the line of user input or an empty string ("")
 */
STATE(ChangePasswordAgainOnInput) {
  /* Read plaintext password */
  char plaintext[MAXLEN_INPUT];
  input = StringOneWord(plaintext, sizeof(plaintext), input);

  /* Match plaintext against crypted password */
  if (!UtilityCryptMatch(client->player->playerData->password, plaintext)) {
    ClientPrint(client, "%sPasswords don't match.  Start over.%s\r\n", Q_GREEN, Q_NORMAL);
    ClientStateChangeByName(client, "ChangePassword");
  } else {
    ClientPrint(client, "%sPassword changed.%s\r\n", Q_GREEN, Q_NORMAL);
    ClientStateChangeByName(client, "Playing");
    PlayerSave(client->player->game, client->player);
    PlayerSaveIndex(client->player->game);
  }
  return (true);
}

/*!
 * Connection state callback.
 * \param client the client
 * \param input the line of user input or an empty string ("")
 */
STATE(ChangePasswordOnFocus) {
  ClientPrint(client, "%sEnter NEW password %s> ", Q_GREEN, Q_NORMAL);
  return (true);
}

/*!
 * Connection state callback.
 * \param client the client
 * \param input the line of user input or an empty string ("")
 */
STATE(ChangePasswordOnInput) {
  char plaintext[MAXLEN_INPUT];
  input = StringOneWord(plaintext, sizeof(plaintext), input);

  if (*plaintext == '\0') {
    ClientPrint(client, "%sNew password is not valid.%s\r\n", Q_GREEN, Q_NORMAL);
    ClientPrint(client, "%sEnter NEW password %s> ", Q_GREEN, Q_NORMAL);
  } else if (strlen(plaintext) < 6) {
    ClientPrint(client, "%sNew password too short.%s\r\n", Q_GREEN, Q_NORMAL);
    ClientPrint(client, "%sEnter NEW password %s> ", Q_GREEN, Q_NORMAL);
  } else {
    /* Crypted password */
    char password[MAXLEN_INPUT];
    UtilityCrypt(password, sizeof(password), plaintext);

    /* Now we need to verify the crypted password */
    StringSet(&client->player->playerData->password, password);
    ClientStateChangeByName(client, "ChangePasswordAgain");
  }
  return (true);
}

/*!
 * Connection state callback.
 * \param client the client
 * \param input the line of user input or an empty string ("")
 */
STATE(ChangePasswordCurrentOnFocus) {
  ClientPrint(client, "%sEnter CURRENT password %s> ", Q_GREEN, Q_NORMAL);
  return (true);
}

/*!
 * Connection state callback.
 * \param client the client
 * \param input the line of user input or an empty string ("")
 */
STATE(ChangePasswordCurrentOnInput) {
  /* Read plaintext password */
  char plaintext[MAXLEN_INPUT];
  input = StringOneWord(plaintext, sizeof(plaintext), input);

  /* Match plaintext against crypted password */
  if (*plaintext == '\0') {
    ClientPrint(client, "%sAborted.%s\r\n", Q_GREEN, Q_NORMAL);
    ClientStateChangeByName(client, "Playing");
  } else if (!UtilityCryptMatch(client->player->playerData->password, plaintext)) {
    ClientPrint(client, "%sPasswords don't match.%s\r\n", Q_GREEN, Q_NORMAL);
    ClientStateChangeByName(client, "Playing");
  } else {
    ClientStateChangeByName(client, "ChangePassword");
  }
  return (true);
}
