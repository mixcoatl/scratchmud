/*!
 * \file creator.c
 *
 * \par Copyright
 * Copyright (C) 1999-2023 scratchmud.org
 * All rights reserved.
 *
 * \author Geoffrey Davis <gdavis@scratchmud.org>
 * \addtogroup creator
 */
#define _SCRATCH_CREATOR_C_

#include <scratch/creator.h>
#include <scratch/log.h>
#include <scratch/memory.h>
#include <scratch/scratch.h>
#include <scratch/string.h>

/*!
 * Constructs a new creator state.
 * \addtogroup creator
 * \return the new creator state or NULL
 * \sa CreatorFree(Creator*)
 * \sa CreatorFreeV(void*)
 */
Creator *CreatorAlloc(void) {
  Creator *creator;
  MemoryCreate(creator, Creator, 1);
  creator->modified     = false;
  creator->name         = NULL;
  return (creator);
}

/*!
 * Frees a creator state.
 * \addtogroup creator
 * \param creator the creator state to free
 * \sa CreatorAlloc()
 * \sa CreatorFreeV(void*)
 */
void CreatorFree(Creator *creator) {
  if (creator) {
    StringFree(creator->name);
    MemoryFree(creator);
  }
}

/*!
 * Frees a creator state.
 * \addtogroup creator
 * \param creator the creator state to free
 * \sa CreatorAlloc()
 * \sa CreatorFree(Creator*)
 */
void CreatorFreeV(void *creator) {
  CreatorFree(creator);
}
