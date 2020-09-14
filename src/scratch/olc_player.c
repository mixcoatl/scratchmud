/*!
 * \file olc_player.c
 *
 * \par Copyright
 * Copyright (C) 1999-2019 scratchmud.org
 * All rights reserved.
 *
 * \author Geoffrey Davis <gdavis@scratchmud.org>
 */
#define _SCRATCH_BUILD_PLAYER_C_

#include <scratch/client.h>
#include <scratch/color.h>
#include <scratch/editor.h>
#include <scratch/game.h>
#include <scratch/gender.h>
#include <scratch/memory.h>
#include <scratch/object.h>
#include <scratch/olc.h>
#include <scratch/olc_player.h>
#include <scratch/player.h>
#include <scratch/rbtree.h>
#include <scratch/scratch.h>
#include <scratch/server.h>
#include <scratch/state.h>
#include <scratch/string.h>
#include <scratch/utility.h>

/* Local functions */
STATE(OlcPlayerConfirmOnFocus);
STATE(OlcPlayerConfirmOnInput);
EDITOR(OlcPlayerDescriptionAborted);
EDITOR(OlcPlayerDescriptionFinished);
STATE(OlcPlayerGenderOnFocus);
STATE(OlcPlayerGenderOnInput);
STATE(OlcPlayerOnFocus);
STATE(OlcPlayerOnInput);
STATE(OlcPlayerPasswordAgainOnFocus);
STATE(OlcPlayerPasswordAgainOnInput);
STATE(OlcPlayerPasswordOnFocus);
STATE(OlcPlayerPasswordOnInput);
STATE(OlcPlayerPlayerNameOnFocus);
STATE(OlcPlayerPlayerNameOnInput);

/*!
 * Connection state callback.
 * \param client the client
 * \param input the line of user input or an empty string ("")
 */
STATE(OlcPlayerConfirmOnFocus) {
  ClientPrint(client, "%sSave this player? %s", Q_GREEN, Q_NORMAL);
  return (true);
}

/*!
 * Connection state callback.
 * \param client the client
 * \param input the line of user input or an empty string ("")
 */
STATE(OlcPlayerConfirmOnInput) {
  /* Read response */
  char arg[MAXLEN_INPUT] = {'\0'};
  input = StringOneWord(arg, sizeof(arg), input);

  if (*arg == '\0') {
    ClientPrint(client, "%sQuit aborted.%s\r\n", Q_RED, Q_NORMAL);
    ClientStateChangeByName(client, "OlcPlayer");
  } else if (strchr("Yy", *arg) != NULL) {
    /* Save pfile and player index */
    PlayerSave(game, client->olc->player);
    PlayerSaveIndex(game);

    /* Logging and messages */
    ClientPrint(client, "%sPlayer saved.%s\r\n", Q_CYAN, Q_NORMAL);
    Log("/OLC/ %s edited player %s.",
	client->player ? ObjectGetName(client->player) :
			 ObjectGetName(client->olc->player),
			 ObjectGetName(client->olc->player));

    /* Cleanup */
    OlcCleanup(client);
  } else if (strchr("Nn", *arg) != NULL) {
    ClientPrint(client, "%sPlayer editor aborted.%s\r\n", Q_RED, Q_NORMAL);
    OlcCleanup(client);
  } else {
    ClientPrint(client, "%sInvalid choice.%s\r\n", Q_RED, Q_NORMAL);
    OlcPlayerConfirmOnFocus(client, game, "");
  }
  return (true);
}

/*!
 * Editor callback.
 * \param client the client
 * \param text the editor text
 * \param userData the user-specified data
 */
EDITOR(OlcPlayerDescriptionAborted) {
  ClientPrint(client, "%sEditor aborted!%s\r\n", Q_GREEN, Q_NORMAL);
  OlcPlayerOnFocus(client, client->server->game, "");
}

/*!
 * Editor callback.
 * \param client the client
 * \param text the editor text
 * \param userData the user-specified data
 */
EDITOR(OlcPlayerDescriptionFinished) {
  StringSet(&client->olc->player->description, text);
  OlcPlayerOnFocus(client, client->server->game, "");
}

/*!
 * Connection state callback.
 * \param client the client
 * \param input the line of user input or an empty string ("")
 */
