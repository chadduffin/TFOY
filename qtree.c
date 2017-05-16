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
  int z = 0;

  if (*x >= *size/2) {
    z += 1;
  } else {
    *x -= *size/2;
  }
  if (*y >= *size/2) {
    z += 2;
  } else {
    *y -= *size/2;
  }

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

void G_QTreeNodeInsert(G_QTree **tree, G_Entity **entity) {
  int x, y, z, i;
  G_QTree *t = *tree;
  G_QTreeLeaf *leaf = NULL;
  
  G_EntityPos(entity, &x, &y);

  while (t->size > 2) {
    G_QTreeQuadrant(&x, &y, &z);

    if (t->nodes[z] == NULL) {
      t->nodes[z] = G_TreeCreate();
      ((G_QTree*)(t->nodes[z]))->size = t->size/2;
      printf("Created new quadrant of size %i.\n", t->size/2);
    }

    t = (G_QTree*)(t->nodes[z]);
  }

  G_QTreeQuadrant(&x, &y, &z);

  if (t->nodes[z] == NULL) {
    t->nodes[z] = (G_QTreeLeaf*)malloc(sizeof(G_QTreeLeaf));
    
    for (i = 0; i < TILE_LAYER_COUNT; i += 1) {
      ((G_QTreeLeaf*)(t->nodes[z]))->entities[i] = NULL;
    }
  }

  leaf = t->nodes[z];
}

void G_QTreeNodeDelete(G_QTree **tree, G_Entity **entity) {

}

G_Entity** G_QTreeNodeFind(G_QTree **tree, int x, int y) {
  return NULL;
}
