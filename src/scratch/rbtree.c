/*!
 * \file rbtree.c
 *
 * \par Copyright
 * Copyright (C) 1999-2019 scratchmud.org
 * All rights reserved.
 *
 * \author Geoffrey Davis <gdavis@scratchmud.org>
 * \addtogroup rbtree
 */
#define _SCRATCH_RBTREE_C_

#include <scratch/log.h>
#include <scratch/memory.h>
#include <scratch/rbtree.h>
#include <scratch/scratch.h>

/*!
 * Returns a tree node color.
 * \addtogroup rbtree
 * \param node the tree node whose color to return
 * \return the color of the specified tree node
 */
#define RBTreeNodeColor(node) \
    (node ? node->color : 'B')

/*!
 * Returns the left tree node.
 * \addtogroup rbtree
 * \param node the tree node whose left tree node to return
 * \return the tree node to the left of the specified tree node
 */
#define RBTreeNodeLeft(node) \
    (node ? node->left : NULL)

/*!
 * Returns the parent tree node.
 * \addtogroup rbtree
 * \param node the tree node whose parent tree node to return
 * \return the parent tree node of the specified tree node
 */
#define RBTreeNodeParent(node) \
    (node ? node->parent : NULL)

/*!
 * Returns the right tree node.
 * \addtogroup rbtree
 * \param node the tree node whose right tree node to return
 * \return the tree node to the right of the specified tree node
 */
#define RBTreeNodeRight(node) \
    (node ? node->right : NULL)

/*!
 * Sets the color of a tree node.
 * \addtogroup rbtree
 * \param node the tree node whose color to set
 * \param theColor the new color: either 'R' or 'B'
 */
#define RBTreeNodeSetColor(node, theColor) \
  do { \
    if (node) { \
      node->color = theColor; \
    } \
  } while (0)

/* Local functions */
static RBTreeNode *RBTreeNodeAlloc(void);
static void RBTreeNodeDelete(RBTree *tree, RBTreeNode *node);
static void RBTreeNodeDeleteBalance(RBTree *tree, RBTreeNode *node);
static bool RBTreeNodeInsert(RBTree *tree, RBTreeNode *node);
static void RBTreeNodeInsertBalance(RBTree *tree, RBTreeNode *node);
static void RBTreeNodeRotateLeft(RBTree *tree, RBTreeNode *node);
static void RBTreeNodeRotateRight(RBTree *tree, RBTreeNode *node);
static size_t RBTreeNodeSize(const RBTreeNode *node);
static void RBTreeNodeSwap(RBTreeNode *left, RBTreeNode *right);

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
	const RBTreeFreeProc freeProc) {
  RBTree *tree = NULL;
  if (!compareProc) {
    Log("invalid `compareProc` RBTreeCompareProc");
  } else {
    MemoryCreate(tree, RBTree, 1);
    tree->compareProc = compareProc;
    tree->freeProc    = freeProc;
    tree->root        = NULL;
  }
  return (tree);
}

/*!
 * Returns the last tree node in a tree.
 * \addtogroup rbtree
 * \param tree the tree to search
 * \return the last tree node in the specified tree, or NULL
 * \sa RBTreeBackKey(RBTree*, void*)
 * \sa RBTreeBackValue(RBTree*, void*)
 */
RBTreeNode *RBTreeBack(RBTree *tree) {
  register RBTreeNode *p = NULL;
  if (!tree) {
    Log("invalid `tree` RBTree");
  } else {
    for (p = tree->root; p && p->right; p = p->right) {
      /* nothing */
    }
  }
  return (p);
}

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
	const RBTreeKey defaultKey) {
  RBTreeKey lastKey = defaultKey;
  if (!tree) {
    Log("invalid `tree` RBTree");
  } else {
    register RBTreeNode *p = RBTreeBack(tree);
    if (p) {
      lastKey = p->key;
    }
  }
  return (lastKey);
}

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
	const RBTreeValue defaultValue) {
  register RBTreeValue lastValue = defaultValue;
  if (!tree) {
    Log("invalid `tree` RBTree");
  } else {
    register RBTreeNode *p = RBTreeBack(tree);
    if (p) {
      lastValue = p->value;
    }
  }
  return (lastValue);
}