STATE(OlcPlayerGenderOnFocus) {
  /* Output buffer */
  char out[MAXLEN_STRING] = {'\0'};
  register size_t outpos = 0;

  /* Iterate genders */
  register size_t count = 0;
  RBTreeForEach(game->genders, tGenderNode) {
    /* Iterator variable */
    const Gender *tGender = tGenderNode->value;

    /* Header */
    if (!outpos) {
      BPrintf(out, sizeof(out), outpos, "%sGender%s:%s\r\n",
        Q_GREEN, Q_WHITE,
        Q_NORMAL);
    }

    BPrintf(out, sizeof(out), outpos,
	"%s<%s%-2.2zu%s> %s%s%s, %s%s%s, %s%s%s, %s%s%s\r\n",

	/* <XX> */
	Q_WHITE, Q_RED, ++count,

	/* Gender name */
	Q_WHITE, Q_GREEN, StringUndefined(tGender->name),

	/* Subject pronoun */
	Q_WHITE, Q_GREEN, StringUndefined(tGender->pronounSubject),

	/* Object pronoun */
	Q_WHITE, Q_GREEN, StringUndefined(tGender->pronounObject),

	/* Determiner */
	Q_WHITE, Q_GREEN, StringUndefined(tGender->determiner),
	Q_NORMAL);
  }
  RBTreeForEachEnd();

  BPrintf(out, sizeof(out), outpos,
	"%sPress %sEnter %sto return to the menu.%s\r\n"
	"%sChoice %s> ",

	/* Press Enter to return to the menu */
	Q_GREEN, Q_RED, Q_GREEN, Q_NORMAL,

	/* Choice */
	Q_GREEN, Q_NORMAL);

  ClientPrint(client, "%s", out);
  return (true);
}

/*!
 * Connection state callback.
 * \param client the client
 * \param input the line of user input or an empty string ("")
 */
STATE(OlcPlayerGenderOnInput) {
  /* Read response */
  char arg[MAXLEN_INPUT] = {'\0'};
  input = StringOneWord(arg, sizeof(arg), input);

  if (*arg == '\0') {
    ClientPrint(client, "%sGender aborted.%s\r\n", Q_RED, Q_NORMAL);
    ClientStateChangeByName(client, "OlcPlayer");
  } else {
    register int choice = atoi(arg);
    if (choice >= 1 && choice <= RBTreeSize(game->genders)) {
      RBTreeForEach(game->genders, tGenderNode) {
	/* Iterator variable */
	Gender *tGender = tGenderNode->value;

	/* Check selected gender */
	if (--choice == 0) {
	  if (client->olc->player->gender != tGender)
	    client->olc->changed = true;
	  StringSet(&client->olc->player->genderName, tGender->name);
	  client->olc->player->gender = tGender;
	  break;
	}
      }
      RBTreeForEachEnd();
      ClientStateChangeByName(client, "OlcPlayer");
    } else
      OlcPlayerGenderOnFocus(client, game, "");
  }
  return (true);
}

/*!
 * Connection state callback.
 * \param client the client
 * \param input the line of user input or an empty string ("")
 */
STATE(OlcPlayerOnFocus) {
  /* Shortcut */
  const Object *player = client->olc->player;

  /* Menu */
  ClientPrint(client,
	"%sPlayer%s: %s%s%s\r\n"
	"%s<%s01%s> %sPlayer Name %s... : %s%s%s\r\n"
	"%s<%s02%s> %sDescription %s... : -%s\r\n"
	"%s%s%s"
	"%s<%s03%s> %sGender %s........ : %s%s%s\r\n"
	"%s<%s04%s> %sPassword %s...... : %s%s%s\r\n"
	"%sEnter %sQ %sto quit.%s\r\n"
	"%sChoice %s> ",

	/* Player */
	Q_GREEN, Q_WHITE, Q_MAGENTA,
	StringUndefined(client->olc->name), Q_NORMAL,

	/* Player name */
	Q_WHITE, Q_RED, Q_WHITE, Q_GREEN, Q_WHITE, Q_MAGENTA,
	StringUndefined(player->playerData->playerName), Q_NORMAL,

	/* Description */
	Q_WHITE, Q_RED, Q_WHITE, Q_GREEN, Q_WHITE, Q_NORMAL,
	Q_YELLOW, *StringBlank(player->description) != '\0' ?
		   StringBlank(player->description) : "<Undefined>\r\n",
		   Q_NORMAL,

	/* Gender name */
	Q_WHITE, Q_RED, Q_WHITE, Q_GREEN, Q_WHITE, Q_YELLOW,
	StringUndefined(player->gender ? player->gender->name :
			player->genderName), Q_NORMAL,

	/* Password */
	Q_WHITE, Q_RED, Q_WHITE, Q_GREEN, Q_WHITE, Q_YELLOW,
		*StringBlank(player->playerData->password) != '\0' ?
		 "<Set>" : "<Unset>", Q_NORMAL,

	/* Enter Q to save and quit */
	Q_GREEN, Q_RED, Q_GREEN, Q_NORMAL,

	/* Choice */
	Q_GREEN, Q_NORMAL);

  return (true);
}

