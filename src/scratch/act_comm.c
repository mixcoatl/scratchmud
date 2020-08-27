/*!
 * \file act_comm.c
 *
 * \par Copyright
 * Copyright (C) 1999-2019 scratchmud.org
 * All rights reserved.
 *
 * \author Geoffrey Davis <gdavis@scratchmud.org>
 * \addtogroup act
 */
#define _SCRATCH_ACT_COMM_C_

#include <scratch/action.h>
#include <scratch/bitvector.h>
#include <scratch/color.h>
#include <scratch/command.h>
#include <scratch/game.h>
#include <scratch/log.h>
#include <scratch/object.h>
#include <scratch/player.h>
#include <scratch/rbtree.h>
#include <scratch/scratch.h>
#include <scratch/string.h>

/* Local functions */
COMMAND(ActEmote);
COMMAND(ActSay);
COMMAND(ActSayTo);
COMMAND(ActSocial);
COMMAND(ActTell);

#define EMOTE_USAGE \
  "Usage: Emote <Message>\r\n" \
  "Use @target in your message to include a character or an object.\r\n" \
  "Use * in your message to include yourself.\r\n"

/*!
 * The EMOTE command.
 * \addtogroup act
 * \param actor the game object that performed the command
 * \param command the command the specified game object performed
 * \param input the command line input to the command
 */
COMMAND(ActEmote) {
  /* Emote buffer */
  char emote[MAXLEN_STRING] = {'\0'};
  register size_t emotepos = 0;

  /* Direct object name */
  char name[MAXLEN_INPUT] = {'\0'};

  /* Action targets */
  Bitvector targetBits = TO_ALL;
  if (BitCheckN(GetPlayerPreferenceBits(actor), PREF_NO_REPEAT))
    BitRemove(targetBits, TO_ACTOR);

  /* Target object */
  Object *direct = NULL;

  /* Process message */
  register const char *ptr;
  for (ptr = StringSkipSpaces(input); ptr && *ptr != '\0'; ++ptr) {
    /*
     * If the actor hasn't indicated where their name should
     * appear in the emote message, we need to prepend the emote
     * message with their name.
     */
    if (!emotepos && strchr(input, '*') == NULL)
      BPrintf(emote, sizeof(emote), emotepos, "{S.RawName} ");

    if (*ptr == '@') {
      /* Read direct object name */
      register size_t namepos = 0;
      for (++ptr; *ptr != '\0' &&
	  (isalnum(*ptr) || strchr("_#$", *ptr) != NULL); ++ptr) {
	BPrintf(name, sizeof(name), namepos, "%c", *ptr);
      }

      /* Resolve direct object name */
      direct = ObjectResolve(actor, name);
      if (*ptr == '\'' && *(ptr + 1) == 's') {
	BPrintf(emote, sizeof(emote), emotepos, "{D.RawNamePossessive}");
	ptr++;
      } else {
	BPrintf(emote, sizeof(emote), emotepos, "{D.RawName}");
	ptr--;
      }
    } else if (*ptr == '*') {
      if (*(ptr + 1) == '\'' && *(ptr + 2) == 's') {
	BPrintf(emote, sizeof(emote), emotepos, "{S.RawNamePossessive}");
	ptr += 2;
      } else {
	BPrintf(emote, sizeof(emote), emotepos, "{S.RawName}");
      }
    } else {
      BPrintf(emote, sizeof(emote), emotepos, "%c", *ptr);
    }
  }

  if (emotepos) {
    /* Make sure everyone knows who emoted */
    BPrintf(emote, sizeof(emote), emotepos, " ({S.RawName})");
  }

  if (*emote == '\0') {
    ObjectPrint(actor, "%s", EMOTE_USAGE);
  } else if (*name != '\0' && !direct) {
    Act(CX_FAILED, TO_ACTOR, "There's nobody here named {D.Text}.", actor, name, NULL);
  } else {
    Act(CX_SOCIAL, targetBits, StringNormalize(emote), actor, direct, NULL); 
    if (BitCheckN(GetPlayerPreferenceBits(actor), PREF_NO_REPEAT))
      Act(CX_OKAY, TO_ACTOR, "Okay.", actor, NULL, NULL);
  }
}

