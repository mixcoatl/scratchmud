/*!
 * \file tree.c
 *
 * \par Copyright
 * Copyright (C) 1999-2023 scratchmud.org
 * All rights reserved.
 *
 * \author Geoffrey Davis <gdavis@scratchmud.org>
 * \addtogroup tree
 */
#define _SCRATCH_TREE_C_

#include <scratch/log.h>
#include <scratch/memory.h>
#include <scratch/scratch.h>
#include <scratch/tree.h>

/*!
 * Returns the color of a tree node.
 * \addtogroup tree
 * \param node the tree node
 * \return the color of the specified tree node
 */
#define TreeNodeColor(node) \
    (node ? node->color : 'B')

/*!
 * Returns the left tree node.
 * \addtogroup tree
 * \param node the tree node
 * \return the tree node to the left of the specified tree node
 */
#define TreeNodeLeft(node) \
    (node ? node->left : NULL)

/*!
 * Returns the parent tree node.
 * \addtogroup tree
 * \param node the tree node
 * \return the tree node above the specified tree node
 */
#define TreeNodeParent(node) \
    (node ? node->parent : NULL)

/*!
 * Returns the right tree node.
 * \addtogroup tree
 * \param node the tree node
 * \return the tree node to the right of the specified tree node
 */
#define TreeNodeRight(node) \
    (node ? node->right : NULL)

/*!
 * Sets the color of a tree node.
 * \addtogroup tree
 * \param node the tree node
 * \param theColor the new color: either 'R' or 'B'
 */
#define TreeNodeSetColor(node, theColor) \
  do { \
    if (node) \
      node->color = theColor; \
  } while (0)

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
	const TreeFreeFunc freeValue) {
  Tree *tree = NULL;
  if (!compare) {
    Log(L_ASSERT, "Invalid `compare` TreeCompareFunc.");
  } else {
    MemoryCreate(tree, Tree, 1);
    tree->compare = compare;
    tree->freeKey = freeKey;
    tree->freeValue = freeValue;
    tree->root = NULL;
  }
  return (tree);
}

/*!
 * Returns the last tree node.
 * \addtogroup tree
 * \param tree the tree instance
 * \return the last tree node in the specified tree or NULL
 * \sa TreeBackKey(Tree*, void*)
 * \sa TreeBackValue(Tree*, void*)
 */
TreeNode *TreeBack(Tree *tree) {
  register TreeNode *back = NULL;
  if (tree) {
    for (back = tree->root;
	 back && back->right; back = back->right) {
      /* Nothing */
    }
  }
  return (back);
}

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
	const void *defaultKey) {
  if (tree) {
    register TreeNode *node;
    if ((node = TreeBack(tree)) != NULL)
      return (node->mappingKey);
  }
  return (void*) (defaultKey);
}

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
	const void *defaultValue) {
  if (tree) {
    register TreeNode *node;
    if ((node = TreeBack(tree)) != NULL)
      return (node->mappingValue);
  }
  return (void*) (defaultValue);
}

/*! Tree helper function. */
static void TreeNodeFreeData(
	Tree *tree,
	TreeNode *node,
	const void *mappingKey,
	const void *mappingValue) {
  if (!tree) {
    Log(L_ASSERT, "Invalid `tree` Tree.");
  } else if (node) {
    if (tree->freeKey && node->mappingKey && node->mappingKey != mappingKey)
      tree->freeKey(node->mappingKey);
    if (tree->freeValue && node->mappingValue && node->mappingValue != mappingValue)
      tree->freeValue(node->mappingValue);
  }
}

/*! Tree helper function. */
static void TreeNodeFree(
	Tree *tree,
	TreeNode *node) {
  if (!tree) {
    Log(L_ASSERT, "Invalid `tree` Tree.");
  } else if (node) {
    TreeNodeFreeData(tree, node, NULL, NULL);
    MemoryFree(node);
  }
}

/*! Tree helper function. */
static void TreeNodeClear(
	Tree *tree,
	TreeNode *node) {
  if (!tree) {
    Log(L_ASSERT, "Invalid `tree` Tree.");
  } else if (node) {
    /* Recurse */
    TreeNodeClear(tree, node->left);
    TreeNodeClear(tree, node->right);

    /* Free tree node */
    TreeNodeFree(tree, node);
  }
}

/*!
 * Clears a tree.
 * \addtogroup tree
 * \param tree the tree to clear
 */
