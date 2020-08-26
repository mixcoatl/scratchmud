/*!
 * \file gender.h
 *
 * \par Copyright
 * Copyright (C) 1999-2019 scratchmud.org
 * All rights reserved.
 *
 * \author Geoffrey Davis <gdavis@scratchmud.org>
 * \addtogroup gender
 */
#ifndef _SCRATCH_GENDER_H_
#define _SCRATCH_GENDER_H_

#include <scratch/scratch.h>

/* Forward type declarations */
typedef struct _Data   Data;
typedef struct _Game   Game;
typedef struct _Gender Gender;

/*!
 * Gender.
 * \addtogroup gender
 * \{
 */
struct _Gender {
  char         *copula;         /*!< The copula: is, are */
  char         *determiner;     /*!< The determiner adjective */
  char         *name;           /*!< The gender name */
  char         *pronounObject;     /*!< The object pronoun */
  char         *pronounPossessive; /*!< The possessive pronoun */
  char         *pronounSubject;    /*!< The subject pronoun */
};
/*! \} */

/*!
 * Constructs a new gender.
 * \addtogroup gender
 * \sa GenderFree(Gender*)
 */
Gender *GenderAlloc(void);

/*!
 * Retrieves a gender.
 * \addtogroup gender
 * \param game the game state
 * \param genderName the gender name of the gender to return
 * \return the gender indicated by the specified gender name or NULL
 */
Gender *GenderByName(
	Game *game,
	const char *genderName);

/*!
 * Returns the size of a gender in bytes.
 * \addtogroup gender
 * \param gender the gender whose size to return
 * \return the size of the specified gender in bytes
 */
size_t GenderCountBytes(const Gender *gender);

/*!
 * Copies a gender.
 * \addtogroup gender
 * \param toGender the location of the copied gender
 * \param fromGender the gender to copy
 */
void GenderCopy(
        Gender *toGender,
        const Gender *fromGender);

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
        const char *genderName);

/*!
 * Emits a gender.
 * \addtogroup gender
 * \param toData the data to which to write
 * \param fromGender the gender to emit
 * \sa GenderParse(Data*, Gender*)
 */
void GenderEmit(
	Data *toData,
        const Gender *fromGender);

/*!
 * Frees a gender.
 * \addtogroup gender
 * \param gender the gender to free
 * \sa GenderAlloc()
 */
void GenderFree(Gender *gender);

/*!
 * Loads the gender index.
 * \param game the game state
 * \addtogroup gender
 * \sa GenderSaveIndex(Game*)
 */
void GenderLoadIndex(Game *game);

/*!
 * Saves the gender index.
 * \param game the game state
 * \addtogroup gender
 * \sa GenderLoadIndex(Game*)
 */
void GenderSaveIndex(Game *game);

/*!
 * Parses a gender.
 * \addtogroup gender
 * \param fromData the data to parse
 * \param toGender the location of the parsed gender
 * \sa GenderEmit(Data*, const Gender*)
 */
void GenderParse(
        Data *fromData,
        Gender *toGender);

/*!
 * Stores a copy of a gender.
 * \addtogroup gender
 * \param game the game state
 * \param gender the gender to store
 * \return a copy of the specified gender or NULL
 */
Gender *GenderStore(
        Game *game,
        const Gender *gender);

#endif /* _SCRATCH_GENDER_H_ */