/*!
 * Clears a tree.
 * \addtogroup rbtree
 * \param tree the tree to clear
 */
void RBTreeClear(RBTree *tree) {
  if (!tree) {
    Log("invalid `tree` RBTree");
  } else {
    while (tree->root)
      RBTreeNodeDelete(tree, tree->root);
  }
}

/*!
 * Clears a tree.
 * \addtogroup rbtree
 * \param tree the tree to clear
 */
void RBTreeClearNoFree(RBTree *tree) {
  if (!tree) {
    Log("invalid `tree` RBTree");
  } else {
    while (tree->root) {
      tree->root->value = NULL;
      RBTreeNodeDelete(tree, tree->root);
    }
  }
}

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
	const RBTreeKey key) {
  register bool retcode = false;
  if (!tree) {
    Log("invalid `tree` RBTree");
  } else {
    retcode = RBTreeGet(tree, key) != NULL;
  }
  return (retcode);
}

/*!
 * Deletes a tree node for a tree.
 * \addtogroup rbtree
 * \param tree the tree from which to delete
 * \param key the key that identifies the tree node to delete
 * \return true if a tree node was successfully deleted
 */
bool RBTreeDelete(
	RBTree *tree,
	const RBTreeKey key) {
  register bool retcode = false;
  if (!tree) {
    Log("invalid `tree` RBTree");
  } else {
    register RBTreeNode *p = RBTreeGet(tree, key);
    if (p) {
      RBTreeNodeDelete(tree, p);
      retcode = true;
    }
  }
  return (retcode);
}

/*!
 * Deletes a tree node for a tree.
 * \addtogroup rbtree
 * \param tree the tree from which to delete
 * \param key the key that identifies the tree node to delete
 * \return true if a tree node was successfully deleted
 */
bool RBTreeDeleteNoFree(
	RBTree *tree,
	const RBTreeKey key) {
  register bool retcode = false;
  if (!tree) {
    Log("invalid `tree` RBTree");
  } else {
    register RBTreeNode *p = RBTreeGet(tree, key);
    if (p) {
      p->value = NULL;
      RBTreeNodeDelete(tree, p);
      retcode = true;
    }
  }
  return (retcode);
}

/*!
 * Frees a tree.
 * \addtogroup rbtree
 * \param tree the tree to free
 * \sa RBTreeClear(RBTree*)
 */
void RBTreeFree(RBTree *tree) {
  if (tree) {
    RBTreeClear(tree);
    MemoryFree(tree);
  }
}

/*!
 * Returns the first tree node in a tree.
 * \addtogroup rbtree
 * \param tree the tree to search
 * \return the first tree node in the specified tree, or NULL
 * \sa RBTreeFrontKey(RBTree*, void*)
 * \sa RBTreeFrontValue(RBTree*, void*)
 */
RBTreeNode *RBTreeFront(RBTree *tree) {
  register RBTreeNode *p = NULL;
  if (!tree) {
    Log("invalid `tree` RBTree");
  } else {
    for (p = tree->root; p && p->left; p = p->left) {
      /* nothing */
    }
  }
  return (p);
}

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
	const RBTreeKey defaultKey) {
  register RBTreeKey firstKey = defaultKey;
  if (!tree) {
    Log("invalid `tree` RBTree");
  } else {
    register RBTreeNode *p = RBTreeFront(tree);
    if (p) {
      firstKey = p->key;
    }
  }
  return (firstKey);
}

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
	const RBTreeValue defaultValue) {
  register RBTreeValue firstValue = defaultValue;
  if (!tree) {
    Log("invalid `tree` RBTree");
  } else {
    register RBTreeNode *p = RBTreeFront(tree);
    if (p) {
      firstValue = p->value;
    }
  }
  return (firstValue);
}

