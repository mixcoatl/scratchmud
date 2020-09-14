/*!
 * \file object.c
 *
 * \par Copyright
 * Copyright (C) 1999-2019 scratchmud.org
 * All rights reserved.
 *
 * \author Geoffrey Davis <gdavis@scratchmud.org>
 * \addtogroup object
 */
#define _SCRATCH_OBJECT_C_

#include <scratch/bitvector.h>
#include <scratch/client.h>
#include <scratch/constant.h>
#include <scratch/data.h>
#include <scratch/game.h>
#include <scratch/gender.h>
#include <scratch/log.h>
#include <scratch/memory.h>
#include <scratch/object.h>
#include <scratch/player.h>
#include <scratch/rbtree.h>
#include <scratch/scratch.h>
#include <scratch/string.h>
#include <scratch/utility.h>

/*!
 * Constructs a new game object.
 * \addtogroup object
 * \param game the game state
 * \return the new game object or NULL
 * \sa ObjectFree(Object*)
 */
Object *ObjectAlloc(Game *game) {
  register Object *object = NULL;
  if (!game) {
    Log("invalid `game` Game");
  } else {
    MemoryCreate(object, Object, 1);
    object->client = NULL;
    object->description = NULL;
    object->game = game;
    object->gender = NULL;
    object->genderName = NULL;
    object->name = NULL;
    object->playerData = NULL;
    object->weight = 0.0;
  }
  return (object);
}

/*!
 * Retrieves a game object.
 * \addtogroup object
 * \param game the game state
 * \param objectName the object name of the game object to return
 */
Object *ObjectByName(
	Game *game,
	const char *objectName) {
  register Object *object = NULL;
  if (!game) {
    Log("invalid `game` Game");
  } else if (*StringBlank(objectName) == '\0') {
    Log("invalid `objectName` string");
  } else {
    object = RBTreeGetValue(game->objects, (RBTreeKey) &objectName, NULL);
  }
  return (object);
}

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
	const char *playerName) {
  if (!game) {
    Log("invalid `game` Game");
  } else if (*StringBlank(playerName) == '\0') {
    Log("invalid `playerName` string");
  } else {
    RBTreeForEach(game->objects, tObjectNode) {
      /* Iterator variable */
      Object *tObject = tObjectNode->value;

      /* Check player special data */
      if (!tObject->playerData)
	continue;

      /* Check player name */
      if (StringCmpCI(tObject->playerData->playerName, playerName) == 0)
	return (tObject);
    }
    RBTreeForEachEnd();
  }
  return (NULL);
}

/*!
 * Copies a game object.
 * \addtogroup object
 * \param toObject the location of the copied game object
 * \param fromObject the game object to copy
 */
static void ObjectCopyObjectData(
	Object *toObject,
	const Object *fromObject) {
  if (!toObject) {
    Log("invalid `toObject` Object");
  } else if (!fromObject) {
    Log("invalid `fromObject` Object");
  } else if (toObject != fromObject) {
    /* Description */
    StringSet(&toObject->description, fromObject->description);

    /* Gender */
    StringSet(&toObject->genderName, fromObject->genderName);
    toObject->gender = GenderByName(toObject->game, toObject->genderName);

    /* Weight */
    toObject->weight = fromObject->weight;
  }
}

/*!
 * Copies a game object.
 * \addtogroup object
 * \param toObject the location of the copied game object
 * \param fromObject the game object to copy
 */
static void ObjectCopyPlayerData(
	Object *toObject,
	const Object *fromObject) {
  if (!toObject) {
    Log("invalid `toObject` Object");
  } else if (!fromObject) {
    Log("invalid `fromObject` Object");
  } else if (toObject != fromObject) {
    /* If needed, free player special data */
    if (toObject->playerData && !fromObject->playerData) {
      PlayerDataFree(toObject->playerData);
      toObject->playerData = NULL;
    }

    /* If needed, create player special data */
    if (!toObject->playerData && fromObject->playerData)
      toObject->playerData = PlayerDataAlloc();

    /* Copy player special data */
    if (toObject->playerData && fromObject->playerData)
      PlayerDataCopy(toObject->playerData, fromObject->playerData);
  }
}

/*!
 * Copies a game object.
 * \addtogroup object
 * \param toObject the location of the copied game object
 * \param fromObject the game object to copy
 */
void ObjectCopy(
	Object *toObject,
	const Object *fromObject) {
  if (!toObject) {
    Log("invalid `toObject` Object");
  } else if (!fromObject) {
    Log("invalid `fromObject` Object");
  } else if (toObject != fromObject) {
    ObjectCopyObjectData(toObject, fromObject);
    ObjectCopyPlayerData(toObject, fromObject);
  }
}

/*!
 * Emits a game object.
 * \addtogroup object
 * \param toData the data to which to write
 * \param fromObject the game object to emit
 */
