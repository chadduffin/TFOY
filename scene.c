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
  node->data = (void*)scene;
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

  if (s->entities == NULL) {
    printf("must be here\n");
  }
  if (s->transitions == NULL) {
    printf("or here\n");
  }

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
  G_Scene *s = *scene;

  G_Entity *entity = G_EntityCreate();
  G_LightComponent *light = (G_LightComponent*)G_EntityComponentInsert(&entity, LIGHT_COMPONENT);
  G_RenderComponent *render = (G_RenderComponent*)G_EntityComponentInsert(&entity, RENDER_COMPONENT);

  light->light.r = 255;
  light->light.g = 255;
  light->light.b = 255;
  light->light.intensity = 12;
  
  render->x = 100;
  render->y = 8;
  render->tile = HUMAN;
  render->layer = CREATURE_LAYER;

  s->focus = entity;

  G_EntityComponentInsert(&entity, CONTROLLER_COMPONENT);
  G_SceneEntityInsert(scene, &entity);

  entity = G_EntityCreate();
  light = (G_LightComponent*)G_EntityComponentInsert(&entity, LIGHT_COMPONENT);
  render = (G_RenderComponent*)G_EntityComponentInsert(&entity, RENDER_COMPONENT);

  light->light.r = 255;
  light->light.g = 51;
  light->light.b = 51;
  light->light.intensity = 28;
  
  render->x = 46;
  render->y = 11;
  render->tile = NOTHING;
  render->layer = ORNAMENT_LAYER;

  G_SceneEntityInsert(scene, &entity);

  entity = G_EntityCreate();
  light = (G_LightComponent*)G_EntityComponentInsert(&entity, LIGHT_COMPONENT);
  render = (G_RenderComponent*)G_EntityComponentInsert(&entity, RENDER_COMPONENT);

  light->light.r = 51;
  light->light.g = 150;
  light->light.b = 255;
  light->light.intensity = 28;
  
  render->x = 51;
  render->y = 27;
  render->tile = NOTHING;
  render->layer = ORNAMENT_LAYER;

  G_SceneEntityInsert(scene, &entity);

  entity = G_EntityCreate();
  light = (G_LightComponent*)G_EntityComponentInsert(&entity, LIGHT_COMPONENT);
  render = (G_RenderComponent*)G_EntityComponentInsert(&entity, RENDER_COMPONENT);

  light->light.r = 153;
  light->light.g = 255;
  light->light.b = 51;
  light->light.intensity = 28;
  
  render->x = 60;
  render->y = 20;
  render->tile = NOTHING;
  render->layer = ORNAMENT_LAYER;

  G_SceneEntityInsert(scene, &entity);

  entity = G_EntityCreate();
  light = (G_LightComponent*)G_EntityComponentInsert(&entity, LIGHT_COMPONENT);
  render = (G_RenderComponent*)G_EntityComponentInsert(&entity, RENDER_COMPONENT);

  light->light.r = 255;
  light->light.g = 255;
  light->light.b = 255;
  light->light.intensity = 24;
  
  render->x = WORLD_COLS-42;
  render->y = WORLD_ROWS-12;
  render->tile = NOTHING;
  render->layer = ORNAMENT_LAYER;

  G_SceneEntityInsert(scene, &entity);


  entity = G_EntityCreate();
  light = (G_LightComponent*)G_EntityComponentInsert(&entity, LIGHT_COMPONENT);
  render = (G_RenderComponent*)G_EntityComponentInsert(&entity, RENDER_COMPONENT);

  light->light.r = 255;
  light->light.g = 255;
  light->light.b = 255;
  light->light.intensity = 12;
  
  render->x = 136;
  render->y = 10;
  render->tile = NOTHING;
  render->layer = ORNAMENT_LAYER;

  G_SceneEntityInsert(scene, &entity);

  entity = G_EntityCreate();
  light = (G_LightComponent*)G_EntityComponentInsert(&entity, LIGHT_COMPONENT);
  render = (G_RenderComponent*)G_EntityComponentInsert(&entity, RENDER_COMPONENT);
  G_ElementComponent *element = (G_ElementComponent*)G_EntityComponentInsert(&entity, ELEMENT_COMPONENT);

  light->light.r = 255;
  light->light.g = 200;
  light->light.b = 200;
  light->light.intensity = 3;
  
  render->x = 24;
  render->y = 24;
  render->tile = BASIC_FIRE;
  render->layer = EFFECT_LAYER;

  element->tile_flags = IS_BURNING;
  element->element_flags = SPREADS_PROPOGATE;

  G_SceneEntityInsert(scene, &entity);

  int x, y;

  for (y = 0; y < s->h; y += 1) {
    for (x = 0; x < s->w; x += 1) {
      s->tiles[x+y*s->w].id.value = -1;

      if (test_scene[y][x] == '.') {
        s->tiles[x+y*s->w].tile = GROUND;
      } else if (test_scene[y][x] == '#') {
        s->tiles[x+y*s->w].tile = WALL;
      } else if (test_scene[y][x] == 'f') {
        s->tiles[x+y*s->w].tile = FUNGUS;
      } else if (test_scene[y][x] == 'l') {
        s->tiles[x+y*s->w].tile = LAVA;
      } else if (test_scene[y][x] == 'w') {
        s->tiles[x+y*s->w].tile = WATER;
      } else if (test_scene[y][x] == 'g') {
        s->tiles[x+y*s->w].tile = GRASS;
      }
    }
  }
}

void G_CreateFire(int x, int y) {
  G_Entity *entity = G_EntityCreate();
  G_LightComponent *light = (G_LightComponent*)G_EntityComponentInsert(&entity, LIGHT_COMPONENT);
  G_RenderComponent *render = (G_RenderComponent*)G_EntityComponentInsert(&entity, RENDER_COMPONENT);
  G_ElementComponent *element = (G_ElementComponent*)G_EntityComponentInsert(&entity, ELEMENT_COMPONENT);

  light->light.r = 255;
  light->light.g = 200;
  light->light.b = 200;
  light->light.intensity = 3;
  
  render->x = x;
  render->y = y;
  render->tile = BASIC_FIRE;
  render->layer = EFFECT_LAYER;

  element->tile_flags = IS_BURNING;
  element->element_flags = SPREADS_PROPOGATE;

  G_SceneEntityInsert(&active_scene, &entity);
}

void G_CreateLight(int x, int y) {
  G_Entity *entity = G_EntityCreate();
  G_LightComponent *light = (G_LightComponent*)G_EntityComponentInsert(&entity, LIGHT_COMPONENT);
  G_RenderComponent *render = (G_RenderComponent*)G_EntityComponentInsert(&entity, RENDER_COMPONENT);

  light->light.r = rand()%255;
  light->light.g = rand()%255;
  light->light.b = rand()%255;
  light->light.intensity = 16+rand()%8;
  
  render->x = x;
  render->y = y;
  render->tile = NOTHING;
  render->layer = EFFECT_LAYER;

  G_SceneEntityInsert(&active_scene, &entity);
}
