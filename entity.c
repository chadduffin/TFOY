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
				value->x_previous = 0;
				value->y_previous = 0;
				value->tile = NOTHING;
				target->components[RENDER_COMPONENT] = value;
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
		if (isPointWithin(render_comp->x, render_comp->y, &view)) {
			dmatrix[DCOLS_OFFSET+render_comp->x_previous-view.x][DROWS_OFFSET+render_comp->y_previous-view.y].changed = 1;
			dmatrix[DCOLS_OFFSET+render_comp->x_previous-view.x][DROWS_OFFSET+render_comp->y_previous-view.y].entity = NOTHING;
			dmatrix[DCOLS_OFFSET+render_comp->x-view.x][DROWS_OFFSET+render_comp->y-view.y].changed = 1;
			dmatrix[DCOLS_OFFSET+render_comp->x-view.x][DROWS_OFFSET+render_comp->y-view.y].entity = render_comp->tile;
		}

		render_comp->x_previous = render_comp->x;
		render_comp->y_previous = render_comp->y;
	}
}

/*
*/
