#include "yendor.h"
#include "globals.h"

/*
** EXTERNS
*/

G_Scene *menu = NULL;
G_Scene *overworld = NULL;
G_Scene *location = NULL;

/*
** FUNCTIONS
*/

void G_InitializeMenu(void) {
	int i;
	menu = (G_Scene*)malloc(sizeof(G_Scene));
	menu->w = COLS;
	menu->h = ROWS;
	menu->l = COLS*ROWS;
	menu->entity_count = 0;
	menu->transition_count = 0;
  menu->scene_step = 0;
	menu->focus = NULL;
	menu->inspect = NULL;
	menu->view.x = 0;
	menu->view.y = 0;
	menu->view.xp = 0;
	menu->view.yp = 0;
	menu->view.w = COLS;
	menu->view.h = ROWS;
	menu->view.unchanged = 0;
	menu->ambient.red = 255;
	menu->ambient.green = 255;
	menu->ambient.blue = 255;
	menu->ambient.intensity = 255;

  menu->entity = NULL;
  menu->transition = NULL;

  G_TreeInitialize(&(menu->entity));
  G_TreeInitialize(&(menu->transition));

	menu->tiles = (G_Tile*)malloc(sizeof(G_Tile)*menu->l);

	for (i = 0; i < menu->l; i += 1) {
		if (title[i/COLS][i%COLS] == 'B') {
			menu->tiles[i].tile = BLACK;
		} else if ((title[i/COLS][i%COLS] != 'B') && (title[i/COLS][i%COLS] != ' ')) {
			menu->tiles[i].tile = title[i/COLS][i%COLS];
		} else if ((i > COLS) && (title[i/COLS][(i-1)%COLS] == 'B')) {
			menu->tiles[i].tile = WHITE;
		} else {
			menu->tiles[i].tile = MAGENTA;
		}

    menu->tiles[i].changed = 0;
	}

	G_InitializeUI(&menu);
}

