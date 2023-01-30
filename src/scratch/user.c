/*!
 * \file user.c
 *
 * \par Copyright
 * Copyright (C) 1999-2023 scratchmud.org
 * All rights reserved.
 *
 * \author Geoffrey Davis <gdavis@scratchmud.org>
 * \addtogroup user
 */
#define _SCRATCH_USER_C_

#include <scratch/data.h>
#include <scratch/game.h>
#include <scratch/log.h>
#include <scratch/memory.h>
#include <scratch/scratch.h>
#include <scratch/string.h>
#include <scratch/tree.h>
#include <scratch/user.h>
#include <scratch/utility.h>

/*!
 * Constructs a new user.
 * \addtogroup user
 * \param game the game state
 * \return the new user or NULL
 * \sa UserFree(User*)
 * \sa UserFreeV(void*)
 */
User *UserAlloc(Game *game) {
  User *user = NULL;
  if (!game) {
    Log(L_ASSERT, "Invalid `game` Game.");
  } else {
    MemoryCreate(user, User, 1);
    user->email = NULL;
    user->game = NULL;
    user->lastLogoff = time(0) - 1;
    user->lastLogon = time(0);
    user->password = NULL;
    user->plan = NULL;
    user->userId = NULL;
  }
  return (user);
}

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
	const char *email) {
  if (!game) {
    Log(L_ASSERT, "Invalid `game` Game.");
  } else if (!email || *email == '\0') {
    Log(L_ASSERT, "Invalid `email` string.");
  } else {
    TreeForEach(game->users, tUserNode) {
      User *tUser = tUserNode->mappingValue;
      if (!StringCaseCompare(tUser->email, email))
	return (tUser);
    }
  }
  return (NULL);
}

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
	const char *userId) {
  register User *user = NULL;
  if (!game) {
    Log(L_ASSERT, "Invalid `game` Game.");
  } else if (!userId || *userId == '\0') {
    Log(L_ASSERT, "Invalid `userId` string.");
  } else {
    user = TreeGetValue(game->users, &userId, NULL);
  }
  return (user);
}

/*!
 * Copies a user.
 * \addtogroup user
 * \param toUser the location of the copied user
 * \param fromUser the user to copy
 */
void UserCopy(
	User *toUser,
	const User *fromUser) {
  if (!toUser) {
    Log(L_ASSERT, "Invalid `toUser` User.");
  } else if (!fromUser) {
    Log(L_ASSERT, "Invalid `fromUser` User.");
  } else if (toUser != fromUser) {
    StringSet(&toUser->email, fromUser->email);
    StringSet(&toUser->password, fromUser->password);
    StringSet(&toUser->plan, fromUser->plan);
    StringSet(&toUser->userId, fromUser->userId);
    toUser->lastLogoff = fromUser->lastLogoff;
    toUser->lastLogon  = fromUser->lastLogon;
  }
}

/*!
 * Returns the size of a user in bytes.
 * \addtogroup user
 * \param user the user whose size to return
 * \return the size of the specified user in bytes
 */
size_t UserCountBytes(const User *user) {
  register size_t nBytes = 0;
  if (user) {
    if (user->email)
      nBytes += strlen(user->email) + 1;
    if (user->password)
      nBytes += strlen(user->password) + 1;
    if (user->plan)
      nBytes += strlen(user->plan) + 1;
    if (user->userId)
      nBytes += strlen(user->userId) + 1;
    nBytes += sizeof(User);
  }
  return (nBytes);
}

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
	const char *userId) {
  register bool result = false;
  if (!game) {
    Log(L_ASSERT, "Invalid `game` Game.");
  } else {
    /* Remove from user index */
    result = TreeDelete(game->users, &userId);

    /* Remove user file */
    char fname[PATH_MAX] = {'\0'};
    if (!UserGetFileName(fname, sizeof(fname), userId)) {
      Log(L_STATE, "Couldn't create filename for `%s` user.", userId);
    } else if (unlink(fname) < 0 && errno != ENOENT) {
      Log(L_SYSTEM, "unlink() failed: fname=%s, errno=%d.", fname, errno);
    }
  }
  return (result);
}

/*!
 * Emits a user.
 * \addtogroup user
 * \param toData the data element to which to write
 * \param fromUser the user to emit
 */
