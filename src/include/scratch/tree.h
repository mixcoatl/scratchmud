/*!
 * \file tree.h
 *
 * \par Copyright
 * Copyright (C) 1999-2023 scratchmud.org
 * All rights reserved.
 *
 * \author Geoffrey Davis <gdavis@scratchmud.org>
 * \addtogroup tree
 */
#ifndef _SCRATCH_TREE_H_
#define _SCRATCH_TREE_H_

#include <scratch/scratch.h>

/* Forward type declarations */
typedef struct Tree Tree;
typedef struct TreeNode TreeNode;

/*! The type of a tree mapping free function. */
typedef void (*TreeFreeFunc)(void *value);

/*! The type of a tree comparison function. */
typedef int (*TreeCompareFunc)(
	const void *left,
	const void *right);

/*!
 * The tree structure.
 * \addtogroup tree
 * \{
 */
struct Tree {
  TreeCompareFunc       compare;        /*!< The function to compare mapping keys */
  TreeFreeFunc          freeKey;        /*!< The function to free mapping keys */
  TreeFreeFunc          freeValue;      /*!< The function to free mapping values */
  TreeNode             *root;           /*!< The root tree node */
};
/*! \} */

/*!
 * The tree node structure.
 * \addtogroup tree
 */
struct TreeNode {
  char                  color;          /*!< The color: 'R' or 'B' */
  TreeNode             *left;           /*!< The left subtree */
  void                 *mappingKey;     /*!< The mapping key */
  void                 *mappingValue;   /*!< The mapping value */
  TreeNode             *parent;         /*!< The parent tree node */
  TreeNode             *right;          /*!< The right subtree */
};
/*! \} */

/*!
 * Constructs a new tree.
 * \addtogroup tree
 * \param compare the function to compare mapping keys
 * \param freeKey the function to free mapping keys
 * \param freeValue the function to free mapping values
 * \return the new tree or NULL
 * \sa TreeFree(Tree*)
 * \sa TreeFreeV(void*)
 */
Tree *TreeAlloc(
	const TreeCompareFunc compare,
	const TreeFreeFunc freeKey,
	const TreeFreeFunc freeValue);

/*!
 * Returns the last tree node.
 * \addtogroup tree
 * \param tree the tree instance
 * \return the last tree node in the specified tree or NULL
 * \sa TreeBackKey(Tree*, void*)
 * \sa TreeBackValue(Tree*, void*)
 */
TreeNode *TreeBack(Tree *tree);

/*!
 * Returns the mapping key of the last tree node.
 * \addtogroup tree
 * \param tree the tree instance
 * \param defaultKey the mapping key to return if the
 *     specified tree does not contain any tree nodes
 * \return the mapping key of the last tree node
 * \sa TreeBack(Tree*)
 * \sa TreeBackValue(Tree*, void*)
 */
void *TreeBackKey(
	Tree *tree,
	const void *defaultKey);

/*!
 * Returns the mapping value of the last tree node.
 * \addtogroup tree
 * \param tree the tree instance
 * \param defaultValue the mapping value to return if the
 *     specified tree does not contain any tree nodes
 * \return the mapping value of the last tree node
 * \sa TreeBackKey(Tree*, void*)
 * \sa TreeBackValue(Tree*, void*)
 */
void *TreeBackValue(
	Tree *tree,
	const void *defaultValue);

/*!
 * Clears a tree.
 * \addtogroup tree
 * \param tree the tree to clear
 */
void TreeClear(Tree *tree);

/*!
 * Clears a tree.
 * \addtogroup tree
 * \param tree the tree to clear
 */
void TreeClearNoFree(Tree *tree);

/*!
 * Deletes a tree node.
 * \addtogroup tree
 * \param tree the tree instance
 * \param mappingKey the mapping key that identifies the tree node to delete
 * \return true if the tree node indicated by the specified mapping key was
 *    successfully deleted
 * \sa TreeDeleteNoFree(Tree*, const void*)
 */
bool TreeDelete(
	Tree *tree,
	const void *mappingKey);

/*!
 * Deletes a tree node.
 * \addtogroup tree
 * \param tree the tree instance
 * \param mappingKey the mapping key that identifies the tree node to delete
 * \return true if the tree node indicated by the specified mapping key was
 *    successfully deleted
 * \sa TreeDelete(Tree*, const void*)
 */