void G_InitializeOverworld(void) {
	int i;
	overworld = (G_Scene*)malloc(sizeof(G_Scene));
	overworld->w = WORLD_COLS;
	overworld->h = WORLD_ROWS;
	overworld->l = WORLD_COLS*WORLD_ROWS;
	overworld->entity_count = 0;
	overworld->transition_count = 0;
  overworld->scene_step = 0;
	overworld->focus = NULL;
	overworld->inspect = NULL;
	overworld->view.x = 0;
	overworld->view.y = 0;
	overworld->view.xp = 0;
	overworld->view.yp = 0;
	overworld->view.w = DCOLS;
	overworld->view.h = DROWS;
	overworld->view.unchanged = 0;
	overworld->ambient.red = 0;
	overworld->ambient.green = 0;
	overworld->ambient.blue = 0;
	overworld->ambient.intensity = 255;

  overworld->entity = NULL;
  overworld->transition = NULL;

  G_TreeInitialize(&(overworld->entity));
  G_TreeInitialize(&(overworld->transition));

	overworld->tiles = (G_Tile*)malloc(sizeof(G_Tile)*overworld->l);

	for (i = 0; i < overworld->l; i += 1) {
		if ((i%WORLD_COLS == 0) || ((i+1)%WORLD_COLS == 0) ||
				(i < WORLD_COLS) || (i > overworld->l-WORLD_COLS)) {
			overworld->tiles[i].tile = WALL;
		} else {
			overworld->tiles[i].tile = (rand()%256 < 1) ? WALL : DIRT;
			if ((i%WORLD_COLS == WORLD_COLS-64) || (i%WORLD_COLS == WORLD_COLS-12) || (i/WORLD_COLS == 200)) {
				overworld->tiles[i].tile = WALL;
			}

			int
				x = i%WORLD_COLS,	
				y = i/WORLD_COLS;
			if ((x+rand()%4 > WORLD_COLS-30) && (y+rand()%4 > WORLD_ROWS-24)) {
				if (overworld->tiles[i].tile != WALL) {
					overworld->tiles[i].tile = WATER;
				}
			} else if ((x+rand()%4 > WORLD_COLS-48) && (x+rand()%4 < WORLD_COLS-24) && (y+rand()%4 > WORLD_ROWS-24)) {
				if (overworld->tiles[i].tile != WALL) {
					overworld->tiles[i].tile = GRASS;
				}
			}
		}

    overworld->tiles[i].changed = 0;
	}

	G_Entity *player = G_CreateEntity(GAME_ENTITY);
	G_LightComponent *l = (G_LightComponent*)G_AddComponent(&player, LIGHT_COMPONENT);
	l->light.red = 255;
	l->light.green = 255;
	l->light.blue = 255;
	l->light.intensity = 64;
	G_AddComponent(&player, CONTROLLER_COMPONENT);
	G_RenderComponent *r = (G_RenderComponent*)G_AddComponent(&player, RENDER_COMPONENT);
	r->tile = HUMAN;
	r->x = 460;
	r->y = 230;
	r->x_previous = r->x;
	r->y_previous = r->y;
	G_AddComponent(&player, CREATURE_COMPONENT);
	G_AddEntity(&overworld, &player);
	G_Entity *t = G_CreateEntity(GAME_ENTITY);
	r = (G_RenderComponent*)G_AddComponent(&t, RENDER_COMPONENT);
	r->tile = FIRE;
	r->x = 480;
	r->y = 210;
	r->x_previous = r->x;
	r->y_previous = r->y;
	l = (G_LightComponent*)G_AddComponent(&t, LIGHT_COMPONENT);
	l->light.red = 0;
	l->light.green = 0;
	l->light.blue = 750;
	l->light.intensity = 24;
	G_AddEntity(&overworld, &t);
	G_Entity *x = G_CreateEntity(GAME_ENTITY);
	r = (G_RenderComponent*)G_AddComponent(&x, RENDER_COMPONENT);
	r->tile = FIRE;
	r->x = 480;
	r->y = 230;
	r->x_previous = r->x;
	r->y_previous = r->y;
	l = (G_LightComponent*)G_AddComponent(&x, LIGHT_COMPONENT);
	l->light.red = 750;
	l->light.green = 0;
	l->light.blue = 0;
	l->light.intensity = 24;
	G_AddEntity(&overworld, &x);
	G_Entity *v = G_CreateEntity(GAME_ENTITY);
	r = (G_RenderComponent*)G_AddComponent(&v, RENDER_COMPONENT);
	r->tile = FIRE;
	r->x = 445;
	r->y = 220;
	r->x_previous = r->x;
	r->y_previous = r->y;
	l = (G_LightComponent*)G_AddComponent(&v, LIGHT_COMPONENT);
	l->light.red = 380;
	l->light.green = 0;
	l->light.blue = 380;
	l->light.intensity = 16;
	G_AddEntity(&overworld, &v);

	G_InitializeUI(&overworld);

  G_TileTransition *test = (G_TileTransition*)malloc(sizeof(G_TileTransition));
  test->x = 480;
  test->y = 215;
  test->when = 10;
  test->next = NULL;
  test->prev = NULL;
  test->is = BURNT_GRASS;
  test->to = GRASS;
  G_ChangeTile(&overworld, 480, 215, BURNT_GRASS, 1);
  G_AddTileTransition(&overworld, &test);
}

void G_ChangeScene(void **scene) {
	int
		x,
		y,
		z;
	location = (G_Scene*)(*scene);
	location->view.unchanged = 0;

	if (location == menu) {
		for (y = 0; y < ROWS; y += 1) {
			for (x = 0; x < COLS; x += 1) {
				dmatrix[x][y].fg = white;
				dmatrix[x][y].bg = black;
				dmatrix[x][y].changed = 1;
				dmatrix[x][y].visible = 1;
				dmatrix[x][y].tile = G_SceneTile(x, y);

				dmatrix[x][y].light.id = -1;
				dmatrix[x][y].light.count = 0;
				dmatrix[x][y].light.light.red = 0;
				dmatrix[x][y].light.light.green = 0;
				dmatrix[x][y].light.light.blue = 0;
				dmatrix[x][y].light.light.intensity = 0;

				for (z = 0; z < TILE_LAYER_COUNT; z += 1) {
					dmatrix[x][y].layers[z] = -1;
				}
			}
		}
	} else {
		int
			x_offset = location->view.x-DCOLS_OFFSET,
			y_offset = location->view.y-DROWS_OFFSET;

		G_FocusView();

		for (y = 0; y < ROWS; y += 1) {
			for (x = 0; x < COLS; x += 1) {
				dmatrix[x][y].changed = 1;

				if ((x >= DCOLS_OFFSET) && (x < DCOLS+DCOLS_OFFSET) &&
						(y >= DROWS_OFFSET) && (y < DROWS+DROWS_OFFSET)) {
					dmatrix[x][y].visible = 0;
					dmatrix[x][y].tile = G_SceneTile(x+x_offset, y+y_offset);
				} else {
					dmatrix[x][y].visible = 1;
					dmatrix[x][y].tile = WHITE;
				}

				dmatrix[x][y].light.id = -1;
				dmatrix[x][y].light.count = 0;
				dmatrix[x][y].light.light.red = 0;
				dmatrix[x][y].light.light.green = 0;
				dmatrix[x][y].light.light.blue = 0;
				dmatrix[x][y].light.light.intensity = 0;

				for (z = 0; z < TILE_LAYER_COUNT; z += 1) {
					dmatrix[x][y].layers[z] = -1;
				}
			}
		}
	}
}

