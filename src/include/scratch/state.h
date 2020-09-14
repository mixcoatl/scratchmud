/*!
 * \file state.h
 *
 * \par Copyright
 * Copyright (C) 1999-2019 scratchmud.org
 * All rights reserved.
 *
 * \author Geoffrey Davis <gdavis@scratchmud.org>
 * \addtogroup state
 */
#ifndef _SCRATCH_STATE_H_
#define _SCRATCH_STATE_H_

#include <scratch/bitvector.h>
#include <scratch/scratch.h>

/*!
 * The state bits.
 * \addtogroup state
 * \{
 */
#define STATE_INITIAL           (0)     /*!< State is the initial state */
#define STATE_PROMPT            (1)     /*!< State displays the prompt */
#define STATE_QUIET             (2)     /*!< State does not echo input */
/*! \} */

/*! How many STATE_x bits? */
#define STATE_COUNT \
  (STATE_QUIET - STATE_INITIAL + 1)

/* Forward type declarations */
typedef struct _Client Client;
typedef struct _Data   Data;
typedef struct _Game   Game;
typedef struct _State  State;

/*!
 * Declares a state callback.
 * \addtogroup state
 */
#define STATE(_Name) \
  bool _Name(Client *client, Game *game, const char *input)

/*!
 * The type of a state callback.
 * \addtogroup state
 */
typedef STATE((*StateProc));

/*!
 * The state structure.
 * \addtogroup state
 * \{
 */
struct _State {
  Bitvector     flags;             /*!< The state flags: STATE_x */
  StateProc     focusLostProc;     /*!< The focus lost callback */
  char         *focusLostProcName; /*!< The focus lost callback name */
  StateProc     focusProc;         /*!< The focus callback */
  char         *focusProcName;     /*!< The focus callback name */
  Game         *game;              /*!< The game state */
  StateProc     inputProc;         /*!< The line input callback */
  char         *inputProcName;     /*!< The line input callback name */
  char         *name;              /*!< The canonical name */
};
/*! \} */

/*!
 * Constructs a new state.
 * \addtogroup state
 * \return the new state or NULL
 * \sa StateFree(State*)
 */
State *StateAlloc(void);

/*!
 * Retrieves a state.
 * \addtogroup state
 * \param game the game state
 * \param stateName the state name of the state to return
 * \return the state indicated by the specified state name or NULL
 */
State *StateByName(
	Game *game,
	const char *stateName);

/*!
 * Copies a state.
 * \addtogroup state
 * \param toState the location of the copied state
 * \param fromState the state to copy
 */
void StateCopy(
	State *toState,
	const State *fromState);

/*!
 * Returns the size of a state in bytes.
 * \addtogroup state
 * \param state the state whose size to return
 * \return the size of the specified state in bytes
 */
size_t StateCountBytes(const State *state);

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
	const char *stateName);

/*!
 * Emits a state.
 * \addtogroup state
 * \param toData the data to which to write
 * \param fromState the state to emit
 * \sa StateParse(Data*, State*)
 */
void StateEmit(
	Data *toData,
	const State *fromState);

/*!
 * Frees a state.
 * \addtogroup state
 * \param state the state to free
 * \sa StateAlloc()
 */
void StateFree(State *state);

/*!
 * Returns an initial state.
 * \addtogroup state
 * \param game the game state
 * \return the first state encountered in the state index that
 *     has its STATE_INITIAL flag set, or NULL
 */
State *StateGetInitial(Game *game);

/*!
 * Loads the state index.
 * \addtogroup state
 * \param game the game state
 * \sa StateSaveIndex(Game*)
 */
void StateLoadIndex(Game *game);

/*!
 * Returns whether a state prints a prompt.
 * \addtogroup state
 * \param state the state for which to return whether to print a prompt
 * \return true if the specified state must print a prompt
 */
bool StateNeedsPrompt(const State *state);

/*!
 * Parses a state.
 * \addtogroup state
 * \param fromData the data to parse
 * \param toState the location of the parsed state
 * \sa StateEmit(Data*, const State*)
 */
void StateParse(
	Data *fromData,
	State *toState);

/*!
 * Saves the state index.
 * \addtogroup state
 * \param game the game state
 * \sa StateLoadIndex(Game*)
 */
void StateSaveIndex(Game *game);

/*!
 * Stores a copy of a state.
 * \addtogroup state
 * \param game the game state
 * \param state the state to store
 * \return a copy of the specified state or NULL
 */
State *StateStore(
	Game *game,
	const State *state);

#endif /* _SCRATCH_STATE_H_ */