static void UserEmitEmail(
	Data *toData,
	const User *fromUser) {
  if (!toData) {
    Log(L_ASSERT, "Invalid `toData` Data.");
  } else if (!fromUser) {
    Log(L_ASSERT, "Invalid `fromUser` User.");
  } else {
    if (fromUser->email && *fromUser->email != '\0')
      DataPutString(toData, "Email", fromUser->email);
  }
}

/*!
 * Emits a user.
 * \addtogroup user
 * \param toData the data element to which to write
 * \param fromUser the user to emit
 */
static void UserEmitPassword(
	Data *toData,
	const User *fromUser) {
  if (!toData) {
    Log(L_ASSERT, "Invalid `toData` Data.");
  } else if (!fromUser) {
    Log(L_ASSERT, "Invalid `fromUser` User.");
  } else {
    if (fromUser->password && *fromUser->password != '\0')
      DataPutString(toData, "Password", fromUser->password);
  }
}

/*!
 * Emits a user.
 * \addtogroup user
 * \param toData the data element to which to write
 * \param fromUser the user to emit
 */
static void UserEmitPlan(
	Data *toData,
	const User *fromUser) {
  if (!toData) {
    Log(L_ASSERT, "Invalid `toData` Data.");
  } else if (!fromUser) {
    Log(L_ASSERT, "Invalid `fromUser` User.");
  } else {
    if (fromUser->plan && *fromUser->plan != '\0')
      DataPutString(toData, "Plan", fromUser->plan);
  }
}

/*!
 * Emits a user.
 * \addtogroup user
 * \param toData the data element to which to write
 * \param fromUser the user to emit
 */
static void UserEmitTime(
	Data *toData,
	const User *fromUser) {
  if (!toData) {
    Log(L_ASSERT, "Invalid `toData` Data.");
  } else if (!fromUser) {
    Log(L_ASSERT, "Invalid `fromUser` User.");
  } else {
    Data *xTime = DataAlloc();
    if (fromUser->lastLogoff &&
	fromUser->lastLogoff != fromUser->lastLogon - 1)
      DataPutTime(xTime, "Logoff", fromUser->lastLogoff);
    if (fromUser->lastLogon)
      DataPutTime(xTime, "Logon", fromUser->lastLogon);

    if (!DataSize(xTime)) {
      DataFree(xTime);
    } else {
      DataPut(toData, "Time", xTime);
      DataSort(xTime);
    }
  }
}

/*!
 * Emits a user.
 * \addtogroup user
 * \param toData the data element to which to write
 * \param fromUser the user to emit
 */
static void UserEmitUserId(
	Data *toData,
	const User *fromUser) {
  if (!toData) {
    Log(L_ASSERT, "Invalid `toData` Data.");
  } else if (!fromUser) {
    Log(L_ASSERT, "Invalid `fromUser` User.");
  } else {
    if (fromUser->userId && *fromUser->userId != '\0')
      DataPutString(toData, "UserId", fromUser->userId);
  }
}

/*!
 * Emits a user.
 * \addtogroup user
 * \param toData the data element to which to write
 * \param fromUser the user to emit
 * \sa UserParse(Data*, User*)
 */
void UserEmit(
	Data *toData,
	const User *fromUser) {
  if (!toData) {
    Log(L_ASSERT, "Invalid `toData` Data.");
  } else if (!fromUser) {
    Log(L_ASSERT, "Invalid `fromUser` User.");
  } else {
    UserEmitEmail(toData, fromUser);
    UserEmitPassword(toData, fromUser);
    UserEmitPlan(toData, fromUser);
    UserEmitTime(toData, fromUser);
    UserEmitUserId(toData, fromUser);
  }
}

/*!
 * Frees a user.
 * \addtogroup user
 * \param user the user to free
 * \sa UserAlloc(Game*)
 * \sa UserFreeV(void*)
 */
void UserFree(User *user) {
  if (user) {
    StringFree(user->email);
    StringFree(user->password);
    StringFree(user->plan);
    StringFree(user->userId);
    MemoryFree(user);
  }
}

/*!
 * Frees a user.
 * \addtogroup user
 * \param user the user to free
 * \sa UserAlloc(Game*)
 * \sa UserFree(User*)
 */
void UserFreeV(void *user) {
  UserFree(user);
}