void G_InitializeUI(G_Scene **scene) {
	assert((scene != NULL) && (*scene != NULL));

	if (*scene == menu) {
		G_Entity *entity = G_CreateEntity(UI_ENTITY);
		G_UIComponent *ui = (G_UIComponent*)G_AddComponent(&entity, UI_COMPONENT);
		ui->x = (COLS/2)-2;
		ui->y = 38;
		ui->l = 4;
		ui->name = (char*)malloc(5);
		ui->name = "PLAY\0";
		ui->border = 1;
		ui->data = (void**)(&overworld);
		ui->on_click = &G_ChangeScene;
		G_AddEntity(scene, &entity);
		entity = G_CreateEntity(UI_ENTITY);
		ui = (G_UIComponent*)G_AddComponent(&entity, UI_COMPONENT);
		ui->x = (COLS/2)-2;
		ui->y = 48;
		ui->l = 4;
		ui->name = (char*)malloc(5);
		ui->name = "QUIT\0";
		ui->border = 1;
		ui->data = NULL;
		ui->on_click = &G_Quit;
		G_AddEntity(scene, &entity);
	} else {
	/*G_Entity *entity = G_CreateEntity(UI_ENTITY);
		G_UIComponent *ui = (G_UIComponent*)G_AddComponent(&entity, UI_COMPONENT);
		ui->x = DCOLS_OFFSET;
		ui->y = 0;
		ui->l = 16;
		ui->name = (char*)malloc(17);
		ui->name = "You see nothing.\0";
		ui->border = 0;
		G_AddEntity(scene, &entity);
		(*scene)->inspect = entity;*/
	}
}

void G_CleanupScene(G_Scene **scene) {
	assert((scene != NULL) && (*scene != NULL));

  G_TreeNode *node = (*scene)->entity->root->left;

	while (node != NIL) {
    G_TreeNodeDelete(&((*scene)->entity), &node);
    node = (*scene)->entity->root->left;
	}

	free((*scene)->entity);
  node = (*scene)->transition->root->left;
	
	while (node != NIL) {
    G_TreeNodeDelete(&((*scene)->transition), &node);
    node = (*scene)->transition->root->left;
	}

	free((*scene)->transition);
}

void G_ChangeTile(G_Scene **scene, int x, int y, Tile tile, boolean changed) {
	assert((scene != NULL) && (*scene != NULL));

  (*scene)->tiles[x+(y*(*scene)->w)].tile = tile;
  (*scene)->tiles[x+(y*(*scene)->w)].changed = changed;
}

void G_CheckTileTransitions(G_Scene **scene) {
	assert((scene != NULL) && (*scene != NULL));

  G_TreeNode *node = G_TreeNodeMinimum(&((*scene)->transition));
  G_TileTransition *transition = (G_TileTransition*)(node->data);

  while ((transition != NULL) && (transition->when <= (*scene)->scene_step)) {
    G_ChangeTile(scene, transition->x, transition->y, transition->to, 0);

    if ((scene == &location) &&
        (G_IsPointWithin(transition->x, transition->y, &((*scene)->view)))) {
      transition->x -= (*scene)->view.x;
      transition->y -= (*scene)->view.y;

      if (G_CellToTile(transition->x, transition->y) == transition->is) {
        dmatrix[transition->x][transition->y].changed = 1;
        dmatrix[transition->x][transition->y].tile = transition->to;
      }
    }

    G_TreeNodeDelete(&((*scene)->transition), &node);
    node = G_TreeNodeMinimum(&((*scene)->transition));
    transition = (G_TileTransition*)(node->data);
  }
}

void G_AddEntity(G_Scene **scene, G_Entity **entity) {
  assert((scene != NULL) && (*scene != NULL) &&
        (entity != NULL) && (*entity != NULL));

  if ((*scene)->focus == NULL) {
    (*scene)->focus = *entity;
  }

  G_TreeNode *node = (G_TreeNode*)malloc(sizeof(G_TreeNode));
  node->key = (*entity)->id;
  node->data = (void*)(*entity);
  G_TreeNodeInsert(&((*scene)->entity), &node);
}

