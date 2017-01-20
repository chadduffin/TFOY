#include "scene.h"

/*
** externs
*/

scene menu;
scene overworld;

scene *location = &menu;

/*
** functions
*/

void initializeMenu() {
	int
		x,
		y;
	menu.w = COLS;
	menu.h = ROWS;

	menu.tiles = (ctile**)malloc(sizeof(ctile*)*menu.w);

	for (x = 0; x < menu.w; x += 1) {
		menu.tiles[x] = (ctile*)malloc(sizeof(ctile)*menu.h);
	}

	for (y = 0; y < menu.h; y += 1) {
		for (x = 0; x < menu.w; x += 1) {
			if (title[y][x] == 'B') {
				menu.tiles[x][y].tile = SOLID_BLACK;
			} else if ((title[y][x] != 'B') && (title[y][x] != ' ')) {
				menu.tiles[x][y].tile = title[y][x];
			} else if ((y > 0) && (title[y-1][x] == 'B')) {
				menu.tiles[x][y].tile = SOLID_WHITE;
			} else {
				menu.tiles[x][y].tile = SOLID_MAGENTA;
			}
		}
	}
}

void initializeOverworld() {
	int
		x,
		y;
	overworld.w = WORLD_COLS;
	overworld.h = WORLD_ROWS;

	overworld.tiles = (ctile**)malloc(sizeof(ctile*)*overworld.w);

	for (x = 0; x < overworld.w; x += 1) {
		overworld.tiles[x] = (ctile*)malloc(sizeof(ctile)*overworld.h);
	}

	for (y = 0; y < overworld.h; y += 1) {
		for (x = 0; x < overworld.w; x += 1) {
			if ((x == 0) || (y == 0) || (x == overworld.w-1) || (y == overworld.h-1)) {
				overworld.tiles[x][y].tile = WALL;
			} else {
				overworld.tiles[x][y].tile = GROUND;
			}
		}
	}
}

void cleanupMenu() {
	free(menu.tiles);
}

void cleanupOverworld() {
	free(overworld.tiles);
}

/*
*/
