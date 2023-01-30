/*!
 * \file state.c
 *
 * \par Copyright
 * Copyright (C) 1999-2023 scratchmud.org
 * All rights reserved.
 *
 * \author Geoffrey Davis <gdavis@scratchmud.org>
 * \addtogroup state
 */
#define _SCRATCH_STATE_C_

#include <scratch/data.h>
#include <scratch/descriptor.h>
#include <scratch/game.h>
#include <scratch/log.h>
#include <scratch/memory.h>
#include <scratch/scratch.h>
#include <scratch/state.h>
#include <scratch/string.h>
#include <scratch/tree.h>
#include <scratch/utility.h>

/*!
 * Constructs a new descriptor state.
 * \addtogroup state
 * \param game the game state
 * \return the new descriptor state or NULL
 * \sa StateFree(State*)
 * \sa StateFreeV(void*)
 */
State *StateAlloc(Game *game) {
  State *state = NULL;
  if (!game) {
    Log(L_ASSERT, "Invalid `game` Game.");
  } else {
    MemoryCreate(state, State, 1);
    state->bits.initial = false;
    state->bits.prompt = false;
    state->bits.quiet = false;
    state->focus = NULL;
    state->focusLost = NULL;
    state->focusLostName = NULL;
    state->focusName = NULL;
    state->game = game;
    state->name = NULL;
    state->received = NULL;
    state->receivedName = NULL;
  }
  return (state);
}

/*!
 * Searches for a descriptor state using its name.
 * \addtogroup state
 * \param game the game state
 * \param stateName the state name of the descriptor state to return
 * \return the descriptor state indicated by the specified state name or NULL
 */
State *StateByName(
	Game *game,
	const char *stateName) {
  register State *state = NULL;
  if (!game) {
    Log(L_ASSERT, "Invalid `game` Game.");
  } else if (!stateName || *stateName == '\0') {
    Log(L_ASSERT, "Invalid `stateName` string.");
  } else {
    state = TreeGetValue(game->states, &stateName, NULL);
  }
  return (state);
}

/*!
 * Changes descriptor states.
 * \addtogroup descriptor
 * \param d the descriptor whose descriptor state to change
 * \param state the descriptor state to which to change
 * \sa StateChangeByName(Descriptor*, const char*)
 */
void StateChange(
	Descriptor *d,
	State *state) {
  if (!d) {
    Log(L_ASSERT, "Invalid `d` Descriptor.");
  } else {
    /* Remember which states were quiet */
    State *lastState = d->state;
    const bool lastQuiet = lastState && lastState->bits.quiet;
    const bool quiet     = state && state->bits.quiet;

    /* Current state lost focus */
    if (d->state && d->state->focusLost)
      if (!d->state->focusLost(d, d->game, ""))
        return;

    /* New state received focus */
    if ((d->state = state) && d->state->focus)
      if (!d->state->focus(d, d->game, ""))
        d->state = lastState;

    if (lastQuiet && !quiet) {
      DescriptorPutCommand(d, DO, TELOPT_ECHO);
      DescriptorPutCommand(d, WONT, TELOPT_ECHO);
    }
    if (!lastQuiet && quiet) {
      DescriptorPutCommand(d, DONT, TELOPT_ECHO);
      DescriptorPutCommand(d, WILL, TELOPT_ECHO);
    }
  }
}

/*!
 * Changes descriptor states.
 * \addtogroup descriptor
 * \param d the descriptor whose descriptor state to change
 * \param stateName the state name of the descriptor state to which to change
 * \sa StateChange(Descriptor*, State*)
 */
void StateChangeByName(
	Descriptor *d,
	const char *stateName) {
  if (!d) {
    Log(L_ASSERT, "Invalid `d` Descriptor.");
  } else {
    /* Descriptor state lookup */
    register State *state = NULL;
    if (stateName && *stateName != '\0' &&
       (state = StateByName(d->game, stateName)) == NULL)
      Log(L_STATE, "Unknown state %s for descriptor %s.", stateName, d->name);

    /* Change descriptor state */
    StateChange(d, state);
  }
}

/*!
 * Copies a descriptor state.
 * \addtogroup state
 * \param toState the location of the copied descriptor state
 * \param fromState the descriptor state to copy
 */
