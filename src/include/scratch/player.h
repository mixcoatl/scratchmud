/*!
 * \file player.h
 *
 * \par Copyright
 * Copyright (C) 1999-2019 scratchmud.org
 * All rights reserved.
 *
 * \author Geoffrey Davis <gdavis@scratchmud.org>
 * \addtogroup player
 */
#ifndef _SCRATCH_PLAYER_H_
#define _SCRATCH_PLAYER_H_

#include <scratch/scratch.h>

/*!
 * Player preference bits.
 * \addtogroup player
 * \{
 */
#define PREF_AUTOSAY    (0)     /*!< Say non-command input */
#define PREF_COMPACT    (1)     /*!< Omit extra blank line before prompt */
#define PREF_NO_REPEAT  (2)     /*!< Do not repeat communication commands */
/*! \} */

/*! How many PREF_x bits? */
#define MAX_PREF_BITS \
  (PREF_NO_REPEAT - PREF_AUTOSAY + 1)

/*!
 * Trust levels.
 * \addtogroup player
 * \{
 */
#define TRUST_NONE      (0)     /*!< No rights whatsoever */
#define TRUST_GUEST     (1)     /*!< Mobiles and controlled access */
#define TRUST_PLAYER    (2)     /*!< Almost everyone is this trust */
#define TRUST_WIZARD    (3)     /*!< Admins without shell access */
#define TRUST_OWNER     (4)     /*!< Admins with shell access */
/*! /} */

/*! How many TRUST_x types? */
#define MAX_TRUST_TYPES \
  (TRUST_OWNER - TRUST_NONE + 1)

/* Forward type declarations */
typedef struct _Data       Data;
typedef struct _Game       Game;
typedef struct _Object     Object;
typedef struct _Player     Player;
typedef struct _PlayerData PlayerData;

/*!
 * Player information.
 * \addtogroup player
 * \{
 */
struct _Player {
  char         *playerName;     /*!< The player's name */
  int           trust;          /*!< The player's trust: TRUST_x */
};
/*! /} */

/*!
 * Player special data.
 * \addtogroup player
 * \{
 */
struct _PlayerData {
  char         *password;       /*!< The password hash */
  char         *playerName;     /*!< The player name */
  Bitvector     preferenceBits; /*!< The player's preference bits: PREF_x */
  int           trust;          /*!< The player's trust: TRUST_x */
};
/*! \} */

/*!
 * Constructs a new player.
 * \addtogroup player
 * \return the new player or NULL
 * \sa PlayerFree(Player*)
 */
Player *PlayerAlloc(void);

/*!
 * Retrieves a player.
 * \addtogroup player
 * \param game the game state
 * \param playerName the player name of the player to return
 * \return the player indicated by the specified player name or NULL
 */
Player *PlayerByName(
	Game *game,
	const char *playerName);

/*!
 * Copies a player.
 * \addtogroup player
 * \param toPlayer the location of the copied player
 * \param fromPlayer the player to copy
 */
void PlayerCopy(
	Player *toPlayer,
	const Player *fromPlayer);

/*!
 * Returns the size of a player in bytes.
 * \addtogroup player
 * \param player the player whose size to return
 * \return the size of the specified player in bytes
 */
size_t PlayerCountBytes(const Player *player);

/*!
 * Constructs new player special data.
 * \addtogroup player
 * \return the new player special data or NULL
 * \sa PlayerDataFree(PlayerData*)
 */
PlayerData *PlayerDataAlloc(void);

/*!
 * Copies player special data.
 * \addtogroup player
 * \param toPlayerData the location of the copied player special data
 * \param fromPlayerData the player special data to copy
 */
void PlayerDataCopy(
	PlayerData *toPlayerData,
	const PlayerData *fromPlayerData);

/*!
 * Emits player special data.
 * \addtogroup player
 * \param toData the data to which to write
 * \param fromPlayerData the player special data to emit
 * \sa PlayerDataParse(Data*, PlayerData*)
 */
void PlayerDataEmit(
	Data *toData,
	const PlayerData *fromPlayerData);

/*!
 * Frees player special data.
 * \addtogroup player
 * \param playerData the player special data to free
 * \sa PlayerDataAlloc()
 */
void PlayerDataFree(PlayerData *playerData);

/*!
 * Parses player special data.
 * \addtogroup player
 * \param fromData the data to parse
 * \param toPlayerData the location of the parsed player special data
 * \sa PlayerDataEmit(Data*, const PlayerData*)
 */
void PlayerDataParse(
	Data *fromData,
	PlayerData *toPlayerData);

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
	const char *playerName);

/*!
 * Frees a player.
 * \addtogroup player
 * \param player the player to free
 * \sa PlayerAlloc(Game*)
 */
void PlayerFree(Player *player);

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
	const char *playerName);

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
	const char *playerName);

/*!
 * Loads the player index.
 * \addtogroup player
 * \param game the game state
 * \sa PlayerSaveIndex(Game*)
 */
void PlayerLoadIndex(Game *game);

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
	const char *newPlayerName);

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
	Object *object);

/*!
 * Saves the player index.
 * \addtogroup player
 * \param game the game state
 * \sa PlayerLoadIndex(Game*)
 */
void PlayerSaveIndex(Game *game);

/*!
 * Stores a copy of a player.
 * \addtogroup player
 * \param game the game state
 * \param player the player to store
 * \return a copy of the specified player or NULL
 */
Player *PlayerStore(
	Game *game,
	const Player *player);

#endif /* _SCRATCH_PLAYER_H_ */