/*!
 * The SAY command.
 * \addtogroup act
 * \param actor the game object that performed the command
 * \param command the command the specified game object performed
 * \param input the command line input to the command
 */
COMMAND(ActSay) {
  /* Make a defensive copy */
  char copy[MAXLEN_INPUT] = {'\0'};
  strlcpy(copy, StringBlank(input), sizeof(copy));

  /* Normalize input */
  StringNormalize(copy);

  /* Action targets */
  Bitvector targetBits = TO_ALL;
  if (BitCheckN(GetPlayerPreferenceBits(actor), PREF_NO_REPEAT))
    BitRemove(targetBits, TO_ACTOR);

  if (*copy == '\0') {
    Act(CX_FAILED, TO_ACTOR, "What do you want to say?", actor, NULL, NULL);
  } else {
    if (StringSuffix(copy, "?") || StringSuffix(copy, "?!")) {
      Act(CX_SAY, targetBits, "{S.Name} ask{:s}, \"{D.Text}\"", actor, copy, NULL);
    } else if (StringSuffix(copy, "!")) {
      Act(CX_SAY, targetBits, "{S.Name} exclaim{:s}, \"{D.Text}\"", actor, copy, NULL);
    } else {
      Act(CX_SAY, targetBits, "{S.Name} say{:s}, \"{D.Text}\"", actor, copy, NULL);
    }
    if (BitCheckN(GetPlayerPreferenceBits(actor), PREF_NO_REPEAT))
      Act(CX_OKAY, TO_ACTOR, "Okay.", actor, NULL, NULL);
  }
}

/*!
 * The SAYTO command.
 * \addtogroup act
 * \param actor the game object that performed the command
 * \param command the command the specified game object performed
 * \param input the command line input to the command
 */
COMMAND(ActSayTo) {
  /* Read target name */
  char name[MAXLEN_INPUT] = {'\0'};
  input = StringOneWord(name, sizeof(name), StringBlank(input));

  /* Read message */
  char copy[MAXLEN_INPUT] = {'\0'};
  strlcpy(copy, StringSkipSpaces(input), sizeof(copy));

  /* Action targets */
  Bitvector targetBits = TO_ALL;
  if (BitCheckN(GetPlayerPreferenceBits(actor), PREF_NO_REPEAT))
    BitRemove(targetBits, TO_ACTOR);

  Object *target = NULL;
  if (*name == '\0') {
    Act(CX_FAILED, TO_ACTOR, "Say something to whom?", actor, NULL, NULL);
  } else if ((target = ObjectResolve(actor, name)) == NULL) {
    Act(CX_FAILED, TO_ACTOR, "There's nobody here named {D.Text}.", actor, name, NULL);
  } else if (*copy == '\0') {
    Act(CX_FAILED, TO_ACTOR, "What do you want to say?", actor, NULL, NULL);
  } else {
    char messg[MAXLEN_STRING] = {'\0'};
    if (StringSuffix(copy, "?") || StringSuffix(copy, "?!")) {
      snprintf(messg, sizeof(messg), "{S.Name} ask{:s} %s, \"{I.Text}\"", actor != target ? "{D.Name}" : "{S.Reflexive}");
    } else if (StringSuffix(copy, "!")) {
      snprintf(messg, sizeof(messg), "{S.Name} exclaim{:s} to %s, \"{I.Text}\"", actor != target ? "{D.Name}" : "{S.Reflexive}");
    } else {
      snprintf(messg, sizeof(messg), "{S.Name} say{:s} to %s, \"{I.Text}\"", actor != target ? "{D.Name}" : "{S.Reflexive}");
    }
    Act(CX_SAY, targetBits, messg, actor, target, copy);
    if (BitCheckN(GetPlayerPreferenceBits(actor), PREF_NO_REPEAT))
      Act(CX_OKAY, TO_ACTOR, "Okay.", actor, NULL, NULL);
  }
}

/*!
 * The SOCIAL command.
 * \addtogroup act
 * \param actor the game object that performed the command
 * \param command the command the specified game object performed
 * \param input the command line input to the command
 */
