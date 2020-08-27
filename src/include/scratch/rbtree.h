/*!
 * \file rbtree.h
 *
 * \par Copyright
 * Copyright (C) 1999-2019 scratchmud.org
 * All rights reserved.
 *
 * \author Geoffrey Davis <gdavis@scratchmud.org>
 * \addtogroup rbtree
 */
#ifndef _SCRATCH_RBTREE_H_
#define _SCRATCH_RBTREE_H_

#include <scratch/scratch.h>

/* Forward type declarations. */
typedef struct _RBTree RBTree;
typedef struct _RBTreeNode RBTreeNode;

/*! The type of a red-black tree key. */
typedef void *RBTreeKey;

/*! The type of a red-black tree value. */
typedef void *RBTreeValue;

/*! The type of a red-black tree key comparison function. */
typedef int (*RBTreeCompareProc)(
	const RBTreeKey left,
	const RBTreeKey right);

/*! The type of a red-black tree free function. */
typedef void (*RBTreeFreeProc)(RBTreeValue value);

/*!
 * The red-black tree structure.
 * \addtogroup rbtree
 * \{
 */
struct _RBTree {
  RBTreeCompareProc     compareProc;    /*!< Function to order keys */
  RBTreeFreeProc        freeProc;       /*!< Function to free tree values */
  RBTreeNode           *root;           /*!< The root tree node */
};
/*! \} */

/*!
 * One node in a red-black tree.
 * \addtogroup rbtree
 */
struct _RBTreeNode {
  char                  color;          /*!< The color: 'R' or 'B' */
  RBTreeKey             key;            /*!< The mapping key */
  RBTreeNode           *left;           /*!< The left subtree */
  RBTreeNode           *parent;         /*!< The parent tree node */
  RBTreeNode           *right;          /*!< The right subtree */
  RBTreeValue           value;          /*!< The mapping value */
};
/*! \} */

/*!
 * Constructs a new tree.
 * \addtogroup rbtree
 * \param compareProc the function to order keys
 * \param freeProc the function to free values
 * \return a new tree or NULL
 * \sa RBTreeFree(RBTree*)
 */
RBTree *RBTreeAlloc(
	const RBTreeCompareProc compareProc,
	const RBTreeFreeProc freeProc);

/*!
 * Returns the last tree node in a tree.
 * \addtogroup rbtree
 * \param tree the tree to search
 * \return the last tree node in the specified tree, or NULL
 * \sa RBTreeBackKey(RBTree*, void*)
 * \sa RBTreeBackValue(RBTree*, void*)
 */
RBTreeNode *RBTreeBack(RBTree *tree);

/*!
 * Returns the key of the last tree node in a tree.
 * \addtogroup rbtree
 * \param tree the tree to search
 * \param defaultKey the key to return if the specified tree does
 *     not contain any tree nodes
 * \return the key of the last tree node, or NULL
 * \sa RBTreeBack(RBTree*)
 * \sa RBTreeBackValue(RBTree*, void*)
 */
RBTreeKey RBTreeBackKey(
	RBTree *tree,
	const RBTreeKey defaultKey);

/*!
 * Returns the value of the last tree node in a tree.
 * \addtogroup rbtree
 * \param tree the tree to search
 * \param defaultValue the value to return if the specified tree
 *     does not contain any tree nodes
 * \return the value of the last tree node, NULL
 * \sa RBTreeBackKey(RBTree*, void*)
 * \sa RBTreeBackValue(RBTree*, void*)
 */
RBTreeValue RBTreeBackValue(
	RBTree *tree,
	const RBTreeValue defaultValue);

/*!
 * Clears a tree.
 * \addtogroup rbtree
 * \param tree the tree to clear
 */
void RBTreeClear(RBTree *tree);

/*!
 * Clears a tree.
 * \addtogroup rbtree
 * \param tree the tree to clear
 */
void RBTreeClearNoFree(RBTree *tree);

/*!
 * Returns whether a tree contains a key.
 * \addtogroup rbtree
 * \param tree the tree to test
 * \param key the key for which to test
 * \return true if the specified tree contains a tree node
 *     for the specified key
 */
bool RBTreeContains(
	RBTree *tree,
	const RBTreeKey key);

/*!
 * Deletes a tree node for a tree.
 * \addtogroup rbtree
 * \param tree the tree from which to delete
 * \param key the key that identifies the tree node to delete
 * \return true if a tree node was successfully deleted
 */
bool RBTreeDelete(
	RBTree *tree,
	const RBTreeKey key);

/*!
 * Deletes a tree node for a tree.
 * \addtogroup rbtree
 * \param tree the tree from which to delete
 * \param key the key that identifies the tree node to delete
 * \return true if a tree node was successfully deleted
 */
