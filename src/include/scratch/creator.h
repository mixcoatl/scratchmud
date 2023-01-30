/*!
 * \file creator.h
 *
 * \par Copyright
 * Copyright (C) 1999-2023 scratchmud.org
 * All rights reserved.
 *
 * \author Geoffrey Davis <gdavis@scratchmud.org>
 * \addtogroup creator
 */
#ifndef _SCRATCH_CREATOR_H_
#define _SCRATCH_CREATOR_H_

#include <scratch/scratch.h>

/* Forward type declarations */
typedef struct Creator Creator;
typedef struct User User;

/*!
 * The creator state structure.
 * \addtogroup creator
 * \{
 */
struct Creator {
  bool                  modified;       /*!< The modification flag */
  char                 *name;           /*!< The canonical name or NULL */
  char                 *password;       /*!< The temporary password buffer */
  User                 *user;           /*!< The user */
};
/*! \} */

/*!
 * Constructs a new creator state.
 * \addtogroup creator
 * \return the new creator state or NULL
 * \sa CreatorFree(Creator*)
 * \sa CreatorFreeV(void*)
 */
Creator *CreatorAlloc(void);

/*!
 * Frees a creator state.
 * \addtogroup creator
 * \param creator the creator state to free
 * \sa CreatorAlloc()
 * \sa CreatorFreeV(void*)
 */
void CreatorFree(Creator *creator);

/*!
 * Frees a creator state.
 * \addtogroup creator
 * \param creator the creator state to free
 * \sa CreatorAlloc()
 * \sa CreatorFree(Creator*)
 */
void CreatorFreeV(void *creator);

#endif /* _SCRATCH_CREATOR_H_ */
