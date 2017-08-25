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

  return z;
}

void G_QTreeDestroy(G_QTree **tree) {
  assert((tree) && (*tree));

  int i;
  G_QTree *t = *tree, *target;

  for (i = 0; i < 4; i += 1) {
    if (t->children[i]) {
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
  assert((tree) && (*tree) && (entity) && (*entity));

  int i, to_x, to_y, to_z, from_x, from_y, from_z;
  G_QTree *t = *tree;
  G_RenderComponent *render = (G_RenderComponent*)G_EntityComponentFind(entity, RENDER_COMPONENT);

  to_x = render->x;
  to_y = render->y;
  from_x = x;
  from_y = y;

  while (t->size > 1) {
    to_z = G_QTreeQuadrant(&to_x, &to_y, &(t->size));
    from_z = G_QTreeQuadrant(&from_x, &from_y, &(t->size));

    if (t->size == 2) {
      if (!t->children[to_z]) {
        G_QTreeLeaf *leaf = (G_QTreeLeaf*)malloc(sizeof(G_QTreeLeaf));
        t->children[to_z] = leaf;

        for (i = 0; i < TILE_LAYER_COUNT; i += 1) {
          leaf->entities[i] = NULL;
        }

        leaf->parent = t;
        leaf->entities[layer] = *entity;
      } else {
        ((G_QTreeLeaf*)(t->children[to_z]))->entities[layer] = *entity;
      }

      if (t->children[from_z]) {
        ((G_QTreeLeaf*)(t->children[from_z]))->entities[layer] = NULL;
      }

      break;
    } else if ((to_z != from_z) || (!t->children[to_z])) {
      if (from_z >= 2) {
        from_y += (t->size)/2;
        from_z -= 2;
      }

      if (from_z >= 1) {
        from_x += (t->size)/2;
        from_z -= 1;
      }

      if (to_z >= 2) {
        to_y += (t->size)/2;
        to_z -= 2;
      }

      if (to_z >= 1) {
        to_x += (t->size)/2;
        to_z -= 1;
      }

      x = render->x;
      y = render->y;

      render->x = from_x;
      render->y = from_y;
      G_QTreeNodeDelete(&t, entity, layer);

      render->x = to_x;
      render->y = to_y;
      G_QTreeNodeInsert(&t, entity, layer);

      render->x = x;
      render->y = y;
      break;
    }

    t = (G_QTree*)(t->children[to_z]);
  }
}

void G_QTreeNodeInsert(G_QTree **tree, G_Entity **entity, TileLayer layer) {
  assert((tree) && (*tree) && (entity) && (*entity));

  int x, y, z, i;
  G_QTree *t = *tree;
  G_QTreeLeaf *leaf = NULL;
  
  G_EntityPos(entity, &x, &y);

  while (t->size > 1) {
    z = G_QTreeQuadrant(&x, &y, &(t->size));

    if (!t->children[z]) {
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
  
  if (!leaf->entities[layer]) {
    while (t) {
      t->count += 1;
      t = t->parent;
    }
  }

  leaf->entities[layer] = *entity;
}

void G_QTreeNodeDelete(G_QTree **tree, G_Entity **entity, TileLayer layer) {
  assert((tree) && (*tree) && (entity) && (*entity));

  int x, y;
  G_QTreeLeaf *leaf = NULL;
  
  G_EntityPos(entity, &x, &y);

  leaf = G_QTreeNodeFind(tree, x, y);

  if ((leaf) && (leaf->entities[layer])) {
    G_QTree *tree = (G_QTree*)(leaf->parent);

    while (tree) {
      tree->count -= 1;
      tree = tree->parent;
    }

    leaf->entities[layer] = NULL;
  }
}

void G_QTreePurge(G_QTree **tree) {
  assert((tree) && (*tree));

  int i;
  G_QTree *t = *tree, *child = NULL;

  for (i = 0; i < 4; i += 1) {
    child = (G_QTree*)(t->children[i]);

    if (child) {
      if ((t->size == CHUNK_SIZE) && (child->count == 0)) {
        G_QTreeDestroy(&child);
        t->children[i] = NULL;
      } else if (t->size > CHUNK_SIZE) {
        G_QTreePurge(&child);
      }
    }
  }
}

G_Entity* G_QTreeEntityFind(G_QTree **tree, TileLayer layer, int x, int y) {
  G_QTreeLeaf *leaf = G_QTreeNodeFind(tree, x, y);

  return (leaf) ? leaf->entities[layer] : NULL;
}

G_QTreeLeaf* G_QTreeNodeFind(G_QTree **tree, int x, int y) {
  assert((tree) && (*tree));

  int z;
  G_QTree *t = *tree;
  
  while (t->size > 1) {
    z = G_QTreeQuadrant(&x, &y, &(t->size));

    if ((!t->children[z]) || (t->size == 2)) {
      return t->children[z];
    }

    t = (G_QTree*)(t->children[z]);
  }
  
  return NULL;
}
