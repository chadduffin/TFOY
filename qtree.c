#include "yendor.h"
#include "globals.h"

G_QTree* G_QTreeCreate(void) {
  int i;
  G_QTree *tree = (G_QTree*)malloc(sizeof(G_QTree));

  for (i = 0; i < 4; i += 1) {
    tree->nodes[i] = NULL;
  }

  tree->level = -1;
}

void G_QTreeDestroy(G_QTree **tree) {
  assert((tree != NULL) && (*tree != NULL));

  int i;
  G_QTree *t = *tree;

  for (i = 0; i < 4; i += 1) {
    if (t->level > 0) {
      G_QTreeDestroy(&(t->nodes[i]));
    } else {
      free(t->nodes[i]);
    }
  }

  free(t);
  t = NULL;
}

void G_QTreeNodeInsert(G_Tree **tree, G_Entity **entity) {

}

void G_QTreeNodeDelete(G_Tree **tree, G_Entity **entity) {

}

G_Entity** G_QTreeNodeFind(G_Tree **tree, int x, int y) {

}
