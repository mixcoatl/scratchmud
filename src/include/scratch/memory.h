/*!
 * \file memory.h
 *
 * \par Copyright
 * Copyright (C) 1999-2023 scratchmud.org
 * All rights reserved.
 *
 * \author Geoffrey Davis <gdavis@scratchmud.org>
 * \addtogroup memory
 */
#ifndef _SCRATCH_MEMORY_H_
#define _SCRATCH_MEMORY_H_

#include <scratch/log.h>
#include <scratch/scratch.h>

/*!
 * Copies a memory block.
 * \addtogroup memory
 * \param toBlock the location of the copied memory block
 * \param fromBlock the memory block to copy
 * \param type the C type of each memory block element
 * \param howMany the memory block element count
 */
#define MemoryCopy(toBlock, fromBlock, type, howMany) \
  do { \
    if ((toBlock) != 0 && (fromBlock) != 0 && (howMany)) \
      memmove((toBlock), (fromBlock), sizeof(type) * (howMany)); \
  } while (0)

/*!
 * Allocates a memory memory.
 * \addtogroup memory
 * \param block the memory block to allocate
 * \param type the C type of each memory block element
 * \param howMany the memory block element count
 * \sa MemoryFree(block)
 */
#define MemoryCreate(block, type, howMany) \
  do { \
    if (howMany) { \
      if (!((block) = calloc((howMany), sizeof(type)))) { \
        Log(L_SYSTEM, "calloc() failed: errno=%d.", errno); \
        abort(); \
      } \
    } else \
      (block) = 0; \
  } while (0);

/*!
 * Frees a memory block.
 * \addtogroup memory
 * \param block the memory block to free
 * \sa MemoryCreate(block, type, howMany)
 */
#define MemoryFree(block) \
  do { \
    if (block) \
      free(block), (block) = 0; \
  } while (0)

/*!
 * Reallocates a memory block.
 * \addtogroup memory
 * \param block the memory block to reallocate
 * \param type the C type of each memory block element
 * \param howMany the memory block element count
 * \sa MemoryCreate(block, type, howMany)
 * \sa MemoryFree(block)
 */
#define MemoryRecreate(block, type, howMany) \
  do { \
    if (howMany) { \
      if (!((block) = realloc((block), sizeof(type) * (howMany)))) { \
        Log(L_SYSTEM, "realloc() failed: errno=%d.", errno); \
        abort(); \
      } \
    } else \
      MemoryFree(block); \
  } while (0)

/*!
 * Initializes a memory block.
 * \addtogroup memory
 * \param block the memory block to initialize
 * \param type the C type of each memory block element
 * \param howMany the memory block element count
 */
#define MemoryZero(block, type, howMany) \
  do { \
    if ((block) != 0 && (howMany)) \
      memset((block), '\0', sizeof(type) * (howMany)); \
  } while (0)

#endif /* _SCRATCH_MEMORY_H_ */
