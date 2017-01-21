#include "entity.h"

entity* createEntity() {
	int i;
	entity *instance = (entity*)malloc(sizeof(entity));

	for (i = 0; i < COMPONENT_TYPE_COUNT; i += 1) {
		instance->components[i] = NULL;
	}

	return instance;
}

void addComponent(entity *target, int component_type, void *component) {
	if (target->components[component_type] == NULL) {
		target->components[component_type] = component;
	}
}

void removeComponent(entity *target, int component_type) {
	if (target->components[component_type] != NULL) {
		free(target->components[component_type]);
		target->components[component_type] = NULL;
	}
}

void* getComponent(entity *target, int component_type) {
	return target->components[component_type];
}
