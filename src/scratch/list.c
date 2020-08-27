/*!
 * \file list.c
 *
 * \par Copyright
 * Copyright (C) 1999-2019 scratchmud.org
 * All rights reserved.
 *
 * \author Geoffrey Davis <gdavis@scratchmud.org>
 * \addtogroup list
 */
#define _SCRATCH_LIST_C_

#include <scratch/list.h>
#include <scratch/log.h>
#include <scratch/memory.h>
#include <scratch/scratch.h>

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
	const ListFreeProc freeProc) {
  List *list;
  MemoryCreate(list, List, 1);
  list->compareProc = compareProc;
  list->freeProc    = freeProc;
  list->front       = NULL;
  return (list);
}

/*!
 * Clears a list.
 * \addtogroup list
 * \param list the list to clear
 */
void ListClear(List *list) {
  if (!list) {
    Log("invalid `list` List");
  } else {
    while (list->front)
      ListNodeRemove(list, list->front);
  }
}

/*!
 * Clears a list.
 * \addtogroup list
 * \param list the list to clear
 */
void ListClearNoFree(List *list) {
  if (!list) {
    Log("invalid `list` List");
  } else {
    while (list->front)
      ListNodeRemoveNoFree(list, list->front);
  }
}

/*!
 * Frees a list.
 * \addtogroup list
 * \param list the list to free
 */
void ListFree(List *list) {
  if (list) {
    ListClear(list);
    MemoryFree(list);
  }
}

/*!
 * Constructs a new list node.
 * \addtogroup list
 * \return a new list node, or NULL
 */
ListNode *ListNodeAlloc(void) {
  ListNode *node;
  MemoryCreate(node, ListNode, 1);
  node->next  = NULL;
  node->value = NULL;
  return (node);
}

/*!
 * Removes a list node.
 * \addtogroup list
 * \param list the list from which to remove a list node
 * \param node the list node to remove
 * \return true if the specified list node was successfully removed
 */
bool ListNodeRemove(
	List *list,
	ListNode *node) {
  register bool result = false;
  if (!list) {
    Log("invalid `list` List");
  } else {
    if (list->front == node) {
      list->front = node->next;
      result = true;
    } else {
      register ListNode *temp;
      for (temp = list->front; temp; temp = temp->next) {
	if (temp && temp->next == node)
	  break;
      }
      if (temp && temp->next == node) {
	temp->next = node->next;
	result = true;
      }
    }
    if (result)
      if (list->freeProc)
	list->freeProc(node->value);

    if (result)
      MemoryFree(node);
  }
  return (result);
}

/*!
 * Removes a list node.
 * \addtogroup list
 * \param list the list from which to remove a list node
 * \param node the list node to remove
 * \return true if the specified list node was successfully removed
 */
bool ListNodeRemoveNoFree(
	List *list,
	ListNode *node) {
  register bool result = false;
  if (!list) {
    Log("invalid `list` List");
  } else {
    if (list->front == node) {
      list->front = node->next;
      result = true;
    } else {
      register ListNode *temp;
      for (temp = list->front; temp; temp = temp->next) {
	if (temp && temp->next == node)
	  break;
      }
      if (temp && temp->next == node) {
	temp->next = node->next;
	result = true;
      }
    }
    if (result)
      MemoryFree(node);
  }
  return (result);
}

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
	const ListValue value) {
  if (!list) {
    Log("invalid `list` List");
  } else {
    register ListNode *node;
    for (node = list->front; node; node = node->next) {
      if (list->compareProc &&
	  list->compareProc(node->value, value))
	return (node);
      if (node->value == value)
	return (node);
    }
  }
  return (NULL);
}

/*!
 * Adds a list value to a list.
 * \addtogroup list
 * \param list the list to which to add a list value
 * \param value the list value to add to the specified list
 * \return the list node that represents the specified list value or NULL
 */
ListNode *ListPushBack(
	List *list,
	const ListValue value) {
  register ListNode *node = NULL;
  if (!list) {
    Log("invalid `list` List");
  } else {
    /* Create a new list node */
    node = ListNodeAlloc();
    node->value = value;

    /* Link it into the list */
    if (!list->front)
      list->front = node;
    else {
      register ListNode *temp;
      for (temp = list->front; temp; temp = temp->next) {
	if (temp && !temp->next)
	  break;
      }
      temp->next = node;
    }
  }
  return (node);
}

/*!
 * Adds a list value to a list.
 * \addtogroup list
 * \param list the list to which to add a list value
 * \param value the list value to add to the specified list
 * \return the list node that represents the specified list value or NULL
 */
ListNode *ListPushFront(
	List *list,
	const ListValue value) {
  register ListNode *node = NULL;
  if (!list) {
    Log("invalid `list` List");
  } else {
    /* Create a new list node */
    node = ListNodeAlloc();
    node->value = value;

    /* Link it into the list */
    node->next = list->front;
    list->front = node;
  }
  return (node);
}

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
	const ListValue value) {
  register bool result = false;
  if (!list) {
    Log("invalid `list` List");
  } else {
    ListNode *node = ListNodeSearch(list, value);
    if (node)
      result = ListNodeRemove(list, node);
  }
  return (result);
}

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
	const ListValue value) {
  register bool result = false;
  if (!list) {
    Log("invalid `list` List");
  } else {
    ListNode *node = ListNodeSearch(list, value);
    if (node)
      result = ListNodeRemoveNoFree(list, node);
  }
  return (result);
}

/*!
 * Returns the list size.
 * \addtogroup list
 * \param list the list whose size to return
 * \return the size of the specified list or zero
 */
size_t ListSize(const List *list) {
  register size_t listSize = 0;
  if (list) {
    register ListNode *node;
    for (node = list->front; node; node = node->next)
      listSize++;
  }
  return (listSize);
}