void G_AddTileTransition(G_Scene **scene, G_TileTransition **transition) {
  assert((scene != NULL) && (*scene != NULL) &&
        (transition != NULL) && (*transition != NULL));

  G_TreeNode *node = (G_TreeNode*)malloc(sizeof(G_TreeNode));
  node->key = (*transition)->x+((*transition)->y*(*scene)->w);
  node->data = (void*)(*transition);
  G_TreeNodeInsert(&((*scene)->transition), &node);
}

void G_TreeInitialize(G_Tree **tree) {
  assert((tree != NULL) && (*tree == NULL));

  (*tree) = (G_Tree*)malloc(sizeof(G_Tree));
  (*tree)->root = (G_TreeNode*)malloc(sizeof(G_TreeNode));
  (*tree)->root->right = (*tree)->root->left = (*tree)->root->parent = NIL;
  (*tree)->root->color = 'b';
  (*tree)->root->data = NULL;
  (*tree)->root->key = -1;
} 

void G_TreeNodeInsert(G_Tree **tree, G_TreeNode **node) {
  assert((tree != NULL) && (*tree != NULL) &&
        (node != NULL) && (*node != NULL));

  G_TreeNode
    *other = (*tree)->root->left,
    *parent = (*tree)->root,
    *uncle = NULL;

  while (other != NIL) {
    parent = other;

    if ((*node)->key < other->key) {
      other = other->left;
    } else {
      other = other->right;
    }
  }

  (*node)->left = NIL;
  (*node)->right = NIL;
  (*node)->parent = parent;
  (*node)->color = 'r';

  if ((parent == (*tree)->root) || (parent->key > (*node)->key)) {
    parent->left = *node;
  } else {
    parent->right = *node;
  }

  while ((*node)->parent->color == 'r') {
    if ((*node)->parent == (*node)->parent->parent->left) {
      uncle = (*node)->parent->parent->right;

      if (uncle->color == 'r') {
        (*node)->parent->color = 'b';
        (*node)->parent->parent->color = 'r';
        uncle->color = 'b';
        node = &((*node)->parent->parent);
      } else {
        if (*node == (*node)->parent->right) {
          *node = (*node)->parent;
          G_TreeNodeRotateLeft(tree, node);
        }

        (*node)->parent->color = 'b';
        (*node)->parent->parent->color = 'r';
        G_TreeNodeRotateRight(tree, &((*node)->parent->parent));
      }
    } else {
      uncle = (*node)->parent->parent->left;

      if (uncle->color == 'r') {
        (*node)->parent->color = 'b';
        (*node)->parent->parent->color = 'r';
        uncle->color = 'b';
        node = &((*node)->parent->parent);
      } else {
        if (*node == (*node)->parent->left) {
          *node = (*node)->parent;
          G_TreeNodeRotateRight(tree, node);
        }

        (*node)->parent->color = 'b';
        (*node)->parent->parent->color = 'r';
        G_TreeNodeRotateLeft(tree, &((*node)->parent->parent));

      }
    }
  }

  (*tree)->root->left->color = 'b';
}

void G_TreeNodeDelete(G_Tree **tree, G_TreeNode **node) {
  assert((tree != NULL) && (*tree != NULL) &&
        (node != NULL) && (*node != NULL));

  G_Tree *t = (*tree);
  G_TreeNode
    *x,
    *y,
    *z = (*node);

  if ((z->left == NIL) || (z->right == NIL)) {
    y = z;
  } else {
    y = G_TreeNodeSuccessor(tree, node);
  }

  x = (y->left == NIL) ? y->right : y->left;
  x->parent = y->parent;

  if (x->parent == t->root) {
    t->root->left = x;
  } else {
    if (y == y->parent->left) {
      y->parent->left = x;
    } else {
      y->parent->right = x;
    }
  }

  if (y->color == 'b') {
    G_TreeDeleteFix(tree, &x);
  }
  if (y != z) {
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
  }

  free(z);
}

void G_TreeNodeRotateLeft(G_Tree **tree, G_TreeNode **node) {
  assert((tree != NULL) && (*tree != NULL));
  G_TreeNode *n = (*node);

  G_TreeNode *child = n->right;
  n->right = child->left;

  if (child->left != NIL) {
    child->left->parent = n;
  }

  child->parent = n->parent;

  if (n == n->parent->left) {
    n->parent->left = child;
  } else {
    n->parent->right = child;
  }

  child->left = n;
  n->parent = child;
}

