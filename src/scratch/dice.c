/*!
 * \file dice.c
 *
 * \par Copyright
 * Copyright (C) 1999-2023 scratchmud.org
 * All rights reserved.
 *
 * \author Geoffrey Davis <gdavis@scratchmud.org>
 * \addtogroup dice
 */
#define _SCRATCH_DICE_C_

#include <scratch/dice.h>
#include <scratch/log.h>
#include <scratch/memory.h>
#include <scratch/random.h>
#include <scratch/scratch.h>
#include <scratch/string.h>

/*!
 * Constructs a new dice state.
 * \addtogroup dice
 * \return the new dice state or NULL
 * \sa DiceFree(Dice*)
 * \sa DiceFreeV(void*)
 */
Dice *DiceAlloc(void) {
  Dice *dice;
  MemoryCreate(dice, Dice, 1);
  dice->howMany = 0;
  dice->howManySides = 0;
  dice->modifier = 0;
  return (dice);
}

/*!
 * Returns the average dice roll.
 * \addtogroup dice
 * \param dice the dice state whose average dice roll to return
 * \return the average dice roll of the specified dice state
 * \sa DiceMaximum(const Dice*)
 * \sa DiceMinimum(const Dice*)
 */
float DiceAverage(const Dice *dice) {
  register float average = 0.0f;
  if (!dice) {
    Log(L_ASSERT, "Invalid `dice` Dice.");
  } else {
    average = dice->modifier;
    if (dice->howMany && dice->howManySides)
      average += (dice->howManySides + 1) / 2.0f * dice->howMany;
  }
  return (average);
}

/*!
 * Copies a dice state.
 * \addtogroup dice
 * \param toDice the location of the copied dice state
 * \param fromDice the dice state to copy
 */
void DiceCopy(
	Dice *toDice,
	const Dice *fromDice) {
  if (!toDice) {
    Log(L_ASSERT, "Invalid `toDice` Dice.");
  } else if (!fromDice) {
    Log(L_ASSERT, "Invalid `fromDice` Dice.");
  } else if (toDice != fromDice) {
    toDice->howMany = fromDice->howMany;
    toDice->howManySides = fromDice->howManySides;
    toDice->modifier = fromDice->modifier;
  }
}

/*!
 * Frees a dice state.
 * \addtogroup dice
 * \param dice the dice state to free
 * \sa DiceAlloc()
 * \sa DiceFreeV(void*)
 */
void DiceFree(Dice *dice) {
  MemoryFree(dice);
}

/*!
 * Frees a dice state.
 * \addtogroup dice
 * \param dice the dice state to free
 * \sa DiceAlloc()
 * \sa DiceFree(Dice*)
 */
void DiceFreeV(void *dice) {
  DiceFree(dice);
}

/*!
 * Returns the maximum dice roll.
 * \addtogroup dice
 * \param dice the dice state whose maximum dice roll to return
 * \return the maximum dice roll of the specified dice state
 * \sa DiceAverage(const Dice*)
 * \sa DiceMinimum(const Dice*)
 */
int DiceMaximum(const Dice *dice) {
  register int maximum = 0;
  if (!dice) {
    Log(L_ASSERT, "Invalid `dice` Dice.");
  } else {
    maximum = dice->howMany * dice->howManySides + dice->modifier;
  }
  return (maximum);
}

/*!
 * Returns the minimum dice roll.
 * \addtogroup dice
 * \param dice the dice state whose minimum dice roll to return
 * \return the minimum dice roll of the specified dice state
 * \sa DiceAverage(const Dice*)
 * \sa DiceMaximum(const Dice*)
 */
int DiceMinimum(const Dice *dice) {
  register int minimum = 0;
  if (!dice) {
    Log(L_ASSERT, "Invalid `dice` Dice.");
  } else {
    minimum = dice->howMany + dice->modifier;
  }
  return (minimum);
}

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
	const char *diceString) {
  register bool result = false;
  if (!dice) {
    Log(L_ASSERT, "Invalid `dice` Dice.");
  } else if (!diceString || *diceString == '\0') {
    Log(L_ASSERT, "Invalid `diceString` string.");
  } else {
    /* Scan dice expression */
    const int howMany = sscanf(diceString, " %hhud%hhu%d ",
	&dice->howMany,
	&dice->howManySides,
	&dice->modifier);

    if (howMany == 1)
      dice->modifier = dice->howMany;
    if (howMany != 2 && howMany != 3)
      dice->howMany = dice->howManySides = 0;
    if (howMany != 1 && howMany != 3)
      dice->modifier = 0;

    /* One, two, or three values */
    result = howMany >= 1 && howMany <= 3;
  }
  return (result);
}

/*!
 * Rolls the dice and returns the result.
 * \addtogroup dice
 * \param dice the dice state to roll
 * \param rng the RNG state used to perform the dice roll
 * \return the total result of the dice roll
 */
int DiceRoll(
	const Dice *dice,
	Random *rng) {
  register int total = 0;
  if (!dice) {
    Log(L_ASSERT, "Invalid `dice` Dice.");
  } else if (!rng) {
    Log(L_ASSERT, "Invalid `rng` Random.");
  } else {
    if (dice->howManySides && dice->howMany) {
      register size_t diceN = 0;
      for (; diceN < dice->howMany; ++diceN) {
	total += RandomNextInt(rng, 1, dice->howManySides);
      }
    }
    total += dice->modifier;
  }
  return (total);
}

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
	const Dice *dice) {
  register size_t outpos = 0;
  if (!out && outlen) {
    Log(L_ASSERT, "Invalid `out` buffer.");
  } else if (!dice) {
    Log(L_ASSERT, "Invalid `dice` Dice.");
  } else {
    /* How many dice and how many sides */
    if (dice->howMany && dice->howManySides) {
      BPrintf(out, outlen, outpos, "%hhud%hhu",
		dice->howMany,
		dice->howManySides);
    }
    /* Dice roll modifier */
    if (dice->modifier)
      BPrintf(out, outlen, outpos, "%+d", dice->modifier);

    /* Zero */
    if (!outpos)
      outpos = strlcpy(out, "0", outlen);
  }
  return (outpos);
}
