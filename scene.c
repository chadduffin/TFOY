#include "yendor.h"
#include "globals.h"

G_Scene* G_SceneCreate(int w, int h, boolean persistent) {
  int i;
  G_Scene *scene = (G_Scene*)malloc(sizeof(G_Scene));
  G_TreeNode *node = (G_TreeNode*)malloc(sizeof(G_TreeNode));

  scene->w = w;
  scene->h = h;
  scene->tw = w*CHUNK_SIZE;
  scene->th = h*CHUNK_SIZE;
  scene->step = 0;
  scene->view.x = 0;
  scene->view.y = 0;
  scene->view.xp = 0;
  scene->view.yp = 0;
  scene->view.w = DCOLS;
  scene->view.h = DROWS;
  scene->view.follows = 0;
  scene->focus = NULL;

  scene->id = G_GetId();

  scene->entities = G_TreeCreate();
  scene->transitions = G_TreeCreate();
  scene->ins_buffer = scene->del_buffer = NULL;

  node->key = scene->id.value;
  node->data = (void*)scene;
  G_TreeNodeInsert(&scenes, &node);

  scene->collision = G_QTreeCreate();
  scene->collision->size = (w > h) ? scene->tw : scene->th;

  scene->ambient.r = 0;
  scene->ambient.g = 0;
  scene->ambient.b = 0;

  scene->persistent = persistent;

  scene->chunks = (G_SceneChunk*)malloc(sizeof(G_SceneChunk)*w*h);;

  for (i = 0; i < w*h; i += 1) {
    scene->chunks[i].tiles = NULL;
    scene->chunks[i].status = NOT_LOADED;
  }

  /* TEST CODE */

  if (scene->id.value == 0) {

  G_Entity *entity = G_EntityCreate();
  G_LightComponent *light = G_EntityComponentInsert(&entity, LIGHT_COMPONENT);
  G_RenderComponent *render = G_EntityComponentInsert(&entity, RENDER_COMPONENT);
  G_ControllerComponent *controller = G_EntityComponentInsert(&entity, CONTROLLER_COMPONENT);

  light->light.r = 64;
  light->light.g = 255;
  light->light.b = 46;
  light->light.intensity = 24;

  render->x = 30;
  render->y = 80;
  render->tile = HUMAN;
  render->layer = CREATURE_LAYER;

  scene->focus = entity;
  G_SceneEntityInsert(&scene, &entity);

  entity = G_EntityCreate();
  light = G_EntityComponentInsert(&entity, LIGHT_COMPONENT);
  render = G_EntityComponentInsert(&entity, RENDER_COMPONENT);

  light->light.r = 255;
  light->light.g = 0;
  light->light.b = 0;
  light->light.intensity = 24;

  render->x = 64;
  render->y = 32;
  render->tile = HUMAN;
  render->layer = ORNAMENT_LAYER;

  G_SceneEntityInsert(&scene, &entity);

  entity = G_EntityCreate();
  light = G_EntityComponentInsert(&entity, LIGHT_COMPONENT);
  render = G_EntityComponentInsert(&entity, RENDER_COMPONENT);

  light->light.r = 0;
  light->light.g = 127;
  light->light.b = 255;
  light->light.intensity = 30;

  render->x = 32;
  render->y = 32;
  render->tile = HUMAN;
  render->layer = ORNAMENT_LAYER;

  G_SceneEntityInsert(&scene, &entity);

  entity = G_EntityCreate();
  G_UIComponent *ui = (G_UIComponent*)G_EntityComponentInsert(&entity, UI_COMPONENT);

  ui->root = G_UIWindowCreate();

  ui->root->x = 0;
  ui->root->y = 0;
  ui->root->w = 16;
  ui->root->h = 16;
  ui->root->visible = 1;

  ui->root->widget = G_UIWidgetCreate();

  ui->root->widget->x = 1;
  ui->root->widget->y = 1;
  ui->root->widget->w = 5;
  ui->root->widget->h = 2;
  ui->root->widget->length = 9;
  ui->root->widget->hotkey = 0;
  ui->root->widget->func = NULL;
  ui->root->widget->data = NULL;
  ui->root->widget->tiles = (G_TileCell*)malloc(sizeof(G_TileCell)*(ui->root->widget->length));
  ui->root->widget->flags = VISIBLE | ACTIVE;

  ui->root->widget->tiles[0].layers[UI_LAYER] = 'P';
  ui->root->widget->tiles[1].layers[UI_LAYER] = 'L';
  ui->root->widget->tiles[2].layers[UI_LAYER] = 'A';
  ui->root->widget->tiles[3].layers[UI_LAYER] = 'Y';
  ui->root->widget->tiles[4].layers[UI_LAYER] = 0;
  ui->root->widget->tiles[5].layers[UI_LAYER] = 'G';
  ui->root->widget->tiles[6].layers[UI_LAYER] = 'A';
  ui->root->widget->tiles[7].layers[UI_LAYER] = 'M';
  ui->root->widget->tiles[8].layers[UI_LAYER] = 'E';

  G_SceneEntityInsert(&scene, &entity);
/*
  entity = G_EntityCreate();
  light = G_EntityComponentInsert(&entity, LIGHT_COMPONENT);
  render = G_EntityComponentInsert(&entity, RENDER_COMPONENT);
  G_ElementComponent *element = G_EntityComponentInsert(&entity, ELEMENT_COMPONENT);

  light->light.r = 255;
  light->light.g = 255;
  light->light.b = 255;
  light->light.intensity = 2;

  render->x = 25;
  render->y = 80;
  render->tile = BASIC_FIRE;
  render->layer = ORNAMENT_LAYER;

  element->spread_chance = 5;
  element->tile_flags = IS_BURNING;
  element->element_flags = SPREADS_PROPOGATE;

  G_SceneEntityInsert(&scene, &entity);
*/  }

  /*************/

  return scene;
}

