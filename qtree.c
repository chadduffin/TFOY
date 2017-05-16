#include "yendor.h"
#include "globals.h"

G_QTree* G_QTreeCreate(void) {
  int i;
  G_QTree *tree = (G_QTree*)malloc(sizeof(G_QTree));

  for (i = 0; i < 4; i += 1) {
    tree->nodes[i] = NULL;
  }

  tree->size = -1;
  tree->count = 0;

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
    if (t->nodes[i] != NULL) {
      if (t->size > 2) {
        target = t->nodes[i];
        G_QTreeDestroy(&target);
      } else {
        free(t->nodes[i]);
      }
    }
  }

  free(t);
  t = NULL;
}

void G_QTreeNodeInsert(G_QTree **tree, G_Entity **entity, TileLayer layer) {
  assert((tree != NULL) && (*tree != NULL) && (entity != NULL) && (*entity != NULL));

  int x, y, z, i;
  G_QTree *t = *tree;
  G_QTreeLeaf *leaf = NULL;
  
  G_EntityPos(entity, &x, &y);

  while (t->size > 1) {
    z = G_QTreeQuadrant(&x, &y, &(t->size));

    if (t->nodes[z] == NULL) {
      if (t->size == 2) {
        t->nodes[z] = (G_QTreeLeaf*)malloc(sizeof(G_QTreeLeaf));
        
        for (i = 0; i < TILE_LAYER_COUNT; i += 1) {
          ((G_QTreeLeaf*)(t->nodes[z]))->entities[i] = NULL;
        }
  
        break;
      } else {
        t->nodes[z] = G_QTreeCreate();
        ((G_QTree*)(t->nodes[z]))->size = t->size/2;
      }
    }

    t = (G_QTree*)(t->nodes[z]);
  }

  leaf = t->nodes[z];
  leaf->entities[layer] = *entity;
}

void G_QTreeNodeDelete(G_QTree **tree, G_Entity **entity, TileLayer layer) {
  assert((tree != NULL) && (*tree != NULL) && (entity != NULL) && (*entity != NULL));

  int x, y;
  G_QTreeLeaf *leaf = NULL;
  
  G_EntityPos(entity, &x, &y);

  leaf = G_QTreeNodeFind(tree, x, y);

  if ((leaf != NULL) && (leaf->entities[layer] != NULL)) {
    leaf->entities[layer] = NULL;
  }
}

G_QTreeLeaf* G_QTreeNodeFind(G_QTree **tree, int x, int y) {
  int z;
  G_QTree *t = *tree;
  
  while (t->size > 1) {
    z = G_QTreeQuadrant(&x, &y, &(t->size));

    if ((t->nodes[z] == NULL) || (t->size == 2)) {
      return t->nodes[z];
    }

    t = (G_QTree*)(t->nodes[z]);
  }
  
  return NULL;
}