void StateCopy(
	State *toState,
	const State *fromState) {
  if (!toState) {
    Log(L_ASSERT, "Invalid `toState` State.");
  } else if (!fromState) {
    Log(L_ASSERT, "Invalid `fromState` State.");
  } else if (toState != fromState) {
    /* Copy strings */
    StringSet(&toState->focusLostName, fromState->focusLostName);
    StringSet(&toState->focusName, fromState->focusName);
    StringSet(&toState->name, fromState->name);
    StringSet(&toState->receivedName, fromState->receivedName);

    /* Some other fields */
    toState->bits.initial = fromState->bits.initial;
    toState->bits.prompt  = fromState->bits.prompt;
    toState->bits.quiet   = fromState->bits.quiet;
    toState->focusLost    = fromState->focusLost;
    toState->focus        = fromState->focus;
    toState->received     = fromState->received;
  }
}

/*!
 * Returns the size of a descriptor state in bytes.
 * \addtogroup state
 * \param state the descriptor state whose size to return
 * \return the size of the specified descriptor state in bytes
 */
size_t StateCountBytes(const State *state) {
  register size_t nBytes = 0;
  if (state) {
    nBytes += sizeof(State);
    if (state->focusLostName)
      nBytes += strlen(state->focusLostName) + 1;
    if (state->focusName)
      nBytes += strlen(state->focusName) + 1;
    if (state->name)
      nBytes += strlen(state->name) + 1;
    if (state->receivedName)
      nBytes += strlen(state->receivedName) + 1;
  }
  return (nBytes);
}

/*!
 * Deletes a descriptor state.
 * \addtogroup state
 * \param game the game state
 * \param stateName the state name of the descriptor state to delete
 * \return true if the descriptor state indicated by the specified
 *     state name was successfully deleted
 */
bool StateDelete(
	Game *game,
	const char *stateName) {
  register bool result = false;
  if (!game) {
    Log(L_ASSERT, "Invalid `game` Game.");
  } else {
    /* Remove from state index */
    result = TreeDelete(game->states, &stateName);
  }
  return (result);
}

/*!
 * Emits a descriptor state.
 * \addtogroup state
 * \param toData the data element to which to write
 * \param fromState the descriptor state to emit
 */
static void StateEmitFunctions(
	Data *toData,
	const State *fromState) {
  if (!toData) {
    Log(L_ASSERT, "Invalid `toData` Data.");
  } else if (!fromState) {
    Log(L_ASSERT, "Invalid `fromState` State.");
  } else {
    Data *functionsData = DataAlloc();
    char procName[MAXLEN_INPUT] = {'\0'};

    /* Focus function */
    snprintf(procName, sizeof(procName), "%sOnFocus", fromState->name);
    if (fromState->focusName && *fromState->focusName != '\0' &&
	StringCompare(fromState->focusName, procName) != 0)
      DataPutString(functionsData, "Focus", fromState->focusName);

    /* Focus lost function */
    snprintf(procName, sizeof(procName), "%sOnFocusLost", fromState->name);
    if (fromState->focusLostName && *fromState->focusLostName != '\0' &&
	StringCompare(fromState->focusLostName, procName) != 0)
      DataPutString(functionsData, "FocusLost", fromState->focusLostName);

    /* Received function */
    snprintf(procName, sizeof(procName), "%sOnReceived", fromState->name);
    if (fromState->receivedName && *fromState->receivedName != '\0' &&
	StringCompare(fromState->receivedName, procName) != 0)
      DataPutString(functionsData, "Received", fromState->receivedName);

    if (!DataSize(functionsData)) {
      DataFree(functionsData);
    } else {
      DataPut(toData, "Functions", functionsData);
      DataSort(functionsData);
    }
  }
}

/*!
 * Emits a descriptor state.
 * \addtogroup state
 * \param toData the data element to which to write
 * \param fromState the descriptor state to emit
 */
static void StateEmitStateBits(
	Data *toData,
	const State *fromState) {
  if (!toData) {
    Log(L_ASSERT, "Invalid `toData` Data.");
  } else if (!fromState) {
    Log(L_ASSERT, "Invalid `fromState` State.");
  } else {
    /* Bit string buffer */
    char bitString[MAXLEN_STRING];
    size_t bitStringN = 0;

    /* Print state bits */
    if (fromState->bits.initial)
      BPrintf(bitString, sizeof(bitString), bitStringN, "Initial");
    if (fromState->bits.prompt) {
      BPrintf(bitString, sizeof(bitString), bitStringN, bitStringN ? ", " : "");
      BPrintf(bitString, sizeof(bitString), bitStringN, "Prompt");
    }
    if (fromState->bits.quiet) {
      BPrintf(bitString, sizeof(bitString), bitStringN, bitStringN ? ", " : "");
      BPrintf(bitString, sizeof(bitString), bitStringN, "Quiet");
    }
    if (bitStringN)
      DataPutString(toData, "StateBits", bitString);
  }
}

/*!
 * Emits a descriptor state.
 * \addtogroup state
 * \param toData the data element to which to write
 * \param fromState the descriptor state to emit
 * \sa StateParse(Data*, State*)
 */
