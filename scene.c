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

  light->light.r = 255;
  light->light.g = 255;
  light->light.b = 255;
  light->light.intensity = 32;

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
  light->light.intensity = 32;

  render->x = 64;
  render->y = 32;
  render->tile = NOTHING;
  render->layer = ORNAMENT_LAYER;

  G_SceneEntityInsert(&scene, &entity);

  entity = G_EntityCreate();
  light = G_EntityComponentInsert(&entity, LIGHT_COMPONENT);
  render = G_EntityComponentInsert(&entity, RENDER_COMPONENT);

  light->light.r = 0;
  light->light.g = 127;
  light->light.b = 255;
  light->light.intensity = 32;

  render->x = 32;
  render->y = 32;
  render->tile = NOTHING;
  render->layer = ORNAMENT_LAYER;

  G_SceneEntityInsert(&scene, &entity);

/*  entity = G_EntityCreate();
  light = G_EntityComponentInsert(&entity, LIGHT_COMPONENT);
  render = G_EntityComponentInsert(&entity, RENDER_COMPONENT);
  G_ElementComponent *element = G_EntityComponentInsert(&entity, ELEMENT_COMPONENT);

  light->light.r = 255;
  light->light.g = 255;
  light->light.b = 255;
  light->light.intensity = 3;

  render->x = 24;
  render->y = 80;
  render->tile = BASIC_FIRE;
  render->layer = ORNAMENT_LAYER;

  element->tile_flags = IS_BURNING;
  element->element_flags = SPREADS_PROPOGATE;

  G_SceneEntityInsert(&scene, &entity);
*/
  }

  /*************/

  return scene;
}

G_TileTransition* G_TileTransitionCreate(int x, int y, long long when, Tile is) {
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
  G_TreeNode *node = G_TreeNodeFind(&scenes, s->id.value);

  free(s->chunks);
  
  G_TreeDestroy(&(s->entities));
  G_TreeDestroy(&(s->transitions));
  G_TreeNodeDelete(&scenes, &node);

  *scene = NULL;
}

void G_SceneEntityInsert(G_Scene **scene, G_Entity **entity) {
  G_Scene *s = *scene;
  G_TreeNode *node = (G_TreeNode*)malloc(sizeof(G_TreeNode));
  
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

boolean G_SceneTilePropogate(G_Scene **scene, G_Entity **entity, int x, int y, boolean sentinel) {
  if (!sentinel) {
    return 0;
  }

  G_Scene *s = *scene;
  G_Entity *e = *entity;
  G_LightComponent *light = (G_LightComponent*)G_EntityComponentFind(entity, LIGHT_COMPONENT);
  G_RenderComponent *render = (G_RenderComponent*)G_EntityComponentFind(entity, RENDER_COMPONENT);
  G_ElementComponent *element = (G_ElementComponent*)G_EntityComponentFind(entity, ELEMENT_COMPONENT);
  Tile tile = G_SceneGetTile(scene, x, y);

  if ((element != NULL) && (tile != NOTHING) && (tile != ERROR_TILE)) {
    switch (element->tile_flags) {
      case IS_BURNING:
        {
          if (G_TileFlagCompare(tile, FLAMMABLE)) {
            G_TileTransitionCreate(x, y, s->step+512, tile+1);

            G_Entity *child = G_EntityCreate();
            G_LightComponent *n_light = (G_LightComponent*)G_EntityComponentInsert(&child, LIGHT_COMPONENT);
            G_RenderComponent *n_render = (G_RenderComponent*)G_EntityComponentInsert(&child, RENDER_COMPONENT);
            G_ElementComponent *n_element = (G_ElementComponent*)G_EntityComponentInsert(&child, ELEMENT_COMPONENT);

            child->parent = e->id;
            
            *n_light = *light;
            *n_render = *render;

            n_element->tile_flags = element->tile_flags;
            n_element->element_flags = element->element_flags;

            n_render->x = x;
            n_render->y = y;

            G_SceneEntityInsert(&active_scene, &child);

            return 1;
          }
        }
        break;
      default:
        {
          //empty
        }
        break;
    }
  }

  return 0;
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
