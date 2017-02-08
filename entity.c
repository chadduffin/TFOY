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
	render_component *value = (render_component*)(getComponent(target, RENDER_COMPONENT));

	if (value != NULL) {
		if ((value->x > view.x) && (value->x < view.x+view.w) &&
				(value->y > view.y) && (value->y < view.y+view.h)) {
			dmatrix[DCOLS_OFFSET+value->x-view.x][DROWS_OFFSET+value->y-view.y].changed = 1;
			dmatrix[DCOLS_OFFSET+value->x-view.x][DROWS_OFFSET+value->y-view.y].entity = value->tile;
		}
		if (checkBoundKey(RIGHT)) {
			value->x += 1;
			phys_keys[virt_keys[RIGHT]] = 0;
		}
		if (checkBoundKey(LEFT)) {
			value->x -= 1;
			phys_keys[virt_keys[LEFT]] = 0;
		}
		if (checkBoundKey(UP)) {
			value->y -= 1;
			phys_keys[virt_keys[UP]] = 0;
		}
		if (checkBoundKey(DOWN)) {
			value->y += 1;
			phys_keys[virt_keys[DOWN]] = 0;
		}
	}
}

/*
*/
