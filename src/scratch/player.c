/*!
 * \file player.c
 *
 * \par Copyright
 * Copyright (C) 1999-2019 scratchmud.org
 * All rights reserved.
 *
 * \author Geoffrey Davis <gdavis@scratchmud.org>
 * \addtogroup player
 */
#define _SCRATCH_PLAYER_C_

#include <scratch/block.h>
#include <scratch/constant.h>
#include <scratch/data.h>
#include <scratch/game.h>
#include <scratch/log.h>
#include <scratch/memory.h>
#include <scratch/object.h>
#include <scratch/player.h>
#include <scratch/rbtree.h>
#include <scratch/scratch.h>
#include <scratch/string.h>
#include <scratch/utility.h>

/*!
 * Constructs a new player.
 * \addtogroup player
 * \return the new player or NULL
 * \sa PlayerFree(Player*)
 */
Player *PlayerAlloc(void) {
  register Player *player = NULL;
  MemoryCreate(player, Player, 1);
  player->playerName = NULL;
  player->trust      = TRUST_NONE;
  return (player);
}

/*!
 * Retrieves a player.
 * \addtogroup player
 * \param game the game state
 * \param playerName the player name of the player to return
 * \return the player indicated by the specified player name or NULL
 */
Player *PlayerByName(
	Game *game,
	const char *playerName) {
  register Player *player = NULL;
  if (!game) {
    Log("invalid `game` Game");
  } else if (*StringBlank(playerName) == '\0') {
    Log("invalid `playerName` string");
  } else {
    player = RBTreeGetValue(game->players, (RBTreeKey) &playerName, NULL);
  }
  return (player);
}

/*!
 * Copies a player.
 * \addtogroup player
 * \param toPlayer the location of the copied player
 * \param fromPlayer the player to copy
 */
void PlayerCopy(
	Player *toPlayer,
	const Player *fromPlayer) {
  if (!toPlayer) {
    Log("invalid `toPlayer` Player");
  } else if (!fromPlayer) {
    Log("invalid `fromPlayer` Player");
  } else if (toPlayer != fromPlayer) {
    StringSet(&toPlayer->playerName, fromPlayer->playerName);
    toPlayer->trust  = fromPlayer->trust;
  }
}

/*!
 * Returns the size of a player in bytes.
 * \addtogroup player
 * \param player the player whose size to return
 * \return the size of the specified player in bytes
 */
size_t PlayerCountBytes(const Player *player) {
  register size_t nBytes = 0;
  if (player) {
    nBytes += sizeof(Player);
    if (player->playerName)
      nBytes += strlen(player->playerName) + 1;
  }
  return (nBytes);
}

/*!
 * Constructs new player special data.
 * \addtogroup player
 * \return the new player special data or NULL
 * \sa PlayerDataFree(PlayerData*)
 */
PlayerData *PlayerDataAlloc(void) {
  PlayerData *playerData;
  MemoryCreate(playerData, PlayerData, 1);
  playerData->password = NULL;
  playerData->playerName = NULL;
  playerData->preferenceBits = PREF_COMPACT;
  playerData->trust = TRUST_GUEST;
  return (playerData);
}

/*!
 * Copies player special data.
 * \addtogroup player
 * \param toPlayerData the location of the copied player special data
 * \param fromPlayerData the player special data to copy
 */
void PlayerDataCopy(
	PlayerData *toPlayerData,
	const PlayerData *fromPlayerData) {
  if (!toPlayerData) {
    Log("invalid `toPlayerData` PlayerData");
  } else if (!fromPlayerData) {
    Log("invalid `fromPlayerData` PlayerData");
  } else if (toPlayerData != fromPlayerData) {
    StringSet(&toPlayerData->password, fromPlayerData->password);
    StringSet(&toPlayerData->playerName, fromPlayerData->playerName);
    toPlayerData->preferenceBits = fromPlayerData->preferenceBits;
    toPlayerData->trust = fromPlayerData->trust;
  }
}

/*!
 * Emits player special data.
 * \addtogroup player
 * \param toData the data to which to write
 * \param fromPlayerData the player special data to emit
 * \sa PlayerDataParse(Data*, PlayerData*)
 */
