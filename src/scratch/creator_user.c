/*!
 * \file creator_user.c
 *
 * \par Copyright
 * Copyright (C) 1999-2023 scratchmud.org
 * All rights reserved.
 *
 * \author Geoffrey Davis <gdavis@scratchmud.org>
 * \addtogroup creator
 * \addtogroup user
 */
#define _SCRATCH_CREATOR_USER_C_

#include <scratch/color.h>
#include <scratch/creator.h>
#include <scratch/descriptor.h>
#include <scratch/editor.h>
#include <scratch/log.h>
#include <scratch/memory.h>
#include <scratch/scratch.h>
#include <scratch/state.h>
#include <scratch/string.h>
#include <scratch/user.h>
#include <scratch/utility.h>

/* Local functions */
STATE(UserConfirmOnFocus);
STATE(UserConfirmOnReceived);
STATE(UserEmailOnFocus);
STATE(UserEmailOnReceived);
STATE(UserOnFocus);
STATE(UserOnReceived);
STATE(UserPasswordAgainOnFocus);
STATE(UserPasswordAgainOnReceived);
STATE(UserPasswordCurrentOnFocus);
STATE(UserPasswordCurrentOnReceived);
STATE(UserPasswordOnFocus);
STATE(UserPasswordOnReceived);
EDITOR(UserPlanOnStringAborted);
EDITOR(UserPlanOnStringFinished);
STATE(UserUserIdOnFocus);
STATE(UserUserIdOnReceived);

/*!
 * Starts the user creator.
 * \addtogroup creator
 * \addtogrup user
 * \param d the descriptor whose creator to start
 * \param user the user to modify or NULL
 */
void UserStartCreator(
	Descriptor *d,
	const User *user) {
  if (!d) {
    Log(L_ASSERT, "Invalid `d` Descriptor.");
  } else {
    /* Create creator */
    if (!d->creator)
      d->creator = CreatorAlloc();

    /* Create creator user */
    if (!d->creator->user)
      d->creator->user = UserAlloc(d->game);

    /* Copy user */
    if (user) {
      StringSet(&d->creator->name, user->userId);
      UserCopy(d->creator->user, user);
    }

    /* Change descriptor state */
    StateChangeByName(d, "User");
  }
}

/*! Descriptor state function. */
STATE(UserConfirmOnFocus) {
  DescriptorPrint(d, "%sSave this user? %s", QX_PROMPT, Q_NORMAL);
  return (true);
}

/*! Descriptor state function. */
STATE(UserConfirmOnReceived) {
  /* Read response */
  char arg[MAXLEN_INPUT] = {'\0'};
  input = StringOneWord(arg, sizeof(arg), input);

  if (*arg == '\0') {
    DescriptorPrint(d, "%sQuit aborted.%s\r\n", QX_FAILED, Q_NORMAL);
    StateChangeByName(d, "User");
  } else if (strchr("Yy", *arg) != NULL) {
    /* Store user */
    if (!UserStore(game, d->creator->user)) {
      DescriptorPrint(d, "%sCouldn't save `%s` user.%s\r\n", QX_FAILED, d->creator->user->userId, Q_NORMAL);
      StateChangeByName(d, "User");
    } else {
      /* Save user and user index */
      UserSave(game, d->creator->user);
      UserSaveIndex(game);

      /* Logging and messages */
      DescriptorPrint(d, "%sUser saved.%s\r\n", QX_OKAY, Q_NORMAL);
      Log(L_USER, "User %s edited user %s.",
		d->user ? d->user->userId : d->creator->user->userId,
		d->creator->user->userId);

      /* Change state */
      if (!StringCaseCompare(d->creator->name, "")) {
	StateChangeByName(d, "LoginUserId");
      } else {
	StateChangeByName(d, "Playing");
      }	

      /* Creator cleanup */
      CreatorFree(d->creator);
      d->creator = NULL;
    }
  } else if (strchr("Nn", *arg) != NULL) {
    DescriptorPrint(d, "%sPlayer editor aborted.%s\r\n", QX_FAILED, Q_NORMAL);
    CreatorFree(d->creator);
    d->creator = NULL;
  } else {
    DescriptorPrint(d, "%sInvalid choice.%s\r\n", QX_FAILED, Q_NORMAL);
    UserConfirmOnFocus(d, game, "");
  }
  return (true);
}

/*! Descriptor state function. */
STATE(UserEmailOnFocus) {
  DescriptorPrint(d, "%sEnter email address %s> %s", QX_PROMPT, QX_PUNCTUATION, Q_NORMAL);
  return (true);
}

/*! Descriptor state function. */
STATE(UserEmailOnReceived) {
  /* Read response */
  char arg[MAXLEN_INPUT] = {'\0'};
  input = StringOneWord(arg, sizeof(arg), input);

  if (*arg == '\0') {
    DescriptorPrint(d, "%sEmail aborted.%s\r\n", QX_FAILED, Q_NORMAL);
  } else {
    /* Check whether email was modified */
    if (StringCompare(d->creator->user->email, arg))
      d->creator->modified = true;

    StringSet(&d->creator->user->email, arg);
  }
  StateChangeByName(d, "User");
  return (true);
}

