#include "yendor.h"
#include "globals.h"

G_Entity* G_EntityCreate(void) {
  int i;
  G_Entity *entity = (G_Entity*)malloc(sizeof(G_Entity));

  entity->id = G_GetId();
  entity->parent.value = -1;
  
  for (i = 0; i < COMPONENT_COUNT; i += 1) {
    entity->components[i] = NULL;
  }

  return entity;
}

void* G_EntityComponentFind(G_Entity **entity, Component component) {
  assert((entity != NULL) && (*entity != NULL));

  return (*entity)->components[component];
}

void* G_EntityComponentInsert(G_Entity **entity, Component component) {
  assert((entity != NULL) && (*entity != NULL));

	switch (component) {
		case CONTROLLER_COMPONENT:
			{
				G_ControllerComponent *controller = (G_ControllerComponent*)malloc(sizeof(G_ControllerComponent));
        controller->entity = (*entity);
				(*entity)->components[CONTROLLER_COMPONENT] = controller;
				return controller;
			}
			break;
		case CREATURE_COMPONENT:
			{
				G_CreatureComponent *creature = (G_CreatureComponent*)malloc(sizeof(G_CreatureComponent));
				(*entity)->components[CREATURE_COMPONENT] = creature;
				return creature;
			}
			break;
    case ELEMENT_COMPONENT:
      {
        G_ElementComponent *element = (G_ElementComponent*)malloc(sizeof(G_ElementComponent));
        element->volume = 128+rand()%32;
        element->spread = 15;
        element->tile_flags = 0;
        element->element_flags = 0;
        (*entity)->components[ELEMENT_COMPONENT] = element;
        return element;
      }
      break;
		case RENDER_COMPONENT:
			{
				G_RenderComponent *render = (G_RenderComponent*)malloc(sizeof(G_RenderComponent));
				render->x = 0;
				render->y = 0;
				render->xp = 0;
				render->yp = 0;
				render->tile = NOTHING;
        render->layer = CREATURE_LAYER;
				(*entity)->components[RENDER_COMPONENT] = render;
				return render;
			}
			break;
		case LIGHT_COMPONENT:
			{
				G_LightComponent *light = (G_LightComponent*)malloc(sizeof(G_LightComponent));
				light->light.r = 0;
				light->light.g = 0;
				light->light.b = 0;
				light->light.intensity = 0;
				(*entity)->components[LIGHT_COMPONENT] = light;
				return light;
			}
			break;
		case UI_COMPONENT:
			{
				G_UIComponent *ui = (G_UIComponent*)malloc(sizeof(G_UIComponent));
				(*entity)->components[UI_COMPONENT] = ui;
				return ui;
			}
			break;
		default:
			printf("Invalid component type.\n");
			return NULL;
  }
}

void G_EntityComponentDelete(G_Entity **entity, Component component) {
  assert((entity != NULL) && (*entity != NULL));
  
  if ((*entity)->components[component] != NULL) {
    free((*entity)->components[component]);
  }
}

void G_EntitySetLayer(G_Entity **entity, TileLayer layer) {
  assert((entity != NULL) && (*entity != NULL));

  G_RenderComponent *render = (G_RenderComponent*)G_EntityComponentFind(entity, RENDER_COMPONENT);

  render->layer = layer;
}

void G_EntityPos(G_Entity **entity, int *x, int *y) {
  assert((entity != NULL) && (*entity != NULL));

  G_RenderComponent *render = (G_RenderComponent*)G_EntityComponentFind(entity, RENDER_COMPONENT);

  *x = render->x;
  *y = render->y;
}

void G_EntityDestroy(G_Entity **entity) {
  assert((entity != NULL) && (*entity != NULL));
  
  free(*entity);
  *entity = NULL;
}

void G_EntityRender(void *entity) {
  assert(entity != NULL);

  G_Entity *e = *((G_Entity**)entity);
  G_RenderComponent *render = (G_RenderComponent*)G_EntityComponentFind(&e, RENDER_COMPONENT);

  if (render != NULL) {
    if (G_PointWithinView(render->x, render->y)) {
      tilemap[render->x-active_scene->view.x+DCOLS_OFFSET][render->y-active_scene->view.y+DROWS_OFFSET].layers[render->layer] = render->tile;
    }
  }
}

void G_EntityUpdate(void *entity) {
  assert(entity != NULL);

  G_Entity *e = *((G_Entity**)entity);

  if (game_info.full) {
    G_ElementComponentUpdate(&e);
    G_ControllerComponentUpdate(&e);
  }
}

void G_ElementComponentUpdate(G_Entity **entity) {
  int x, y, spread;
  G_Entity *e = *((G_Entity**)entity);
  G_RenderComponent *render = (G_RenderComponent*)G_EntityComponentFind(&e, RENDER_COMPONENT);
  G_ElementComponent *element = (G_ElementComponent*)G_EntityComponentFind(&e, ELEMENT_COMPONENT);

  if ((render != NULL) && (element != NULL)) {
    for (y = -1; y < 2; y += 1) {
      for (x = -1; x < 2; x += 1) {
        if (!((x == 0) && (y == 0))) {
          spread = rand()%100;

          if (spread <= element->spread) {
            G_SceneTileExpose(&active_scene, entity, render->x+x, render->y+y);
          }
        }
      }

      element->volume -= 1;
    
      if (element->volume == 0) {
        element->volume = -1;
        G_SceneEntityDelete(&active_scene, entity);
      }
    } 
  } 
}

void G_ControllerComponentUpdate(G_Entity **entity) {
  G_Entity *e = *entity;
  G_RenderComponent *render = (G_RenderComponent*)G_EntityComponentFind(&e, RENDER_COMPONENT);
  G_ControllerComponent *controller = (G_ControllerComponent*)G_EntityComponentFind(&e, CONTROLLER_COMPONENT);

  if (controller != NULL) {
    if (game_info.phys[SDL_SCANCODE_H]) {
      if (render->x > 0) {
        render->x -= 1;
      }
    }
    if (game_info.phys[SDL_SCANCODE_J]) {
      if (render->y > 0) {
        render->y -= 1;
      }
    }
    if (game_info.phys[SDL_SCANCODE_K]) {
      if (render->y < active_scene->h-2) {
        render->y += 1;
      }
    }
    if (game_info.phys[SDL_SCANCODE_L]) {
      if (render->x < active_scene->w-2) {
        render->x += 1;
      }
    }
  }
}

void G_EntityLightAdd(void *entity) {
  assert(entity != NULL);

  G_Entity *e = *((G_Entity**)entity);
  G_LightComponent *light = (G_LightComponent*)G_EntityComponentFind(&e, LIGHT_COMPONENT);
  G_RenderComponent *render = (G_RenderComponent*)G_EntityComponentFind(&e, RENDER_COMPONENT);

  if ((light != NULL) && (render != NULL)) {
    int x = render->x-(active_scene->view.x+active_scene->view.w/2),
        y = render->y-(active_scene->view.y+active_scene->view.h/2),
        dist = sqrt(x*x+y*y)-light->light.intensity;

    if (dist < sqrt((DCOLS/2)*(DCOLS/2)+(DROWS/2)*(DROWS/2))) {
		  G_LightNode node;

  		node.x = render->x;
  		node.y = render->y;
  		node.r = light->light.r;
  		node.g = light->light.g;
  		node.b = light->light.b;
  		node.intensity = light->light.intensity;
  		node.id = e->id;

  		G_GenerateFOV(render->x, render->y, &node, &G_AddLight);
    }
  }
}
