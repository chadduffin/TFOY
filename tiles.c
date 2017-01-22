#include "yendor.h"
#include "globals.h"

/*
** externs
*/

dtile descriptor_tiles[TILE_TYPE_COUNT] = {
	{"empty", "description", EMPTY_TILE, 0, 0, 0, 0, BLACK, BLACK},
	{"ground", "the ground", GROUND_TILE, 0, 0, 14, 2, GREY, DARK_BLUE},
	{"wall", "a wall", SOLID_TILE, 0, 0, 3, 2, BROWN, GREY},

	{"", "", COLOR_TILE, 0, 0, 0, 0, BLACK, BLACK},
	{"", "", COLOR_TILE, 0, 0, 0, 0, WHITE, WHITE},
	{"", "", COLOR_TILE, 0, 0, 0, 0, RED, RED},
	{"", "", COLOR_TILE, 0, 0, 0, 0, GREEN, GREEN},
	{"", "", COLOR_TILE, 0, 0, 0, 0, BLUE, BLUE},
	{"", "", COLOR_TILE, 0, 0, 0, 0, YELLOW, YELLOW},
	{"", "", COLOR_TILE, 0, 0, 0, 0, MAGENTA, MAGENTA},
	{"", "", COLOR_TILE, 0, 0, 0, 0, AQUA, AQUA},
};

/*
*/