void StateEmit(
	Data *toData,
	const State *fromState) {
  if (!toData) {
    Log(L_ASSERT, "Invalid `toData` Data.");
  } else if (!fromState) {
    Log(L_ASSERT, "Invalid `fromState` State.");
  } else {
    Data *stateData = DataAlloc();
    StateEmitFunctions(stateData, fromState);
    StateEmitStateBits(stateData, fromState);

    /* Optimize state file layout */
    const char *bitString = DataGetString(stateData, "StateBits", "");
    if (DataSize(stateData) == 1 && *bitString != '\0')
      DataPutString(stateData, "", bitString);

    DataPut(toData, fromState->name, stateData);
    DataSort(stateData);
  }
}

/*!
 * Frees a descriptor state.
 * \addtogroup state
 * \param state the descriptor state to free
 * \sa StateAlloc(Game*)
 * \sa StateFreeV(void*)
 */
void StateFree(State *state) {
  if (state) {
    StringFree(state->focusName);
    StringFree(state->focusLostName);
    StringFree(state->name);
    StringFree(state->receivedName);
    MemoryFree(state);
  }
}

/*!
 * Frees a descriptor state.
 * \addtogroup state
 * \param state the descriptor state to free
 * \sa StateAlloc(Game*)
 * \sa StateFree(State*)
 */
void StateFreeV(void *state) {
  StateFree(state);
}

/*!
 * Returns an initial descriptor state.
 * \addtogroup state
 * \param game the game state
 * \return the first descriptor state encountered in the descriptor
 *     state index that has its initial bitfield set or NULL
 */
State *StateInitial(Game *game) {
  if (!game) {
    Log(L_ASSERT, "Invalid `game` Game.");
  } else {
    TreeForEach(game->states, tStateNode) {
      State *tState = tStateNode->mappingValue;
      if (tState->bits.initial)
	return (tState);
    }
  }
  Log(L_STATE, "Couldn't find initial descriptor state.");
  return (NULL);
}

#define STATE_INDEX_FILE	"data/state.dat"

/*!
 * Loads the descriptor state index.
 * \addtogroup state
 * \param game the game state
 * \sa StateSaveIndex(Game*)
 */
void StateLoadIndex(Game *game) {
  if (!game) {
    Log(L_ASSERT, "Invalid `game` Game.");
  } else {
    Data *root = DataLoadFile(STATE_INDEX_FILE);
    if (!root) {
      Log(L_STATE, "Couldn't load state file `%s`.", STATE_INDEX_FILE);
    } else {
      /* Load descriptor states */
      DataForEach(root, tStateEntry) {
	State *state = StateAlloc(game);
	StringSet(&state->name, tStateEntry->key);
	StateParse(tStateEntry->value, state);

	if (UtilityNameValid(state->name))
	  StateStore(game, state);

	StateFree(state);
      }
      DataFree(root);
    }

    /* Calculate metrics */
    register size_t nStates = 0;
    register size_t nStateBytes = 0;
    TreeForEach(game->states, tStateNode) {
      const State *tState = tStateNode->mappingValue;
      nStateBytes += StateCountBytes(tState);
      nStates++;
    }
    Log(L_STATE, "Loaded %zu state(s), %zu byte(s).", nStates, nStateBytes);
  }
}

/*!
 * Parses a descriptor state.
 * \addtogroup state
 * \param fromData the data element to parse
 * \param toState the location of the parsed descriptor state
 */
static void StateParseFunctions(
	Data *fromData,
	State *toState) {
  if (!fromData) {
    Log(L_ASSERT, "Invalid `fromData` Data.");
  } else if (!toState) {
    Log(L_ASSERT, "Invalid `toState` State.");
  } else {
    Data *functionsData = DataGet(fromData, "Functions");

    /* Focus function */
    char focusName[MAXLEN_INPUT] = {'\0'};
    snprintf(focusName, sizeof(focusName), "%sOnFocus", toState->name);
    StringFree(toState->focusName);
    toState->focusName = DataGetStringCopy(functionsData, "Focus", focusName);
    toState->focus = UtilityProcByName(focusName);

    /* Focus lost function */
    char focusLostName[MAXLEN_INPUT] = {'\0'};
    snprintf(focusLostName, sizeof(focusLostName), "%sOnFocusLost", toState->name);
    StringFree(toState->focusLostName);
    toState->focusLostName = DataGetStringCopy(functionsData, "FocusLost", focusLostName);
    toState->focusLost = UtilityProcByName(toState->focusLostName);

    /* Received function */
    char receivedName[MAXLEN_INPUT] = {'\0'};
    snprintf(receivedName, sizeof(receivedName), "%sOnReceived", toState->name);
    StringFree(toState->receivedName);
    toState->receivedName = DataGetStringCopy(functionsData, "Received", receivedName);
    toState->received = UtilityProcByName(toState->receivedName);
  }
}

