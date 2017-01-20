#ifndef __SCENE__
#define __SCENE__

#include "tiles.h"
#include "globals.h"

/*
** typedefs
*/

typedef struct scene {
	short
		w,
		h;
	ctile **tiles;
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

/*
*/

#endif /* __SCENE__ */