void ObjectEmitObjectData(
	Data *toData,
	const Object *fromObject) {
  if (!toData) {
    Log("invalid `toData` Data");
  } else if (!fromObject) {
    Log("invalid `fromObject` Object");
  } else {
    if (*StringBlank(fromObject->description) != '\0')
      DataPutString(toData, "Description", fromObject->description);
    if (fromObject->gender)
      DataPutString(toData, "Gender", fromObject->gender->name);
    else if (*StringBlank(fromObject->genderName) != '\0')
      DataPutString(toData, "Gender", fromObject->genderName);
    if (fromObject->weight)
      DataPutNumber(toData, "Weight", fromObject->weight);
  }
}

/*!
 * Emits a game object.
 * \addtogroup object
 * \param toData the data to which to write
 * \param fromObject the game object to emit
 */
void ObjectEmitPlayerData(
	Data *toData,
	const Object *fromObject) {
  if (!toData) {
    Log("invalid `toData` Data");
  } else if (!fromObject) {
    Log("invalid `fromObject` Object");
  } else {
    if (fromObject->playerData)
      PlayerDataEmit(toData, fromObject->playerData);
  }
}

/*!
 * Emits a game object.
 * \addtogroup object
 * \param toData the data to which to write
 * \param fromObject the game object to emit
 * \sa ObjectParse(Data*, Object*)
 */
void ObjectEmit(
	Data *toData,
	const Object *fromObject) {
  if (!toData) {
    Log("invalid `toData` Data");
  } else if (!fromObject) {
    Log("invalid `fromObject` Object");
  } else {
    Data *xObject = DataAlloc();
    ObjectEmitObjectData(xObject, fromObject);
    ObjectEmitPlayerData(xObject, fromObject);

    if (DataSize(xObject) == 0) {
      DataFree(xObject);
    } else if (fromObject->playerData) {
      DataPut(toData, "Player", xObject);
      DataSort(xObject);
    } else if (*StringBlank(fromObject->name) != '\0') {
      DataPut(toData, fromObject->name, xObject);
      DataSort(xObject);
    } else {
      DataFree(xObject);
    }
  }
}

/*!
 * Extracts a game object.
 * \addtogroup object
 * \param game the game state
 * \param object the game object to extract
 */
void ObjectExtract(
	Game *game,
	Object *object) {
  if (!game) {
    Log("invalid `game` Game");
  } else if (!object) {
    Log("invalid `object` Object");
  } else {
    if (object->client && object->client->player == object) {
      RBTreeDeleteNoFree(game->objects, (RBTreeKey) &object->name);
    } else if (ObjectByName(game, object->name) == object) {
      RBTreeDelete(game->objects, (RBTreeKey) &object->name);
    } else {
      ObjectFree(object);
    }
  }
}

/*!
 * Frees a game object.
 * \addtogroup object
 * \param object the game object to free
 * \sa ObjectAlloc(Game*)
 */
void ObjectFree(Object *object) {
  if (object) {
    if (object->client && object->client->player == object) {
      ClientClose(object->client);
      object->client->player = NULL;
      object->client = NULL;
    }
    StringFree(object->description);
    StringFree(object->genderName);
    StringFree(object->name);
    PlayerDataFree(object->playerData);
    MemoryFree(object);
  }
}

/*!
 * \brief Returns a non-canonical object name.
 * If the specified game object is a player this macro returns
 * the player's player name. Otherwise it returns the game object's
 * canonical name. This macro never returns NULL.
 * \addtogroup object
 * \param object the game object whose name to return
 * \return a non-canonical name for the specified object
 */
const char *ObjectGetName(const Object *object) {
  if (!object) {
    Log("invalid `object` Object");
  } else {
    const PlayerData *playerData = object->playerData;
    if (playerData && *StringBlank(playerData->playerName) != '\0')
      return StringBlank(playerData->playerName);
    if (*StringBlank(object->name) != '\0')
      return StringBlank(object->name);
  }
  return ("");
}

/*!
 * Parses a game object.
 * \addtogroup object
 * \param fromData the data to parse
 * \param toObject the location of the parsed game object
 */
static void ObjectParseObjectData(
	Data *fromData,
	Object *toObject) {
  if (!fromData) {
    Log("invalid `fromData` Data");
  } else if (!toObject) {
    Log("invalid `toObject` Object");
  } else {
    /* Description */
    StringFree(toObject->description);
    toObject->description = DataGetStringCopy(fromData, "Description", NULL);

    /* Gender */
    StringFree(toObject->genderName);
    toObject->genderName = DataGetStringCopy(fromData, "Gender", "Common");
    toObject->gender = GenderByName(toObject->game, toObject->genderName);

    /* Weight */
    toObject->weight = DataGetNumber(fromData, "Weight", 0.0);
  }
}

/*!
 * Parses a game object.
 * \addtogroup object
 * \param fromData the data to parse
 * \param toObject the location of the parsed game object
 */
