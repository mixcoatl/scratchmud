/*!
 * \file action.c
 *
 * \par Copyright
 * Copyright (C) 1999-2019 scratchmud.org
 * All rights reserved.
 *
 * \author Geoffrey Davis <gdavis@scratchmud.org>
 * \addtogroup action
 */
#define _SCRATCH_ACTION_C_

#include <scratch/action.h>
#include <scratch/bitvector.h>
#include <scratch/block.h>
#include <scratch/color.h>
#include <scratch/constant.h>
#include <scratch/game.h>
#include <scratch/gender.h>
#include <scratch/log.h>
#include <scratch/object.h>
#include <scratch/rbtree.h>
#include <scratch/scratch.h>
#include <scratch/string.h>

/*!
 * Sends an action message.
 * \addtogroup action
 * \param color the C_x color of the action message
 * \param targets the TO_x targets flags
 * \param message the action message string
 * \param actor the game object that performs the action
 * \param direct the direct object, usually a game object, or NULL
 * \param indirect the indirect object, usually a game object, or NULL
 */
void Act(
	const int color,
	const Bitvector targets,
	const char *message,
	Object *actor,
	const void *direct,
	const void *indirect) {
  if (!actor) {
    Log("invalid `actor` Object");
  } else if (*StringBlank(message) != '\0') {
    RBTreeForEach(actor->game->objects, tObjectNode) {
      /* Iterator variable */
      Object *tObject = tObjectNode->value;

      /* Figure out what sort of object `to` might be */
      const bool isActor = (tObject == actor);
      const bool isTarget = (tObject == direct || tObject == indirect);
      const bool isNearby = !isActor && !isTarget;

      /* Skip if the message shouldn't go to this object */
      if (!BitCheck(targets, TO_ACTOR) && isActor)
	continue;
      if (!BitCheck(targets, TO_TARGET) && isTarget)
	continue;
      if (!BitCheck(targets, TO_NEARBY) && isNearby)
	continue;

      /* Send the action message */
      ActPerform(color, message, actor, direct, indirect, tObject);
    }
    RBTreeForEachEnd();
  }
}

/*!
 * Interprets an act macro.
 * \addtogroup action
 * \param out the output buffer
 * \param outlen the length of the specified buffer
 * \param actor the game object that performs the action
 * \param which the game object whose macro to interpret
 * \param to the game object to which to send the action message
 * \return the number of characters actually printed
 */
size_t ActInterpret(
	char *out, const size_t outlen, 
	const char *macro,
	Object *actor,
	const void *which,
	Object *to) {
  register size_t outpos = 0;
  if (!out && outlen) {
    Log("invalid `out` buffer");
  } else if (*StringBlank(macro) == '\0') {
    Log("invalid `macro` string");
  } else if (!actor) {
    Log("invalid `actor` Object");
  } else if (!which) {
    Log("invalid `which` void pointer");
  } else if (!to) {
    Log("invalid `to` Object");
  } else {
    /* Convert pointer to object */
    const Object *whichObject = NULL;
    if (StringCmpCI(macro, "Text") != 0)
	whichObject = (const Object*) which;

    /* Shortcut for logic below */
    const Gender *gender = whichObject ? whichObject->gender : NULL;

    /* Search for the macro by name */
    const int macroN = BlockSearch(_G_actMacroStrings, macro);

    /* Differentiate between raw and non-raw macros */
    const bool isRaw = StringPrefixCI(macro, "Raw");

    /* Process supported macros */
    switch (macroN) {
    case 0 /* Copula */ :
    case 6 /* RawCopula */ :
      if (!whichObject) {
	BPrintf(out, outlen, outpos, "<Null>");
      } else if (!isRaw && to == which) {
	BPrintf(out, outlen, outpos, "are");
      } else if (gender && *StringBlank(gender->copula) != '\0') {
	BPrintf(out, outlen, outpos, "%s", gender->copula);
      } else {
	BPrintf(out, outlen, outpos, "is");
      }
      break;
    case 1 /* Determiner */ :
    case 7 /* RawDeterminer */ :
      if (!whichObject) {
	BPrintf(out, outlen, outpos, "<Null>");
      } else if (!isRaw && to == which) {
	BPrintf(out, outlen, outpos, "your");
      } else if (gender && *StringBlank(gender->determiner) != '\0') {
	BPrintf(out, outlen, outpos, "%s", gender->determiner);
      } else {
	BPrintf(out, outlen, outpos, "its");
      }
      break;
    case 2 /* Name */ :
    case 8 /* RawName */ :
      if (!whichObject) {
	BPrintf(out, outlen, outpos, "<Null>");
      } else if (!isRaw && to == which) {
	BPrintf(out, outlen, outpos, "you");
      } else {
	BPrintf(out, outlen, outpos, "%s", ObjectGetName(whichObject));
      }
      break;
    case 3 /* NamePossessive */ :
    case 9 /* RawNamePossessive */ :
      if (!whichObject) {
	BPrintf(out, outlen, outpos, "<Null>");
      } else if (!isRaw && to == which) {
	BPrintf(out, outlen, outpos, "your");
      } else {
	BPrintf(out, outlen, outpos, "%s's", ObjectGetName(whichObject));
      }
      break;
    case 4  /* Object */ :
    case 10 /* RawObject */ :
      if (!whichObject) {
	BPrintf(out, outlen, outpos, "<Null>");
      } else if (!isRaw && to == whichObject) {
	BPrintf(out, outlen, outpos, "you");
      } else if (gender && *StringBlank(gender->pronounObject) != '\0') {
	BPrintf(out, outlen, outpos, "%s", gender->pronounObject);
      } else {
	BPrintf(out, outlen, outpos, "it");
      }
      break;
    case 5  /* Possessive */ :
    case 11 /* RawPossessive */ :
      if (!whichObject) {
	BPrintf(out, outlen, outpos, "<Null>");
      } else if (!isRaw && to == which) {
	BPrintf(out, outlen, outpos, "yours");
      } else if (gender && *StringBlank(gender->pronounPossessive) != '\0') {
	BPrintf(out, outlen, outpos, "%s", gender->pronounPossessive);
      } else {
	BPrintf(out, outlen, outpos, "its");
      }
      break;
    case 12 /* RawReflexive */ :
    case 14 /* Reflexive */ :
      if (!whichObject) {
	BPrintf(out, outlen, outpos, "<Null>");
      } else if (!isRaw && to == which) {
	BPrintf(out, outlen, outpos, "yourself");
      } else if (gender && *StringBlank(gender->pronounObject) != '\0') {
	BPrintf(out, outlen, outpos, "%sself", gender->pronounObject);
      } else {
	BPrintf(out, outlen, outpos, "it");
      }
      break;
    case 13 /* RawSubject */ :
    case 15 /* Subject */ :
      if (!whichObject) {
	BPrintf(out, outlen, outpos, "<Null>");
      } else if (!isRaw && to == which) {
	BPrintf(out, outlen, outpos, "you");
      } else if (gender && *StringBlank(gender->pronounSubject) != '\0') {
	BPrintf(out, outlen, outpos, "%s", gender->pronounSubject);
      } else {
	BPrintf(out, outlen, outpos, "it");
      }
      break;
    case 16 /* Text */ :
      BPrintf(out, outlen, outpos, "%s", StringBlank((const char*) which));
      break;
    default:
      BPrintf(out, outlen, outpos, "<%s>", macro);
      break;
    }
  }
  return (outpos);
}