/*! Descriptor state function. */
STATE(UserOnFocus) {
  /* Shortcut */
  const User *user = d->creator->user;

  /* Menu */
  DescriptorPrint(d,
	"%sUser%s: %s%s%s\r\n"
	"%s<%s01%s> %sEmail Address%s... %s%s%s\r\n"
	"%s<%s02%s> %sPassword%s........ %s%s%s\r\n"
	"%s<%s03%s> %sPlan%s............ -%s\r\n"
	"%s%s%s"
	"%s<%s04%s> %sUser ID%s......... %s%s%s\r\n"
	"%sEnter %sQ %sto quit.%s\r\n"
	"%sChoice %s> %s",

	/* User */
	QX_PROMPT, QX_PUNCTUATION, QX_ORDINAL,
	d->creator->name && *d->creator->name != '\0' ?
	d->creator->name : "<NEW>", Q_NORMAL,

	/* Email address */
	QX_PUNCTUATION, QX_KEY, QX_PUNCTUATION, QX_PROMPT, QX_PUNCTUATION, QX_TEXT,
	user->email && *user->email != '\0' ? user->email : "<Blank>",
	Q_NORMAL,

	/* Password */
	QX_PUNCTUATION, QX_KEY, QX_PUNCTUATION, QX_PROMPT, QX_PUNCTUATION, QX_TEXT,
	user->password && *user->password != '\0' ? "<Set>" : "<Blank>",
	Q_NORMAL,

	/* Plan */
	QX_PUNCTUATION, QX_KEY, QX_PUNCTUATION, QX_PROMPT, QX_PUNCTUATION, Q_NORMAL, QX_TEXT,
	user->plan && *user->plan != '\0' ? user->plan : "<Blank>\r\n",
	Q_NORMAL,

	/* User ID */
	QX_PUNCTUATION, QX_KEY, QX_PUNCTUATION, QX_PROMPT, QX_PUNCTUATION, QX_TEXT,
	user->userId && *user->userId != '\0' ? user->userId : "<Blank>",
	Q_NORMAL,

	/* Enter Q to save and quit */
	QX_PROMPT, QX_KEY, QX_PROMPT, Q_NORMAL,

	/* Choice */
	QX_PROMPT, QX_PUNCTUATION, Q_NORMAL);

  return (true);
}

/*! Descriptor state function. */
STATE(UserOnReceived) {
  /* Read response */
  char arg[MAXLEN_INPUT] = {'\0'};
  input = StringOneWord(arg, sizeof(arg), input);

  if (*arg == '\0') {
    UserOnFocus(d, game, "");
  } else if (strchr("Qq", *arg) != NULL) { /* Save */
    if (!d->creator->modified) {
      DescriptorPrint(d, "%sNo changes detected.%s\r\n", QX_OKAY, Q_NORMAL);
      if (d->creator->name && *d->creator->name != '\0') {
	StateChangeByName(d, "Playing");
      } else {
	StateChangeByName(d, "LoginUserId");
      }
      CreatorFree(d->creator);
      d->creator = NULL;
    } else {
      StateChangeByName(d, "UserConfirm");
    }
  } else {
    const int choice = atoi(arg);
    switch (choice) {
    case /* Email */ 1:
      StateChangeByName(d, "UserEmail");
      break;
    case /* Password */ 2:
      if (d->creator->name && *d->creator->name != '\0') {
	StateChangeByName(d, "UserPasswordCurrent");
      } else {
	StateChangeByName(d, "UserPassword");
      }
      break;
    case /* Plan */ 3:
      EditorStart(d,
	d->creator->user->plan,
	1024,
	UserPlanOnStringAborted,
	UserPlanOnStringFinished,
	NULL);
      break;
    case /* User ID */ 4:
      StateChangeByName(d, "UserUserId");
      break;
    default:
      DescriptorPrint(d, "%sInvalid choice.%s\r\n", QX_FAILED, Q_NORMAL);
      UserOnFocus(d, game, "");
      break;
    }
  }
  return (true);
}

/*! Descriptor state function. */
STATE(UserPasswordAgainOnFocus) {
  DescriptorPrint(d, "%sEnter NEW password again %s> %s", QX_PROMPT, QX_PUNCTUATION, Q_NORMAL);
  return (true);
}

/*! Descriptor state function. */
STATE(UserPasswordAgainOnReceived) {
  /* Read plaintext password */
  char plaintext[MAXLEN_INPUT];
  input = StringOneWord(plaintext, sizeof(plaintext), input);

  /* No password */
  if (*plaintext == '\0') {
    DescriptorPrint(d, "%sPassword aborted.%s\r\n", QX_FAILED, Q_NORMAL);
    StateChangeByName(d, "User");
  /* Match plaintext against crypted password */
  } else if (!UtilityCryptMatch(d->creator->password, plaintext)) {
    DescriptorPrint(d, "%sPasswords don't match.  Start over.%s\r\n", QX_FAILED, Q_NORMAL);
    StateChangeByName(d, "UserPassword");
  } else {
    StringSet(&d->creator->user->password, d->creator->password);
    StringSet(&d->creator->password, NULL);
    DescriptorPrint(d, "%sPassword changed.%s\r\n", QX_OKAY, Q_NORMAL);
    StateChangeByName(d, "User");
  }
  return (true);
}

