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