/*!
 * Sends an action message.
 * \addtogroup action
 * \param color the C_x color of the action message
 * \param message the action message string
 * \param actor the game object that performs the action
 * \param direct the direct object, usually a game object, or NULL
 * \param indirect the indirect object, usually a game object, or NULL
 * \param to the game object to which to send the action message
 */
void ActPerform(
	const int color,
	const char *message,
	Object *actor,
	const void *direct,
	const void *indirect,
	Object *to) {
  if (*StringBlank(message) == '\0') {
    Log("invalid `message` string");
  } else if (!actor) {
    Log("invalid `actor` Object");
  } else if (!to) {
    Log("invalid `to` Object");
  } else {
    /* The output buffer */
    char out[MAXLEN_STRING] = {'\0'};
    register size_t outpos = 0;

    /* Process the action message */
    register const char *ptr;
    for (ptr = message; *ptr != '\0'; ++ptr) {
      if (*ptr == '{') {
	/* Some temporary buffers */
	char name[MAXLEN_INPUT] = {'\0'};
	char macro[MAXLEN_INPUT] = {'\0'};
	register size_t macropos = 0;

	/* Look for the end of the macro */
	register const char *end;
	for (end = ptr + 1; *end != '\0' && *end != '}'; ++end) {
	  if (*end == ':') {
	    if (to == actor) {
	      while (*end != '\0' && *(end + 1) != '}')
		++end;
	    } else {
	      strlcpy(name, "", sizeof(name));
	      strlcpy(macro, "", sizeof(macro));
	      macropos = 0;
	    }
	  } else if (*end == '.') {
	    if (*name == '\0') {
	      strlcpy(name, macro, sizeof(name));
	      strlcpy(macro, "", sizeof(macro));
	      macropos = 0;
	    } else {
	      BPrintf(macro, sizeof(macro), macropos, "%c", *end);
	    }
	  } else {
	    BPrintf(macro, sizeof(macro), macropos, "%c", *end);
	  }
	}
	if (*name == '\0') {
	  BPrintf(out, sizeof(out), outpos, "%s", macro);
	} else if (*macro != '\0') {
	  const void *which = NULL;
	  if (strchr("sS", *name) != NULL) {
	    which = actor;
	  } else if (strchr("dD", *name) != NULL) {
	    which = direct;
	  } else if (strchr("iI", *name) != NULL) {
	    which = indirect;
	  }
	  if (!which) {
	    BPrintf(out, sizeof(out), outpos, "<Null>");
	  } else {
	    outpos += ActInterpret(
		out + outpos, sizeof(out) - outpos,
		macro, actor, which, to);
	  }
	}
	ptr = end;
      } else {
	BPrintf(out, sizeof(out), outpos, "%c", *ptr);
      }
    }

    /* Capitalize */
    *out = toupper(*out);

    /* Print out the message */
    ObjectPrint(to, "%s%s%s\r\n",
	ColorGet(to->client, color), out,
	ColorGet(to->client, C_NORMAL));
  }
}