static void ObjectParsePlayerData(
	Data *fromData,
	Object *toObject) {
  if (!fromData) {
    Log("invalid `fromData` Data");
  } else if (!toObject) {
    Log("invalid `toObject` Object");
  } else {
    /* Check for player special data */
    Data *xPlayerData = DataGet(fromData, "PlayerData");

    /* Create player special data */
    if (!toObject->playerData && xPlayerData)
      toObject->playerData = PlayerDataAlloc();

    /* Free player special data */
    if (toObject->playerData && !xPlayerData) {
      PlayerDataFree(toObject->playerData);
      toObject->playerData = NULL;
    }

    /* Parse player special data */
    if (toObject->playerData && xPlayerData)
      PlayerDataParse(xPlayerData, toObject->playerData);
  }
}

/*!
 * Parses a game object.
 * \addtogroup object
 * \param fromData the data to parse
 * \param toObject the location of the parsed game object
 * \sa ObjectEmit(Data*, const Object*)
 */
void ObjectParse(
	Data *fromData,
	Object *toObject) {
  if (!fromData) {
    Log("invalid `fromData` Data");
  } else if (!toObject) {
    Log("invalid `toObject` Object");
  } else {
    const char *xObjectKey = DataKeyAt(fromData, 0);
    Data *xObject = DataValueAt(fromData, 0);
    if (StringCmpCI(xObjectKey, "Player") == 0)
      ObjectRename(toObject, NULL);
    else {
      StringFree(toObject->name);
      toObject->name = strdup(xObjectKey);
    }
    ObjectParseObjectData(xObject, toObject);
    ObjectParsePlayerData(xObject, toObject);
  }
}
/*!
 * Prints a message.
 * \addtogroup object
 * \param object the game object to which to print a message
 * \param format the printf-style format specifier string
 */
void ObjectPrint(
	Object *object,
	const char *format, ...) {
  if (!object) {
    Log("invalid `object` Object");
  } else if (*StringBlank(format) != '\0') {
    /* Format message */
    va_list args;
    va_start(args, format);
    char messg[MAXLEN_STRING] = {'\0'};
    vsnprintf(messg, sizeof(messg), format, args);
    va_end(args);

    /* And send the message */
    if (object->client)
      ClientPrint(object->client, "%s", messg);
  }
}

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
	const char *objectName) {
  if (!object) {
    Log("invalid `object` Object");
  } else {
    /* Copy the object name */
    char rObjectName[MAXLEN_INPUT] = {'\0'};
    strlcpy(rObjectName, StringBlank(objectName), sizeof(rObjectName));

    /* Looks like we need to pick a random object name */
    while (*StringBlank(rObjectName) == '\0') {
      if (!UtilityNameGenerate(rObjectName, sizeof(rObjectName)))
	MemoryZero(rObjectName, char, sizeof(rObjectName));
      else if (ObjectByName(object->game, rObjectName) != NULL)
	MemoryZero(rObjectName, char, sizeof(rObjectName));
    }

    /* Remove from object index if necessary */
    register bool insert = false;
    if (object->name && ObjectByName(object->game, object->name) == object) {
      RBTreeDeleteNoFree(object->game->objects, (RBTreeKey) &object->name);
      insert = true;
    }

    /* The specified object name is used */
    if (ObjectByName(object->game, rObjectName) != NULL) {
      Log("object name `%s` already used", rObjectName);
    } else {
      /* Set the new object name */
      StringSet(&object->name, rObjectName);

      /* Insert into object index is necessary */
      if (insert && !RBTreeInsert(object->game->objects, (RBTreeKey) &object->name, object)) {
	Log("Couldn't add object `%s` to object index", object->name);
	ObjectFree(object), object = NULL;
      }
    }
  }
}

/*!
 * Resolves user input to a game object.
 * \addtogroup object
 * \param actor the game object to resolve the user input
 * \param input the user input to resolve
 * \return the game object indicated by the specified user input
 */
Object *ObjectResolve(
	Object *actor,
	const char *input) {
  if (!actor) {
    Log("invalid `actor` Object");
  } else if (!actor->game) {
    Log("object `%s` missing game state", actor->name);
  } else if (*StringBlank(input) != '\0') {
    if (StringCmpCI(input, "me") == 0) {
      return (actor);
    } else if (*input == '#' && *(input + 1) != '\0') {
      return ObjectByName(actor->game, input + 1);
    } else {
      /* Look for an exact match */
      RBTreeForEach(actor->game->objects, tObjectNode) {
	/* Iterator variable */
	Object *tObject = tObjectNode->value;

	/* Check for exact name match */
	if (StringCmpCI(ObjectGetName(tObject), input) == 0)
	  return (tObject);
      }
      RBTreeForEachEnd();

      /* Look for an prefix match instead */
      RBTreeForEach(actor->game->objects, tObjectNode) {
	/* Iterator variable */
	Object *tObject = tObjectNode->value;

	/* Check for partial name match */
	if (StringPrefixCI(ObjectGetName(tObject), input))
	  return (tObject);
      }
      RBTreeForEachEnd();
    }
  }
  return (NULL);
}
