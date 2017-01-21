#ifndef __ENTITY__
#define __ENTITY__

#include <stdlib.h>

#include "components.h"

/*
** typedef
*/

typedef struct entity {
	unsigned int id;
	void
		*components[COMPONENT_TYPE_COUNT],
		*prev,
		*next;
} entity;

/*
** functions
*/

entity* createEntity(unsigned int id);
void addComponent(entity *target, int component_type, void *component);
void removeComponent(entity *target, int component_type);
void* getComponent(entity *target, int component_type);

/*
*/

#endif /* __ENTITY__ */
