/*!
 * \file gender.c
 *
 * \par Copyright
 * Copyright (C) 1999-2019 scratchmud.org
 * All rights reserved.
 *
 * \author Geoffrey Davis <gdavis@scratchmud.org>
 * \addtogroup gender
 */
#define _SCRATCH_GENDER_C_

#include <scratch/data.h>
#include <scratch/game.h>
#include <scratch/gender.h>
#include <scratch/log.h>
#include <scratch/memory.h>
#include <scratch/rbtree.h>
#include <scratch/scratch.h>
#include <scratch/string.h>

/*!
 * Constructs a new gender.
 * \addtogroup gender
 * \sa GenderFree(Gender*)
 */
Gender *GenderAlloc(void) {
  register Gender *gender;
  MemoryCreate(gender, Gender, 1);
  gender->copula = NULL;
  gender->determiner = NULL;
  gender->name = NULL;
  gender->pronounObject = NULL;
  gender->pronounPossessive = NULL;
  gender->pronounSubject = NULL;
  return (gender);
}

/*!
 * Retrieves a gender.
 * \addtogroup gender
 * \param game the game state
 * \param genderName the gender name of the gender to return
 * \return the gender indicated by the specified gender name or NULL
 */
Gender *GenderByName(
	Game *game,
	const char *genderName) {
  register Gender *gender = NULL;
  if (!game) {
    Log("invalid `game` Game");
  } else if (*StringBlank(genderName) == '\0') {
    Log("invalid `genderName` string");
  } else {
    gender = RBTreeGetValue(game->genders, (RBTreeKey) &genderName, NULL);
  }
  return (gender);
}

/*!
 * Returns the size of a gender in bytes.
 * \addtogroup gender
 * \param gender the gender whose size to return
 * \return the size of the specified gender in bytes
 */
size_t GenderCountBytes(const Gender *gender) {
  register size_t nBytes = 0;
  if (gender) {
    if (gender->copula)
      nBytes += strlen(gender->copula) + 1;
    if (gender->determiner)
      nBytes += strlen(gender->determiner) + 1;
    if (gender->name)
      nBytes += strlen(gender->name) + 1;
    if (gender->pronounObject)
      nBytes += strlen(gender->pronounObject) + 1;
    if (gender->pronounPossessive)
      nBytes += strlen(gender->pronounPossessive) + 1;
    if (gender->pronounSubject)
      nBytes += strlen(gender->pronounSubject) + 1;
    nBytes += sizeof(Gender);
  }
  return (nBytes);
}

/*!
 * Copies a gender.
 * \addtogroup gender
 * \param toGender the location of the copied gender
 * \param fromGender the gender to copy
 */
void GenderCopy(
        Gender *toGender,
        const Gender *fromGender) {
  if (!toGender) {
    Log("invalid `toGender` Gender");
  } else if (!fromGender) {
    Log("invalid `fromGender` Gender");
  } else if (toGender != fromGender) {
    StringSet(&toGender->copula, fromGender->copula);
    StringSet(&toGender->determiner, fromGender->determiner);
    StringSet(&toGender->name, fromGender->name);
    StringSet(&toGender->pronounObject, fromGender->pronounObject);
    StringSet(&toGender->pronounPossessive, fromGender->pronounPossessive);
    StringSet(&toGender->pronounSubject, fromGender->pronounSubject);
  }
}

/*!
 * Deletes a gender.
 * \addtogroup gender
 * \param game the game state
 * \param genderName the gender name of the gender to delete
 * \return true if the gender indicated by the specified gender name
 *     was successfully deleted
 */
bool GenderDelete(
        Game *game,
        const char *genderName) {
  register bool result = false;
  if (!game) {
    Log("invalid `game` Game");
  } else {
    result = RBTreeDelete(game->genders, (RBTreeKey) &genderName);
  }
  return (result);
}

/*!
 * Emits a gender.
 * \addtogroup gender
 * \param toData the data to which to write
 * \param fromGender the gender to emit
 */
static void GenderEmitCopula(
	Data *toData,
        const Gender *fromGender) {
  if (!toData) {
    Log("invalid `toData` Data");
  } else if (!fromGender) {
    Log("invalid `fromGender` Gender");
  } else {
    if (*StringBlank(fromGender->copula) != '\0')
      DataPutString(toData, "Copula", fromGender->copula);
  }
}