COMMAND(ActSocial) {
  /* Read target name */
  char name[MAXLEN_INPUT] = {'\0'};
  input = StringOneWord(name, sizeof(name), StringBlank(input));

  /* We might need to hide the actor */
  Bitvector targetBits = TO_ALL;
  if (command->socialData && command->socialData->hide)
    BitSet(targetBits, TO_HIDE);
  if (BitCheckN(GetPlayerPreferenceBits(actor), PREF_NO_REPEAT))
    BitRemove(targetBits, TO_ACTOR);

  if (!command->socialData) {
    Act(CX_FAILED, TO_ACTOR, "The {D.Text} social is broken.", actor, command->name, NULL);
  } else if (*name == '\0') {
    if (*StringBlank(command->socialData->noArgument) == '\0') {
      Act(CX_FAILED, TO_ACTOR, "You can't use {D.Text} without a target.", actor, command->name, NULL);
    } else {
      Act(CX_SOCIAL, targetBits, command->socialData->noArgument, actor, NULL, NULL);
      if (BitCheckN(GetPlayerPreferenceBits(actor), PREF_NO_REPEAT))
	Act(CX_OKAY, TO_ACTOR, "Okay.", actor, NULL, NULL);
    }
  } else {
    register Object *target = NULL;
    if ((target = ObjectResolve(actor, name)) == NULL) {
      Act(CX_FAILED, TO_ACTOR, "There's nobody here named {D.Text}.", actor, name, NULL);
    } else if (target == actor) {
      if (*StringBlank(command->socialData->foundAuto) == '\0') {
	Act(CX_FAILED, TO_ACTOR, "You can't use {D.Text} on yourself.", actor, command->name, NULL);
      } else {
	Act(CX_SOCIAL, targetBits, command->socialData->foundAuto, actor, target, NULL); 
	if (BitCheckN(GetPlayerPreferenceBits(actor), PREF_NO_REPEAT))
	  Act(CX_OKAY, TO_ACTOR, "Okay.", actor, NULL, NULL);
      }
    } else {
      if (*StringBlank(command->socialData->found) == '\0') {
	Act(CX_FAILED, TO_ACTOR, "You can't use {D.Text} on someone.", actor, command->name, NULL);
      } else {
	Act(CX_SOCIAL, targetBits, command->socialData->found, actor, target, NULL); 
	if (BitCheckN(GetPlayerPreferenceBits(actor), PREF_NO_REPEAT))
	  Act(CX_OKAY, TO_ACTOR, "Okay.", actor, NULL, NULL);
      }
    }
  }
}

/*!
 * The TELL command.
 * \addtogroup act
 * \param actor the game object that performed the command
 * \param command the command the specified game object performed
 * \param input the command line input to the command
 */
COMMAND(ActTell) {
  /* Read target name */
  char name[MAXLEN_INPUT] = {'\0'};
  input = StringOneWord(name, sizeof(name), StringBlank(input));

  /* Read message */
  char copy[MAXLEN_INPUT] = {'\0'};
  strlcpy(copy, StringSkipSpaces(input), sizeof(copy));

  /* Action targets */
  Bitvector targetBits = TO_ACTOR | TO_TARGET;
  if (BitCheckN(GetPlayerPreferenceBits(actor), PREF_NO_REPEAT))
    BitRemove(targetBits, TO_ACTOR);

  Object *target = NULL;
  if (*name == '\0') {
    Act(CX_FAILED, TO_ACTOR, "Tell something to whom?", actor, NULL, NULL);
  } else if ((target = ObjectResolve(actor, name)) == NULL) {
    Act(CX_FAILED, TO_ACTOR, "There's nobody here named {D.Text}.", actor, name, NULL);
  } else if (target == actor) {
    Act(CX_FAILED, TO_ACTOR, "You can't use {D.Text} on yourself.", actor, command->name, NULL);
  } else if (*copy == '\0') {
    Act(CX_FAILED, TO_ACTOR, "What do you want to tell {D.Name}?", actor, target, NULL);
  } else {
    Act(CX_TELL, targetBits, "{S.Name} tell{:s} {D.Name}, \"{I.Text}\"", actor, target, copy);
    if (BitCheckN(GetPlayerPreferenceBits(actor), PREF_NO_REPEAT))
      Act(CX_OKAY, TO_ACTOR, "Okay.", actor, NULL, NULL);
  }
}
