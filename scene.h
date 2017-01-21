#ifndef __SCENE__
#define __SCENE__

#include "tiles.h"
#include "entity.h"
#include "globals.h"

/*
** defines
*/

#define MAX_ENTITY_COUNT 1

/*
** typedefs
*/

typedef struct scene {
	short
		w,
		h;
	ctile **tiles;
	entity *entities[MAX_ENTITY_COUNT];
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
void cleanupMenu();
void cleanupOverworld();
void addEntity(scene *dest, entity *target);

/*
*/

#endif /* __SCENE__ */
