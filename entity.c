#include "yendor.h"
#include "globals.h"

entity* createEntity(unsigned int id) {
	int i;
	entity *instance = (entity*)malloc(sizeof(entity));

	instance->id = id;
	instance->prev = NULL;
	instance->next = NULL;

	for (i = 0; i < COMPONENT_TYPE_COUNT; i += 1) {
		instance->components[i] = NULL;
	}

	return instance;
}

void* addComponent(entity *target, int component_type) {
	switch (component_type) {
		case CREATURE_COMPONENT:
			{
				creature_component *value = (creature_component*)malloc(sizeof(creature_component));
				target->components[CREATURE_COMPONENT] = value;
				return value;
			}
			break;
		case RENDER_COMPONENT:
			{
				render_component *value = (render_component*)malloc(sizeof(render_component));
				value->x = 0;
				value->y = 0;
				value->z = 0;
				value->x_previous = -1;
				value->y_previous = -1;
				value->x_drawn = -1;
				value->y_drawn = -1;
				value->tile = NOTHING;
				target->components[RENDER_COMPONENT] = value;
				return value;
			}
			break;
		case LIGHT_COMPONENT:
			{
				light_component *value = (light_component*)malloc(sizeof(light_component));
				value->light_value.intensity = 0;
				value->light_value.value = black;
				target->components[LIGHT_COMPONENT] = value;
				return value;
			}
			break;
		default:
			printf("Invalid component type.\n");
			return NULL;
	}
}

void* getComponent(entity *target, int component_type) {
	return target->components[component_type];
}

void removeComponent(entity *target, int component_type) {
	if (target->components[component_type] != NULL) {
		free(target->components[component_type]);
		target->components[component_type] = NULL;
	}
}

void entityUpdate(entity *target) {
	// render component
	render_component *render_comp = (render_component*)(getComponent(target, RENDER_COMPONENT));

	if (render_comp != NULL) {
		if ((render_comp->x_drawn != -1) && (render_comp->y_drawn != -1)) {
			render_comp->x_drawn += view_previous.x-view.x;
			render_comp->y_drawn += view_previous.y-view.y;
			if ((render_comp->x_drawn >= DCOLS_OFFSET) && (render_comp->x_drawn < DCOLS_OFFSET+DCOLS) && (render_comp->y_drawn >= DROWS_OFFSET) && (render_comp->y_drawn < DROWS_OFFSET+DROWS)) {
				dmatrix[render_comp->x_drawn][render_comp->y_drawn].changed = 1;
				dmatrix[render_comp->x_drawn][render_comp->y_drawn].entity = NOTHING;
			}
		}

		if (isPointWithin(render_comp->x, render_comp->y, &view)) {
			render_comp->x_drawn = DCOLS_OFFSET+render_comp->x-view.x;
			render_comp->y_drawn = DROWS_OFFSET+render_comp->y-view.y;
			dmatrix[render_comp->x_drawn][render_comp->y_drawn].changed = 1;
			dmatrix[render_comp->x_drawn][render_comp->y_drawn].entity = render_comp->tile;
		} else {
			render_comp->x_drawn = -1;
			render_comp->y_drawn = -1;
		}

		render_comp->x_previous = render_comp->x;
		render_comp->y_previous = render_comp->y;

		light_component *light_comp = (light_component*)(getComponent(target, LIGHT_COMPONENT));
		if (light_comp != NULL) {
			addLight(render_comp->x, render_comp->y, light_comp->light_value);
		}
	}
}

/*
*/