void PlayerDataEmit(
	Data *toData,
	const PlayerData *fromPlayerData) {
  if (!toData) {
    Log("invalid `toData` Data");
  } else if (!fromPlayerData) {
    Log("invalid `fromPlayerData` PlayerData");
  } else {
    Data *xPlayerData = DataAlloc();
    if (*StringBlank(fromPlayerData->password) != '\0')
      DataPutString(xPlayerData, "Password", fromPlayerData->password);
    if (*StringBlank(fromPlayerData->playerName) != '\0')
      DataPutString(xPlayerData, "PlayerName", fromPlayerData->playerName);
    if (fromPlayerData->preferenceBits)
      DataPutBits(xPlayerData, "PreferenceBits", _G_preferenceBits, fromPlayerData->preferenceBits);
    if (fromPlayerData->trust != TRUST_NONE)
      DataPutType(xPlayerData, "Trust", _G_trustTypes, fromPlayerData->trust);

    if (DataSize(xPlayerData) == 0) {
      DataFree(xPlayerData);
    } else {
      DataPut(toData, "PlayerData", xPlayerData);
      DataSort(xPlayerData);
    }
  }
}

/*!
 * Frees player special data.
 * \addtogroup player
 * \param playerData the player special data to free
 * \sa PlayerDataAlloc()
 */
void PlayerDataFree(PlayerData *playerData) {
  if (playerData) {
    StringFree(playerData->password);
    StringFree(playerData->playerName);
    MemoryFree(playerData);
  }
}

/*!
 * Parses player special data.
 * \addtogroup player
 * \param fromData the data to parse
 * \param toPlayerData the location of the parsed player special data
 * \sa PlayerDataEmit(Data*, const PlayerData*)
 */
void PlayerDataParse(
	Data *fromData,
	PlayerData *toPlayerData) {
  if (!fromData) {
    Log("invalid `fromData` Data");
  } else if (!toPlayerData) {
    Log("invalid `toPlayerData` PlayerData");
  } else {
    /* Password */
    StringFree(toPlayerData->password);
    toPlayerData->password = DataGetStringCopy(fromData, "Password", NULL);

    /* Player Name */
    StringFree(toPlayerData->playerName);
    toPlayerData->playerName = DataGetStringCopy(fromData, "PlayerName", NULL);

    /* Preferences bits */
    toPlayerData->preferenceBits = DataGetBits(fromData, "PreferenceBits", _G_preferenceBits, 0);

    /* Trust */
    toPlayerData->trust = DataGetType(fromData, "Trust", _G_trustTypes, TRUST_NONE);
  }
}

/*!
 * Deletes a player.
 * \addtogroup player
 * \param game the game state
 * \param playerName the player name of the player to delete
 * \return true if the player indicated by the specified player name
 *     was successfully deleted
 */
bool PlayerDelete(
	Game *game,
	const char *playerName) {
  register bool result = false;
  if (!game) {
    Log("invalid `game` Game");
  } else if (*StringBlank(playerName) == '\0') {
    Log("invalid `playerName` string");
  } else {
    /* Remove player index */
    result = RBTreeDelete(game->players, (RBTreeKey) &playerName);

    /* Remove pfile */
    char fname[PATH_MAX] = {'\0'};
    if (!PlayerGetFileName(fname, sizeof(fname), playerName)) {
      Log("Couldn't create filename for player `%s`", playerName);
    } else if (unlink(fname) < 0 && errno != ENOENT) {
      Log("unlink() failed: fname=%s, errno=%d", fname, errno);
    }
  }
  return (result);
}

/*!
 * Frees a player.
 * \addtogroup player
 * \param player the player to free
 * \sa PlayerAlloc()
 */
void PlayerFree(Player *player) {
  if (player) {
    StringFree(player->playerName);
    MemoryFree(player);
  }
}

#define PLAYER_DIRECTORY	"data/player"
#define PLAYER_FILE_EXTENSION	"pfile"

/*!
 * Generates a player filename.
 * \addtogroup player
 * \param fname the player filename buffer
 * \param fnamelen the length of the specified buffer.
 * \param playerName the player name
 * \return true if a player filename was successfully generated
 */
bool PlayerGetFileName(
	char *fname, const size_t fnamelen,
	const char *playerName) {
  register bool result = false;
  if (!fname && fnamelen) {
    Log("invalid `fname` buffer");
  } else if (*StringBlank(playerName) == '\0') {
    Log("invalid `playerName` string");
  } else {
    result = UtilityGetFileName(
	fname, fnamelen,
	PLAYER_DIRECTORY,
	PLAYER_FILE_EXTENSION,
	playerName);
  }
  return (result);
}