void TreeClear(Tree *tree) {
  if (!tree) {
    Log(L_ASSERT, "Invalid `tree` Tree.");
  } else {
    TreeNodeClear(tree, tree->root);
    tree->root = NULL;
  }
}

/*! Tree helper function. */
static void TreeNodeClearNoFree(
	Tree *tree,
	TreeNode *node) {
  if (!tree) {
    Log(L_ASSERT, "Invalid `tree` Tree.");
  } else if (node) {
    /* Recurse */
    TreeNodeClearNoFree(tree, node->left);
    TreeNodeClearNoFree(tree, node->right);

    /* Free node */
    MemoryFree(node);
  }
}

/*!
 * Clears a tree.
 * \addtogroup tree
 * \param tree the tree to clear
 */
void TreeClearNoFree(Tree *tree) {
  if (!tree) {
    Log(L_ASSERT, "Invalid `tree` Tree.");
  } else {
    TreeNodeClearNoFree(tree, tree->root);
    tree->root = NULL;
  }
}

/*! Tree helper function. */
static void TreeNodeRotateLeft(
	Tree *tree,
	TreeNode *node) {
  if (!tree) {
    Log(L_ASSERT, "Invalid `tree` Tree.");
  } else if (!node) {
    Log(L_ASSERT, "Invalid `node` TreeNode.");
  } else {
    TreeNode *right = node->right;
    node->right = right->left;

    if (right->left)
      right->left->parent = node;

    right->parent = node->parent;

    if (!node->parent) {
      tree->root = right;
    } else if (node->parent->left == node) {
      node->parent->left = right;
    } else {
      node->parent->right = right;
    }
    right->left = node;
    node->parent = right;
  }
}

/*! Tree helper function. */
static void TreeNodeRotateRight(
	Tree *tree,
	TreeNode *node) {
  if (!tree) {
    Log(L_ASSERT, "Invalid `tree` Tree.");
  } else if (!node) {
    Log(L_ASSERT, "Invalid `node` TreeNode.");
  } else {
    TreeNode *left = node->left;
    node->left = left->right;

    if (left->right)
      left->right->parent = node;

    left->parent = node->parent;

    if (!node->parent) {
      tree->root = left;
    } else if (node->parent->right == node) {
      node->parent->right = left;
    } else {
      node->parent->left = left;
    }
    left->right = node;
    node->parent = left;
  }
}

/*! Tree helper function. */
static void TreeNodeSwap(
	TreeNode *left,
	TreeNode *right) {
  if (!left) {
    Log(L_ASSERT, "Invalid `left` TreeNode.");
  } else if (!right) {
    Log(L_ASSERT, "Invalid `right` TreeNode.");
  } else if (left != right) {
    /* Swap tree node keys */
    void *leftKey = left->mappingKey;
    left->mappingKey = right->mappingKey;
    right->mappingKey = leftKey;

    /* Swap tree node values */
    void *leftValue = left->mappingValue;
    left->mappingValue = right->mappingValue;
    right->mappingValue = leftValue;
  }
}

