#include "yendor.h"
#include "globals.h"

G_Scene* G_SceneCreate(int w, int h) {
  G_Scene *scene = (G_Scene*)malloc(sizeof(G_Scene));
  G_TreeNode *node = (G_TreeNode*)malloc(sizeof(G_TreeNode));

  scene->w = w;
  scene->h = h;
  scene->step = 0;
  scene->view.x = 0;
  scene->view.y = 0;
  scene->view.xp = 0;
  scene->view.yp = 0;
  scene->view.w = DCOLS;
  scene->view.h = DROWS;
  scene->focus = NULL;

  scene->id = G_GetId();

  scene->tiles = (G_Tile*)malloc(sizeof(G_Tile)*w*h);

  scene->entities = G_TreeCreate();
  scene->transitions = G_TreeCreate();
  scene->ins_buffer = scene->del_buffer = NULL;

  node->key = scene->id.value;
  G_TreeNodeInsert(&scenes, &node);

  scene->ambient.r = 0;
  scene->ambient.g = 0;
  scene->ambient.b = 0;

  return scene;
}

G_TileTransition* G_TileTransitionCreate(int x, int y, long long when, Tile is) {
  assert ((x >= 0) && (x < active_scene->w) && (y >= 0) && (y < active_scene->h));

  G_Tile tile;
  G_TileTransition *transition = (G_TileTransition*)malloc(sizeof(G_TileTransition));

  transition->x = x;
  transition->y = y;
  transition->when = when;
  transition->id = tile.id = G_GetId();
  transition->is = tile.tile = is;
  transition->to = G_SceneGetTile(&active_scene, x, y);

  G_SceneSetGTile(&active_scene, tile, x, y);
  G_SceneTransitionInsert(&active_scene, &transition);

  return transition;
}

void G_SceneChange(G_Scene **scene) {
  assert((scene != NULL) && (*scene != NULL));

  if (*scene != active_scene) {
    active_scene = *scene;
  }  
}

void G_SceneDestroy(G_Scene **scene) {
  G_Scene *s = *scene;
  G_TreeNode *node = G_TreeNodeFind(&scenes, s->id.value);

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
  G_Scene *s = *scene;

  if ((x >= 0) || (x < s->w) || (y >= 0) || (y < s->h)) {
    s->tiles[x+(y*s->w)] = tile;
  }
}

Tile G_SceneGetTile(G_Scene **scene, int x, int y) {
  G_Scene *s = *scene;

  if ((x < 0) || (x >= s->w) || (y < 0) || (y >= s->h)) {
    return NOTHING;
  }

  return s->tiles[x+(y*s->w)].tile;
}

G_Tile G_SceneGetGTile(G_Scene **scene, int x, int y) {
  G_Tile tile;
  G_Scene *s = *scene;

  if ((x < 0) || (x >= s->w) || (y < 0) || (y >= s->h)) {
    tile.id.value = -1;
    tile.tile = NOTHING;

    return tile;
  }

  return s->tiles[x+(y*s->w)];
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

  if ((element != NULL) && (tile != NOTHING)) {
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

  tile.id.value = -1;

  s->ambient.r = 255;
  s->ambient.g = 255;
  s->ambient.b = 255;

  for (y = 0; y < ROWS; y += 1) {
    for (x = 0; x < COLS; x += 1) {
      if (title[y][x] == 'B') {
        tile.tile = SOLID_BLACK;
      } else if ((y > 0) && (title[y-1][x] == 'B')) {
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

void G_TestScene(G_Scene **scene) {
  int q;
  G_Scene *s = *scene;

  for (q = 0; q < 16; q += 1) {
    G_Entity *player = G_EntityCreate();
    G_LightComponent *light = (G_LightComponent*)G_EntityComponentInsert(&player, LIGHT_COMPONENT);
    G_RenderComponent *render = (G_RenderComponent*)G_EntityComponentInsert(&player, RENDER_COMPONENT);

    light->light.r = G_RandomNumber(0, 255);
    light->light.g = G_RandomNumber(0, 255);
    light->light.b = G_RandomNumber(0, 255);
    light->light.intensity = 16+rand()%24;
  
    render->x = G_RandomNumber(0, 108);
    render->y = G_RandomNumber(0, 64);
    render->tile = NOTHING;

    if (s->focus == NULL) {
      s->focus = player;
      render->x = 2;
      render->y = 2;
      render->tile = HUMAN;
      light->light.intensity = 32;
      G_EntityComponentInsert(&player, CONTROLLER_COMPONENT);
    }

    G_SceneEntityInsert(scene, &player);
  }

  G_Entity *fire = G_EntityCreate();
  G_LightComponent *light = (G_LightComponent*)G_EntityComponentInsert(&fire, LIGHT_COMPONENT);
  G_RenderComponent *render = (G_RenderComponent*)G_EntityComponentInsert(&fire, RENDER_COMPONENT);
  G_ElementComponent *element = (G_ElementComponent*)G_EntityComponentInsert(&fire, ELEMENT_COMPONENT);

  light->light.r = 255;
  light->light.g = 255;
  light->light.b = 255;
  light->light.intensity = 3;
  
  render->x = 24;
  render->y = 24;
  render->tile = BASIC_FIRE;
  render->layer = EFFECT_LAYER;

  element->tile_flags = IS_BURNING;
  element->element_flags = SPREADS_PROPOGATE;

  G_SceneEntityInsert(scene, &fire);

  int w = s->w, h = s->h, i;

  for (i = 0; i < w*h; i += 1) {
    s->tiles[i].id.value = -1;
    s->tiles[i].tile = (G_RandomNumber(0, 100) == 1) ? WALL : GROUND;
  }

  for (h = 0; h < 48; h += 1) {
    for (w = 0; w < 48; w += 1) {
      int dist = sqrt(h*h+w*w);
      G_Tile tile;

      if ((dist > 8) && (dist < 12+rand()%2)) {
        tile.tile = WATER;
      } else if (dist < 36+rand()%12) {
        tile.tile = GRASS;
      } else {
        tile.tile = GROUND;
      }

      tile.id.value = -1;
      G_SceneSetGTile(&active_scene, tile, w, h);
    }
  }

  for (h = 16; h < 48; h += 1) {
    for (w = 64; w < 96; w += 1) {
      int dist = sqrt((h-32)*(h-32)+(w-80)*(w-80));
      G_Tile tile;
      tile.tile = LAVA;

      if (dist < 10+rand()%3) {
        tile.id.value = -1;
        G_SceneSetGTile(&active_scene, tile, w, h);

      }
    }
  }

  for (h = 100; h < 120; h += 1) {
    for (w = 24; w < 40; w += 1) {
      int dist = sqrt((h-110)*(h-110)+(w-32)*(w-32));
      G_Tile tile;
      tile.tile = FUNGUS;

      if (dist < 2+rand()%10) {
        tile.id.value = -1;
        G_SceneSetGTile(&active_scene, tile, w, h);

      }
    }
  }
}