/*!
 * Connection state callback.
 * \param client the client
 * \param input the line of user input or an empty string ("")
 */
STATE(OlcPlayerOnInput) {
  /* Read response */
  char arg[MAXLEN_INPUT] = {'\0'};
  input = StringOneWord(arg, sizeof(arg), input);

  if (*arg == '\0') {
    OlcPlayerOnFocus(client, game, "");
  } else if (strchr("Qq", *arg) != NULL) { /* Save */
    if (!client->olc->changed) {
      ClientPrint(client, "%sNo changes detected.%s\r\n", Q_CYAN, Q_NORMAL);
      OlcCleanup(client);
    } else {
      ClientStateChangeByName(client, "OlcPlayerConfirm");
    }
  } else {
    const int choice = atoi(arg);
    switch (choice) {
    case /* Player Name */ 1:
      ClientStateChangeByName(client, "OlcPlayerPlayerName");
      break;
    case /* Description */ 2:
      EditorStart(client,
		  client->olc->player->description,
		  1024,
		  OlcPlayerDescriptionAborted,
		  OlcPlayerDescriptionFinished,
		  NULL);
      break;
    case /* Gender */ 3:
      ClientStateChangeByName(client, "OlcPlayerGender");
      break;
    case /* Password */ 4:
      if (*StringBlank(client->olc->name) == '\0') {
	ClientStateChangeByName(client, "OlcPlayerPassword");
      } else {
	ClientStateChangeByName(client, "OlcPlayerPasswordCurrent");
      }
      break;
    default:
      ClientPrint(client, "%sInvalid choice.%s\r\n", Q_RED, Q_NORMAL);
      OlcPlayerOnFocus(client, game, "");
      break;
    }
  }
  return (true);
}

/*!
 * Connection state callback.
 * \param client the client
 * \param input the line of user input or an empty string ("")
 */
STATE(OlcPlayerPasswordAgainOnFocus) {
  ClientPrint(client, "%sEnter NEW password again %s> ", Q_GREEN, Q_NORMAL);
  return (true);
}

/*!
 * Connection state callback.
 * \param client the client
 * \param input the line of user input or an empty string ("")
 */
STATE(OlcPlayerPasswordAgainOnInput) {
  /* Read plaintext password */
  char plaintext[MAXLEN_INPUT];
  input = StringOneWord(plaintext, sizeof(plaintext), input);

  /* No password  */
  if (*plaintext == '\0') {
    ClientPrint(client, "%sPassword aborted.%s\r\n", Q_RED, Q_NORMAL);
    ClientStateChangeByName(client, "OlcPlayer");
  /* Match plaintext against crypted password */
  } else if (!UtilityCryptMatch(client->olc->password, plaintext)) {
    ClientPrint(client, "%sPasswords don't match.  Start over.%s\r\n", Q_GREEN, Q_NORMAL);
    ClientStateChangeByName(client, "OlcPlayerPassword");
  } else {
    StringSet(&client->olc->player->playerData->password, client->olc->password);
    StringSet(&client->olc->password, NULL);
    ClientPrint(client, "%sPassword changed.%s\r\n", Q_CYAN, Q_NORMAL);
    ClientStateChangeByName(client, "OlcPlayer");
  }
  return (true);
}

/*!
 * Connection state callback.
 * \param client the client
 * \param input the line of user input or an empty string ("")
 */
STATE(OlcPlayerPasswordCurrentOnFocus) {
  ClientPrint(client, "%sEnter CURRENT password %s> ", Q_GREEN, Q_NORMAL);
  return (true);
}

/*!
 * Connection state callback.
 * \param client the client
 * \param input the line of user input or an empty string ("")
 */
STATE(OlcPlayerPasswordCurrentOnInput) {
  /* Read plaintext password */
  char plaintext[MAXLEN_INPUT];
  input = StringOneWord(plaintext, sizeof(plaintext), input);

  /* No password  */
  if (*plaintext == '\0') {
    ClientPrint(client, "%sPassword aborted.%s\r\n", Q_RED, Q_NORMAL);
    ClientStateChangeByName(client, "OlcPlayer");
  /* Match plaintext against crypted password */
  } else if (!UtilityCryptMatch(client->olc->player->playerData->password, plaintext)) {
    ClientPrint(client, "%sPasswords don't match.%s\r\n", Q_RED, Q_NORMAL);
    ClientStateChangeByName(client, "OlcPlayer");
  } else {
    ClientStateChangeByName(client, "OlcPlayerPassword");
  }
  return (true);
}

/*!
 * Connection state callback.
 * \param client the client
 * \param input the line of user input or an empty string ("")
 */
