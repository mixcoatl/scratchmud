/*!
 * \file list.c
 *
 * \par Copyright
 * Copyright (C) 1999-2023 scratchmud.org
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
 * \param compare the function to compare list values
 * \param free the function to free list values
 * \return the new list or NULL
 * \sa ListFree(List*)
 * \sa ListFreeV(void*)
 */
List *ListAlloc(
	const ListCompareFunc compare,
	const ListFreeFunc free) {
  List *list;
  MemoryCreate(list, List, 1);
  list->compare = compare;
  list->free = free;
  list->front = NULL;
  return (list);
}

/*!
 * Returns the last list node.
 * \addtogroup list
 * \param list the list instance
 * \return the last list node in the specified list, or NULL
 * \sa ListBackValue(List*, const void*)
 */
ListNode *ListBack(List *list) {
  register ListNode *node = NULL;
  if (list) {
    for (node = list->front; node; node = node->next) {
      if (!node->next)
	break;
    }
  }
  return (node);
}

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
	const void *defaultValue) {
  if (list) {
    register ListNode *node;
    if ((node = ListBack(list)) != NULL)
      return (node->value);
  }
  return (void*) defaultValue;
}

/* List helper function. */
static bool ListNodeRemove(
	List *list,
	ListNode *node) {
  register bool result = false;
  if (!list) {
    Log(L_ASSERT, "Invalid `list` List.");
  } else if (!node) {
    Log(L_ASSERT, "Invalid `node` ListNode.");
  } else {
    /* Unlink list node */
    if (list->front == node) {
      list->front = node->next;
      result = true;
    } else {
      register ListNode *temp = list->front;
      for (; temp && !result; temp = temp->next) {
	if (temp && temp->next == node) {
	  temp->next = node->next;
	  result = true;
	}
      }
    }

    /* Free list value */
    if (result && list->free && node->value)
      list->free(node->value);

    /* Free list node */
    if (result)
      MemoryFree(node);
  }
  return (result);
}

/*!
 * Clears a list.
 * \addtogroup list
 * \param list the list to clear
 * \sa ListClearNoFree(List*)
 */
void ListClear(List *list) {
  if (!list) {
    Log(L_ASSERT, "Invalid `list` List.");
  } else {
    while (list->front)
      ListNodeRemove(list, list->front);
  }
}

/* List helper function. */
static bool ListNodeRemoveNoFree(
	List *list,
	ListNode *node) {
  register bool result = false;
  if (!list) {
    Log(L_ASSERT, "Invalid `list` List.");
  } else if (!node) {
    Log(L_ASSERT, "Invalid `node` ListNode.");
  } else {
    /* Unlink list node */
    if (list->front == node) {
      list->front = node->next;
      result = true;
    } else {
      register ListNode *temp = list->front;
      for (; temp && !result; temp = temp->next) {
	if (temp && temp->next == node) {
	  temp->next = node->next;
	  result = true;
	}
      }
    }

    /* Free list node */
    if (result)
      MemoryFree(node);
  }
  return (result);
}

/*!
 * Clears a list.
 * \addtogroup list
 * \param list the list to clear
 * \sa ListClear(List*)
 */
void ListClearNoFree(List *list) {
  if (!list) {
    Log(L_ASSERT, "Invalid `list` List.");
  } else {
    while (list->front)
      ListNodeRemoveNoFree(list, list->front);
  }
}

/*!
 * Frees a list.
 * \addtogroup list
 * \param list the list to free
 * \sa ListAlloc(const ListCompareFunc, const ListFreeFunc)
 * \sa ListFreeV(void*)
 */
void ListFree(List *list) {
  if (list) {
    ListClear(list);
    MemoryFree(list);
  }
}

/*!
 * Frees a list.
 * \addtogroup list
 * \param list the list to free
 * \sa ListAlloc(const ListCompareFunc, const ListFreeFunc)
 * \sa ListFree(List*)
 */
void ListFreeV(void *list) {
  ListFree(list);
}

/*!
 * Returns the first list node.
 * \addtogroup list
 * \param list the list instance
 * \return the first list node in the specified list, or NULL
 * \sa ListFrontValue(List*, const void*)
 */
ListNode *ListFront(List *list) {
  return (list ? list->front : NULL);
}

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
	const void *defaultValue) {
  if (list) {
    register ListNode *node;
    if ((node = ListFront(list)) != NULL)
      return (node->value);
  }
  return (void*) defaultValue;
}

/*!
 * Searches for a list node.
 * \addtogroup list
 * \param list the list instance
 * \param value the list value for which to search
 * \return the list node indicated by the specified list value, or NULL
 */
ListNode *ListGet(
	List *list,
	const void *value) {
  if (!list) {
    Log(L_ASSERT, "Invalid `list` List.");
  } else {
    register ListNode *node;
    for (node = list->front; node; node = node->next) {
      if (list->compare && list->compare(node->value, value))
	return (node);
      if (node->value == value)
	return (node);
    }
  }
  return (NULL);
}

/* List helper function */
static ListNode *ListNodeAlloc(void) {
  ListNode *node;
  MemoryCreate(node, ListNode, 1);
  node->next = NULL;
  node->value = NULL;
  return (node);
}

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
	const void *value) {
  register ListNode *node = NULL;
  if (!list) {
    Log(L_ASSERT, "Invalid `list` List.");
  } else {
    /* Create new list node */
    node = ListNodeAlloc();
    node->value = (void*) value;

    /* Link list node into list */
    if (!list->front) {
      list->front = node;
    } else {
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
 * \param list the list instance
 * \param value the list value to add
 * \return the new list node or NULL
 * \sa ListPushBack(List*, const void*)
 */
ListNode *ListPushFront(
	List *list,
	const void *value) {
  register ListNode *node = NULL;
  if (!list) {
    Log(L_ASSERT, "Invalid `list` List.");
  } else {
    /* Create new list node */
    node = ListNodeAlloc();
    node->value = (void*) value;

    /* Link list node into list */
    node->next = list->front;
    list->front = node;
  }
  return (node);
}

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
	const void *value) {
  register bool result = false;
  if (!list) {
    Log(L_ASSERT, "Invalid `list` List.");
  } else {
    register ListNode *node;
    if ((node = ListGet(list, value)) != NULL)
      result = ListNodeRemove(list, node);
  }
  return (result);
}

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
	const void *value) {
  register bool result = false;
  if (!list) {
    Log(L_ASSERT, "Invalid `list` List.");
  } else {
    register ListNode *node;
    if ((node = ListGet(list, value)) != NULL)
      result = ListNodeRemoveNoFree(list, node);
  }
  return (result);
}

/*!
 * Returns the size of a list.
 * \addtogroup list
 * \param list the list instance
 * \return the size of the specified list or zero
 */
size_t ListSize(const List *list) {
  register size_t howMany = 0;
  if (list) {
    register ListNode *node;
    for (node = list->front; node; node = node->next)
      howMany++;
  }
  return (howMany);
}

/*!
 * Returns the successor for a list node.
 * \addtogroup list
 * \param list the list instance
 * \param node the successor for the specified list node or NULL
 */
ListNode *ListSuccessor(
	List *list,
	ListNode *node) {
  return (list && node ? node->next : NULL);
}