/*!
 * Emits a gender.
 * \addtogroup gender
 * \param toData the data to which to write
 * \param fromGender the gender to emit
 */
static void GenderEmitDeterminer(
	Data *toData,
        const Gender *fromGender) {
  if (!toData) {
    Log("invalid `toData` Data");
  } else if (!fromGender) {
    Log("invalid `fromGender` Gender");
  } else {
    if (*StringBlank(fromGender->determiner) != '\0')
      DataPutString(toData, "Determiner", fromGender->determiner);
  }
}

/*!
 * Emits a gender.
 * \addtogroup gender
 * \param toData the data to which to write
 * \param fromGender the gender to emit
 */
static void GenderEmitPronouns(
	Data *toData,
        const Gender *fromGender) {
  if (!toData) {
    Log("invalid `toData` Data");
  } else if (!fromGender) {
    Log("invalid `fromGender` Gender");
  } else {
    Data *xPronouns = DataAlloc();
    if (*StringBlank(fromGender->pronounObject) != '\0')
      DataPutString(xPronouns, "Object", fromGender->pronounObject);
    if (*StringBlank(fromGender->pronounPossessive) != '\0')
      DataPutString(xPronouns, "Possessive", fromGender->pronounPossessive);
    if (*StringBlank(fromGender->pronounSubject) != '\0')
      DataPutString(xPronouns, "Subject", fromGender->pronounSubject);

    if (DataSize(xPronouns) == 0) {
      DataFree(xPronouns);
    } else {
      DataPut(toData, "Pronouns", xPronouns);
      DataSort(xPronouns);
    }
  }
}

/*!
 * Emits a gender.
 * \addtogroup gender
 * \param toData the data to which to write
 * \param fromGender the gender to emit
 * \sa GenderParse(Data*, Gender*)
 */
void GenderEmit(
	Data *toData,
        const Gender *fromGender) {
  if (!toData) {
    Log("invalid `toData` Data");
  } else if (!fromGender) {
    Log("invalid `fromGender` Gender");
  } else {
    Data *xGender = DataAlloc();
    GenderEmitCopula(xGender, fromGender);
    GenderEmitDeterminer(xGender, fromGender);
    GenderEmitPronouns(xGender, fromGender);

    if (DataSize(xGender) == 0) {
      DataFree(xGender);
    } else {
      DataPut(toData, fromGender->name, xGender);
      DataSort(xGender);
    }
  }
}

/*!
 * Frees a gender.
 * \addtogroup gender
 * \param gender the gender to free
 * \sa GenderAlloc()
 */
void GenderFree(Gender *gender) {
  if (gender) {
    StringFree(gender->copula);
    StringFree(gender->determiner);
    StringFree(gender->name);
    StringFree(gender->pronounObject);
    StringFree(gender->pronounPossessive);
    StringFree(gender->pronounSubject);
    MemoryFree(gender);
  }
}

#define GENDER_INDEX_FILE "data/gender.dat"

/*!
 * Loads the gender index.
 * \param game the game state
 * \addtogroup gender
 * \sa GenderSaveIndex(Game*)
 */
void GenderLoadIndex(Game *game) {
  if (!game) {
    Log("invalid `game` Game");
  } else {
    Data *root = DataLoadFile(GENDER_INDEX_FILE);
    if (!root) {
      Log("Couldn't load gender file `%s`", GENDER_INDEX_FILE);
    } else {
      /* Load up connection genders */
      Data *xGenderList = DataGet(root, "Genders");
      DataForEach(xGenderList, xGenderKey, xGender) {
        Gender *gender = GenderAlloc();
        StringSet(&gender->name, xGenderKey);
        GenderParse(xGender, gender);
        if (*StringBlank(gender->name) != '\0')
          GenderStore(game, gender);
        GenderFree(gender);
      }
      DataForEachEnd();
      DataFree(root);
    }

    /* Calculate some metrics */
    register size_t nGenderBytes = 0;
    register size_t nGenders = 0;
    RBTreeForEach(game->genders, tGenderNode) {
      /* Iterator variable */
      const Gender *tGender = tGenderNode->value;

      /* Update byte and record counts */
      nGenderBytes += GenderCountBytes(tGender);
      nGenders++;
    }
    RBTreeForEachEnd();
    Log("Loaded %zu gender(s), %zu byte(s).", nGenders, nGenderBytes);
  }
}