/*!
 * Searches a tree for a tree node.
 * \addtogroup rbtree
 * \param tree the tree to search
 * \param key the key that identifies the desired tree node
 * \return the tree node indicated by the specified key, or NULL
 */
RBTreeNode *RBTreeGet(
	RBTree *tree,
	const RBTreeKey key) {
  register RBTreeNode *p = NULL;
  if (!tree) {
    Log("invalid `tree` RBTree");
  } else {
    for (p = tree->root; p; ) {
      const int cmp = tree->compareProc(key, p->key);
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
	const RBTreeValue defaultValue) {
  register RBTreeValue value = defaultValue;
  if (!tree) {
    Log("invalid `tree` RBTree");
  } else {
    register RBTreeNode *p = RBTreeGet(tree, key);
    if (p) {
      value = p->value;
    }
  }
  return (value);
}

/*!
 * Constructs a tree node.
 * \addtogroup rbtree
 * \return a new tree node or NULL
 */
static RBTreeNode *RBTreeNodeAlloc(void) {
  RBTreeNode *node;
  MemoryCreate(node, RBTreeNode, 1);
  node->color  = 'B';
  node->key    = NULL;
  node->left   = NULL;
  node->parent = NULL;
  node->right  = NULL;
  node->value  = NULL;
  return (node);
}

/*!
 * Deletes a tree node.
 * \addtogroup rbtree
 * \param tree the tree from which to delete
 * \param node the tree node to delete
 */
static void RBTreeNodeDelete(
	RBTree *tree,
	RBTreeNode *node) {
  if (!tree) {
    Log("invalid `tree` RBTree");
  } else if (!node) {
    Log("invalid `node` RBTreeNode");
  } else {
    register RBTreeNode *p = node;
    register RBTreeNode *replacement = NULL;
    if (RBTreeNodeLeft(p) && RBTreeNodeRight(p)) {
      p = RBTreeSuccessor(node);
      RBTreeNodeSwap(p, node);
    }
    replacement = RBTreeNodeLeft(p) ? RBTreeNodeLeft(p) : RBTreeNodeRight(p);

    if (replacement) {
      replacement->parent = RBTreeNodeParent(p);
      if (!RBTreeNodeParent(p)) {
	tree->root = replacement;
      } else if (RBTreeNodeLeft(RBTreeNodeParent(p)) == p) {
	p->parent->left = replacement;
      } else {
	p->parent->right = replacement;
      }
      if (RBTreeNodeColor(p) == 'B') {
	RBTreeNodeDeleteBalance(tree, replacement);
      }
    } else if (!RBTreeNodeParent(p)) {
      tree->root = NULL;
    } else {
      if (RBTreeNodeColor(p) == 'B') {
	RBTreeNodeDeleteBalance(tree, p);
      }
      if (RBTreeNodeParent(p)) {
	if (RBTreeNodeLeft(RBTreeNodeParent(p)) == p) {
	  p->parent->left = NULL;
	} else {
	  p->parent->right = NULL;
	}
      }
    }
    if (p != node) {
      if (p->value && tree->freeProc)
	tree->freeProc(p->value);
      MemoryFree(p);
   } else {
      if (node->value && tree->freeProc)
	tree->freeProc(node->value);
      MemoryFree(node);
    }
  }
}

/*!
 * Fixes up a tree after a deletion.
 * \addtogroup rbtree
 * \param tree the tree to fix up
 * \param node the tree node to fix up
 */
static void RBTreeNodeDeleteBalance(
	RBTree *tree,
	RBTreeNode *node) {
  if (!tree) {
    Log("invalid `tree` RBTree");
  } else if (!node) {
    Log("invalid `node` RBTreeNode");
  } else {
    while (node != tree->root && RBTreeNodeColor(node) != 'R') {
      if (RBTreeNodeLeft(RBTreeNodeParent(node)) == node) {
	RBTreeNode *sibling = RBTreeNodeRight(RBTreeNodeParent(node));
	if (RBTreeNodeColor(sibling) == 'R') {
	  RBTreeNodeSetColor(sibling, 'B');
	  RBTreeNodeSetColor(RBTreeNodeParent(node), 'R');
	  RBTreeNodeRotateLeft(tree, RBTreeNodeParent(node));
	  sibling = RBTreeNodeRight(RBTreeNodeParent(node));
	}
	if (RBTreeNodeColor(RBTreeNodeLeft(sibling)) != 'R' &&
	    RBTreeNodeColor(RBTreeNodeRight(sibling)) != 'R') {
	  RBTreeNodeSetColor(sibling, 'R');
	  node = RBTreeNodeParent(node);
	} else {
	  if (RBTreeNodeColor(RBTreeNodeRight(sibling)) != 'R') {
	    RBTreeNodeSetColor(RBTreeNodeLeft(sibling), 'B');
	    RBTreeNodeSetColor(sibling, 'R');
	    RBTreeNodeRotateRight(tree, sibling);
	    sibling = RBTreeNodeRight(RBTreeNodeParent(node));
	  }
	  RBTreeNodeSetColor(sibling, RBTreeNodeColor(RBTreeNodeParent(node)));
	  RBTreeNodeSetColor(RBTreeNodeParent(node), 'B');
	  RBTreeNodeSetColor(RBTreeNodeRight(sibling), 'B');
	  RBTreeNodeRotateLeft(tree, RBTreeNodeParent(node));
	  node = tree->root;
	}
      } else {
	RBTreeNode *sibling = RBTreeNodeLeft(RBTreeNodeParent(node));
	if (RBTreeNodeColor(sibling) == 'R') {
	  RBTreeNodeSetColor(sibling, 'B');
	  RBTreeNodeSetColor(RBTreeNodeParent(node), 'R');
	  RBTreeNodeRotateRight(tree, RBTreeNodeParent(node));
	  sibling = RBTreeNodeLeft(RBTreeNodeParent(node));
	}
	if (RBTreeNodeColor(RBTreeNodeRight(sibling)) != 'R' &&
	    RBTreeNodeColor(RBTreeNodeLeft(sibling)) != 'R') {
	  RBTreeNodeSetColor(sibling, 'R');
	  node = RBTreeNodeParent(node);
	} else {
	  if (RBTreeNodeColor(RBTreeNodeLeft(sibling)) != 'R') {
	    RBTreeNodeSetColor(RBTreeNodeRight(sibling), 'B');
	    RBTreeNodeSetColor(sibling, 'R');
	    RBTreeNodeRotateLeft(tree, sibling);
	    sibling = RBTreeNodeLeft(RBTreeNodeParent(node));
	  }
	  RBTreeNodeSetColor(sibling, RBTreeNodeColor(RBTreeNodeParent(node)));
	  RBTreeNodeSetColor(RBTreeNodeParent(node), 'B');
	  RBTreeNodeSetColor(RBTreeNodeLeft(sibling), 'B');
	  RBTreeNodeRotateRight(tree, RBTreeNodeParent(node));
	  node = tree->root;
	}
      }
    }
    RBTreeNodeSetColor(node, 'B');
  }
}

/*!
 * Inserts one tree node.
 * \addtogroup rbtree
 * \param tree the tree into which to insert
 * \param node the tree node to insert
 * \return true if the tree node was successfully inserted
 */
static bool RBTreeNodeInsert(
	RBTree *tree,
	RBTreeNode *node) {
  bool retcode = false;
  if (!tree) {
    Log("invalid `tree` RBTree");
  } else if (!node) {
    Log("invalid `node` RBTreeNode");
  } else {
    if (!tree->root) {
      tree->root = node;
      RBTreeNodeSetColor(node, 'B');
      retcode = true;
    } else {
      register int cmp = 0;
      register RBTreeNode *parent = NULL;
      register RBTreeNode *p = tree->root;

      do {
	parent = p;
	cmp = tree->compareProc(node->key, p->key);
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
	RBTreeNodeInsertBalance(tree, node);
	retcode = true;
      }
    }
  }
  return (retcode);
}

/*!
 * Fixes up a tree after an insertion.
 * \addtogroup rbtree
 * \param tree the tree to fix up
 * \param node the tree node to fix up
 */
static void RBTreeNodeInsertBalance(
	RBTree *tree,
	RBTreeNode *node) {
  if (!tree) {
    Log("invalid `tree` RBTree");
  } else if (!node) {
    Log("invalid `node` RBTreeNode");
  } else {
    RBTreeNodeSetColor(node, 'R');
    while (node != tree->root && node->parent->color == 'R') {
      if (RBTreeNodeParent(node) == RBTreeNodeLeft(RBTreeNodeParent(RBTreeNodeParent(node)))) {
	RBTreeNode *y = RBTreeNodeRight(RBTreeNodeParent(RBTreeNodeParent(node)));
	if (RBTreeNodeColor(y) == 'R') {
	  RBTreeNodeSetColor(RBTreeNodeParent(node), 'B');
	  RBTreeNodeSetColor(y, 'B');
	  RBTreeNodeSetColor(RBTreeNodeParent(RBTreeNodeParent(node)), 'R');
	  node = RBTreeNodeParent(RBTreeNodeParent(node));
	} else {
	  if (RBTreeNodeRight(RBTreeNodeParent(node)) == node) {
	    node = RBTreeNodeParent(node);
	    RBTreeNodeRotateLeft(tree, node);
	  }
	  RBTreeNodeSetColor(RBTreeNodeParent(node), 'B');
	  RBTreeNodeSetColor(RBTreeNodeParent(RBTreeNodeParent(node)), 'R');
	  RBTreeNodeRotateRight(tree, RBTreeNodeParent(RBTreeNodeParent(node)));
	}
      } else {
	RBTreeNode *y = RBTreeNodeLeft(RBTreeNodeParent(RBTreeNodeParent(node)));
	if (RBTreeNodeColor(y) == 'R') {
	  RBTreeNodeSetColor(RBTreeNodeParent(node), 'B');
	  RBTreeNodeSetColor(y, 'B');
	  RBTreeNodeSetColor(RBTreeNodeParent(RBTreeNodeParent(node)), 'R');
	  node = RBTreeNodeParent(RBTreeNodeParent(node));
	} else {
	  if (RBTreeNodeLeft(RBTreeNodeParent(node)) == node) {
	    node = RBTreeNodeParent(node);
	    RBTreeNodeRotateRight(tree, node);
	  }
	  RBTreeNodeSetColor(RBTreeNodeParent(node), 'B');
	  RBTreeNodeSetColor(RBTreeNodeParent(RBTreeNodeParent(node)), 'R');
	  RBTreeNodeRotateLeft(tree, RBTreeNodeParent(RBTreeNodeParent(node)));
	}
      }
    }
    RBTreeNodeSetColor(tree->root, 'B');
  }
}

/*!
 * Performs a left rotation.
 * \addtogroup rbtree
 * \param tree the tree whose tree node to rotate
 * \param node the tree node to rotate
 */
static void RBTreeNodeRotateLeft(
	RBTree *tree,
	RBTreeNode *node) {
  if (!tree) {
    Log("invalid `tree` RBTree");
  } else if (!node) {
    Log("invalid `node` RBTreeNode");
  } else {
    RBTreeNode *right = node->right;
    node->right = right->left;
    if (right->left) {
      right->left->parent = node;
    }
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

/*!
 * Performs a right rotation.
 * \addtogroup rbtree
 * \param tree the tree whose tree node to rotate
 * \param node the tree node to rotate
 */
static void RBTreeNodeRotateRight(
	RBTree *tree,
	RBTreeNode *node) {
  if (!tree) {
    Log("invalid `tree` RBTree");
  } else if (!node) {
    Log("invalid `node` RBTreeNode");
  } else {
    RBTreeNode *left = node->left;
    node->left = left->right;
    if (left->right) {
      left->right->parent = node;
    }
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

/*!
 * Recursive size helper.
 * \addtogroup rbtree
 * \param node the current tree node
 * \return the size of the subtree
 */
static size_t RBTreeNodeSize(const RBTreeNode *node) {
  register size_t retval = 0;
  if (node) {
    retval += RBTreeNodeSize(node->left);
    retval += RBTreeNodeSize(node->right);
    retval++;
  }
  return (retval);
}

/*!
 * Swaps two tree nodes.
 * \addtogroup rbtree
 * \param left the first tree node to swap
 * \param right the second tree node to swap
 */
static void RBTreeNodeSwap(
	RBTreeNode *left,
	RBTreeNode *right) {
  if (!left) {
    Log("invalid `left` RBTreeNode");
  } else if (!right) {
    Log("invalid `right` RBTreeNode");
  } else if (left != right) {
    /* Swap tree node keys */
    const RBTreeKey leftKey = left->key;
    left->key = right->key;
    right->key = leftKey;

    /* Swap tree node values */
    const RBTreeValue leftValue = left->value;
    left->value = right->value;
    right->value = leftValue;
  }
}

/*!
 * Returns a tree node's predecessor.
 * \addtogroup rbtree
 * \param node the tree node whose predecessor to return
 * \return the tree node that precedes the specified tree node or NULL
 * \sa RBTreeSuccessor(RBTreeNode*)
 */
RBTreeNode *RBTreePredecessor(RBTreeNode *node) {
  register RBTreeNode *p = NULL;
  if (node) {
    if (node->left) {
      for (p = node->left; p->right; p = p->right) {
	/* Nothing */
      }
    } else {
      register RBTreeNode *temp = node;
      for (p = node->parent; p && p->left == temp; ) {
	temp = p;
	p = p->parent;
      }
    }
  }
  return (p);
}

/*!
 * Returns a tree node's successor.
 * \addtogroup rbtree
 * \param node the tree node whose successor to return
 * \return the tree node that follows the specified tree node or NULL
 * \sa RBTreePredecessor(RBTreeNode*)
 */
RBTreeNode *RBTreeSuccessor(RBTreeNode *node) {
  register RBTreeNode *p = NULL;
  if (node) {
    if (node->right) {
      for (p = node->right; p->left; p = p->left) {
	/* Nothing */
      }
    } else {
      register RBTreeNode *temp = node;
      for (p = node->parent; p && p->right == temp; ) {
	temp = p;
	p = p->parent;
      }
    }
  }
  return (p);
}

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
	const RBTreeValue value) {
  register RBTreeNode *p = NULL;
  if (!tree) {
    Log("invalid `tree` RBTree");
  } else {
    p = RBTreeGet(tree, key);
    if (!p) {
      p = RBTreeNodeAlloc();
      if (p) {
	p->key = key;
	p->value = value;
	if (!RBTreeNodeInsert(tree, p))
	  MemoryFree(p);
      }
    } else {
      if (p->value != value && tree->freeProc) {
	tree->freeProc(p->value);
      }
      p->key   = key;
      p->value = value;
    }
  }
  return (p);
}

/*!
 * Returns the size of a tree.
 * \addtogroup rbtree
 * \param tree the tree whose size to return
 * \return the size of the specified tree or zero
 */
size_t RBTreeSize(const RBTree *tree) {
  return tree ? RBTreeNodeSize(tree->root) : 0;
}
