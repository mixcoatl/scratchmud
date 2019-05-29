/*!
 * \file memory.h
 *
 * \par Copyright
 * Copyright (C) 1999-2019 scratchmud.org
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
 * Copies memory.
 * \addtogroup memory
 * \param dest the pointer to which to copy
 * \param src the pointer from which to copy
 * \param type the C type of the memory to copy
 * \param count the number of elements of the specified C type
 */
#define MemoryCopy(dest, src, type, count) \
  do { \
    if ((dest) && (src) && (count) > 0) \
      memmove((dest), (src), sizeof(type) * (count)); \
  } while (0)

/*!
 * Allocates memory.
 * \addtogroup memory
 * \param ptr the pointer to allocate
 * \param type the C type of the memory to allocate
 * \param count the number of elements of the specified C type
 */
#define MemoryCreate(ptr, type, count) \
  do { \
    if ((count) > 0) { \
      if (((ptr) = calloc((count), sizeof(type))) == 0) { \
        Log("calloc() failed: errno=%d", errno); \
        abort(); \
      } \
    } else \
      (ptr) = 0; \
  } while (0);

/*!
 * Frees memory.
 * \addtogroup memory
 * \param ptr the pointer to free
 */
#define MemoryFree(ptr) \
  do { \
    if (ptr) \
      free(ptr), (ptr) = 0; \
  } while (0)

/*!
 * Reallocates memory.
 * \addtogroup memory
 * \param ptr the pointer to reallocate
 * \param type the C type of the memory to reallocate
 * \param count the number of elements of the specified C type
 */
#define MemoryRecreate(ptr, type, count) \
  do { \
    if ((count) > 0) { \
      if (((ptr) = (type*) realloc((ptr), sizeof(type) * (count))) == 0) { \
        Log("realloc() failed: errno=%d", errno); \
        abort(); \
      } \
    } else \
      MemoryFree(ptr); \
  } while (0)

/*!
 * Initializes memory.
 * \addtogroup memory
 * \param ptr the memory to initialize
 * \param type the C structure or primitive type to initialize
 * \param count the number of elements to initialize
 */
#define MemoryZero(ptr, type, count) \
  do { \
    if ((ptr) && sizeof(type) * (count)) \
      memset((ptr), '\0', sizeof(type) * (count)); \
  } while (0)

#endif /* _SCRATCH_MEMORY_H_ */
