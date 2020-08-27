/*!
 * \file list.h
 *
 * \par Copyright
 * Copyright (C) 1999-2019 scratchmud.org
 * All rights reserved.
 *
 * \author Geoffrey Davis <gdavis@scratchmud.org>
 * \addtogroup list
 */
#ifndef _SCRATCH_LIST_H_
#define _SCRATCH_LIST_H_

#include <scratch/scratch.h>

/* Forward type declarations */
typedef struct _List List;
typedef struct _ListNode ListNode;

/*! The type of a list value. */
typedef void *ListValue;

/*! The type of a list comparison function. */
typedef bool (*ListCompareProc)(
	const ListValue xValue,
	const ListValue yValue);

/*! The type of a list free function. */
typedef void (*ListFreeProc)(ListValue value);

/*!
 * The list structure.
 * \addtogroup list
 * \{
 */
struct _List {
  ListCompareProc compareProc;  /*!< The function to compare list values */
  ListFreeProc  freeProc;       /*!< The function to free list values */
  ListNode     *front;          /*!< The first list node in the list */
};
/*! \} */

/*!
 * One list node.
 * \addtogroup list
 * \{
 */
struct _ListNode {
  ListNode     *next;           /*!< The next list node in the list */
  ListValue     value;          /*!< The list value */
};
/*! \} */

/*!
 * Constructs a new list.
 * \addtogroup list
 * \param compareProc the function to compare list values
 * \param freeProc the function to free list value
 * \return a new list, or NULL
 * \sa ListFree(List*)
 */
List *ListAlloc(
	const ListCompareProc compareProc,
	const ListFreeProc freeProc);

/*!
 * Clears a list.
 * \addtogroup list
 * \param list the list to clear
 */
void ListClear(List *list);

/*!
 * Clears a list.
 * \addtogroup list
 * \param list the list to clear
 */
void ListClearNoFree(List *list);

/*!
 * Opens a cursor over a list.
 * \addtogroup list
 * \param list the list to iterate
 * \param name the name of the cursor variable
 * \sa ListForEachEnd()
 */
#define ListForEach(list, name) \
  do { \
    if ((list) && (list)->front) { \
      register ListNode *name = 0, *name##_next = 0; \
      for (name = (list)->front; name; name = name##_next) { \
	name##_next = name->next; \
	{

/*!
 * Closes a list cursor.
 * \addtogroup list
 * \sa ListForEach(list, name)
 */
#define ListForEachEnd() \
	} \
      } \
    } \
  } while (0)

/*!
 * Frees a list.
 * \addtogroup list
 * \param list the list to free
 */
void ListFree(List *list);

/*!
 * Constructs a new list node.
 * \addtogroup list
 * \return a new list node, or NULL
 */
ListNode *ListNodeAlloc(void);

/*!
 * Removes a list node.
 * \addtogroup list
 * \param list the list from which to remove a list node
 * \param node the list node to remove
 * \return true if the specified list node was successfully removed
 */
bool ListNodeRemove(
	List *list,
	ListNode *node);

/*!
 * Removes a list node.
 * \addtogroup list
 * \param list the list from which to remove a list node
 * \param node the list node to remove
 * \return true if the specified list node was successfully removed
 */
bool ListNodeRemoveNoFree(
	List *list,
	ListNode *node);

/*!
 * Searches for a list node.
 * \addtogroup list
 * \param list the list to search
 * \param value the list value for which to search
 * \return the list node that represents the specified
 *     list value in the specified list or NULL
 */
ListNode *ListNodeSearch(
	List *list,
	const ListValue value);

/*!
 * Adds a list value to a list.
 * \addtogroup list
 * \param list the list to which to add a list value
 * \param value the list value to add to the specified list
 * \return the list node that represents the specified list value or NULL
 */
ListNode *ListPushBack(
	List *list,
	const ListValue value);

/*!
 * Adds a list value to a list.
 * \addtogroup list
 * \param list the list to which to add a list value
 * \param value the list value to add to the specified list
 * \return the list node that represents the specified list value or NULL
 */
ListNode *ListPushFront(
	List *list,
	const ListValue value);

/*!
 * Removes a list value.
 * \addtogroup list
 * \param list the list from which to remove a list value
 * \param value the list value to remove
 * \return true if the list node indicated by the specified list
 *     value was successfully removed
 */
bool ListRemove(
	List *list,
	const ListValue value);

/*!
 * Removes a list value.
 * \addtogroup list
 * \param list the list from which to remove a list value
 * \param value the list value to remove
 * \return true if the list node indicated by the specified list
 *     value was successfully removed
 */
bool ListRemoveNoFree(
	List *list,
	const ListValue value);

/*!
 * Returns the list size.
 * \addtogroup list
 * \param list the list whose size to return
 * \return the size of the specified list or zero
 */
size_t ListSize(const List *list);

#endif /* _SCRATCH_LIST_H_ */
