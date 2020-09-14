/*!
 * \file object.h
 *
 * \par Copyright
 * Copyright (C) 1999-2019 scratchmud.org
 * All rights reserved.
 *
 * \author Geoffrey Davis <gdavis@scratchmud.org>
 * \addtogroup object
 */
#ifndef _SCRATCH_OBJECT_H_
#define _SCRATCH_OBJECT_H_

#include <scratch/bitvector.h>
#include <scratch/scratch.h>

/*!
 * \brief Returns a player's preference bits.
 * If the specified game object is a player this macro returns
 * the player's preference bits.  Otherwise it returns a value of
 * zero.  This macro never returns an invalid bitvector value.
 * \addtogroup player
 * \param object the game object whose player preference bits to return
 * \return the player preference bits for the specified game object
 */
#define GetPlayerPreferenceBits(object) \
  ((object) && \
   (object)->playerData ? \
   (object)->playerData->preferenceBits : 0)

/*!
 * \brief Returns a player's trust.
 * If the specified game object is a player this macro returns
 * the player's trust.  Otherwise it returns value of TRUST_NONE.
 * This macro never returns an undefined, out-of-range, or
 * otherwise invalid trust value.
 * \addtogroup player
 * \param object the game object whose trust to return
 * \return the trust for the specified game object
 */
#define GetPlayerTrust(object) \
  ((object) && \
   (object)->playerData ? \
   (object)->playerData->trust : TRUST_NONE)

/* Forward type declarations */
typedef struct _Client     Client;
typedef struct _Data       Data;
typedef struct _Game       Game;
typedef struct _Gender     Gender;
typedef struct _Object     Object;
typedef struct _PlayerData PlayerData;

/*!
 * One game object.
 * \addtogroup object
 * \{
 */
struct _Object {
  Client       *client;         /*!< The client using this object */
  char         *description;    /*!< The object description */
  Game         *game;           /*!< The game state */
  Gender       *gender;         /*!< The object gender */
  char         *genderName;     /*!< The object gender name */
  char         *name;           /*!< The object name */
  PlayerData   *playerData;     /*!< The player special data */
  double        weight;         /*!< The object weight */
};
/*! \} */

/*!
 * Constructs a new game object.
 * \addtogroup object
 * \param game the game state
 * \return the new game object or NULL
 * \sa ObjectFree(Object*)
 */
Object *ObjectAlloc(Game *game);

/*!
 * Retrieves a game object.
 * \addtogroup object
 * \param game the game state
 * \param objectName the object name of the game object to return
 * \return the game object indicated by the specified object name or NULL
 */
Object *ObjectByName(
	Game *game,
	const char *objectName);

/*!
 * Retrieves a game object.
 * \addtogroup object
 * \param game the game state
 * \param playerName the player name of the player
 * \return the game object that belongs to the player indicated
 *     by the specified player name or NULL
 */
Object *ObjectByPlayerName(
	Game *game,
	const char *playerName);

/*!
 * Copies a game object.
 * \addtogroup object
 * \param toObject the location of the copied game object
 * \param fromObject the game object to copy
 */
void ObjectCopy(
	Object *toObject,
	const Object *fromObject);

/*!
 * Emits a game object.
 * \addtogroup object
 * \param toData the data to which to write
 * \param fromObject the game object to emit
 * \sa ObjectParse(Data*, Object*)
 */
void ObjectEmit(
	Data *toData,
	const Object *fromObject);

/*!
 * Extracts a game object.
 * \addtogroup object
 * \param game the game state
 * \param object the game object to extract
 */
void ObjectExtract(
	Game *game,
	Object *object);

/*!
 * Frees a game object.
 * \addtogroup object
 * \param object the game object to free
 * \sa ObjectAlloc(Game*)
 */
void ObjectFree(Object *object);

/*!
 * \brief Returns a non-canonical object name.
 * If the specified game object is a player this macro returns
 * the player's player name. Otherwise it returns the game object's
 * canonical name. This macro never returns NULL.
 * \addtogroup object
 * \param object the game object whose name to return
 * \return a non-canonical name for the specified object
 */
const char *ObjectGetName(const Object *object);

/*!
 * Parses a game object.
 * \addtogroup object
 * \param fromData the data to parse
 * \param toObject the location of the parsed game object
 * \sa ObjectEmit(Data*, const Object*)
 */
void ObjectParse(
	Data *fromData,
	Object *toObject);

/*!
 * Prints a message.
 * \addtogroup object
 * \param object the game object to which to print a message
 * \param format the printf-style format specifier string
 */
void ObjectPrint(
	Object *object,
	const char *format, ...);

/*!
 * Renames a game object.
 * \addtogroup object
 * \param object the game object to rename
 * \param objectName the new object name to which to rename
 *     the specified game object. This function will generate
 *     a new, random object name if a value of NULL or empty
 *     string ("") is passed
 */
void ObjectRename(
	Object *object,
	const char *objectName);

/*!
 * Resolves user input to a game object.
 * \addtogroup object
 * \param actor the game object to resolve the user input
 * \param input the user input to resolve
 * \return the game object indicated by the specified user input
 */
Object *ObjectResolve(
	Object *actor,
	const char *input);

#endif /* _SCRATCH_OBJECT_H_ */
