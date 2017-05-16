#include "yendor.h"
#include "globals.h"

/*
**  MIT RED BLACK TREE IMPLEMENTATION USED FOR THE FELLOWSHIPS OF YENDOR.
**
**  Redistribution and use in source and binary forms, with or without
**  modification, are permitted provided that neither the name of Emin
**  Martinian nor the names of any contributors are be used to endorse or
**  promote products derived from this software without specific prior
**  written permission.
**  
**  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
**  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
**  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
**  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
**  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
**  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
**  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
**  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
**  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
**  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
**  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

G_Tree* G_TreeCreate(void) {
  G_Tree *tree = (G_Tree*)malloc(sizeof(G_Tree));

  tree->root = (G_TreeNode*)malloc(sizeof(G_TreeNode));
  tree->nil = (G_TreeNode*)malloc(sizeof(G_TreeNode));
  tree->nil->right = tree->nil->left = tree->nil->parent = tree->nil;
  tree->root->right = tree->root->left = tree->root->parent = tree->nil;
  tree->root->color = 'b';
  tree->root->data = NULL;
  tree->root->key = 0;
  tree->nil->key = 0;
  tree->size = 0;

  return tree;
} 

void G_TreeDestroy(G_Tree **tree) {
  assert((tree != NULL) && (*tree != NULL));

  G_Tree *t = *tree;

  while (t->root->left != t->nil) {
    G_TreeNodeDelete(tree, &(t->root->left));
  }

  free(t->root);
  free(t->nil);
  free(t);
  *tree = NULL;
}

void G_TreeNodeInsert(G_Tree **tree, G_TreeNode **node) {
  assert((tree != NULL) && (*tree != NULL) &&
        (node != NULL) && (*node != NULL));

  G_Tree *t = *tree;
  G_TreeNode *x = *node, *y = x;
  G_TreeNodeInsertInner(tree, node);

  x->color = 'r';

  while (x->parent->color == 'r') {
    if (x->parent == x->parent->parent->left) {
      y = x->parent->parent->right;
      
      if (y->color == 'r') {
        x->parent->color = 'b';
        y->color = 'b';
        x->parent->parent->color = 'r';
        x = x->parent->parent;
      } else {
        if (x == x->parent->right) {
          x = x->parent;
          G_TreeNodeRotateLeft(tree, &x);
        }

        x->parent->color = 'b';
        x->parent->parent->color = 'r';
        G_TreeNodeRotateRight(tree, &(x->parent->parent));
      }
    } else {
      y = x->parent->parent->left;

      if (y->color == 'r') {
        x->parent->color = 'b';
        y->color = 'b';
        x->parent->parent->color = 'r';
        x = x->parent->parent;
      } else {
        if (x == x->parent->left) {
          x = x->parent;
          G_TreeNodeRotateRight(tree, &x);
        }

        x->parent->color = 'b';
        x->parent->parent->color = 'r';
        G_TreeNodeRotateLeft(tree, &(x->parent->parent));
      }
    }
  }

  t->root->left->color = 'b';
  t->size += 1;
}

void G_TreeNodeInsertInner(G_Tree **tree, G_TreeNode **node) {
  assert((tree != NULL) && (*tree != NULL) &&
        (node != NULL) && (*node != NULL));

  G_Tree *t = *tree;
  G_TreeNode *x, *y, *z = *node, *nil = (*tree)->nil;

  z->left = z->right = nil;
  y = t->root;
  x = t->root->left;

  while (x != nil) {
    y = x;

    if (x->key > z->key) {
      x = x->left;
    } else {
      x = x->right;
    } 
  }

  z->parent = y;
  
  if ((y == t->root) || (y->key > z->key)) {
    y->left = z;
  } else {
    y->right = z;
  }
}

void G_TreeNodeDelete(G_Tree **tree, G_TreeNode **node) {
  assert((tree != NULL) && (*tree != NULL) &&
        (node != NULL) && (*node != NULL));

  G_TreeNode *z = *node, *x, *y, *nil = (*tree)->nil, *root = (*tree)->root;

  y = ((z->left == nil) || (z->right == nil)) ? z : G_TreeNodeSuccessor(tree, &z);
  x = (y->left == nil) ? y->right : y->left;
  x->parent = y->parent;

  if (root == x->parent) {
    root->left = x;
  } else {
    if (y == y->parent->left) {
      y->parent->left = x;
    } else {
      y->parent->right = x;
    }
  }

  if (y != z) {
    if (y->color == 'b') {
      G_TreeDeleteFix(tree, &x);
    }

    y->left = z->left;
    y->right = z->right;
    y->parent = z->parent;
    y->color = z->color;
    z->left->parent = z->right->parent = y;

    if (z == z->parent->left) {
      z->parent->left = y;
    } else {
      z->parent->right = y;
    }

    free(z);
    (*tree)->size -= 1;
  } else {
    if (y->color == 'b') {
      G_TreeDeleteFix(tree, &x);
    }

    free(y);
    (*tree)->size -= 1;
  }
}

void G_TreeNodeSucceed(G_Tree **tree, G_TreeNode **node) {
  assert((tree != NULL) && (*tree != NULL) &&
        (node != NULL) && (*node != NULL));

  void *data;
  G_Tree *t = *tree;
  G_TreeNode *successor = G_TreeNodeSuccessor(tree, node), *n = *node;

  if (successor == t->nil) {
    G_TreeNodeDelete(tree, node);
  } else {
    data = successor->data;
    n->key = successor->key;
    n->data = successor->data;
    successor->data = data;

    G_TreeNodeDelete(tree, &successor);
  }
}

void G_TreeNodeRotateLeft(G_Tree **tree, G_TreeNode **node) {
  assert((tree != NULL) && (*tree != NULL));

  G_TreeNode
    *x = (*node),
    *y = x->right,
    *nil = (*tree)->nil;

  x->right = y->left;

  if (y->left != nil) {
    y->left->parent = x;
  }

  y->parent = x->parent;

  if (x == x->parent->left) {
    x->parent->left = y;
  } else {
    x->parent->right = y;
  }

  y->left = x;
  x->parent = y;
}

void G_TreeNodeRotateRight(G_Tree **tree, G_TreeNode **node) {
  assert((tree != NULL) && (*tree != NULL));

  G_TreeNode
    *y = (*node),
    *x = y->left,
    *nil = (*tree)->nil;

    y->left = x->right;

  if (x->right != nil) {
    x->right->parent = y;
  }

  x->parent = y->parent;

  if (y == y->parent->left) {
    y->parent->left = x;
  } else {
    y->parent->right = x;
  }

  x->right = y;
  y->parent = x;
}

void G_TreeDeleteFix(G_Tree **tree, G_TreeNode **node) {
  assert((tree != NULL) && (*tree != NULL) &&
        (node != NULL) && (*node != NULL));

  G_TreeNode *x = *node, *root = (*tree)->root->left, *w;

  while ((x->color == 'b') && (root != x)) {
    if (x == x->parent->left) {
      w = x->parent->right;

      if (w->color == 'r') {
        w->color = 'b';
        x->parent->color = 'r';
        G_TreeNodeRotateLeft(tree, &(x->parent));
        w = x->parent->right;
      }
      
      if ((w->right->color == 'b') && (w->left->color == 'b')) {
        w->color = 'r';
        x = x->parent;
      } else {
        if (w->right->color == 'b') {
          w->left->color = 'b';
          w->color = 'r';
          G_TreeNodeRotateRight(tree, &w);
          w = x->parent->right;
        }

        w->color = x->parent->color;
        x->parent->color = 'b';
        w->right->color = 'b';
        G_TreeNodeRotateLeft(tree, &(x->parent));
        x = root;
      }
    } else {
      w = x->parent->left;

      if (w->color == 'r') {
        w->color = 'b';
        x->parent->color = 'r';
        G_TreeNodeRotateRight(tree, &(x->parent));
        w = x->parent->left;
      }
      
      if ((w->left->color == 'b') && (w->right->color == 'b')) {
        w->color = 'r';
        x = x->parent;
      } else {
        if (w->left->color == 'b') {
          w->right->color = 'b';
          w->color = 'r';
          G_TreeNodeRotateLeft(tree, &w);
          w = x->parent->left;
        }

        w->color = x->parent->color;
        x->parent->color = 'b';
        w->left->color = 'b';
        G_TreeNodeRotateRight(tree, &(x->parent));
        x = root;
      }
    }
  }

  x->color = 'b';
}

void G_TreeNodeIterate(G_Tree **tree, G_TreeNode **node, void (*func)(void*)) {
  assert((node != NULL) && (*node != NULL));

  G_Tree *t = *tree;
  G_TreeNode *n = *node;

  if (n != t->nil) {
    func(&(n->data));

    if (n->left != t->nil) {
      G_TreeNodeIterate(tree, &(n->left), func);
    }

    if (n->right != t->nil) {
      G_TreeNodeIterate(tree, &(n->right), func);
    }
  }
}

int G_TreeSize(G_Tree **tree) {
  return (*tree)->size;
}

G_TreeNode* G_TreeNodeFind(G_Tree **tree, long int key) {
  assert((tree != NULL) && (*tree != NULL));

  G_TreeNode *node = (*tree)->root->left;

  while ((node != (*tree)->nil) && (node->key != key)) {
    node = (node->key < key) ? node->right : node->left;
  }

  return (node == (*tree)->nil) ? NULL : node;
}

G_TreeNode* G_TreeNodeMinimum(G_Tree **tree) {
  assert((tree != NULL) && (*tree != NULL));

  G_TreeNode *other = (*tree)->root->left;

  while (other->left != (*tree)->nil) {
    other = other->left;
  }

  return (other == (*tree)->nil) ? (*tree)->nil : other;
}

G_TreeNode* G_TreeNodeSuccessor(G_Tree **tree, G_TreeNode **node) {
  G_TreeNode *x = (*node), *y = x->right, *nil = (*tree)->nil, *root = (*tree)->root;

  if (nil != y) {
    while (y->left != nil) {
      y = y->left;
    }
  } else {
    y = x->parent;

    while (x == y->right) {
      x = y;
      y = y->parent;
    }

    if (y == root) {
      y = nil;
    }
  }

  return y;
}
