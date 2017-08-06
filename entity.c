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
        element->x = 0;
        element->y = 0;
        element->count = 0;
        element->magnitude = 0;
        element->tile_flags = 0;
        element->element_flags = 0;
        element->head = element->tail = NULL;
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
        ui->root = NULL;
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
    if (component == UI_COMPONENT) {
      G_UIComponent *ui = (G_UIComponent*)G_EntityComponentFind(entity, component);
      G_UIWindowDestroy(&(ui->root));
    } else {
      free((*entity)->components[component]);
    }
  }
}

void G_EntitySetLayer(G_Entity **entity, TileLayer layer) {
  assert((entity != NULL) && (*entity != NULL));

  G_RenderComponent *render = (G_RenderComponent*)G_EntityComponentFind(entity, RENDER_COMPONENT);

  render->layer = layer;
}

void G_EntityPos(G_Entity **entity, int *x, int *y) {
  if ((entity != NULL) && (*entity != NULL)) {
    G_RenderComponent *render = (G_RenderComponent*)G_EntityComponentFind(entity, RENDER_COMPONENT);

    *x = render->x;
    *y = render->y;
  } else {
    *x = 0;
    *y = 0;
  }
}

void G_EntityMove(G_Entity **entity, boolean relative, int x, int y) {
  assert((entity != NULL) && (*entity != NULL));

  int cur_x = 0, cur_y = 0;
  G_RenderComponent *render = (G_RenderComponent*)G_EntityComponentFind(entity, RENDER_COMPONENT);

  if (render != NULL) {
    G_EntityPos(entity, &cur_x, &cur_y);

    if (relative) {
      x += cur_x;
      y += cur_y;
    }

    render->x = x;
    render->y = y;

    G_QTreeNodeMove(&(active_scene->collision), entity, render->layer, cur_x, cur_y);
  }
}

void G_EntityDestroy(G_Entity **entity) {
  assert((entity != NULL) && (*entity != NULL));

  int i;
  G_Entity *e = *entity;

  for (i = 0; i < COMPONENT_COUNT; i += 1) {
    if (e->components[i] != NULL) {
      G_EntityComponentDelete(entity, i);
    }
  }
  
  free(*entity);
  *entity = NULL;
}

void G_EntityRender(void *entity) {
  assert(entity != NULL);

  G_Entity *e = *((G_Entity**)entity);
  G_UIComponent *ui = (G_UIComponent*)G_EntityComponentFind(&e, UI_COMPONENT);
  G_RenderComponent *render = (G_RenderComponent*)G_EntityComponentFind(&e, RENDER_COMPONENT);

  if (render != NULL) {
    if (G_PointWithinView(render->x, render->y)) {
      tilemap[render->x-active_scene->view.x+DCOLS_OFFSET][render->y-active_scene->view.y+DROWS_OFFSET].layers[render->layer] = render->tile;
    }
  } else if ((ui != NULL) && (ui->root != NULL) && (ui->root->visible)) {
    G_EntityRenderUI(&ui);
  }
}

void G_EntityRenderUI(G_UIComponent **ui) {
  G_RenderUIWindow(&((*ui)->root));
}

void G_EntityUpdate(void *entity) {
  assert(entity != NULL);

  G_Entity *e = *((G_Entity**)entity);

  G_UIComponentUpdate(&e);

  if (game_info.full) {
    G_ElementComponentUpdate(&e);
    G_ControllerComponentUpdate(&e);
  }
}

void G_UIComponentUpdate(G_Entity **entity) {
  G_Entity *e = *((G_Entity**)entity);
  G_UIComponent *ui = (G_UIComponent*)G_EntityComponentFind(&e, UI_COMPONENT);

  if (ui != NULL) {
    G_UpdateUIWindow(&(ui->root));
  }
}

void G_ElementComponentUpdate(G_Entity **entity) {
  G_Entity *e = *((G_Entity**)entity);
  G_RenderComponent *render = (G_RenderComponent*)G_EntityComponentFind(&e, RENDER_COMPONENT);
  G_ElementComponent *element = (G_ElementComponent*)G_EntityComponentFind(&e, ELEMENT_COMPONENT);

  if ((render != NULL) && (element != NULL)) {
    switch (element->element_flags) {
      case SPREADS_DIFFUSE:
        {
          // spreads volume outwards
        }
        break;
      case SPREADS_PROPOGATE:
        {
          // spreads propogates
        }
        break;
      case SPREADS_EXPLODE:
        {
          // spreads explode
        }
        break;
      default:
        break;
    }
  } 
}

void G_ControllerComponentUpdate(G_Entity **entity) {
  int x, y;
  G_RenderComponent *render = (G_RenderComponent*)G_EntityComponentFind(entity, RENDER_COMPONENT);
  G_ControllerComponent *controller = (G_ControllerComponent*)G_EntityComponentFind(entity, CONTROLLER_COMPONENT);

  if (controller != NULL) {
    x = render->x;
    y = render->y;

    if (game_info.phys[SDL_SCANCODE_H]) {
      if (render->x > 0) {
        if (!G_SceneTileObstructs(&active_scene, render->x-1, render->y)) {
          G_EntityMove(entity, 1, -1, 0);
          game_info.phys[SDL_SCANCODE_H] = 0;
        }
      }
    }
    if (game_info.phys[SDL_SCANCODE_K]) {
      if (render->y > 0) {
        if (!G_SceneTileObstructs(&active_scene, render->x, render->y-1)) {
          G_EntityMove(entity, 1, 0, -1);
          game_info.phys[SDL_SCANCODE_K] = 0;
        }
      }
    }
    if (game_info.phys[SDL_SCANCODE_J]) {
      if (render->y < active_scene->th-2) {
        if (!G_SceneTileObstructs(&active_scene, render->x, render->y+1)) {
          G_EntityMove(entity, 1, 0, 1);
          game_info.phys[SDL_SCANCODE_J] = 0;
        }
      }
    }
    if (game_info.phys[SDL_SCANCODE_L]) {
      if (render->x < active_scene->tw-2) {
        if (!G_SceneTileObstructs(&active_scene, render->x+1, render->y)) {
          G_EntityMove(entity, 1, 1, 0);
          game_info.phys[SDL_SCANCODE_L] = 0;
        }
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

      if (node.intensity > 2) {
  		  G_GenerateFOV(render->x, render->y, node.intensity, &node, &G_AddLight);
      } else {
        G_GenerateFOVSimple(render->x, render->y, &node, &G_AddLight);
      }
    }
  }
}
