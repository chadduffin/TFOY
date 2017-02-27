#include "yendor.h"
#include "globals.h"

/*
** FUNCTIONS
*/

G_Entity* G_CreateEntity(void) {
	int i;
	G_Entity *entity = (G_Entity*)malloc(sizeof(G_Entity));

	entity->id = G_GetID();
	entity->prev = NULL;
	entity->next = NULL;

	for (i = 0; i < COMPONENT_COUNT; i += 1) {
		entity->components[i] = NULL;
	}

	return entity;
}

void* G_AddComponent(G_Entity **entity, Component component) {
	switch (component) {
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
		default:
			printf("Invalid component type.\n");
			return NULL;
	}
}

void* G_GetComponent(G_Entity **entity, Component component) {
	if ((entity != NULL) && (*entity != NULL)) {
		return (*entity)->components[component];
	}

	return NULL;
}

void G_RemoveComponent(G_Entity **entity, Component component) {
	if ((*entity)->components[component] != NULL) {
		free((*entity)->components[component]);
		(*entity)->components[component] = NULL;
	}
}

void G_EntityPos(G_Entity **entity, int *x, int *y) {
	if ((entity != NULL) && (*entity != NULL)) {
		G_RenderComponent *render = G_GetComponent(entity, RENDER_COMPONENT);

		if (render != NULL) {
			*x = render->x;
			*y = render->y;
		}
	}
}

void G_EntityMov(G_Entity **entity, G_Scene **src, G_Scene **dst) {
	if ((entity != NULL) && (*entity != NULL) &&
			(src != NULL) && (*src != NULL) && 
			(dst != NULL) && (*dst != NULL)) {
		G_PopEntity(src, entity);
		G_AddEntity(dst, entity);
	}
}

void G_EntityUpdate(G_Entity **entity) {
	if ((entity != NULL) && (*entity != NULL)) {
		G_RenderComponent *render = (G_RenderComponent*)(G_GetComponent(entity, RENDER_COMPONENT));

		if (render != NULL) {
			render->x_previous = render->x;
			render->y_previous = render->y;

			if ((*entity)->id == 0) {
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
		}
	}
}
	
void G_EntityRender(G_Entity **entity) {
	if ((entity != NULL) && (*entity != NULL)) {
		G_View *view = G_SceneView(&location);
		G_LightComponent *light = (G_LightComponent*)(G_GetComponent(entity, LIGHT_COMPONENT));
		G_RenderComponent *render = (G_RenderComponent*)(G_GetComponent(entity, RENDER_COMPONENT));
	
		if (render != NULL) {
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
	
			if (G_IsPointWithin(render->x, render->y, view)) {
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
	}
}
	
/*
*/
