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
	menu.entity_count = 0;

	menu.tiles = (ctile**)malloc(sizeof(ctile*)*menu.w);
	menu.head = NULL;
	menu.tail = NULL;

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
	overworld.entity_count = 0;

	overworld.tiles = (ctile**)malloc(sizeof(ctile*)*overworld.w);
	overworld.head = NULL;
	overworld.tail = NULL;

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

void cleanupScene(scene *target) {
	entity *head = target->head;
	
	while (head != NULL) {
		target->head = (entity*)(head->next);
		free(head);
		head = target->head;
	}

	free(target->tiles);
}

void addEntity(scene *dest, entity *target) {

	if (dest->head == NULL) {
		dest->head = target;
		dest->tail = target;
	} else {
		target->prev = (void*)(dest->tail);
		dest->tail->next = (void*)target;
		dest->tail = target;
	}

	dest->entity_count += 1;
}

void delEntity(scene *dest, entity *target) {
	if (target != NULL) {
		if ((target != dest->head) && (target != dest->tail)) {
			if (target->next != NULL) {
				((entity*)(target->next))->prev = target->prev;
			}
			if (target->prev != NULL) {
				((entity*)(target->prev))->next = target->next;
			}
			free(target);
		} else {
			dest->head = (target == dest->head) ? (entity*)(dest->head->next) : dest->head;
			dest->tail = (target == dest->tail) ? (entity*)(dest->tail->prev) : dest->tail;

			if (target->next != NULL) {
				((entity*)(target->next))->prev = NULL;
			}
			if (target->prev != NULL) {
				((entity*)(target->prev))->next = NULL;
			}

			free(target);
		}
	}
}

entity* getEntities(scene *source) {
	return (source == NULL) ? NULL : source->head;
}

/*
*/
