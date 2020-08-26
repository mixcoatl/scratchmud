/*!
 * \file action.h
 *
 * \par Copyright
 * Copyright (C) 1999-2019 scratchmud.org
 * All rights reserved.
 *
 * \author Geoffrey Davis <gdavis@scratchmud.org>
 * \addtogroup action
 */
#ifndef _SCRATCH_ACTION_H_
#define _SCRATCH_ACTION_H_

#include <scratch/bitvector.h>
#include <scratch/scratch.h>

/*!
 * Act target flags.
 * \addtogroup action
 * \{
 */
#define TO_ACTOR        Bit(0)  /*!< Send message to its actor */
#define TO_HIDE         Bit(1)  /*!< The action is hidden */
#define TO_NEARBY       Bit(2)  /*!< Send message to nearby objects */
#define TO_TARGET       Bit(3)  /*!< Send message to its target */
/*! \} */

/*! Some shortcut macros. \{ */
#define TO_ALL		(TO_ACTOR  | TO_NEARBY | TO_TARGET)
#define TO_NOT_ACTOR	(TO_NEARBY | TO_TARGET)
#define TO_NOT_TARGET	(TO_ACTOR  | TO_NEARBY)
/*! \} */

/* Forward type declarations */
typedef struct _Object Object;

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
	const void *indirect);

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
	Object *to);

#endif /* _SCRATCH_ACTION_H_ */