G_TileTransition* G_TileTransitionCreate(int x, int y, unsigned int when, Tile is) {
  assert ((x >= 0) && (x < active_scene->tw) && (y >= 0) && (y < active_scene->th));

  G_Tile tile;
  G_TileTransition *transition = (G_TileTransition*)malloc(sizeof(G_TileTransition));

  transition->x = x;
  transition->y = y;
  transition->when = when;
  transition->is = tile.tile = is;
  transition->to = G_SceneGetTile(&active_scene, x, y);

  G_SceneSetGTile(&active_scene, tile, x, y);
  G_SceneTransitionInsert(&active_scene, &transition);

  return transition;
}

void G_SceneChange(G_Scene **scene) {
  assert((scene != NULL) && (*scene != NULL));

  if (*scene != active_scene) {
	  game_info.redraw = 1;
    active_scene = *scene;

    if (active_scene->id.value == menu_id.value) {
      G_ResizeDPort(0, 0, COLS, ROWS);
    } else {
      G_ResizeDPort(4, 4, COLS-8, ROWS-8);
    }
  }  
}

void G_SceneDestroy(G_Scene **scene) {
  G_Scene *s = *scene;
  G_Entity *entity = NULL;
  G_TreeNode *node = NULL;

  free(s->chunks);

  while ((node = G_TreeNodeMinimum(&(s->entities))) != NULL) {
    entity = (G_Entity*)(node->data);
    node->data = NULL;
    G_EntityDestroy(&entity);
    G_TreeNodeDelete(&(s->entities), &node);
  }
  
  G_TreeDestroy(&(s->entities));
  G_TreeDestroy(&(s->transitions));
  G_QTreeDestroy(&(s->collision));

  node = G_TreeNodeFind(&scenes, s->id.value);

  G_TreeNodeDelete(&scenes, &node);

  *scene = NULL;
}

void G_SceneEntityInsert(G_Scene **scene, G_Entity **entity) {
  G_Scene *s = *scene;
  G_TreeNode *node = (G_TreeNode*)malloc(sizeof(G_TreeNode));
  G_RenderComponent *render = (G_RenderComponent*)G_EntityComponentFind(entity, RENDER_COMPONENT);
  
  node->key = (*entity)->id.value;
  node->data = (void*)(*entity);
  node->right = NULL;
  node->left = NULL;

  if (s->ins_buffer == NULL) {
    s->ins_buffer = node;
  } else {
    node->left = s->ins_buffer;
    s->ins_buffer = node;
  }

  if (render != NULL) {
    G_QTreeNodeInsert(&(s->collision), entity, render->layer);
  }
}