void G_TreeNodeRotateRight(G_Tree **tree, G_TreeNode **node) {
  assert((tree != NULL) && (*tree != NULL));

  G_TreeNode *child = (*node)->left;
  (*node)->left = child->right;

  if (child->right != NIL) {
    child->right->parent = *node;
  }

  child->parent = (*node)->parent;

  if ((*node) == (*node)->parent->right) {
    (*node)->parent->right = child;
  } else {
    (*node)->parent->left = child;
  }

  child->right = (*node);
  (*node)->parent = child;
}

void G_TreeDeleteFix(G_Tree **tree, G_TreeNode **node) {
  assert((tree != NULL) && (*tree != NULL) &&
        (node != NULL) && (*node != NULL));

  G_TreeNode
    *n = (*node),
    *sibling;

  while (n->color == 'b') {
    if (n == n->parent->left) {
      sibling = n->parent->right;

      if (sibling->color == 'r') {
        sibling->color = 'b';
        n->parent->color = 'r';
        G_TreeNodeRotateLeft(tree, &(n->parent));
        sibling = n->parent->right;
      }
      if ((sibling->right->color == 'b') && (sibling->left->color == 'b')) {
        sibling->color = 'r';
        n = n->parent;
      } else {
        if (sibling->right->color == 'b') {
          sibling->left->color = 'b';
          sibling->color = 'r';
          G_TreeNodeRotateRight(tree, &sibling);
          sibling = n->parent->right;
        }

        sibling->color = n->parent->color;
        n->parent->color = 'b';
        sibling->right->color = 'b';
        G_TreeNodeRotateLeft(tree, &(n->parent));
        break;
      }
    } else {
      sibling = n->parent->left;

      if (sibling->color == 'r') {
        sibling->color = 'b';
        n->parent->color = 'r';
        G_TreeNodeRotateRight(tree, &(n->parent));
        sibling = n->parent->left;
      }
      if ((sibling->left->color == 'b') && (sibling->right->color == 'b')) {
        sibling->color = 'r';
        n = n->parent;
      } else {
        if (sibling->left->color == 'b') {
          sibling->right->color = 'b';
          sibling->color = 'r';
          G_TreeNodeRotateLeft(tree, &sibling);
          sibling = n->parent->left;
        }

        sibling->color = n->parent->color;
        n->parent->color = 'b';
        sibling->left->color = 'b';
        G_TreeNodeRotateRight(tree, &(n->parent));
        break;
      }
    }
  }
}

G_TreeNode* G_TreeNodeRoot(G_Tree **tree) {
  return (*tree)->root->left;
}

G_TreeNode* G_TreeNodeFind(G_Tree **tree, long long key) {
  assert((tree != NULL) && (*tree != NULL));

  G_TreeNode *node = (*tree)->root->left;

  while ((node != NIL) && (node->key != key)) {
    node = (node->key < key) ? node->right : node->left;
  }

  return (node == NIL) ? NULL : node;
}

G_TreeNode* G_TreeNodeMinimum(G_Tree **tree) {
  assert((tree != NULL) && (*tree != NULL));

  G_TreeNode *other = (*tree)->root;

  while (other->left != NIL) {
    other = other->left;
  }

  return (other == NIL) ? NIL : other;
}

G_TreeNode* G_TreeNodeSuccessor(G_Tree **tree, G_TreeNode **node) {
  G_Tree *t = (*tree);
  G_TreeNode
    *n = (*node),
    *succ = n->right;

  if (succ != NIL) {
    while (succ->left != NIL) {
      succ = succ->left;
    }
  } else {
    for (succ = n->parent; n == succ->right; succ = succ->parent) {
      n = succ;
    }

    if (succ == t->root) {
      succ = NIL;
    }
  }

  return succ;
}

G_View* G_SceneView(G_Scene **scene) {
	assert((scene != NULL) && (*scene != NULL));

	return &((*scene)->view);
}

G_Tree* G_GetEntities(G_Scene **scene) {
	assert((scene != NULL) && (*scene != NULL));

	return (*scene)->entity;
}

G_Entity* G_FindEntity(G_Scene **scene, long long ID) {
	assert((scene != NULL) && (*scene != NULL));

	G_TreeNode *node = G_TreeNodeFind(&((*scene)->entity), ID);

	return (node == NULL) ? NULL : (G_Entity*)(node->data);
}

Tile G_SceneTile(int x, int y) {
	if ((x < 0) || (x >= location->w) ||
			(y < 0) || (y >= location->h) ||
			(location == NULL)) {
		return WALL;
	}

	return (location->tiles[x+(y*location->w)].tile);
}

/*
*/