/*!
 * Loads a player object.
 * \addtogroup player
 * \param game the game state
 * \param playerName the player name of the player to load
 * \return the object that represents the player indicated
 *     by the specified player name
 * \sa PlayerSave(Game*, Object*)
 */
Object *PlayerLoad(
	Game *game,
	const char *playerName) {
  register Object *object = NULL;
  if (!game) {
    Log("invalid `game` Game");
  } else if (*StringBlank(playerName) == '\0') {
    Log("invalid `playerName` string");
  } else if (PlayerByName(game, playerName) != NULL) {
    char fname[PATH_MAX] = {'\0'};
    if (!PlayerGetFileName(fname, sizeof(fname), playerName)) {
      Log("Couldn't create filename for player `%s`", playerName);
    } else {
      Data *fromData = DataLoadFile(fname);
      if (!fromData) {
	Log("Couldn't load pfile `%s`", fname);
      } else {
	object = ObjectAlloc(game);
	ObjectParse(fromData, object);
	DataFree(fromData);
      }
    }
  }
  return (object);
}

#define PLAYER_INDEX_FILE	"data/player/index"

/*!
 * Loads the player index.
 * \addtogroup player
 * \param game the game state
 * \sa PlayerSaveIndex(Game*)
 */
void PlayerLoadIndex(Game *game) {
  if (!game) {
    Log("invalid `game` Game");
  } else {
    FILE *stream = fopen(PLAYER_INDEX_FILE, "rt");
    if (!stream) {
      Log("Couldn't open player index file `%s` for reading", PLAYER_INDEX_FILE);
    } else {
      const char *ptr = NULL;
      char line[MAXLEN_INPUT] = {'\0'};
      while ((ptr = fgets(line, sizeof(line), stream)) != NULL) {
	/* Read player name */
	char playerName[MAXLEN_INPUT] = {'\0'};
	ptr = StringOneWord(playerName, sizeof(playerName), ptr);

	/* Read trust name */
	char trustName[MAXLEN_INPUT] = {'\0'};
	ptr = StringOneWord(trustName, sizeof(trustName), ptr);

	/* Create the player */
	Player *player = PlayerAlloc();
	StringSet(&player->playerName, playerName);
	if ((player->trust = BlockSearch(_G_trustTypes, trustName)) < 0)
	  player->trust = TRUST_PLAYER;

	/* Store the player information */
	PlayerStore(game, player);
	PlayerFree(player);
      }
      fclose(stream);
    }

    /* Calculate some metrics */
    register size_t nPlayers = 0;
    register size_t nPlayerBytes = 0;
    RBTreeForEach(game->players, tPlayerNode) {
      /* Iterator variable */
      const Player *tPlayer = tPlayerNode->value;

      /* Updage byte and record counts */
      nPlayerBytes += PlayerCountBytes(tPlayer);
      nPlayers++;
    }
    RBTreeForEachEnd();
    Log("Loaded %zu player(s), %zu byte(s).", nPlayers, nPlayerBytes);
  }
}

/*!
 * Renames a player.
 * \addtogroup player
 * \param game the game state
 * \param playerName the player name of the player to rename
 * \param newPlayerName the new player name
 * \return true if the player indicated by the specified player name
 *     was successfully renamed
 */
