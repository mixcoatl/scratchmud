/*!
 * \file state.c
 *
 * \par Copyright
 * Copyright (C) 1999-2019 scratchmud.org
 * All rights reserved.
 *
 * \author Geoffrey Davis <gdavis@scratchmud.org>
 * \addtogroup state
 */
#define _SCRATCH_STATE_C_

#include <scratch/bitvector.h>
#include <scratch/client.h>
#include <scratch/constant.h>
#include <scratch/data.h>
#include <scratch/game.h>
#include <scratch/log.h>
#include <scratch/memory.h>
#include <scratch/rbtree.h>
#include <scratch/scratch.h>
#include <scratch/state.h>
#include <scratch/string.h>
#include <scratch/utility.h>

/*!
 * Constructs a new state.
 * \addtogroup state
 * \return the new state or NULL
 * \sa StateFree(State*)
 */
State *StateAlloc(void) {
  register State *state;
  MemoryCreate(state, State, 1);
  state->flags = 0;
  state->focusLostProc = NULL;
  state->focusLostProcName = NULL;
  state->focusProc = NULL;
  state->focusProcName = NULL;
  state->inputProc = NULL;
  state->inputProcName = NULL;
  state->name = NULL;
  return (state);
}

/*!
 * Retrieves a state.
 * \addtogroup state
 * \param game the game state
 * \param stateName the state name of the state to return
 * \return the state indicated by the specified state name or NULL
 */
State *StateByName(
	Game *game,
	const char *stateName) {
  register State *state = NULL;
  if (!game) {
    Log("invalid `game` Game");
  } else if (*StringBlank(stateName) == '\0') {
    Log("invalid `stateName` string");
  } else {
    state = RBTreeGetValue(game->states, (RBTreeKey) &stateName, NULL);
  }
  return (state);
}

/*!
 * Copies a state.
 * \addtogroup state
 * \param toState the location of the copied state
 * \param fromState the state to copy
 */
void StateCopy(
	State *toState,
	const State *fromState) {
  if (!toState) {
    Log("invalid `toState` State");
  } else if (!fromState) {
    Log("invalid `fromState` State");
  } else if (toState != fromState) {
    StringSet(&toState->focusLostProcName, fromState->focusLostProcName);
    StringSet(&toState->focusProcName,     fromState->focusProcName);
    StringSet(&toState->inputProcName,     fromState->inputProcName);
    StringSet(&toState->name,              fromState->name);
    toState->flags         = fromState->flags;
    toState->focusLostProc = fromState->focusLostProc;
    toState->focusProc     = fromState->focusProc;
    toState->inputProc     = fromState->inputProc;
  }
}

/*!
 * Returns the size of a state in bytes.
 * \addtogroup state
 * \param state the state whose size to return
 * \return the size of the specified state in bytes
 */
size_t StateCountBytes(const State *state) {
  register size_t nBytes = 0;
  if (state) {
    if (state->name)
      nBytes += strlen(state->name) + 1;
    if (state->focusLostProcName)
      nBytes += strlen(state->focusLostProcName) + 1;
    if (state->focusProcName)
      nBytes += strlen(state->focusProcName) + 1;
    if (state->inputProcName)
      nBytes += strlen(state->inputProcName) + 1;
    nBytes += sizeof(State);
  }
  return (nBytes);
}

/*!
 * Deletes a state.
 * \addtogroup state
 * \param game the game state
 * \param stateName the state name of the state to delete
 * \return true if the state indicated by the specified state name
 *     was successfully deleted
 */
bool StateDelete(
	Game *game,
	const char *stateName) {
  register bool result = false;
  if (!game) {
    Log("invalid `game` Game");
  } else {
    result = RBTreeDelete(game->states, (RBTreeKey) &stateName);
  }
  return (result);
}

/*!
 * Emits a state.
 * \addtogroup state
 * \param toData the data to which to write
 * \param fromState the state to emit
 */
static void StateEmitFunctions(
	Data *toData,
	const State *fromState) {
  if (!toData) {
    Log("invalid `toData` Data");
  } else if (!fromState) {
    Log("invalid `fromState` State");
  } else {
    Data *xFunctions = DataAlloc();
    char procName[MAXLEN_INPUT] = {'\0'};

    /* Focus lost function */
    snprintf(procName, sizeof(procName), "%sOnFocusLost", fromState->name);
    if (*StringBlank(fromState->focusLostProcName) != '\0' &&
	 StringCmp(fromState->focusLostProcName, procName) != 0)
      DataPutString(xFunctions, "FocusLost", fromState->focusLostProcName);

    /* Focus function */
    snprintf(procName, sizeof(procName), "%sOnFocus", fromState->name);
    if (*StringBlank(fromState->focusProcName) != '\0' &&
	 StringCmp(fromState->focusProcName, procName) != 0)
      DataPutString(xFunctions, "Focus", fromState->focusProcName);

    /* Input function */
    snprintf(procName, sizeof(procName), "%sOnInput", fromState->name);
    if (*StringBlank(fromState->inputProcName) != '\0' &&
	 StringCmp(fromState->inputProcName, procName) != 0)
      DataPutString(xFunctions, "Input", fromState->inputProcName);

    if (DataSize(xFunctions) == 0) {
      DataFree(xFunctions);
    } else {
      DataPut(toData, "Functions", xFunctions);
      DataSort(xFunctions);
    }
  }
}