/*!
 * Parses a gender.
 * \addtogroup gender
 * \param fromData the data to parse
 * \param toGender the location of the parsed gender
 */
static void GenderParseCopula(
        Data *fromData,
        Gender *toGender) {
  if (!fromData) {
    Log("invalid `fromData` Data");
  } else if (!toGender) {
    Log("invalid `toGender` Gender");
  } else {
    StringFree(toGender->copula);
    toGender->copula = DataGetStringCopy(fromData, "Copula", "are");
  }
}

/*!
 * Parses a gender.
 * \addtogroup gender
 * \param fromData the data to parse
 * \param toGender the location of the parsed gender
 */
static void GenderParseDeterminer(
        Data *fromData,
        Gender *toGender) {
  if (!fromData) {
    Log("invalid `fromData` Data");
  } else if (!toGender) {
    Log("invalid `toGender` Gender");
  } else {
    StringFree(toGender->determiner);
    toGender->determiner = DataGetStringCopy(fromData, "Determiner", "their");
  }
}

/*!
 * Parses a gender.
 * \addtogroup gender
 * \param fromData the data to parse
 * \param toGender the location of the parsed gender
 */
static void GenderParsePronouns(
        Data *fromData,
        Gender *toGender) {
  if (!fromData) {
    Log("invalid `fromData` Data");
  } else if (!toGender) {
    Log("invalid `toGender` Gender");
  } else {
    Data *xPronouns = DataGet(fromData, "Pronouns");
    StringFree(toGender->pronounObject);
    toGender->pronounObject = DataGetStringCopy(xPronouns, "Object", "them");
    StringFree(toGender->pronounPossessive);
    toGender->pronounPossessive = DataGetStringCopy(xPronouns, "Possessive", "theirs");
    StringFree(toGender->pronounSubject);
    toGender->pronounSubject = DataGetStringCopy(xPronouns, "Subject", "they");
  }
}

/*!
 * Parses a gender.
 * \addtogroup gender
 * \param fromData the data to parse
 * \param toGender the location of the parsed gender
 * \sa GenderEmit(Data*, const Gender*)
 */
void GenderParse(
        Data *fromData,
        Gender *toGender) {
  if (!fromData) {
    Log("invalid `fromData` Data");
  } else if (!toGender) {
    Log("invalid `toGender` Gender");
  } else {
    GenderParseCopula(fromData, toGender);
    GenderParseDeterminer(fromData, toGender);
    GenderParsePronouns(fromData, toGender);
  }
}

/*!
 * Saves the gender index.
 * \param game the game state
 * \addtogroup gender
 * \sa GenderLoadIndex(Game*)
 */
void GenderSaveIndex(Game *game) {
  if (!game) {
    Log("invalid `game` Game");
  } else {
    Data *root = DataAlloc();
    Data *xGenderList = DataAlloc();
    RBTreeForEach(game->genders, tGenderNode) {
      /* Iterator variable */
      const Gender *tGender = tGenderNode->value;

      /* Emit gender information */
      GenderEmit(xGenderList, tGender);
    }
    RBTreeForEachEnd();

    if (DataSize(xGenderList) == 0) {
      DataFree(xGenderList);
    } else {
      DataPut(root, "Genders", xGenderList);
      DataSort(xGenderList);
    }
    if (!DataSaveFile(root, GENDER_INDEX_FILE))
      Log("Couldn't save gender file `%s`", GENDER_INDEX_FILE);

    DataFree(root);
  }
}

/*!
 * Stores a copy of a gender.
 * \addtogroup gender
 * \param game the game state
 * \param gender the gender to store
 * \return a copy of the specified gender or NULL
 */
Gender *GenderStore(
        Game *game,
        const Gender *gender) {
  register Gender *copied = NULL;
  if (!game) {
    Log("invalid `game` Game");
  } else if (!gender) {
    Log("invalid `gender` Gender");
  } else {
    copied = GenderByName(game, gender->name);
    if (!copied) {
      copied = GenderAlloc();
      StringSet(&copied->name, gender->name);
      if (!RBTreeInsert(game->genders, (RBTreeKey) &copied->name, copied)) {
        Log("Couldn't add gender `%s` to gender index", copied->name);
        GenderFree(copied), copied = NULL;
      }
    }
    if (copied)
      GenderCopy(copied, gender);
  }
  return (copied);
}