#define USER_DIRECTORY		"data/user"
#define USER_FILE_EXTENSION	"dat"

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
	const char *userId) {
  register bool result = false;
  if (!fname && fnamelen) {
    Log(L_ASSERT, "Invalid `fname` buffer.");
  } else if (!userId || *userId == '\0') {
    Log(L_ASSERT, "Invalid `userId` string.");
  } else {
    result = UtilityGetFileName(
	fname, fnamelen,
	USER_DIRECTORY,
	USER_FILE_EXTENSION,
	userId);
  }
  return (result);
}

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
	const char *userId) {
  register User *user = NULL;
  if (!game) {
    Log(L_ASSERT, "Invalid `game` Game.");
  } else if (!userId || *userId == '\0') {
    Log(L_ASSERT, "Invalid `userId` string.");
  } else {
    char fname[PATH_MAX] = {'\0'};
    if (!UserGetFileName(fname, sizeof(fname), userId)) {
      Log(L_USER, "Couldn't create filename for `%s` user.", userId);
    } else {
      Data *fromData = DataLoadFile(fname);
      if (!fromData) {
	Log(L_USER, "Couldn't load user file `%s`.", fname);
      } else {
	user = UserAlloc(game);
	UserParse(fromData, user);
	DataFree(fromData);
      }
    }
  }
  return (user);
}

#define USER_INDEX_FILE		"data/user/index"

/*!
 * Loads the user index.
 * \addtogroup user
 * \param game the game state
 * \sa UserSaveIndex(Game*)
 */
void UserLoadIndex(Game *game) {
  if (!game) {
    Log(L_ASSERT, "Invalid `game` Game.");
  } else {
    FILE *stream = fopen(USER_INDEX_FILE, "rt");
    if (!stream) {
      Log(L_USER, "Couldn't open user index file `%s` for reading.", USER_INDEX_FILE);
    } else {
      char line[MAXLEN_INPUT] = {'\0'};
      while (fgets(line, sizeof(line), stream) != NULL) {
	/* Read user ID */
	char userId[MAXLEN_INPUT] = {'\0'};
	StringOneWord(userId, sizeof(userId), line);

	/* Skip user IDs */
	if (*userId == '\0')
	  continue;

	/* Load user */
	register User *user;
	if ((user = UserLoad(game, userId)) != NULL) {
	  UserStore(game, user);
	  UserFree(user);
	}
      }
      fclose(stream);
    }

    /* Calculate metrics */
    register size_t nUsers = 0;
    register size_t nUserBytes = 0;
    TreeForEach(game->users, tUserNode) {
      const User *tUser = tUserNode->mappingValue;
      nUserBytes += UserCountBytes(tUser);
      nUsers++;
    }
    Log(L_USER, "Loaded %zu user(s), %zu byte(s).", nUsers, nUserBytes);
  }
}

/*!
 * Parses a user.
 * \addtogroup user
 * \param fromData the data element to parse
 * \param toUser the location of the parsed user
 */
static void UserParseEmail(
	Data *fromData,
	User *toUser) {
  if (!fromData) {
    Log(L_ASSERT, "Invalid `fromData` Data.");
  } else if (!toUser) {
    Log(L_ASSERT, "Invalid `toUser` User.");
  } else {
    StringFree(toUser->email);
    toUser->email = DataGetStringCopy(fromData, "Email", NULL);
  }
}

/*!
 * Parses a user.
 * \addtogroup user
 * \param fromData the data element to parse
 * \param toUser the location of the parsed user
 */
static void UserParsePassword(
	Data *fromData,
	User *toUser) {
  if (!fromData) {
    Log(L_ASSERT, "Invalid `fromData` Data.");
  } else if (!toUser) {
    Log(L_ASSERT, "Invalid `toUser` User.");
  } else {
    StringFree(toUser->password);
    toUser->password = DataGetStringCopy(fromData, "Password", NULL);
  }
}

/*!
 * Parses a user.
 * \addtogroup user
 * \param fromData the data element to parse
 * \param toUser the location of the parsed user
 */
static void UserParsePlan(
	Data *fromData,
	User *toUser) {
  if (!fromData) {
    Log(L_ASSERT, "Invalid `fromData` Data.");
  } else if (!toUser) {
    Log(L_ASSERT, "Invalid `toUser` User.");
  } else {
    StringFree(toUser->plan);
    toUser->plan = DataGetStringCopy(fromData, "Plan", NULL);
  }
}

