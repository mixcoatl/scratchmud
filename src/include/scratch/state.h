/*!
 * \file state.h
 *
 * \par Copyright
 * Copyright (C) 1999-2023 scratchmud.org
 * All rights reserved.
 *
 * \author Geoffrey Davis <gdavis@scratchmud.org>
 * \addtogroup state
 */
#ifndef _SCRATCH_STATE_H_
#define _SCRATCH_STATE_H_

#include <scratch/scratch.h>

/* Forward type declarations */
typedef struct Data Data;
typedef struct Descriptor Descriptor;
typedef struct Game Game;
typedef struct State State;
typedef struct StateBits StateBits;

/*!
 * The state bitfield structure.
 * \addtogroup state
 * \{
 */
struct StateBits {
  uint8_t               initial: 1;     /*!< State is initial */
  uint8_t               prompt: 1;      /*!< State displays prompt */
  uint8_t               quiet: 1;       /*!< State does not echo input */
};
/*! \} */

/*!
 * Declares a state function.
 * \addtogroup state
 */
#define STATE(name) \
  bool name(Descriptor *d, Game *game, const char *input)

/*!
 * The type of a state function.
 * \addtogroup state
 */
typedef STATE((*StateFunc));

/*!
 * The state structure.
 * \addtogroup state
 * \{
 */
struct State {
  StateBits             bits;           /*!< The state bits */
  StateFunc             focus;          /*!< The focus callback */
  StateFunc             focusLost;      /*!< The focus lost callback */
  char                 *focusLostName;  /*!< The focus lost callback name */
  char                 *focusName;      /*!< The focus callback name */
  Game                 *game;           /*!< The game state */
  char                 *name;           /*!< The canonical name */
  StateFunc             received;       /*!< The input received callback */
  char                 *receivedName;   /*!< The input received callback name */
};
/*! \} */

/*!
 * Constructs a new descriptor state.
 * \addtogroup state
 * \param game the game state
 * \return the new descriptor state or NULL
 * \sa StateFree(State*)
 * \sa StateFreeV(void*)
 */
State *StateAlloc(Game *game);

/*!
 * Searches for a descriptor state using its name.
 * \addtogroup state
 * \param game the game state
 * \param stateName the state name of the descriptor state to return
 * \return the descriptor state indicated by the specified state name or NULL
 */
State *StateByName(
	Game *game,
	const char *stateName);

/*!
 * Changes descriptor states.
 * \addtogroup descriptor
 * \param d the descriptor whose descriptor state to change
 * \param state the descriptor state to which to change
 * \sa StateChangeByName(Descriptor*, const char*)
 */
void StateChange(
	Descriptor *d,
	State *state);

/*!
 * Changes descriptor states.
 * \addtogroup descriptor
 * \param d the descriptor whose descriptor state to change
 * \param stateName the state name of the descriptor state to which to change
 * \sa StateChange(Descriptor*, State*)
 */
void StateChangeByName(
	Descriptor *d,
	const char *stateName);

/*!
 * Copies a descriptor state.
 * \addtogroup state
 * \param toState the location of the copied descriptor state
 * \param fromState the descriptor state to copy
 */
void StateCopy(
	State *toState,
	const State *fromState);

/*!
 * Returns the size of a descriptor state in bytes.
 * \addtogroup state
 * \param state the descriptor state whose size to return
 * \return the size of the specified descriptor state in bytes
 */
size_t StateCountBytes(const State *state);

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
	const char *stateName);

/*!
 * Emits a descriptor state.
 * \addtogroup state
 * \param toData the data element to which to write
 * \param fromState the descriptor state to emit
 * \sa StateParse(Data*, State*)
 */
void StateEmit(
	Data *toData,
	const State *fromState);

/*!
 * Frees a descriptor state.
 * \addtogroup state
 * \param state the descriptor state to free
 * \sa StateAlloc(Game*)
 * \sa StateFreeV(void*)
 */
void StateFree(State *state);

/*!
 * Frees a descriptor state.
 * \addtogroup state
 * \param state the descriptor state to free
 * \sa StateAlloc(Game*)
 * \sa StateFree(State*)
 */
void StateFreeV(void *state);

/*!
 * Returns an initial descriptor state.
 * \addtogroup state
 * \param game the game state
 * \return the first descriptor state encountered in the descriptor
 *     state index that has its initial bitfield set or NULL
 */
State *StateInitial(Game *game);

/*!
 * Loads the descriptor state index.
 * \addtogroup state
 * \param game the game state
 * \sa StateSaveIndex(Game*)
 */
void StateLoadIndex(Game *game);

/*!
 * Parses a descriptor state.
 * \addtogroup state
 * \param fromData the data element to parse
 * \param toState the location of the parsed descriptor state
 * \sa StateEmit(Data*, const State*)
 */
void StateParse(
	Data *fromData,
	State *toState);

/*!
 * Saves the descriptor state index.
 * \addtogroup state
 * \param game the game state
 * \sa StateLoadIndex(Game*)
 */
void StateSaveIndex(Game *game);

/*!
 * Stores a copy of a descriptor state.
 * \addtogroup state
 * \param game the game state
 * \param state the descriptor state to store
 * \return a copy of the specified descriptor state or NULL
 */
State *StateStore(
	Game *game,
	const State *state);

#endif /* _SCRATCH_STATE_H_ */
