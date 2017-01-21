#ifndef __SCENE__
#define __SCENE__

#include "tiles.h"
#include "entity.h"
#include "globals.h"

/*
** typedefs
*/

typedef struct scene {
	short
		w,
		h,
		entity_count;
	ctile **tiles;
	entity
		*head,
		*tail;
} scene;

/*
** externs
*/

extern scene menu;
extern scene overworld;

extern scene *location;

/*
** functions
*/

void initializeMenu();
void initializeOverworld();
void cleanupScene(scene *target);
void addEntity(scene *dest, entity *target);
void delEntity(scene *dest, entity *target);
entity* getEntities(scene *source);

/*
*/

#endif /* __SCENE__ */