void G_SceneEntityDelete(G_Scene **scene, G_Entity **entity) {
  assert((scene != NULL) && (*scene != NULL));

  G_Scene *s = *scene;
  G_Entity *e = *entity;
  G_TreeNode *node = (G_TreeNode*)malloc(sizeof(G_TreeNode));

  node->key = e->id.value;
  node->right = NULL;
  node->left = NULL;

  if (s->del_buffer == NULL) {
    s->del_buffer = node;
  } else {
    node->left = s->del_buffer;
    s->del_buffer = node;
  }
}

void G_SceneTransitionInsert(G_Scene **scene, G_TileTransition **transition) {
  G_TreeNode *node = (G_TreeNode*)malloc(sizeof(G_TreeNode));
  
  node->key = (*transition)->when;
  node->data = (void*)(*transition);

  G_TreeNodeInsert(&((*scene)->transitions), &node);
}

void G_SceneSetGTile(G_Scene **scene, G_Tile tile, int x, int y) {
  int
    id,
    chunk_x = x/CHUNK_SIZE,
    chunk_y = y/CHUNK_SIZE,
    offset_x = x%CHUNK_SIZE,
    offset_y = y%CHUNK_SIZE;
  G_Scene *s = *scene;

  id = chunk_x+chunk_y*(s->w);

  if ((chunk_x >= 0) && (chunk_x < s->w) && (chunk_y >= 0) && (chunk_y < s->h)) {
    if ((s->chunks[id].status == IS_LOADED) &&
        (offset_x >= 0) && (offset_x < CHUNK_SIZE) &&
        (offset_y >= 0) && (offset_y < CHUNK_SIZE)) {
      s->chunks[id].tiles[offset_x+(offset_y*CHUNK_SIZE)] = tile;

      if ((x >= s->view.x) && (x < s->view.x+s->view.w) && (y >= s->view.y) && (y < s->view.y+s->view.h)) {
        tilemap[x-s->view.x+DCOLS_OFFSET][y-s->view.y+DROWS_OFFSET].layers[BASE_LAYER] = tile.tile;
      }
    }
  }
}

Tile G_SceneGetTile(G_Scene **scene, int x, int y) {
  int
    id,
    chunk_x = x/CHUNK_SIZE,
    chunk_y = y/CHUNK_SIZE,
    offset_x = x%CHUNK_SIZE,
    offset_y = y%CHUNK_SIZE;
  Tile tile = ERROR_TILE;
  G_Scene *s = *scene;

  id = chunk_x+chunk_y*(s->w);

  if ((chunk_x >= 0) && (chunk_x < s->w) && (chunk_y >= 0) && (chunk_y < s->h)) {
    if ((s->chunks[id].status == IS_LOADED) &&
        (offset_x >= 0) && (offset_x < CHUNK_SIZE) &&
        (offset_y >= 0) && (offset_y < CHUNK_SIZE)) {
      tile = s->chunks[id].tiles[offset_x+(offset_y*CHUNK_SIZE)].tile;
    }
  }
  
  if (tile >= END_TILE) {
    tile = ERROR_TILE;
  }

  return tile;
}

G_Tile G_SceneGetGTile(G_Scene **scene, int x, int y) {
  G_Tile tile;

  tile.tile = G_SceneGetTile(scene, x, y);

  return tile;
}

boolean G_SceneTileObstructs(G_Scene **scene, int x, int y) {
  return ((G_TileFlags(G_SceneGetTile(scene, x, y)) & OBSTRUCTS_VISION) == OBSTRUCTS_VISION);
}

void G_InitMenu(G_Scene **scene) {
  assert((scene != NULL) && (*scene != NULL));

  int x, y;
  G_Tile tile;
  G_Scene *s = *scene;

  s->chunks[0].tiles = (G_Tile*)malloc(sizeof(G_Tile)*CHUNK_SIZE*CHUNK_SIZE);
  s->chunks[0].status = IS_LOADED;

  s->ambient.r = 255;
  s->ambient.g = 255;
  s->ambient.b = 255;

  for (y = 0; y < ROWS; y += 1) {
    for (x = 0; x < COLS; x += 1) {
      if (title[y][x] == 'B') {
        tile.tile = SOLID_BLACK;
      } else if ((x > 0) && (title[y][x-1] == 'B')) {
        tile.tile = SOLID_WHITE;
      } else if (title[y][x] == ' ') {
        tile.tile = SOLID_MAGENTA;
      } else {
        tile.tile = title[y][x];
      }

      G_SceneSetGTile(scene, tile, x, y);
    }
  }
}