bool TreeDeleteNoFree(
	Tree *tree,
	const void *mappingKey);

/*!
 * Opens a cursor over a tree.
 * \addtogroup tree
 * \param tree the tree instance
 * \param cursor the name of the cursor variable
 */
#define TreeForEach(tree, cursor) \
  for (TreeNode *cursor = TreeFront(tree); \
		 cursor; cursor = TreeSuccessor(cursor))

/*!
 * Frees a tree.
 * \addtogroup tree
 * \param tree the tree to free
 * \sa TreeAlloc(const TreeCompareFunc, const TreeFreeFunc, const TreeFreeFunc)
 * \sa TreeFreeV(void*)
 */
void TreeFree(Tree *tree);

/*!
 * Frees a tree.
 * \addtogroup tree
 * \param tree the tree to free
 * \sa TreeAlloc(const TreeCompareFunc, const TreeFreeFunc, const TreeFreeFunc)
 * \sa TreeFree(Tree*)
 */
void TreeFreeV(void *tree);

/*!
 * Returns the first tree node.
 * \addtogroup tree
 * \param tree the tree instance
 * \return the first tree node in the specified tree or NULL
 * \sa TreeFrontKey(Tree*, void*)
 * \sa TreeFrontValue(Tree*, void*)
 */
TreeNode *TreeFront(Tree *tree);

/*!
 * Returns the mapping key of the first tree node.
 * \addtogroup tree
 * \param tree the tree instance
 * \param defaultKey the mapping key to return if the
 *     specified tree does not contain any tree nodes
 * \return the mapping key of the first tree node
 * \sa TreeFront(Tree*, void*)
 * \sa TreeFrontValue(Tree*, void*)
 */
void *TreeFrontKey(
	Tree *tree,
	const void *defaultKey);

/*!
 * Returns the mapping value of the first tree node.
 * \addtogroup tree
 * \param tree the tree instance
 * \param defaultValue the mapping value to return if the
 *     specified tree does not contain any tree nodes
 * \return the mapping value of the first tree node
 * \sa TreeFront(Tree*, void*)
 * \sa TreeFrontKey(Tree*, void*)
 */
void *TreeFrontValue(
	Tree *tree,
	const void *defaultValue);

/*!
 * Searches for a tree node.
 * \addtogroup tree
 * \param tree the tree instance
 * \param mappingKey the mapping key for which to search
 * \return the tree node indicated by the specified key or NULL
 */
TreeNode *TreeGet(
	Tree *tree,
	const void *mappingKey);

/*!
 * Searches for a tree node.
 * \addtogroup tree
 * \param tree the tree instance
 * \param mappingKey the mapping key for which to search
 * \param defaultValue the mapping value to return if the
 *     specified tree does not contain any tree nodes, or the
 *     specified mapping key cannot be resolved
 * \return the mapping value indicated by the specified mapping key
 */
void *TreeGetValue(
	Tree *tree,
	const void *mappingKey,
	const void *defaultValue);

/*!
 * Inserts a tree node.
 * \addtogroup tree
 * \param tree the tree instance
 * \param mappingKey the mapping key of the new tree node
 * \param mappingValue the mapping value of the new tree node
 * \return the new tree node or NULL
 */
TreeNode *TreeInsert(
	Tree *tree,
	const void *mappingKey,
	const void *mappingValue);

/*!
 * Returns the predecessor for a tree node.
 * \addtogroup tree
 * \param node the tree node whose predecessor to return
 * \return the predecessor for the specified tree node or NULL
 * \sa TreeSuccessor(Tree*, TreeNode*)
 */
TreeNode *TreePredecessor(TreeNode *node);

/*!
 * Returns the size of a tree.
 * \addtogroup tree
 * \param tree the tree instance
 * \return the size of the specified tree or zero
 */
size_t TreeSize(const Tree *tree);

/*!
 * Returns the successor for a tree node.
 * \addtogroup tree
 * \param node the tree node whose successor to return
 * \return the successor for the specified tree node or NULL
 * \sa TreePredecessor(Tree*, TreeNode*)
 */
TreeNode *TreeSuccessor(TreeNode *node);

#endif /* _SCRATCH_TREE_H_ */