/*!
 * Parses a descriptor state.
 * \addtogroup state
 * \param fromBitString the state bits bit string to parse
 * \param toState the location of the parsed descriptor state
 */
static void StateParseStateBitsBitString(
	const char *fromBitString,
	State *toState) {
  if (!toState) {
    Log(L_ASSERT, "Invalid `toState` State.");
  } else {
    /* Clear previous state bits */
    MemoryZero(&toState->bits, sizeof(toState->bits), 1);

    /* Process state bits bit string */
    for (const char *p = fromBitString; p && *p != '\0'; ) {
      /* Skip leading whitespace */
      p = StringSkipSpaces(p);

      /* Temporary buffer */
      char bitName[MAXLEN_STRING] = {'\0'};
      register size_t bitNameN = 0;

      /* Read bit name */
      for (; *p != '\0' && *p != ','; ++p)
	BPrintf(bitName, sizeof(bitName), bitNameN, "%c", *p);

      /* Skip over commas */
      if (*p == ',')
	++p;

      /* Resolve bit name */
      if (!StringCaseCompare(bitName, "Initial")) {
	toState->bits.initial = true;
      } else if (!StringCaseCompare(bitName, "Prompt")) {
	toState->bits.prompt = true;
      } else if (!StringCaseCompare(bitName, "Quiet")) {
	toState->bits.quiet = true;
      } else {
	Log(L_STATE, "Unknown `%s` bit in `%s` state.", bitName, toState->name);
      }
    }
  }
}

/*!
 * Parses a descriptor state.
 * \addtogroup state
 * \param fromData the data element to parse
 * \param toState the location of the parsed descriptor state
 */
static void StateParseStateBits(
	Data *fromData,
	State *toState) {
  if (!fromData) {
    Log(L_ASSERT, "Invalid `fromData` Data.");
  } else if (!toState) {
    Log(L_ASSERT, "Invalid `toState` State.");
  } else {
    StateParseStateBitsBitString(
	DataGetString(fromData, "StateBits", ""),
	toState);
  }
}

/*!
 * Parses a descriptor state.
 * \addtogroup state
 * \param fromData the data element to parse
 * \param toState the location of the parsed descriptor state
 * \sa StateEmit(Data*, const State*)
 */
void StateParse(
	Data *fromData,
	State *toState) {
  if (!fromData) {
    Log(L_ASSERT, "Invalid `fromData` Data.");
  } else if (!toState) {
    Log(L_ASSERT, "Invalid `toState` State.");
  } else {
    StateParseFunctions(fromData, toState);
    StateParseStateBits(fromData, toState);

    if (!DataSize(fromData))
      StateParseStateBitsBitString(fromData->value, toState);
  }
}

/*!
 * Saves the descriptor state index.
 * \addtogroup state
 * \param game the game state
 * \sa StateLoadIndex(Game*)
 */
void StateSaveIndex(Game *game) {
  if (!game) {
    Log(L_ASSERT, "Invalid `game` Game.");
  } else {
    /* Emit state information */
    Data *root = DataAlloc();
    TreeForEach(game->states, tStateNode) {
      State *tState = tStateNode->mappingValue;
      StateEmit(root, tState);
    }
    DataSort(root);

    /* Save state index file */
    if (!DataSaveFile(root, STATE_INDEX_FILE))
      Log(L_STATE, "Couldn't save state file `%s`.", STATE_INDEX_FILE);

    /* Cleanup */
    DataFree(root);
  }
}

/*!
 * Stores a copy of a descriptor state.
 * \addtogroup state
 * \param game the game state
 * \param state the descriptor state to store
 * \return a copy of the specified descriptor state or NULL
 */
State *StateStore(
	Game *game,
	const State *state) {
  register State *copied = NULL;
  if (!game) {
    Log(L_ASSERT, "Invalid `game` Game.");
  } else if (!state) {
    Log(L_ASSERT, "Invalid `state` State.");
  } else {
    copied = StateByName(game, state->name);
    if (!copied) {
      copied = StateAlloc(game);
      copied->name = state->name ? strdup(state->name) : NULL;
      if (!TreeInsert(game->states, &copied->name, copied)) {
	Log(L_STATE, "Couldn't add state `%s` to state index.", copied->name);
	StateFree(copied), copied = NULL;
      }
    }
    if (copied)
      StateCopy(copied, state);
  }
  return (copied);
}