bool RBTreeDeleteNoFree(
	RBTree *tree,
	const RBTreeKey key);

/*!
 * Opens a cursor over a tree.
 * \addtogroup rbtree
 * \param tree the tree to iterate
 * \param name the name of the cursor variable
 * \sa RBTreeForEachEnd()
 */
#define RBTreeForEach(tree, name) \
  do { \
    if ((tree) && (tree)->root) { \
      register RBTreeNode *name = 0, *name##_next = 0; \
      for (name = RBTreeFront(tree); name; name = name##_next) { \
	name##_next = RBTreeSuccessor(name); \
	{

/*!
 * Closes a tree cursor.
 * \addtogroup rbtree
 * \sa RBTreeForEach(tree, name)
 */
#define RBTreeForEachEnd() \
	} \
      } \
    } \
  } while (0)

/*!
 * Frees a tree.
 * \addtogroup rbtree
 * \param tree the tree to free
 * \sa RBTreeClear(RBTree*)
 */
void RBTreeFree(RBTree *tree);

/*!
 * Returns the first tree node in a tree.
 * \addtogroup rbtree
 * \param tree the tree to search
 * \return the first tree node in the specified tree, or NULL
 * \sa RBTreeFrontKey(RBTree*, void*)
 * \sa RBTreeFrontValue(RBTree*, void*)
 */
RBTreeNode *RBTreeFront(RBTree *tree);

/*!
 * Returns the key of the first tree node in a tree.
 * \addtogroup rbtree
 * \param tree the tree to search
 * \param defaultKey the key to return if the specified tree does
 *     not contain any tree nodes
 * \return the key of the first tree node, or NULL
 * \sa RBTreeFront(RBTree*, void*)
 * \sa RBTreeFrontValue(RBTree*, void*)
 */
RBTreeKey RBTreeFrontKey(
	RBTree *tree,
	const RBTreeKey defaultKey);

/*!
 * Returns the value of the first tree node in a tree.
 * \addtogroup rbtree
 * \param tree the tree to search
 * \param defaultValue the value to return if the specified tree
 *     does not contain any tree nodes
 * \return the value of the first tree node, NULL
 * \sa RBTreeFront(RBTree*, void*)
 * \sa RBTreeFrontKey(RBTree*, void*)
 */
RBTreeValue RBTreeFrontValue(
	RBTree *tree,
	const RBTreeValue defaultValue);

/*!
 * Searches a tree for a tree node.
 * \addtogroup rbtree
 * \param tree the tree to search
 * \param key the key that identifies the desired tree node
 * \return the tree node indicated by the specified key, or NULL
 */
RBTreeNode *RBTreeGet(
	RBTree *tree,
	const RBTreeKey key);

/*!
 * Searches a tree for a tree node.
 * \addtogroup rbtree
 * \param tree the tree to search
 * \param key the key that identifies the desired tree node
 * \param defaultValue the value to return if the specified
 *     key does not resolve to an etry in the specified tree
 * \return the value or the specified default value
 */
RBTreeValue RBTreeGetValue(
	RBTree *tree,
	const RBTreeKey key,
	const RBTreeValue defaultValue);

/*!
 * Inserts a tree node into a tree.
 * \addtogroup rbtree
 * \param tree the tree into which to insert
 * \param key the key to assign to the new tree node
 * \param value the value to assign to the new tree node
 * \return the new tree node or NULL
 */
RBTreeNode *RBTreeInsert(
	RBTree *tree,
	const RBTreeKey key,
	const RBTreeValue value);

/*!
 * Returns a tree node's predecessor.
 * \addtogroup rbtree
 * \param node the tree node whose predecessor to return
 * \return the tree node that precedes the specified tree node or NULL
 * \sa RBTreeSuccessor(RBTree*, RBTreeNode*)
 */
RBTreeNode *RBTreePredecessor(RBTreeNode *node);

/*!
 * Returns the size of a tree.
 * \addtogroup rbtree
 * \param tree the tree whose size to return
 * \return the size of the specified tree or zero
 * \sa RBTreeEmpty(const RBTree*)
 */
size_t RBTreeSize(const RBTree *tree);

/*!
 * Returns a tree node's successor.
 * \addtogroup rbtree
 * \param node the tree node whose successor to return
 * \return the tree node that follows the specified tree node or NULL
 * \sa RBTreePredecessor(RBTree*, RBTreeNode*)
 */
RBTreeNode *RBTreeSuccessor(RBTreeNode *node);

#endif /* _SCRATCH_RBTREE_H_ */