bool PlayerRename(
        Game *game,
        const char *playerName,
        const char *newPlayerName) {
  register bool result = false;
  if (!game) {
    Log("invalid `game` Game");
  } else if (!UtilityNameValid(playerName)) {
    Log("invalid `playerName` string");
  } else if (!UtilityNameValid(newPlayerName)) {
    Log("invalid `newPlayerName` string");
  } else if (StringCmp(playerName, newPlayerName) != 0) {
    register Player *player;
    if ((player = PlayerByName(game, playerName)) == NULL) {
      Log("player `%s` does not exist", playerName);
    } else if (PlayerByName(game, newPlayerName) != NULL) {
      Log("new player name `%s` already assigned", newPlayerName);
    } else {
      /* Find or load player object */
      register Object *playerObject;
      if ((playerObject = ObjectByPlayerName(game, playerName)) != NULL)
	playerObject = PlayerLoad(game, playerName);

      /* Rename player index */
      if (RBTreeDeleteNoFree(game->players, (RBTreeKey) &playerName)) {
	StringSet(&player->playerName, newPlayerName);
	result = RBTreeInsert(game->players, (RBTreeKey) &player->playerName, player);
      }

      /* Rename player object */
      if (result && playerObject)
	StringSet(&playerObject->playerData->playerName, newPlayerName);

      /* Save player and player index */
      if (result && playerObject) {
	PlayerSave(game, playerObject);
	PlayerSaveIndex(game);
      }

      if (result) {
	/* Pfile filename */
	char fname[PATH_MAX];
	if (!PlayerGetFileName(fname, sizeof(fname), playerName)) {
	  Log("Couldn't create filename for player name `%s`", playerName);
	} else if (unlink(fname) < 0 && errno != ENOENT) {
	  Log("unlink() failed: fname=%s, errno=%d", fname, errno);
	}
      }

      /* Cleanup player object */
      if (ObjectByPlayerName(game, newPlayerName) != playerObject)
	ObjectFree(playerObject);
    }
  }
  return (result);
}

/*!
 * Saves a player object.
 * \addtogroup player
 * \param game the game state
 * \param object the game object to save
 * \return true if the player represented by the specified
 *     game object was successfully saved
 * \sa PlayerLoad(Game*, const char*)
 */
bool PlayerSave(
	Game *game,
	Object *object) {
  register bool result = false;
  if (!object) {
    Log("invalid `player` Object");
  } else if (!object->playerData) {
    Log("object `%s` missing player special data", object->name);
  } else {
    char fname[PATH_MAX] = {'\0'};
    if (!PlayerGetFileName(fname, sizeof(fname), object->playerData->playerName)) {
      Log("Couldn't create filename for player `%s`", object->playerData->playerName);
    } else {
      /* Save the pfile */
      Data *toData = DataAlloc();
      ObjectEmit(toData, object);

      /* Save the pfile */
      result = DataSaveFile(toData, fname);
      DataFree(toData);

      if (!result)
	Log("Couldn't save pfile `%s`", fname);
      else {
	/* Create the player */
	Player *player = PlayerAlloc();
	StringSet(&player->playerName, object->playerData->playerName);
	player->trust = object->playerData->trust;

	/* Update the player index */
	if (!PlayerStore(game, player))
	  Log("Couldn't add player `%s` to player index", player->playerName);

	PlayerFree(player);
      }
    }
  }
  return (result);
}

/*!
 * Saves the player index.
 * \addtogroup player
 * \param game the game state
 * \sa PlayerLoadIndex(Game*)
 */
void PlayerSaveIndex(Game *game) {
  if (!game) {
    Log("invalid `game` Game");
  } else {
    FILE *stream = fopen(PLAYER_INDEX_FILE, "wt");
    if (!stream) {
      Log("Couldn't open player index file `%s` for writing", PLAYER_INDEX_FILE);
    } else {
      char trustName[MAXLEN_INPUT];
      RBTreeForEach(game->players, tPlayerNode) {
	/* Iterator variable */
	const Player *tPlayer = tPlayerNode->value;

	/* Emit player information */
	BlockPrintType(trustName, sizeof(trustName), _G_trustTypes, tPlayer->trust);
	fprintf(stream, "%s %s\n", tPlayer->playerName, trustName);
      }
      RBTreeForEachEnd();
      fclose(stream);
    }
  }
}

/*!
 * Stores a copy of a player.
 * \addtogroup player
 * \param game the game state
 * \param player the player to store
 * \return a copy of the specified player or NULL
 */
Player *PlayerStore(
	Game *game,
	const Player *player) {
  register Player *copied = NULL;
  if (!game) {
    Log("invalid `game` Game");
  } else if (!player) {
    Log("invalid `player` Player");
  } else {
    copied = PlayerByName(game, player->playerName);
    if (!copied) {
      copied = PlayerAlloc();
      StringSet(&copied->playerName, player->playerName);
      if (!RBTreeInsert(game->players, (RBTreeKey) &copied->playerName, copied)) {
	Log("Couldn't add player `%s` to player index", copied->playerName);
	PlayerFree(copied), copied = NULL;
      }
    }
    if (copied)
      PlayerCopy(copied, player);
  }
  return (copied);
}