/*!
 * Parses a user.
 * \addtogroup user
 * \param fromData the data element to parse
 * \param toUser the location of the parsed user
 */
static void UserParseTime(
	Data *fromData,
	User *toUser) {
  if (!fromData) {
    Log(L_ASSERT, "Invalid `fromData` Data.");
  } else if (!toUser) {
    Log(L_ASSERT, "Invalid `toUser` User.");
  } else {
    Data *xTime = DataGet(fromData, "Time");
    toUser->lastLogon  = DataGetTime(xTime, "Logon", time(0));
    toUser->lastLogoff = DataGetTime(xTime, "Logoff", toUser->lastLogon - 1);
  }
}

/*!
 * Parses a user.
 * \addtogroup user
 * \param fromData the data element to parse
 * \param toUser the location of the parsed user
 */
static void UserParseUserId(
	Data *fromData,
	User *toUser) {
  if (!fromData) {
    Log(L_ASSERT, "Invalid `fromData` Data.");
  } else if (!toUser) {
    Log(L_ASSERT, "Invalid `toUser` User.");
  } else {
    StringFree(toUser->userId);
    toUser->userId = DataGetStringCopy(fromData, "UserId", NULL);
  }
}

/*!
 * Parses a user.
 * \addtogroup user
 * \param fromData the data element to parse
 * \param toUser the location of the parsed user
 * \sa UserEmit(Data*, const User*)
 */
void UserParse(
	Data *fromData,
	User *toUser) {
  if (!fromData) {
    Log(L_ASSERT, "Invalid `fromData` Data.");
  } else if (!toUser) {
    Log(L_ASSERT, "Invalid `toUser` User.");
  } else {
    UserParseUserId(fromData, toUser);
    UserParseEmail(fromData, toUser);
    UserParsePassword(fromData, toUser);
    UserParsePlan(fromData, toUser);
    UserParseTime(fromData, toUser);
  }
}

/*!
 * Saves a user.
 * \addtogroup user
 * \param game the game state
 * \param user the user to save
 * \sa UserLoad(Game*, const char*)
 */
void UserSave(
	Game *game,
	const User *user) {
  char fname[PATH_MAX] = {'\0'};
  if (!game) {
    Log(L_ASSERT, "Invalid `game` Game.");
  } else if (!user) {
    Log(L_ASSERT, "Invalid `user` User.");
  } else if (!UserGetFileName(fname, sizeof(fname), user->userId)) {
    Log(L_USER, "Couldn't create filename for `%s` user.", user->userId);
  } else {
    /* Emit user */
    Data *toData = DataAlloc();
    UserEmit(toData, user);

    /* Save user file */
    if (!DataSaveFile(toData, fname))
      Log(L_USER, "Couldn't save user file `%s`.", fname);

    /* Cleanup */
    DataFree(toData);
  }
}

/*!
 * Saves the user index.
 * \addtogroup user
 * \param game the game state
 * \sa UserLoadIndex(Game*)
 */
void UserSaveIndex(Game *game) {
  if (!game) {
    Log(L_ASSERT, "Invalid `game` Game.");
  } else {
    FILE *stream = fopen(USER_INDEX_FILE, "wt");
    if (!stream) {
      Log(L_USER, "Couldn't open user index file `%s` for writing.", USER_INDEX_FILE);
    } else {
      TreeForEach(game->users, tUserNode) {
	const User *tUser = tUserNode->mappingValue;
	fprintf(stream, "%s\n", tUser->userId);
      }
      fclose(stream);
    }
  }
}

/*!
 * Stores a copy of a user.
 * \addtogroup user
 * \param game the game state
 * \param user the user to store
 * \return a copy of the specified user or NULL
 */
User *UserStore(
	Game *game,
	const User *user) {
  register User *copied = NULL;
  if (!game) {
    Log(L_ASSERT, "Invalid `game` Game.");
  } else if (!user) {
    Log(L_ASSERT, "Invalid `user` User.");
  } else {
    copied = UserByUserId(game, user->userId);
    if (!copied) {
      copied = UserAlloc(game);
      StringSet(&copied->userId, user->userId);
      if (!TreeInsert(game->users, &copied->userId, copied)) {
	Log(L_USER, "Couldn't add `%s` user to user index.", copied->userId);
	UserFree(copied), copied = NULL;
      }
    }
    if (copied)
      UserCopy(copied, user);
  }
  return (copied);
}