/*!
 * Emits a state.
 * \addtogroup state
 * \param toData the data to which to write
 * \param fromState the state to emit
 */
static void StateEmitStateBits(
	Data *toData,
	const State *fromState) {
  if (!toData) {
    Log("invalid `toData` Data");
  } else if (!fromState) {
    Log("invalid `fromState` State");
  } else {
    if (fromState->flags)
      DataPutBits(toData, "StateBits", _G_stateBits, fromState->flags);
  }
}

/*!
 * Emits a state.
 * \addtogroup state
 * \param toData the data to which to write
 * \param fromState the state to emit
 * \sa StateParse(Data*, State*)
 */
void StateEmit(
	Data *toData,
	const State *fromState) {
  if (!toData) {
    Log("invalid `toData` Data");
  } else if (!fromState) {
    Log("invalid `fromState` State");
  } else {
    Data *xState = DataAlloc();
    StateEmitFunctions(xState, fromState);
    StateEmitStateBits(xState, fromState);
    if (DataSize(xState) == 0) {
      DataPutString(toData, fromState->name, "");
      DataFree(xState);
    } else {
      DataPut(toData, fromState->name, xState);
      DataSort(xState);
    }
  }
}

/*!
 * Frees a state.
 * \addtogroup state
 * \param state the state to free
 * \sa StateAlloc()
 */
void StateFree(State *state) {
  if (state) {
    StringFree(state->focusLostProcName);
    StringFree(state->focusProcName);
    StringFree(state->inputProcName);
    StringFree(state->name);
    MemoryFree(state);
  }
}

/*!
 * Returns an initial state.
 * \addtogroup state
 * \param game the game state
 * \return the first state encountered in the state index that
 *     has its STATE_INITIAL flag set, or NULL
 */
State *StateGetInitial(Game *game) {
  if (!game) {
    Log("invalid `game` Game");
  } else {
    RBTreeForEach(game->states, tStateNode) {
      /* Iterator variable */
      State *tState = tStateNode->value;

      /* Check whether we're an initial state */
      if (BitCheckN(tState->flags, STATE_INITIAL))
	return (tState);
    }
    RBTreeForEachEnd();
  }
  return (NULL);
}

#define STATE_INDEX_FILE "data/state.dat"

/*!
 * Loads the state index.
 * \addtogroup state
 * \param game the game state
 * \sa StateSaveIndex(Game*)
 */
void StateLoadIndex(Game *game) {
  if (!game) {
    Log("invalid `game` Game");
  } else {
    Data *root = DataLoadFile(STATE_INDEX_FILE);
    if (!root) {
      Log("Couldn't load state file `%s`", STATE_INDEX_FILE);
    } else {
      /* Load up connection states */
      Data *xStateList = DataGet(root, "States");
      DataForEach(xStateList, xStateKey, xState) {
	State *state = StateAlloc();
	StringSet(&state->name, xStateKey);
	StateParse(xState, state);
	if (UtilityNameValid(state->name))
	  StateStore(game, state);
	StateFree(state);
      }
      DataForEachEnd();
      DataFree(root);
    }

    /* Calculate some metrics */
    register size_t nStateBytes = 0;
    register size_t nStates = 0;
    RBTreeForEach(game->states, tStateNode) {
      /* Iterator variable */
      const State *tState = tStateNode->value;

      /* Update byte and record counts */
      nStateBytes += StateCountBytes(tState);
      nStates++;
    }
    RBTreeForEachEnd();
    Log("Loaded %zu client state(s), %zu byte(s).", nStates, nStateBytes);
  }
}

/*!
 * Returns whether a state prints a prompt.
 * \addtogroup state
 * \param state the state for which to return whether to print a prompt
 * \return true if the specified state must print a prompt
 */
bool StateNeedsPrompt(const State *state) {
  return state && BitCheckN(state->flags, STATE_PROMPT);
}

/*!
 * Parses a state.
 * \addtogroup state
 * \param fromData the data to parse
 * \param toState the location of the parsed state
 */
