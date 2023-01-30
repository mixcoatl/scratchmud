/*!
 * \file user.h
 *
 * \par Copyright
 * Copyright (C) 1999-2023 scratchmud.org
 * All rights reserved.
 *
 * \author Geoffrey Davis <gdavis@scratchmud.org>
 * \addtogroup user
 */
#ifndef _SCRATCH_USER_H_
#define _SCRATCH_USER_H_

#include <scratch/scratch.h>

/* Forward type declarations */
typedef struct Data Data;
typedef struct Descriptor Descriptor;
typedef struct Game Game;
typedef struct User User;

/*!
 * The user structure.
 * \addtogroup user
 * \{
 */
struct User {
  char                 *email;          /*!< The user's email address */
  Game                 *game;           /*!< The game state */
  time_t                lastLogoff;     /*!< The last log-off timestamp */
  time_t                lastLogon;      /*!< The last log-on timestamp */
  char                 *password;       /*!< The salted password hash */
  char                 *plan;           /*!< The user's plan */
  char                 *userId;         /*!< The user's user ID */
};
/*! \} */

/*!
 * Constructs a new user.
 * \addtogroup user
 * \param game the game state
 * \return the new user or NULL
 * \sa UserFree(User*)
 * \sa UserFreeV(void*)
 */
User *UserAlloc(Game *game);

/*!
 * Searches for a user using its email address.
 * \addtogroup user
 * \param game the game state
 * \param email the email address of the user to return
 * \return the user indicated by the specified email address or NULL
 * \sa UserByUserId(Game*, const char*)
 */
User *UserByEmail(
	Game *game,
	const char *email);

/*!
 * Searches for a user using its user ID.
 * \addtogroup user
 * \param game the game state
 * \param userId the user ID of the user to return
 * \return the user indicated by the specified user ID or NULL
 * \sa UserByEmail(Game*, const char*)
 */
User *UserByUserId(
	Game *game,
	const char *userId);

/*!
 * Copies a user.
 * \addtogroup user
 * \param toUser the location of the copied user
 * \param fromUser the user to copy
 */
void UserCopy(
	User *toUser,
	const User *fromUser);

/*!
 * Returns the size of a user in bytes.
 * \addtogroup user
 * \param user the user whose size to return
 * \return the size of the specified user in bytes
 */
size_t UserCountBytes(const User *user);

/*!
 * Deletes a user.
 * \addtogroup user
 * \param game the game state
 * \param userId the user ID of the user to delete
 * \return true if the user indicated by the specified user ID was
 *     successfully deleted
 */
bool UserDelete(
	Game *game,
	const char *userId);

/*!
 * Emits a user.
 * \addtogroup user
 * \param toData the data element to which to write
 * \param fromUser the user to emit
 * \sa UserParse(Data*, User*)
 */
void UserEmit(
	Data *toData,
	const User *fromUser);

/*!
 * Frees a user.
 * \addtogroup user
 * \param user the user to free
 * \sa UserAlloc(Game*)
 * \sa UserFreeV(void*)
 */
void UserFree(User *user);

/*!
 * Frees a user.
 * \addtogroup user
 * \param user the user to free
 * \sa UserAlloc(Game*)
 * \sa UserFree(User*)
 */
void UserFreeV(void *user);

/*!
 * Generates a user filename.
 * \addtogroup user
 * \param fname the user filename buffer
 * \param fnamelen the length of the specified buffer.
 * \param userId the user name
 * \return true if a user filename was successfully generated
 */
bool UserGetFileName(
	char *fname, const size_t fnamelen,
	const char *userId);

/*!
 * Loads a user.
 * \addtogroup user
 * \param game the game state
 * \param userId the user name of the user to load
 * \return the loaded user or NULL
 * \sa UserSave(Game*, User*)
 */
User *UserLoad(
	Game *game,
	const char *userId);

/*!
 * Loads the user index.
 * \addtogroup user
 * \param game the game state
 * \sa UserSaveIndex(Game*)
 */
void UserLoadIndex(Game *game);

/*!
 * Parses a user.
 * \addtogroup user
 * \param fromData the data element to parse
 * \param toUser the location of the parsed user
 * \sa UserEmit(Data*, const User*)
 */
void UserParse(
	Data *fromData,
	User *toUser);

/*!
 * Saves a user.
 * \addtogroup user
 * \param game the game state
 * \param user the user to save
 * \sa UserLoad(Game*, const char*)
 */
void UserSave(
	Game *game,
	const User *user);

/*!
 * Saves the user index.
 * \addtogroup user
 * \param game the game state
 * \sa UserLoadIndex(Game*)
 */
void UserSaveIndex(Game *game);

/*!
 * Starts the user creator.
 * \addtogroup creator
 * \addtogrup user
 * \param d the descriptor whose creator to start
 * \param user the user to modify or NULL
 */
void UserStartCreator(
	Descriptor *d,
	const User *user);

/*!
 * Stores a copy of a user.
 * \addtogroup user
 * \param game the game state
 * \param user the user to store
 * \return a copy of the specified user or NULL
 */
User *UserStore(
	Game *game,
	const User *user);

#endif /* _SCRATCH_USER_H_ */
