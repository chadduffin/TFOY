#include "yendor.h"
#include "globals.h"

/*
** FUNCTIONS
*/

G_Entity* G_CreateEntity(EntityType type) {
	int i;
	G_Entity *entity = (G_Entity*)malloc(sizeof(G_Entity));

	entity->id = G_GetID();
	entity->prev = NULL;
	entity->next = NULL;
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
		case BUTTON_COMPONENT:
			{
				G_ButtonComponent *button = (G_ButtonComponent*)malloc(sizeof(G_ButtonComponent));
				button->hotkey = '\0';
				button->name = NULL;
				button->x = 0;
				button->y = 0;
				button->l = 0;
				button->data = NULL;
				button->func = NULL;
				button->state = ACTIVE | CHANGED;
				button->border = 1;
				(*entity)->components[BUTTON_COMPONENT] = button;
				return button;
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

void G_EntityMov(G_Entity **entity, G_Scene **src, G_Scene **dst) {
	assert((entity != NULL) && (*entity != NULL) && (src != NULL) && (*src != NULL) && (dst != NULL) && (*dst != NULL));

	G_PopEntity(src, entity);
	G_AddEntity(dst, entity);
}

void G_EntityUpdate(G_Entity **entity) {
	assert((entity != NULL) && (*entity != NULL));
	
	G_RenderComponent *render = (G_RenderComponent*)(G_GetComponent(entity, RENDER_COMPONENT));
	G_ButtonComponent *button = (G_ButtonComponent*)(G_GetComponent(entity, BUTTON_COMPONENT));
	G_ControllerComponent *controller = (G_ControllerComponent*)(G_GetComponent(entity, CONTROLLER_COMPONENT));

	if (render != NULL) {
		render->x_previous = render->x;
		render->y_previous = render->y;
		
		if (controller != NULL) {
			if (G_CheckBound(RIGHT)) {
				render->x += 1;
				phys_keys[virt_keys[RIGHT]] = 0;
				location->view.unchanged = 0;
			}
			if (G_CheckBound(LEFT)) {
				render->x -= 1;
				phys_keys[virt_keys[LEFT]] = 0;
				location->view.unchanged = 0;
			}
			if (G_CheckBound(UP)) {
				render->y -= 1;
				phys_keys[virt_keys[UP]] = 0;
				location->view.unchanged = 0;
			}
			if (G_CheckBound(DOWN)) {
				render->y += 1;
				phys_keys[virt_keys[DOWN]] = 0;
				location->view.unchanged = 0;
			}
		}

		if (render->tile != NOTHING) {
			G_View *view = G_SceneView(&location);

			if ((render->x == render->x_previous) && (render->y == render->y_previous)) {
				int
					rx = render->x_previous+(view->x-view->xp),
					ry = render->y_previous+(view->y-view->yp);
		
				if (G_IsPointWithin(rx, ry, view)) {
					dmatrix[rx-view->x+DCOLS_OFFSET][ry-view->y+DROWS_OFFSET].changed = 1;
					dmatrix[rx-view->x+DCOLS_OFFSET][ry-view->y+DROWS_OFFSET].entity = NOTHING;
				}
			} else {
				if (G_IsPointWithin(render->x_previous, render->y_previous, view)) {
					dmatrix[render->x_previous-view->x+DCOLS_OFFSET][render->y_previous-view->y+DROWS_OFFSET].changed = 1;
					dmatrix[render->x_previous-view->x+DCOLS_OFFSET][render->y_previous-view->y+DROWS_OFFSET].entity = NOTHING;
				}
			}
		}
	}

	if (button != NULL) {
		int
			mouse_x = (game_info.mouse_x-game_info.display_x)/game_info.tile_w,
			mouse_y = (game_info.mouse_y-game_info.display_y)/game_info.tile_h;

		ButtonState state = button->state;

		if ((mouse_x < button->x-button->border) || (mouse_x >= button->x+button->l+button->border) ||
				(mouse_y < button->y-button->border) || (mouse_y >= button->y+button->border)) {
			if ((button->state & PRESSED) == PRESSED) {
				button->state = button->state ^ PRESSED;
			}
			if ((button->state & HOVER) == HOVER) {
				button->state = button->state ^ HOVER;
			}
		} else {
			if (game_info.mouse_lb > 0) {
				button->state = button->state | PRESSED;
			} else if ((button->state & PRESSED) == PRESSED) {
				button->func(button->data);
			}

			button->state = button->state | HOVER;
		}

		if (button->state != state) {
			button->state = button->state | CHANGED;
		}
	}
}
	
void G_EntityRender(G_Entity **entity) {
	assert((entity != NULL) && (*entity != NULL));

	G_View *view = G_SceneView(&location);
	G_LightComponent *light = (G_LightComponent*)(G_GetComponent(entity, LIGHT_COMPONENT));
	G_RenderComponent *render = (G_RenderComponent*)(G_GetComponent(entity, RENDER_COMPONENT));
	G_ButtonComponent *button = (G_ButtonComponent*)(G_GetComponent(entity, BUTTON_COMPONENT));

	if (render != NULL) {
		if ((render->tile != NOTHING) && (G_IsPointWithin(render->x, render->y, view))) {
				dmatrix[render->x-view->x+DCOLS_OFFSET][render->y-view->y+DROWS_OFFSET].changed = 1;
				dmatrix[render->x-view->x+DCOLS_OFFSET][render->y-view->y+DROWS_OFFSET].entity = render->tile;
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

	if (button != NULL) {
		if ((button->state & CHANGED) == CHANGED) {
			int
				x,
				y;
			button->state = button->state ^ CHANGED;

			for (y = -(button->border); y <= (button->border); y += 1) {
				for (x = -(button->border); x < (button->l+button->border); x += 1) {
					if ((x == -1) || (x == button->l) ||
							(y == -1) || (y == button->border)) {
						dmatrix[x+button->x][y+button->y].changed = 1;
						dmatrix[x+button->x][y+button->y].tile = BLACK;
					} else {
						dmatrix[x+button->x][y+button->y].changed = 1;
						dmatrix[x+button->x][y+button->y].tile = button->name[x];
					}
				}
			}
		}
	}
}

EntityType G_GetEntityType(G_Entity **entity) {
	assert((entity != NULL) && (*entity != NULL));
	
	return (*entity)->type;
}
	
/*
*/
