#include "yendor.h"
#include "globals.h"

/*
** FUNCTIONS
*/

G_Entity* G_CreateEntity(EntityType type) {
	int i;
	G_Entity *entity = (G_Entity*)malloc(sizeof(G_Entity));

	entity->id = G_GetID();
	entity->type = type;

	for (i = 0; i < COMPONENT_COUNT; i += 1) {
		entity->components[i] = NULL;
	}

	return entity;
}

void* G_AddComponent(G_Entity **entity, Component component) {
	assert((entity != NULL) && (*entity != NULL));

	switch (component) {
		case CONTROLLER_COMPONENT:
			{
				G_ControllerComponent *controller = (G_ControllerComponent*)malloc(sizeof(G_ControllerComponent));
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
        (*entity)->components[ELEMENT_COMPONENT] = element;
        return element;
      }
      break;
		case RENDER_COMPONENT:
			{
				G_RenderComponent *render = (G_RenderComponent*)malloc(sizeof(G_RenderComponent));
				render->x = 0;
				render->y = 0;
				render->z = 0;
				render->x_previous = -1;
				render->y_previous = -1;
				render->tile = NOTHING;
				(*entity)->components[RENDER_COMPONENT] = render;
				return render;
			}
			break;
		case LIGHT_COMPONENT:
			{
				G_LightComponent *light = (G_LightComponent*)malloc(sizeof(G_LightComponent));
				light->light.red = 0;
				light->light.green = 0;
				light->light.blue = 0;
				light->light.intensity = 0;
				(*entity)->components[LIGHT_COMPONENT] = light;
				return light;
			}
			break;
		case UI_COMPONENT:
			{
				G_UIComponent *ui = (G_UIComponent*)malloc(sizeof(G_UIComponent));
				ui->hotkey = '\0';
				ui->name = NULL;
				ui->x = 0;
				ui->y = 0;
				ui->l = 0;
				ui->focus = 0;
				ui->data = NULL;
				ui->on_click = NULL;
				ui->on_hover = NULL;
				ui->state = ACTIVE | CHANGED;
				ui->border = 1;
				(*entity)->components[UI_COMPONENT] = ui;
				return ui;
			}
			break;
		default:
			printf("Invalid component type.\n");
			return NULL;
	}
}

void* G_GetComponent(G_Entity **entity, Component component) {
	assert((entity != NULL) && (*entity != NULL));

	return (*entity)->components[component];
}

void G_RemoveComponent(G_Entity **entity, Component component) {
	assert((entity != NULL) && (*entity != NULL) && ((*entity)->components[component] != NULL));
		
	free((*entity)->components[component]);
	(*entity)->components[component] = NULL;
}

void G_EntityPos(G_Entity **entity, int *x, int *y) {
	assert((entity != NULL) && (*entity != NULL) && (x != NULL) && (y != NULL));

	G_RenderComponent *render = G_GetComponent(entity, RENDER_COMPONENT);

	if (render != NULL) {
		*x = render->x;
		*y = render->y;
	}
}

void G_EntityUpdate(G_Entity **entity) {
	assert((entity != NULL) && (*entity != NULL));
	
	G_UIComponent *ui = (G_UIComponent*)(G_GetComponent(entity, UI_COMPONENT));
	G_RenderComponent *render = (G_RenderComponent*)(G_GetComponent(entity, RENDER_COMPONENT));
	G_CreatureComponent *creature = (G_CreatureComponent*)(G_GetComponent(entity, CREATURE_COMPONENT));
	G_ControllerComponent *controller = (G_ControllerComponent*)(G_GetComponent(entity, CONTROLLER_COMPONENT));

	if (render != NULL) {
		render->x_previous = render->x;
		render->y_previous = render->y;
		
		if (controller != NULL) {
			if (G_CheckBound(RIGHT)) {
				render->x += 1;
				phys_keys[virt_keys[RIGHT]] = 0;
				G_InvalidateView();
			}
			if (G_CheckBound(LEFT)) {
				render->x -= 1;
				phys_keys[virt_keys[LEFT]] = 0;
				G_InvalidateView();
			}
			if (G_CheckBound(UP)) {
				render->y -= 1;
				phys_keys[virt_keys[UP]] = 0;
				G_InvalidateView();
			}
			if (G_CheckBound(DOWN)) {
				render->y += 1;
				phys_keys[virt_keys[DOWN]] = 0;
				G_InvalidateView();
			}
		}

		if (render->tile != NOTHING) {
			TileLayer layer;
			G_View *view = G_SceneView(&location);

			if (creature != NULL) {
				layer = CREATURE_LAYER;
			} else {
				layer = ORNAMENT_LAYER;
			}

			if (layer != -1) {
				if ((render->x == render->x_previous) && (render->y == render->y_previous)) {
					int
						rx = render->x_previous+(view->x-view->xp),
						ry = render->y_previous+(view->y-view->yp);
			
					if (G_IsPointWithin(rx, ry, view)) {
						dmatrix[rx-view->x+DCOLS_OFFSET][ry-view->y+DROWS_OFFSET].changed = 1;
						if (dmatrix[rx-view->x+DCOLS_OFFSET][ry-view->y+DROWS_OFFSET].layers[layer] == (*entity)->id) {
							dmatrix[rx-view->x+DCOLS_OFFSET][ry-view->y+DROWS_OFFSET].layers[layer] = -1;
						}
					}
				} else {
					if (G_IsPointWithin(render->x_previous, render->y_previous, view)) {
						dmatrix[render->x_previous-view->x+DCOLS_OFFSET][render->y_previous-view->y+DROWS_OFFSET].changed = 1;
						if (dmatrix[render->x_previous-view->x+DCOLS_OFFSET][render->y_previous-view->y+DROWS_OFFSET].layers[layer] == (*entity)->id) {
							dmatrix[render->x_previous-view->x+DCOLS_OFFSET][render->y_previous-view->y+DROWS_OFFSET].layers[layer] = -1;
						}
					}
				}
			}
		}
	}

  G_UIEntityUpdate(&ui);
}
	