/*! Descriptor state function. */
STATE(UserPasswordCurrentOnFocus) {
  DescriptorPrint(d, "%sEnter CURRENT password %s> %s", QX_PROMPT, QX_PUNCTUATION, Q_NORMAL);
  return (true);
}

/*! Descriptor state function. */
STATE(UserPasswordCurrentOnReceived) {
  /* Read plaintext password */
  char plaintext[MAXLEN_INPUT];
  input = StringOneWord(plaintext, sizeof(plaintext), input);

  /* No password  */
  if (*plaintext == '\0') {
    DescriptorPrint(d, "%sPassword aborted.%s\r\n", QX_FAILED, Q_NORMAL);
    StateChangeByName(d, "User");
  /* Match plaintext against crypted password */
  } else if (!UtilityCryptMatch(d->creator->user->password, plaintext)) {
    DescriptorPrint(d, "%sPasswords don't match.%s\r\n", QX_FAILED, Q_NORMAL);
    StateChangeByName(d, "User");
  } else {
    StateChangeByName(d, "UserPassword");
  }
  return (true);
}

/*! Descriptor state function. */
STATE(UserPasswordOnFocus) {
  DescriptorPrint(d, "%sEnter NEW password %s> %s", QX_PROMPT, QX_PUNCTUATION, Q_NORMAL);
  return (true);
}

/*! Descriptor state function. */
STATE(UserPasswordOnReceived) {
  /* Read plaintext password */
  char plaintext[MAXLEN_INPUT];
  input = StringOneWord(plaintext, sizeof(plaintext), input);

  /* No password */
  if (*plaintext == '\0') {
    DescriptorPrint(d, "%sPassword aborted.%s\r\n", QX_FAILED, Q_NORMAL);
    StateChangeByName(d, "User");
  } else {
    /* Crypted password */
    char password[MAXLEN_INPUT];
    UtilityCrypt(password, sizeof(password), plaintext);

    /* Verify crypted password */
    StringSet(&d->creator->password, password);
    StateChangeByName(d, "UserPasswordAgain");
  }
  return (true);
}

/*! Descriptor state function. */
EDITOR(UserPlanOnStringAborted) {
  DescriptorPrint(d, "%sPlan aborted!%s\r\n", QX_FAILED, Q_NORMAL);
  UserOnFocus(d, game, "");
}

/*! Descriptor state function. */
EDITOR(UserPlanOnStringFinished) {
  /* Check whether plan was modified */
  if (StringCompare(d->creator->user->plan, text))
    d->creator->modified = true;

  StringSet(&d->creator->user->plan, text);
  UserOnFocus(d, game, "");
}

/*! Descriptor state function. */
STATE(UserUserIdOnFocus) {
  DescriptorPrint(d, "%sEnter user ID %s> %s", QX_PROMPT, QX_PUNCTUATION, Q_NORMAL);
  return (true);
}

/*! Descriptor state function. */
STATE(UserUserIdOnReceived) {
  /* Read user ID */
  char userId[MAXLEN_INPUT];
  input = StringOneWord(userId, sizeof(userId), input);

  /* Convert to title case */
  if (*userId != '\0') {
    register char *ptr = userId;
    for (; *ptr != '\0'; ++ptr) {
      *ptr = ptr != userId ? tolower(*ptr) : toupper(*ptr);
    }
  }

  /* No user ID */
  if (*userId == '\0') {
    DescriptorPrint(d, "%sUser ID aborted!%s\r\n", QX_FAILED, Q_NORMAL);
    StateChangeByName(d, "User");
  } else if (strlen(userId) < 3) {
    DescriptorPrint(d, "%sUser ID is too short.%s\r\n", QX_FAILED, Q_NORMAL);
    UserUserIdOnFocus(d, game, "");
  } else if (strlen(userId) > 14) {
    DescriptorPrint(d, "%sUser ID is too long.%s\r\n", QX_FAILED, Q_NORMAL);
    UserUserIdOnFocus(d, game, "");
  } else if (!UtilityNameValid(userId)) {
    DescriptorPrint(d, "%sUser ID isn't valid.%s\r\n", QX_FAILED, Q_NORMAL);
    UserUserIdOnFocus(d, game, "");
  } else {
    register User *user;
    if ((user = UserByUserId(game, userId)) &&
	        StringCaseCompare(d->creator->name, user->userId)) {
      DescriptorPrint(d, "%sUser ID already used.%s\r\n", QX_FAILED, Q_NORMAL);
      UserUserIdOnFocus(d, game, "");
    } else {
      /* Check whether user ID was modified */
      if (StringCompare(d->creator->user->userId, userId) != 0)
	d->creator->modified = true;

      StringSet(&d->creator->user->userId, userId);
      StateChangeByName(d, "User");
    }
  }
  return (true);
}
