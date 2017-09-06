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
    G_ElementComponent *element = G_EntityComponentInsert(&entity, ELEMENT_COMPONENT);

    light->light.r = 255;
    light->light.g = 255;
    light->light.b = 255;
    light->light.intensity = 2;

    element->amount = 128;
    element->intensity = 32;
    element->dissipation = 1;
    element->tile_flags = IS_BURNING | FLICKERS_REGULAR | ILLUMINATING;
    element->target_flag = FLAMMABLE;
    element->element_flags = SPREADS_PROPOGATE;
    element->func = &G_BurnTile;

    render->x = 35;
    render->y = 80;
    render->tile = BASIC_FIRE;
    render->layer = PROPOGATE_LAYER;

    scene->focus = entity;
    G_SceneEntityInsert(&scene, &entity);

    entity = G_EntityCreate();
    render = G_EntityComponentInsert(&entity, RENDER_COMPONENT);
    element = G_EntityComponentInsert(&entity, ELEMENT_COMPONENT);

    element->amount = 128;
    element->group = (G_ElementGroup*)malloc(sizeof(G_ElementGroup));
    element->group->amount = 128;
    element->group->node_count = 1;
    element->group->id.value = entity->id.value;
    element->tile_flags = IS_EXTINGUISHING | FLICKERS_QUICK;
    element->target_flag = 0;
    element->element_flags = SPREADS_DIFFUSE;
    element->directions = AL;
    element->func = NULL;

    render->x = 30;
    render->y = 70;
    render->tile = WATER;
    render->layer = PROPOGATE_LAYER;

    G_SceneEntityInsert(&scene, &entity);

    entity = G_EntityCreate();
    render = G_EntityComponentInsert(&entity, RENDER_COMPONENT);
    element = G_EntityComponentInsert(&entity, ELEMENT_COMPONENT);

    element->amount = 1024;
    element->intensity = 32;
    element->dissipation = 1;
    element->tile_flags = IS_FREEZING | FLICKERS_REGULAR | ILLUMINATING;
    element->target_flag = FREEZABLE;
    element->element_flags = SPREADS_PROPOGATE;
    element->func = &G_FreezeTile;

    render->x = 28;
    render->y = 60;
    render->tile = ICE;
    render->layer = PROPOGATE_LAYER;

    G_SceneEntityInsert(&scene, &entity);

    entity = G_EntityCreate();
    G_EntityComponentInsert(&entity, CONTROLLER_COMPONENT);
    light = G_EntityComponentInsert(&entity, LIGHT_COMPONENT);
    render = G_EntityComponentInsert(&entity, RENDER_COMPONENT);

    light->light.r = 255;
    light->light.g = 255;
    light->light.b = 255;
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
  }

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
  assert((scene) && (*scene));

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

  while ((node = G_TreeNodeMinimum(&(s->entities)))) {
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

  if (!s->ins_buffer) {
    s->ins_buffer = node;
  } else {
    node->left = s->ins_buffer;
    s->ins_buffer = node;
  }

  if (render) {
    G_QTreeNodeInsert(&(s->collision), entity, render->layer);
  }
}

void G_SceneEntityDelete(G_Scene **scene, G_Entity **entity) {
  assert((scene) && (*scene));

  G_Scene *s = *scene;
  G_Entity *e = *entity;
  G_TreeNode *node = (G_TreeNode*)malloc(sizeof(G_TreeNode));

  G_EntityMove(entity, 0, -1, -1);

  node->key = e->id.value;
  node->right = NULL;
  node->left = NULL;

  if (!s->del_buffer) {
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

void G_BurnTile(int x, int y, unsigned int when) {
  G_TileTransition *transition = G_TileTransitionCreate(x, y, when, BURNT_GRASS);

  G_SceneTransitionInsert(&active_scene, &transition);
}

void G_FreezeTile(int x, int y, unsigned int when) {
  G_TileTransition *transition = G_TileTransitionCreate(x, y, when, FROZEN_GRASS);

  G_SceneTransitionInsert(&active_scene, &transition);
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