/*! Tree helper function. */
static void TreeNodeDeleteBalance(
	Tree *tree,
	TreeNode *node) {
  if (!tree) {
    Log(L_ASSERT, "Invalid `tree` Tree.");
  } else if (!node) {
    Log(L_ASSERT, "Invalid `node` TreeNode.");
  } else {
    while (node != tree->root && TreeNodeColor(node) != 'R') {
      if (TreeNodeLeft(TreeNodeParent(node)) == node) {
	TreeNode *sibling = TreeNodeRight(TreeNodeParent(node));
	if (TreeNodeColor(sibling) == 'R') {
	  TreeNodeSetColor(sibling, 'B');
	  TreeNodeSetColor(TreeNodeParent(node), 'R');
	  TreeNodeRotateLeft(tree, TreeNodeParent(node));
	  sibling = TreeNodeRight(TreeNodeParent(node));
	}
	if (TreeNodeColor(TreeNodeLeft(sibling)) != 'R' &&
	    TreeNodeColor(TreeNodeRight(sibling)) != 'R') {
	  TreeNodeSetColor(sibling, 'R');
	  node = TreeNodeParent(node);
	} else {
	  if (TreeNodeColor(TreeNodeRight(sibling)) != 'R') {
	    TreeNodeSetColor(TreeNodeLeft(sibling), 'B');
	    TreeNodeSetColor(sibling, 'R');
	    TreeNodeRotateRight(tree, sibling);
	    sibling = TreeNodeRight(TreeNodeParent(node));
	  }
	  TreeNodeSetColor(sibling, TreeNodeColor(TreeNodeParent(node)));
	  TreeNodeSetColor(TreeNodeParent(node), 'B');
	  TreeNodeSetColor(TreeNodeRight(sibling), 'B');
	  TreeNodeRotateLeft(tree, TreeNodeParent(node));
	  node = tree->root;
	}
      } else {
	TreeNode *sibling = TreeNodeLeft(TreeNodeParent(node));
	if (TreeNodeColor(sibling) == 'R') {
	  TreeNodeSetColor(sibling, 'B');
	  TreeNodeSetColor(TreeNodeParent(node), 'R');
	  TreeNodeRotateRight(tree, TreeNodeParent(node));
	  sibling = TreeNodeLeft(TreeNodeParent(node));
	}
	if (TreeNodeColor(TreeNodeRight(sibling)) != 'R' &&
	    TreeNodeColor(TreeNodeLeft(sibling)) != 'R') {
	  TreeNodeSetColor(sibling, 'R');
	  node = TreeNodeParent(node);
	} else {
	  if (TreeNodeColor(TreeNodeLeft(sibling)) != 'R') {
	    TreeNodeSetColor(TreeNodeRight(sibling), 'B');
	    TreeNodeSetColor(sibling, 'R');
	    TreeNodeRotateLeft(tree, sibling);
	    sibling = TreeNodeLeft(TreeNodeParent(node));
	  }
	  TreeNodeSetColor(sibling, TreeNodeColor(TreeNodeParent(node)));
	  TreeNodeSetColor(TreeNodeParent(node), 'B');
	  TreeNodeSetColor(TreeNodeLeft(sibling), 'B');
	  TreeNodeRotateRight(tree, TreeNodeParent(node));
	  node = tree->root;
	}
      }
    }
    TreeNodeSetColor(node, 'B');
  }
}

/*! Tree helper function. */
static void TreeNodeDelete(
	Tree *tree,
	TreeNode *node) {
  if (!tree) {
    Log(L_ASSERT, "Invalid `tree` Tree.");
  } else if (!node) {
    Log(L_ASSERT, "Invalid `node` TreeNode.");
  } else {
    register TreeNode *p = node;
    register TreeNode *replacement = NULL;
    if (TreeNodeLeft(p) && TreeNodeRight(p)) {
      p = TreeSuccessor(node);
      TreeNodeSwap(p, node);
    }
    replacement = TreeNodeLeft(p) ? TreeNodeLeft(p) : TreeNodeRight(p);

    if (replacement) {
      replacement->parent = TreeNodeParent(p);
      if (!TreeNodeParent(p)) {
	tree->root = replacement;
      } else if (TreeNodeLeft(TreeNodeParent(p)) == p) {
	p->parent->left = replacement;
      } else {
	p->parent->right = replacement;
      }
      if (TreeNodeColor(p) == 'B') {
	TreeNodeDeleteBalance(tree, replacement);
      }
    } else if (!TreeNodeParent(p)) {
      tree->root = NULL;
    } else {
      if (TreeNodeColor(p) == 'B')
	TreeNodeDeleteBalance(tree, p);

      if (TreeNodeParent(p)) {
	if (TreeNodeLeft(TreeNodeParent(p)) == p) {
	  p->parent->left = NULL;
	} else {
	  p->parent->right = NULL;
	}
      }
    }

    /* Free tree node */
    TreeNodeFree(tree, p);
  }
}

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
	const void *mappingKey) {
  register bool retcode = false;
  if (!tree) {
    Log(L_ASSERT, "Invalid `tree` Tree.");
  } else {
    register TreeNode *node;
    if ((node = TreeGet(tree, mappingKey)) != NULL) {
      TreeNodeDelete(tree, node);
      retcode = true;
    }
  }
  return (retcode);
}

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
	const void *mappingKey) {
  register bool retcode = false;
  if (!tree) {
    Log(L_ASSERT, "Invalid `tree` Tree.");
  } else {
    register TreeNode *node;
    if ((node = TreeGet(tree, mappingKey)) != NULL) {
      node->mappingKey = NULL;
      node->mappingValue = NULL;
      TreeNodeDelete(tree, node);
      retcode = true;
    }
  }
  return (retcode);
}

/*!
 * Frees a tree.
 * \addtogroup tree
 * \param tree the tree to free
 * \sa TreeAlloc(const TreeCompareFunc, const TreeFreeFunc, const TreeFreeFunc)
 * \sa TreeFreeV(void*)
 */
