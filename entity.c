#include "yendor.h"
#include "globals.h"

G_Entity* G_EntityCreate(void) {
  int i;
  G_Entity *entity = (G_Entity*)malloc(sizeof(G_Entity));

  entity->id = G_GetId();
  
  for (i = 0; i < COMPONENT_COUNT; i += 1) {
    entity->components[i] = NULL;
  }

  return entity;
}

void* G_EntityComponentFind(G_Entity **entity, Component component) {
  assert((entity) && (*entity));

  return (*entity)->components[component];
}

void* G_EntityComponentInsert(G_Entity **entity, Component component) {
  assert((entity) && (*entity));

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
        element->amount = 0;
        element->intensity = 0;
        element->dissipation = 0;
        element->tile_flags = 0;
        element->target_flag = 0;
        element->element_flags = 0;
        element->directions = AL;
        element->func = NULL;
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
  assert((entity) && (*entity));
  
  if ((*entity)->components[component]) {
    if (component == UI_COMPONENT) {
      G_UIComponent *ui = (G_UIComponent*)G_EntityComponentFind(entity, component);
      G_UIWindowDestroy(&(ui->root));
    } else {
      free((*entity)->components[component]);
    }
  }
}

void G_EntitySetLayer(G_Entity **entity, TileLayer layer) {
  assert((entity) && (*entity));

  G_RenderComponent *render = (G_RenderComponent*)G_EntityComponentFind(entity, RENDER_COMPONENT);

  render->layer = layer;
}

void G_EntityPos(G_Entity **entity, int *x, int *y) {
  if ((entity) && (*entity)) {
    G_RenderComponent *render = (G_RenderComponent*)G_EntityComponentFind(entity, RENDER_COMPONENT);

    *x = render->x;
    *y = render->y;
  } else {
    *x = 0;
    *y = 0;
  }
}

void G_EntityMove(G_Entity **entity, boolean relative, int x, int y) {
  assert((entity) && (*entity));

  int cur_x = 0, cur_y = 0;
  G_RenderComponent *render = (G_RenderComponent*)G_EntityComponentFind(entity, RENDER_COMPONENT);

  if (render) {
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
  assert((entity) && (*entity));

  int i;
  G_Entity *e = *entity;

  for (i = 0; i < COMPONENT_COUNT; i += 1) {
    if (e->components[i]) {
      G_EntityComponentDelete(entity, i);
    }
  }
  
  free(*entity);
  *entity = NULL;
}

void G_EntityRender(void *entity) {
  assert(entity);

  G_Entity *e = *((G_Entity**)entity);
  G_UIComponent *ui = (G_UIComponent*)G_EntityComponentFind(&e, UI_COMPONENT);
  G_RenderComponent *render = (G_RenderComponent*)G_EntityComponentFind(&e, RENDER_COMPONENT);

  if (render) {
    if (G_PointWithinView(render->x, render->y)) {
      tilemap[render->x-active_scene->view.x+DCOLS_OFFSET][render->y-active_scene->view.y+DROWS_OFFSET].layers[render->layer] = render->tile;
    }
  } else if ((ui) && (ui->root) && (ui->root->visible)) {
    G_EntityRenderUI(&ui);
  }
}

void G_EntityRenderUI(G_UIComponent **ui) {
  G_RenderUIWindow(&((*ui)->root));
}

void G_EntityUpdate(void *entity) {
  assert(entity);

  G_Entity *e = *((G_Entity**)entity);

  G_UIComponentUpdate(&e);

  if (game_info.full) {
    G_ElementComponentUpdate(&e);
    G_ControllerComponentUpdate(&e);
  }
}

void G_UIComponentUpdate(G_Entity **entity) {
  G_Entity *e = *entity;
  G_UIComponent *ui = (G_UIComponent*)G_EntityComponentFind(&e, UI_COMPONENT);

  if (ui) {
    G_UpdateUIWindow(&(ui->root));
  }
}

void G_ElementComponentUpdate(G_Entity **entity) {
  G_Entity *e = *entity;
  G_RenderComponent *render = (G_RenderComponent*)G_EntityComponentFind(&e, RENDER_COMPONENT);
  G_ElementComponent *element = (G_ElementComponent*)G_EntityComponentFind(&e, ELEMENT_COMPONENT);

  if ((render) && (element)) {
    switch (element->element_flags) {
      case SPREADS_DIFFUSE:
        {
          // spreads volume outwards
        }
        break;
      case SPREADS_PROPOGATE:
        {
          if (element->amount <= 0) {
            if (element->func) {
              (element->func)(render->x, render->y, active_scene->step+G_RandomNumber(512, 768));
            }

            G_SceneEntityDelete(&active_scene, entity);
          } else {
            G_ElementPropogate(entity);
          }
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

  if (controller) {
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
  assert(entity);

  G_Entity *e = *((G_Entity**)entity);
  G_LightComponent *light = (G_LightComponent*)G_EntityComponentFind(&e, LIGHT_COMPONENT);
  G_RenderComponent *render = (G_RenderComponent*)G_EntityComponentFind(&e, RENDER_COMPONENT);

  if ((light) && (render)) {
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

void G_ElementDiffuse(G_Entity **entity) {

}

void G_ElementPropogate(G_Entity **entity) {
  G_QTreeLeaf *leaf;
  G_Entity *e = *entity, *nearby;
  G_RenderComponent *render = (G_RenderComponent*)G_EntityComponentFind(&e, RENDER_COMPONENT);
  G_ElementComponent *element = (G_ElementComponent*)G_EntityComponentFind(&e, ELEMENT_COMPONENT);
  G_Position position;

  element->amount -= 1;

  if (((element->directions & CA) != NA) && (element->intensity > 0)) {
    unsigned int i, x, y, value, amount;
    G_Entity *subentity = NULL;
    G_RenderComponent *subrender = NULL;
    G_ElementComponent *subelement = NULL;

    for (i = 1; i <= SE; i *= 2) {
      position = G_GetDirectionComponents(i);

      x = render->x+position.x;
      y = render->y+position.y;

      value = G_RandomNumber(0, 100);

      leaf = G_QTreeNodeFind(&(active_scene->collision), x, y);
      nearby = (leaf) ? leaf->entities[ELEMENT_LAYER] : NULL;

      if ((value < element->intensity) && ((element->directions & i) != NA)) {
        Tile tile = G_SceneGetTile(&active_scene, x, y);

        if ((!leaf) || (!leaf->entities[ELEMENT_LAYER])) {
          if (G_TileFlags(tile) & element->target_flag) {
            if (element->target_flag == FLAMMABLE) {
              amount = G_RandomNumber(0, G_TileFlags(tile) & FLAMMABLE);
            } else {
              amount = G_RandomNumber(0, 4);
            }
          } else {
            continue;
          }

          subentity = G_EntityCreate();
          subrender = G_EntityComponentInsert(&subentity, RENDER_COMPONENT);
          subelement = G_EntityComponentInsert(&subentity, ELEMENT_COMPONENT);

          memcpy((void*)subrender, (void*)render, sizeof(G_RenderComponent));
          memcpy((void*)subelement, (void*)element, sizeof(G_ElementComponent));

          subrender->x = x;
          subrender->y = y;

          element->directions = element->directions ^ i;

          subelement->amount += amount;
          subelement->intensity -= subelement->dissipation;
          subelement->directions = AL ^ ((i > NW) ? (i >> 4) : (i << 4));

          G_SceneEntityInsert(&active_scene, &subentity);
        }
      }
    }
  }
}

void G_ElementExplode(G_Entity **entity) {

}
