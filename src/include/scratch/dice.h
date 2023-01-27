/*!
 * \file dice.h
 *
 * \par Copyright
 * Copyright (C) 1999-2023 scratchmud.org
 * All rights reserved.
 *
 * \author Geoffrey Davis <gdavis@scratchmud.org>
 * \addtogroup dice
 */
#ifndef _SCRATCH_DICE_H_
#define _SCRATCH_DICE_H_

#include <scratch/scratch.h>

/* Forward type declarations */
typedef struct Dice Dice;
typedef struct Random Random;

/*!
 * The dice structure.
 * \addtogroup dice
 * \{
 */
struct Dice {
  uint8_t               howMany;        /*!< How many dice? */
  uint8_t               howManySides;   /*!< How many sides for each die? */
  int                   modifier;       /*!< The dice roll modifier */
};
/*! \} */

/*!
 * Constructs a new dice state.
 * \addtogroup dice
 * \return the new dice state or NULL
 * \sa DiceFree(Dice*)
 * \sa DiceFreeV(void*)
 */
Dice *DiceAlloc(void);

/*!
 * Returns the average dice roll.
 * \addtogroup dice
 * \param dice the dice state whose average dice roll to return
 * \return the average dice roll of the specified dice state
 * \sa DiceMaximum(const Dice*)
 * \sa DiceMinimum(const Dice*)
 */
float DiceAverage(const Dice *dice);

/*!
 * Copies a dice state.
 * \addtogroup dice
 * \param toDice the location of the copied dice state
 * \param fromDice the dice state to copy
 */
void DiceCopy(
	Dice *toDice,
	const Dice *fromDice);

/*!
 * Frees a dice state.
 * \addtogroup dice
 * \param dice the dice state to free
 * \sa DiceAlloc()
 * \sa DiceFreeV(void*)
 */
void DiceFree(Dice *dice);

/*!
 * Frees a dice state.
 * \addtogroup dice
 * \param dice the dice state to free
 * \sa DiceAlloc()
 * \sa DiceFree(Dice*)
 */
void DiceFreeV(void *dice);

/*!
 * Returns the maximum dice roll.
 * \addtogroup dice
 * \param dice the dice state whose maximum dice roll to return
 * \return the maximum dice roll of the specified dice state
 * \sa DiceAverage(const Dice*)
 * \sa DiceMinimum(const Dice*)
 */
int DiceMaximum(const Dice *dice);

/*!
 * Returns the minimum dice roll.
 * \addtogroup dice
 * \param dice the dice state whose minimum dice roll to return
 * \return the minimum dice roll of the specified dice state
 * \sa DiceAverage(const Dice*)
 * \sa DiceMaximum(const Dice*)
 */
int DiceMinimum(const Dice *dice);

/*!
 * Parses a dice state.
 * \addtogroup dice
 * \param dice the location of the parsed dice state
 * \param diceString the dice expression in the form of XdY+Z
 * \return true if the specified dice expression was successfully parsed
 * \sa DiceToString(char*, const size_t, const Dice*)
 */
bool DiceParse(
	Dice *dice,
	const char *diceString);

/*!
 * Rolls the dice and returns the result.
 * \addtogroup dice
 * \param dice the dice state to roll
 * \param rng the RNG state used to perform the dice roll
 * \return the total result of the dice roll
 */
int DiceRoll(
	const Dice *dice,
	Random *rng);

/*!
 * Converts a dice state to a string.
 * \addtogroup dice
 * \param out the output buffer
 * \param outlen the length of the specified buffer
 * \param dice the dice state to convert
 * \return the number of bytes written to the specified buffer
 * \sa DiceParse(Dice*, const char*)
 */
size_t DiceToString(
	char *out, const size_t outlen,
	const Dice *dice);

#endif /* _SCRATCH_DICE_H_ */
