#include "yendor.h"
#include "globals.h"

/*
** externs
*/

dtile descriptor_tiles[TILE_TYPE_COUNT] = {
	{"empty", "description", EMPTY, 0, 0, 0, &black, &black},
	{"ground", "the ground", GROUND, 0, 14, 2, &grey, &dark_blue},
	{"wall", "a wall", SOLID, 0, 3, 2, &brown, &grey},

	{"character", "", ENTITY, 0, 1, 0, &white, &black},

	{"", "", COLOR, 0, 0, 0, &aqua, &aqua},
	{"", "", COLOR, 0, 0, 0, &black, &black},
	{"", "", COLOR, 0, 0, 0, &blue, &blue},
	{"", "", COLOR, 0, 0, 0, &green, &green},
	{"", "", COLOR, 0, 0, 0, &magenta, &magenta},
	{"", "", COLOR, 0, 0, 0, &red, &red},
	{"", "", COLOR, 0, 0, 0, &white, &white},
	{"", "", COLOR, 0, 0, 0, &yellow, &yellow},
};

const dtile* lookupTile(unsigned int value) {
	return &descriptor_tiles[value-NOTHING];
}

void lookupTileSource(SDL_Rect *source, unsigned int value) {
	if (value < 256) {
		source->x = (value%16)*TILE_SOURCE_WIDTH;
		source->y = (value/16)*TILE_SOURCE_HEIGHT;
	} else {
		value -= 256;
		source->x = descriptor_tiles[value].x*TILE_SOURCE_WIDTH;
		source->y = descriptor_tiles[value].y*TILE_SOURCE_HEIGHT;
	}
}

/*
*/
