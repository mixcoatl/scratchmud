/*!
 * \file list.h
 *
 * \par Copyright
 * Copyright (C) 1999-2023 scratchmud.org
 * All rights reserved.
 *
 * \author Geoffrey Davis <gdavis@scratchmud.org>
 * \addtogroup list
 */
#ifndef _SCRATCH_LIST_H_
#define _SCRATCH_LIST_H_

#include <scratch/scratch.h>

/* Forward type declarations */
typedef struct List List;
typedef struct ListNode ListNode;

/*! The type of a list value free function. */
typedef void (*ListFreeFunc)(void *value);

/*! The type of a list value comparison function. */
typedef bool (*ListCompareFunc)(
	const void *left,
	const void *right);

/*!
 * The list structure.
 * \addtogroup list
 * \{
 */
struct List {
  ListCompareFunc       compare;        /*!< The function to compare list values */
  ListFreeFunc          free;           /*!< The function to free list values */
  ListNode             *front;          /*!< The first list node */
};
/*! \} */

/*!
 * The list node structure.
 * \addtogroup list
 * \{
 */
struct ListNode {
  ListNode             *next;           /*!< The next list node */
  void                 *value;          /*!< The list value */
};
/*! \} */

/*!
 * Constructs a new list.
 * \addtogroup list
 * \param compare the function to compare list values
 * \param free the function to free list values
 * \return the new list or NULL
 * \sa ListFree(List*)
 * \sa ListFreeV(void*)
 */
List *ListAlloc(
	const ListCompareFunc compare,
	const ListFreeFunc free);

/*!
 * Returns the last list node.
 * \addtogroup list
 * \param list the list instance
 * \return the last list node in the specified list, or NULL
 * \sa ListBackValue(List*, const void*)
 */
ListNode *ListBack(List *list);

/*!
 * Returns the list value of the last list node.
 * \addtogroup list
 * \param list the list instance
 * \param defaultValue the list value to return if the
 *     specified list does not contain any list nodes
 * \return the list value of last list node
 * \sa ListBack(List*)
 */
void *ListBackValue(
	List *list,
	const void *defaultValue);

/*!
 * Clears a list.
 * \addtogroup list
 * \param list the list to clear
 * \sa ListClearNoFree(List*)
 */
void ListClear(List *list);

/*!
 * Clears a list.
 * \addtogroup list
 * \param list the list to clear
 * \sa ListClear(List*)
 */
void ListClearNoFree(List *list);

/*!
 * Opens a cursor over a list.
 * \addtogroup list
 * \param list the list instance
 * \param cursor the name of the cursor variable
 */
#define ListForEach(list, cursor) \
  for (ListNode *cursor = ListFront(list), \
		*cursor##Next = ListSuccessor(list, cursor); \
		 cursor; cursor = cursor##Next, \
		 cursor##Next = ListSuccessor(list, cursor))

/*!
 * Frees a list.
 * \addtogroup list
 * \param list the list to free
 * \sa ListAlloc(const ListCompareFunc, const ListFreeFunc)
 * \sa ListFreeV(void*)
 */
void ListFree(List *list);

/*!
 * Frees a list.
 * \addtogroup list
 * \param list the list to free
 * \sa ListAlloc(const ListCompareFunc, const ListFreeFunc)
 * \sa ListFree(List*)
 */
void ListFreeV(void *list);

/*!
 * Returns the first list node.
 * \addtogroup list
 * \param list the list instance
 * \return the first list node in the specified list, or NULL
 * \sa ListFrontValue(List*, const void*)
 */
ListNode *ListFront(List *list);

/*!
 * Returns the list value of the first list node.
 * \addtogroup list
 * \param list the list instance
 * \param defaultValue the list value to return if the
 *     specified list does not contain any list nodes
 * \return the list value of first list node
 * \sa ListFront(List*)
 */
void *ListFrontValue(
	List *list,
	const void *defaultValue);

/*!
 * Searches for a list node.
 * \addtogroup list
 * \param list the list instance
 * \param value the list value for which to search
 * \return the list node indicated by the specified list value, or NULL
 */
ListNode *ListGet(
	List *list,
	const void *value);

/*!
 * Adds a list value to a list.
 * \addtogroup list
 * \param list the list instance
 * \param value the list value to add
 * \return the new list node or NULL
 * \sa ListPushFront(List*, const void*)
 */
ListNode *ListPushBack(
	List *list,
	const void *value);

/*!
 * Adds a list value to a list.
 * \addtogroup list
 * \param list the list instance
 * \param value the list value to add
 * \return the new list node or NULL
 * \sa ListPushBack(List*, const void*)
 */
ListNode *ListPushFront(
	List *list,
	const void *value);

/*!
 * Removes a list node.
 * \addtogroup list
 * \param list the list instance
 * \param value the list value that identifies the list node to remove
 * \return true if the list node indicated by the specified list value
 *     was successfully removed
 * \sa ListRemoveNoFree(List*, const void*)
 */
bool ListRemove(
	List *list,
	const void *value);

/*!
 * Removes a list node.
 * \addtogroup list
 * \param list the list instance
 * \param value the list value that identifies the list node to remove
 * \return true if the list node indicated by the specified list value
 *     was successfully removed
 * \sa ListRemove(List*, const void*)
 */
bool ListRemoveNoFree(
	List *list,
	const void *value);

/*!
 * Returns the size of a list.
 * \addtogroup list
 * \param list the list instance
 * \return the size of the specified list or zero
 */
size_t ListSize(const List *list);

/*!
 * Returns the successor for a list node.
 * \addtogroup list
 * \param list the list instance
 * \param node the successor for the specified list node or NULL
 */
ListNode *ListSuccessor(
	List *list,
	ListNode *node);

#endif /* _SCRATCH_LIST_H_ */