void G_UIEntityUpdate(G_UIComponent **ui) {
  if ((ui != NULL) && (*ui != NULL)) {
  	int
  		mouse_x = (game_info.mouse_x-game_info.display_x)/game_info.tile_w,
  		mouse_y = (game_info.mouse_y-game_info.display_y)/game_info.tile_h;

  	UIState state = (*ui)->state;
  
  	if ((mouse_x < (*ui)->x-(*ui)->border) || (mouse_x >= (*ui)->x+(*ui)->l+(*ui)->border) ||
  			(mouse_y < (*ui)->y-(*ui)->border) || (mouse_y > (*ui)->y+(*ui)->border)) {
  		if (((*ui)->state & PRESSED) == PRESSED) {
  			(*ui)->state = (*ui)->state ^ PRESSED;
  		}
  		if (((*ui)->state & HOVER) == HOVER) {
  			(*ui)->state = (*ui)->state ^ HOVER;
  		}
  		if (((*ui)->focus > 0) && (((*ui)->on_hover != NULL) || ((*ui)->on_click != NULL))) {
  			(*ui)->focus -= 17;
  			(*ui)->state = (*ui)->state | CHANGED;
  			G_InvalidateView();
  		}
  	} else {
  		if ((((*ui)->state & HOVER) != HOVER) && ((*ui)->on_hover != NULL)) {
  			(*ui)->on_hover((*ui)->data);
  		}
  		if (game_info.mouse_lb > 0) {
  			(*ui)->state = (*ui)->state | PRESSED;
  		} else if (((*ui)->state & PRESSED) == PRESSED) {
  			(*ui)->state = (*ui)->state ^ PRESSED;
  			if ((*ui)->on_click != NULL) {
  				(*ui)->on_click((*ui)->data);
  			}
  		}
  
  		if (((*ui)->focus < 255) && (((*ui)->on_hover != NULL) || ((*ui)->on_click != NULL))) {
  			(*ui)->focus += 17;
  			(*ui)->state = (*ui)->state | CHANGED;
  			G_InvalidateView();
  		}
  
  		(*ui)->state = (*ui)->state | HOVER;
  	}
  
  	if ((*ui)->state != state) {
  		(*ui)->state = (*ui)->state | CHANGED;
  	}		
  }
}
  
void G_EntityRender(G_Entity **entity) {
	assert((entity != NULL) && (*entity != NULL));

	G_View *view = G_SceneView(&location);
	G_LightComponent *light = (G_LightComponent*)(G_GetComponent(entity, LIGHT_COMPONENT));
	G_RenderComponent *render = (G_RenderComponent*)(G_GetComponent(entity, RENDER_COMPONENT));
	G_CreatureComponent *creature = (G_CreatureComponent*)(G_GetComponent(entity, CREATURE_COMPONENT));
	G_UIComponent *ui = (G_UIComponent*)(G_GetComponent(entity, UI_COMPONENT));

	if (render != NULL) {
		TileLayer layer;

		if (creature != NULL) {
			layer = CREATURE_LAYER;
		} else {
			layer = ORNAMENT_LAYER;
		}

		if ((layer != -1) && (render->tile != NOTHING) && (G_IsPointWithin(render->x, render->y, view))) {
			dmatrix[render->x-view->x+DCOLS_OFFSET][render->y-view->y+DROWS_OFFSET].changed = 1;
			dmatrix[render->x-view->x+DCOLS_OFFSET][render->y-view->y+DROWS_OFFSET].layers[layer] = (*entity)->id;
		}

		if (light != NULL) {
			G_LightNode node;
			node.x = render->x;
			node.y = render->y;
			node.id = (*entity)->id;
			node.light = light->light;

			G_GenerateFOV(render->x, render->y, &node, &G_AddLight);
		}
	}

	if (ui != NULL) {
		if ((location->view.unchanged == 0) || (ui->state & CHANGED) == CHANGED) {
			int
				x,
				y;
			ui->state = ui->state ^ CHANGED;

			for (y = -(ui->border); y <= (ui->border); y += 1) {
				for (x = -(ui->border); x < (ui->l+ui->border); x += 1) {
					if ((x == -1) || (x == ui->l) ||
							(y == -1) || (y == 1)) {
						dmatrix[x+ui->x][y+ui->y].tile = ' ';
					} else {
						dmatrix[x+ui->x][y+ui->y].tile = ui->name[x];
					}

					dmatrix[x+ui->x][y+ui->y].changed = 1;
					dmatrix[x+ui->x][y+ui->y].fg = white;
					dmatrix[x+ui->x][y+ui->y].fg.blue = 255-ui->focus;
					dmatrix[x+ui->x][y+ui->y].bg.red = ui->focus/7;
					dmatrix[x+ui->x][y+ui->y].bg.green = ui->focus/7;
					dmatrix[x+ui->x][y+ui->y].bg.blue = ui->focus/7;
				}
			}
		}
	}
}

EntityType G_GetEntityType(G_Entity **entity) {
	assert((entity != NULL) && (*entity != NULL));
	
	return (*entity)->type;
}

Tile G_EntityIDToTile(int ID) {
	G_Entity *entity = G_FindEntity(&location, ID);
	
	if (entity != NULL) {
		G_RenderComponent *render = G_GetComponent(&entity, RENDER_COMPONENT);

		if (render != NULL) {
			return render->tile;
		}
	}

	return NOTHING;
}

/*
*/