void TreeFree(Tree *tree) {
  if (tree) {
    TreeClear(tree);
    MemoryFree(tree);
  }
}

/*!
 * Frees a tree.
 * \addtogroup tree
 * \param tree the tree to free
 * \sa TreeAlloc(const TreeCompareFunc, const TreeFreeFunc, const TreeFreeFunc)
 * \sa TreeFree(Tree*)
 */
void TreeFreeV(void *tree) {
  TreeFree(tree);
}

/*!
 * Returns the first tree node.
 * \addtogroup tree
 * \param tree the tree instance
 * \return the first tree node in the specified tree or NULL
 * \sa TreeFrontKey(Tree*, void*)
 * \sa TreeFrontValue(Tree*, void*)
 */
TreeNode *TreeFront(Tree *tree) {
  register TreeNode *front = NULL;
  if (tree) {
    for (front = tree->root;
	 front && front->left; front = front->left) {
      /* Nothing */
    }
  }
  return (front);
}

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
	const void *defaultKey) {
  if (tree) {
    register TreeNode *node;
    if ((node = TreeFront(tree)) != NULL)
      return (node->mappingKey);
  }
  return (void*) (defaultKey);
}

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
	const void *defaultValue) {
  if (tree) {
    register TreeNode *node;
    if ((node = TreeFront(tree)) != NULL)
      return (node->mappingValue);
  }
  return (void*) (defaultValue);
}

/*!
 * Searches for a tree node.
 * \addtogroup tree
 * \param tree the tree instance
 * \param mappingKey the mapping key for which to search
 * \return the tree node indicated by the specified key or NULL
 */
TreeNode *TreeGet(
	Tree *tree,
	const void *mappingKey) {
  register TreeNode *p = NULL;
  if (!tree) {
    Log(L_ASSERT, "Invalid `tree` Tree.");
  } else {
    for (p = tree->root; p; ) {
      const int cmp = tree->compare(mappingKey, p->mappingKey);
      if (cmp < 0) {
	p = p->left;
      } else if (cmp > 0) {
	p = p->right;
      } else
	break;
    }
  }
  return (p);
}

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
	const void *defaultValue) {
  if (!tree) {
    Log(L_ASSERT, "Invalid `tree` Tree.");
  } else {
    register TreeNode *node;
    if ((node = TreeGet(tree, mappingKey)) != NULL)
      return (node->mappingValue);
  }
  return (void*) (defaultValue);
}

/*! Tree helper function. */
static void TreeNodeInsertBalance(
	Tree *tree,
	TreeNode *node) {
  if (!tree) {
    Log(L_ASSERT, "Invalid `tree` Tree.");
  } else if (!node) {
    Log(L_ASSERT, "Invalid `node` TreeNode.");
  } else {
    TreeNodeSetColor(node, 'R');
    while (node != tree->root && node->parent->color == 'R') {
      if (TreeNodeParent(node) == TreeNodeLeft(TreeNodeParent(TreeNodeParent(node)))) {
	TreeNode *y = TreeNodeRight(TreeNodeParent(TreeNodeParent(node)));
	if (TreeNodeColor(y) == 'R') {
	  TreeNodeSetColor(TreeNodeParent(node), 'B');
	  TreeNodeSetColor(y, 'B');
	  TreeNodeSetColor(TreeNodeParent(TreeNodeParent(node)), 'R');
	  node = TreeNodeParent(TreeNodeParent(node));
	} else {
	  if (TreeNodeRight(TreeNodeParent(node)) == node) {
	    node = TreeNodeParent(node);
	    TreeNodeRotateLeft(tree, node);
	  }
	  TreeNodeSetColor(TreeNodeParent(node), 'B');
	  TreeNodeSetColor(TreeNodeParent(TreeNodeParent(node)), 'R');
	  TreeNodeRotateRight(tree, TreeNodeParent(TreeNodeParent(node)));
	}
      } else {
	TreeNode *y = TreeNodeLeft(TreeNodeParent(TreeNodeParent(node)));
	if (TreeNodeColor(y) == 'R') {
	  TreeNodeSetColor(TreeNodeParent(node), 'B');
	  TreeNodeSetColor(y, 'B');
	  TreeNodeSetColor(TreeNodeParent(TreeNodeParent(node)), 'R');
	  node = TreeNodeParent(TreeNodeParent(node));
	} else {
	  if (TreeNodeLeft(TreeNodeParent(node)) == node) {
	    node = TreeNodeParent(node);
	    TreeNodeRotateRight(tree, node);
	  }
	  TreeNodeSetColor(TreeNodeParent(node), 'B');
	  TreeNodeSetColor(TreeNodeParent(TreeNodeParent(node)), 'R');
	  TreeNodeRotateLeft(tree, TreeNodeParent(TreeNodeParent(node)));
	}
      }
    }
    TreeNodeSetColor(tree->root, 'B');
  }
}