static void StateParseFunctions(
	Data *fromData,
	State *toState) {
  if (!fromData) {
    Log("invalid `fromData` Data");
  } else if (!toState) {
    Log("invalid `toState` State");
  } else {
    char procName[MAXLEN_INPUT] = {'\0'};
    Data *xFunctions = DataGet(fromData, "Functions");

    /* Focus lost function */
    StringFree(toState->focusLostProcName);
    toState->focusLostProcName = DataGetStringCopy(xFunctions, "FocusLost", NULL);

    /* Resolve focus lost function */
    toState->focusLostProc = NULL;
    if (*StringBlank(toState->focusLostProcName) != '\0')
      toState->focusLostProc = UtilityProcByName(toState->focusLostProcName);
    else {
      snprintf(procName, sizeof(procName), "%sOnFocusLost", toState->name);
      if ((toState->focusLostProc = UtilityProcByName(procName)) != NULL)
	StringSet(&toState->focusLostProcName, procName);
    }

    /* Focus function */
    StringFree(toState->focusProcName);
    toState->focusProcName = DataGetStringCopy(xFunctions, "Focus", NULL);

    /* Resolve focus function */
    toState->focusProc = NULL;
    if (*StringBlank(toState->focusProcName) != '\0')
      toState->focusProc = UtilityProcByName(toState->focusProcName);
    else {
      snprintf(procName, sizeof(procName), "%sOnFocus", toState->name);
      if ((toState->focusProc = UtilityProcByName(procName)) != NULL)
	StringSet(&toState->focusProcName, procName);
    }

    /* Input function */
    StringFree(toState->inputProcName);
    toState->inputProcName = DataGetStringCopy(xFunctions, "Input", NULL);

    /* Resolve input function */
    toState->inputProc = NULL;
    if (*StringBlank(toState->inputProcName) != '\0')
      toState->inputProc = UtilityProcByName(toState->inputProcName);
    else {
      snprintf(procName, sizeof(procName), "%sOnInput", toState->name);
      if ((toState->inputProc = UtilityProcByName(procName)) != NULL)
	StringSet(&toState->inputProcName, procName);
    }
  }
}

/*!
 * Parses a state.
 * \addtogroup state
 * \param fromData the data to parse
 * \param toState the location of the parsed state
 */
static void StateParseStateBits(
	Data *fromData,
	State *toState) {
  if (!fromData) {
    Log("invalid `fromData` Data");
  } else if (!toState) {
    Log("invalid `toState` State");
  } else {
    toState->flags = DataGetBits(fromData, "StateBits", _G_stateBits, 0);
  }
}

/*!
 * Parses a state.
 * \addtogroup state
 * \param fromData the data to parse
 * \param toState the location of the parsed state
 * \sa StateEmit(Data*, const State*)
 */
void StateParse(
	Data *fromData,
	State *toState) {
  if (!fromData) {
    Log("invalid `fromData` Data");
  } else if (!toState) {
    Log("invalid `toState` State");
  } else {
    StateParseFunctions(fromData, toState);
    StateParseStateBits(fromData, toState);
  }
}

/*!
 * Saves the state index.
 * \addtogroup state
 * \param game the game state
 * \sa StateLoadIndex(Game*)
 */
void StateSaveIndex(Game *game) {
  if (!game) {
    Log("invalid `game` Game");
  } else {
    Data *root = DataAlloc();
    Data *xStateList = DataAlloc();
    RBTreeForEach(game->states, tStateNode) {
      /* Iterator variable */
      const State *tState = tStateNode->value;

      /* Emit state information */
      StateEmit(xStateList, tState);
    }
    RBTreeForEachEnd();

    if (DataSize(xStateList) == 0) {
      DataFree(xStateList);
    } else {
      DataPut(root, "States", xStateList);
      DataSort(xStateList);
    }
    if (!DataSaveFile(root, STATE_INDEX_FILE))
      Log("Couldn't save state file `%s`", STATE_INDEX_FILE);

    DataFree(root);
  }
}

/*!
 * Stores a copy of a state.
 * \addtogroup state
 * \param game the game state
 * \param state the state to store
 * \return a copy of the specified state or NULL
 */
State *StateStore(
	Game *game,
	const State *state) {
  register State *copied = NULL;
  if (!game) {
    Log("invalid `game` Game");
  } else if (!state) {
    Log("invalid `state` State");
  } else {
    copied = StateByName(game, state->name);
    if (!copied) {
      copied = StateAlloc();
      StringSet(&copied->name, state->name);
      if (!RBTreeInsert(game->states, (RBTreeKey) &copied->name, copied)) {
	Log("Couldn't add state `%s` to state index", copied->name);
	StateFree(copied), copied = NULL;
      }
    }
    if (copied)
      StateCopy(copied, state);
  }
  return (copied);
}
