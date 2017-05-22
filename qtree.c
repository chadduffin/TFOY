#include "yendor.h"
#include "globals.h"

G_QTree* G_QTreeCreate(void) {
  int i;
  G_QTree *tree = (G_QTree*)malloc(sizeof(G_QTree));

  for (i = 0; i < 4; i += 1) {
    tree->children[i] = NULL;
  }

  tree->size = -1;
  tree->count = 0;
  tree->parent = NULL;

  return tree;
}

int G_QTreeQuadrant(int *x, int *y, int *size) {
  int a = *x, b = *y, c = *size, z = 0;

  if (a >= c/2) {
    z += 1;
    a -= c/2;
  }

  if (b >= c/2) {
    z += 2;
    b -= c/2;
  }

  *x = a;
  *y = b;
  *size = c;

  return z;
}

void G_QTreeDestroy(G_QTree **tree) {
  assert((tree != NULL) && (*tree != NULL));

  int i;
  G_QTree *t = *tree, *target;

  for (i = 0; i < 4; i += 1) {
    if (t->children[i] != NULL) {
      if (t->size > 2) {
        target = t->children[i];
        G_QTreeDestroy(&target);
      } else {
        free(t->children[i]);
      }
    }
  }

  free(t);
  t = NULL;
}

void G_QTreeNodeMove(G_QTree **tree, G_Entity **entity, TileLayer layer, int x, int y) {
  assert((tree != NULL) && (*tree != NULL) && (entity != NULL) && (*entity != NULL));

  G_QTreeLeaf *leaf = G_QTreeNodeFind(tree, x, y);

  if (leaf != NULL) {
    leaf->entities[layer] = NULL;
  }

  G_QTreeNodeInsert(tree, entity, layer);
}

void G_QTreeNodeInsert(G_QTree **tree, G_Entity **entity, TileLayer layer) {
  assert((tree != NULL) && (*tree != NULL) && (entity != NULL) && (*entity != NULL));

  int x, y, z, i;
  G_QTree *t = *tree;
  G_QTreeLeaf *leaf = NULL;
  
  G_EntityPos(entity, &x, &y);

  while (t->size > 1) {
    z = G_QTreeQuadrant(&x, &y, &(t->size));

    if (t->children[z] == NULL) {
      if (t->size == 2) {
        t->children[z] = (G_QTreeLeaf*)malloc(sizeof(G_QTreeLeaf));
        ((G_QTreeLeaf*)(t->children[z]))->parent = t;

        for (i = 0; i < TILE_LAYER_COUNT; i += 1) {
          ((G_QTreeLeaf*)(t->children[z]))->entities[i] = NULL;
        }
  
        break;
      } else {
        t->children[z] = G_QTreeCreate();
        ((G_QTree*)(t->children[z]))->parent = t;
        ((G_QTree*)(t->children[z]))->size = t->size/2;
      }
    }

    if (t->size > 2) {
      t = (G_QTree*)(t->children[z]);
    } else {
      break;
    }
  }

  leaf = t->children[z];
  
  if (leaf->entities[layer] == NULL) {
    while (t != NULL) {
      t->count += 1;
      t = t->parent;
    }
  }

  leaf->entities[layer] = *entity;
}

void G_QTreeNodeDelete(G_QTree **tree, G_Entity **entity, TileLayer layer) {
  assert((tree != NULL) && (*tree != NULL) && (entity != NULL) && (*entity != NULL));

  int x, y;
  G_QTreeLeaf *leaf = NULL;
  
  G_EntityPos(entity, &x, &y);

  leaf = G_QTreeNodeFind(tree, x, y);

  if ((leaf != NULL) && (leaf->entities[layer] != NULL)) {
    G_QTree *tree = (G_QTree*)(leaf->parent);

    while (tree != NULL) {
      tree->count -= 1;
      tree = tree->parent;
    }

    leaf->entities[layer] = NULL;
  }
}

void G_QTreePurge(G_QTree **tree) {
  assert((tree != NULL) && (*tree != NULL));

  int i;
  G_QTree *t = *tree, *child = NULL;

  for (i = 0; i < 4; i += 1) {
    child = (G_QTree*)(t->children[i]);

    if (child != NULL) {
      if ((t->size == CHUNK_SIZE) && (child->count == 0)) {
        G_QTreeDestroy(&child);
        t->children[i] = NULL;
      } else if (t->size > CHUNK_SIZE) {
        G_QTreePurge(&child);
      }
    }
  }
}

G_QTreeLeaf* G_QTreeNodeFind(G_QTree **tree, int x, int y) {
  int z;
  G_QTree *t = *tree;
  
  while (t->size > 1) {
    z = G_QTreeQuadrant(&x, &y, &(t->size));

    if ((t->children[z] == NULL) || (t->size == 2)) {
      return t->children[z];
    }

    t = (G_QTree*)(t->children[z]);
  }
  
  return NULL;
}