/*! Tree helper function. */
static bool TreeNodeInsert(
	Tree *tree,
	TreeNode *node) {
  bool retcode = false;
  if (!tree) {
    Log(L_ASSERT, "Invalid `tree` Tree.");
  } else if (!node) {
    Log(L_ASSERT, "Invalid `node` TreeNode.");
  } else {
    if (!tree->root) {
      tree->root = node;
      TreeNodeSetColor(node, 'B');
      retcode = true;
    } else {
      register int cmp = 0;
      register TreeNode *parent = NULL;
      register TreeNode *p = tree->root;

      do {
	parent = p;
	cmp = tree->compare(node->mappingKey, p->mappingKey);
	if (cmp < 0) {
	  p = p->left;
	} else if (cmp > 0) {
	  p = p->right;
	}
      } while (p && cmp != 0);

      if (cmp) {
	if (cmp < 0) {
	  node->parent = parent;
	  parent->left = node;
	} else if (cmp > 0) {
	  node->parent = parent;
	  parent->right = node;
	}
	TreeNodeInsertBalance(tree, node);
	retcode = true;
      }
    }
  }
  return (retcode);
}

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
	const void *mappingValue) {
  register TreeNode *node = NULL;
  if (!tree) {
    Log(L_ASSERT, "Invalid `tree` Tree.");
  } else {
    node = TreeGet(tree, mappingKey);
    if (!node) {
      MemoryCreate(node, TreeNode, 1);
      node->color = 'B';
      node->left = NULL;
      node->mappingKey = (void*) mappingKey;
      node->mappingValue = (void*) mappingValue;
      node->parent = NULL;
      node->right = NULL;

      if (!TreeNodeInsert(tree, node)) {
	TreeNodeFree(tree, node);
	node = NULL;
      }
    } else {
      /* Free mapping data */
      TreeNodeFreeData(tree, node, mappingKey, mappingValue);

      /* Set new mapping data */
      node->mappingKey = (void*) mappingKey;
      node->mappingValue = (void*) mappingValue;
    }
  }
  return (node);
}

/*!
 * Returns the predecessor for a tree node.
 * \addtogroup tree
 * \param node the tree node whose predecessor to return
 * \return the predecessor for the specified tree node or NULL
 * \sa TreeSuccessor(Tree*, TreeNode*)
 */
TreeNode *TreePredecessor(TreeNode *node) {
  register TreeNode *p = NULL;
  if (node) {
    if (node->left) {
      for (p = node->left; p->right; p = p->right) {
	/* Nothing */
      }
    } else {
      register TreeNode *temp = node;
      for (p = node->parent; p && p->left == temp; ) {
	temp = p;
	p = p->parent;
      }
    }
  }
  return (p);
}

/*! Tree helper function. */
static size_t TreeNodeSize(const TreeNode *node) {
  register size_t result = 0;
  if (node) {
    /* Recurse */
    result += TreeNodeSize(node->left);
    result += TreeNodeSize(node->right);

    /* Count self */
    result++;
  }
  return (result);
}

/*!
 * Returns the size of a tree.
 * \addtogroup tree
 * \param tree the tree instance
 * \return the size of the specified tree or zero
 */
size_t TreeSize(const Tree *tree) {
  return TreeNodeSize(tree->root);
}

/*!
 * Returns the successor for a tree node.
 * \addtogroup tree
 * \param node the tree node whose successor to return
 * \return the successor for the specified tree node or NULL
 * \sa TreePredecessor(Tree*, TreeNode*)
 */
TreeNode *TreeSuccessor(TreeNode *node) {
  register TreeNode *p = NULL;
  if (node) {
    if (node->right) {
      for (p = node->right; p->left; p = p->left) {
	/* Nothing */
      }
    } else {
      register TreeNode *temp = node;
      for (p = node->parent; p && p->right == temp; ) {
	temp = p;
	p = p->parent;
      }
    }
  }
  return (p);
}