STATE(OlcPlayerPasswordOnFocus) {
  ClientPrint(client, "%sEnter NEW password %s> ", Q_GREEN, Q_NORMAL);
  return (true);
}

/*!
 * Connection state callback.
 * \param client the client
 * \param input the line of user input or an empty string ("")
 */
STATE(OlcPlayerPasswordOnInput) {
  /* Read plaintext password */
  char plaintext[MAXLEN_INPUT];
  input = StringOneWord(plaintext, sizeof(plaintext), input);

  /* No password */
  if (*plaintext == '\0') {
    ClientPrint(client, "%sPassword aborted.%s\r\n", Q_RED, Q_NORMAL);
    ClientStateChangeByName(client, "OlcPlayer");
  } else {
    /* Crypted password */
    char password[MAXLEN_INPUT];
    UtilityCrypt(password, sizeof(password), plaintext);

    /* Now we need to verify the crypted password */
    StringSet(&client->olc->password, password);
    ClientStateChangeByName(client, "OlcPlayerPasswordAgain");
  }
  return (true);
}

/*!
 * Connection state callback.
 * \param client the client
 * \param input the line of user input or an empty string ("")
 */
STATE(OlcPlayerPlayerNameOnFocus) {
  ClientPrint(client, "%sEnter player name %s> ", Q_GREEN, Q_NORMAL);
  return (true);
}

/*!
 * Connection state callback.
 * \param client the client
 * \param input the line of user input or an empty string ("")
 */
STATE(OlcPlayerPlayerNameOnInput) {
  /* Read player name */
  char playerName[MAXLEN_INPUT];
  input = StringOneWord(playerName, sizeof(playerName), input);

  /* Convert to title case */
  if (*playerName != '\0') {
    register char *ptr;
    for (ptr = playerName; *ptr != '\0'; ++ptr) {
      *ptr = ptr != playerName ? tolower(*ptr) : toupper(*ptr);
    }
  }

  /* No player name */
  register Player *player = NULL;
  if (*playerName == '\0') {
    ClientPrint(client, "%sPlayer name aborted!%s\r\n", Q_RED, Q_NORMAL);
    ClientStateChangeByName(client, "OlcPlayer");
  } else if (strlen(playerName) < 3) {
    ClientPrint(client, "%sPlayer name %s%s %sis too short.%s\r\n",
	Q_GREEN, Q_WHITE, playerName, Q_GREEN,
	Q_NORMAL);
    OlcPlayerPlayerNameOnFocus(client, game, "");
  } else if (strlen(playerName) > 14) {
    ClientPrint(client, "%sPlayer name %s%s %sis too long.%s\r\n",
	Q_GREEN, Q_WHITE, playerName, Q_GREEN,
	Q_NORMAL);
    OlcPlayerPlayerNameOnFocus(client, game, "");
  } else if (!UtilityNameValid(playerName)) {
    ClientPrint(client, "%sPlayer name %s%s %sis not valid.%s\r\n",
	Q_GREEN, Q_WHITE, playerName, Q_GREEN,
	Q_NORMAL);
    OlcPlayerPlayerNameOnFocus(client, game, "");
  } else if ((player = PlayerByName(game, playerName)) != NULL &&
		       StringCmp(client->olc->name, player->playerName) != 0) {
    ClientPrint(client, "%sPlayer name %s%s %sis already used.%s\r\n",
	Q_GREEN, Q_WHITE, playerName, Q_GREEN,
	Q_NORMAL);
    OlcPlayerPlayerNameOnFocus(client, game, "");
  } else {
    if (StringCmp(client->olc->player->playerData->playerName, playerName) != 0)
      client->olc->changed = true;

    StringSet(&client->olc->player->playerData->playerName, playerName);
    ClientStateChangeByName(client, "OlcPlayer");
  }
  return (true);
}

/*!
 * Edits a player.
 * \addtogroup olc
 * \addtogroup player
 * \param client the client that is editing the player
 * \param player the player object to edit, or NULL
 */
void OlcPlayerStart(
	Client *client,
	const Object *player) {
  if (!client) {
    Log("invalid `client` Client");
  } else if (client->olc) {
    Log("client %s already has OLC state", client->name);
  } else if (player && !player->playerData) {
    Log("object `%s` missing player special data", player->name);
  } else {
    client->olc = OlcAlloc();
    client->olc->player = ObjectAlloc(client->server->game);
    client->olc->player->playerData = PlayerDataAlloc();

    if (client->state) {
      client->olc->lastState = client->state;
      client->olc->lastStateName = strdup(client->state->name);
    }
    if (player) {
      ObjectCopy(client->olc->player, player);
      StringSet(&client->olc->name, player->playerData->playerName);
    }
    ClientStateChangeByName(client, "OlcPlayer");
  }
}
