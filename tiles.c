#include "yendor.h"
#include "globals.h"

/*
** EXTERNS
*/

G_TileDescriptor descriptor_tiles[TILE_COUNT] = {
	{"empty", "description", 0, 0, &black, &black, 0, EMPTY},
	{"ground", "the ground", 14, 2, &grey, &dark_blue, 0, GROUND},
	{"water", "some water", 14, 7, &blue, &scott_blue, FLICKERS, LIQUID},
	{"wall", "a wall", 3, 2, &brown, &grey, OBSTRUCTS, SOLID},

	{"character", "", 1, 0, &white, &black, 0, ENTITY},

	{"", "", 0, 0, &aqua, &aqua, 0, COLOR},
	{"", "", 0, 0, &black, &black, 0, COLOR},
	{"", "", 0, 0, &blue, &blue, 0, COLOR},
	{"", "", 0, 0, &green, &green, 0, COLOR},
	{"", "", 0, 0, &magenta, &magenta, 0, COLOR},
	{"", "", 0, 0, &red, &red, 0, COLOR},
	{"", "", 0, 0, &white, &white, 0, COLOR},
	{"", "", 0, 0, &yellow, &yellow, 0, COLOR},
};

const char* G_TileName(Tile tile) {
	assert((tile >= NOTHING) && (tile < END_TILE));

	return (descriptor_tiles[tile-NOTHING].name);
}

const char* G_TileDescription(Tile tile) {
	assert((tile >= NOTHING) && (tile < END_TILE));

	return (descriptor_tiles[tile-NOTHING].description);
}

G_Color G_TileForeground(Tile tile) {
	assert((tile >= NOTHING) && (tile < END_TILE));

	return *(descriptor_tiles[tile-NOTHING].fg);
}

G_Color G_TileBackground(Tile tile) {
	assert((tile >= NOTHING) && (tile < END_TILE));

	return *(descriptor_tiles[tile-NOTHING].bg);
}

TileFlag G_TileFlags(Tile tile) {
	assert((tile >= NOTHING) && (tile < END_TILE));

	return (descriptor_tiles[tile-NOTHING].flags);
}	

boolean G_TileSolid(Tile tile) {
	assert((tile >= NOTHING) && (tile < END_TILE));

	return (descriptor_tiles[tile-NOTHING].base == SOLID);
}

boolean G_TileFlickers(Tile tile) {
	if ((tile < NOTHING) || (tile >= END_TILE)) {
		return 0;
	}

	return ((descriptor_tiles[tile-NOTHING].flags & FLICKERS) == FLICKERS);
}

boolean G_TileObstructs(Tile tile) {
	assert((tile >= NOTHING) && (tile < END_TILE));

	return ((descriptor_tiles[tile-NOTHING].flags & OBSTRUCTS) == OBSTRUCTS);
}

void G_TileSource(Tile tile, SDL_Rect *source) {
	assert((tile >= 0) && (tile < END_TILE) && (source != NULL));

	if (tile < 256) {
		source->x = (tile%16)*TILE_SOURCE_WIDTH;
		source->y = (tile/16)*TILE_SOURCE_HEIGHT;
	} else {
		source->x = descriptor_tiles[tile-NOTHING].x*TILE_SOURCE_WIDTH;
		source->y = descriptor_tiles[tile-NOTHING].y*TILE_SOURCE_HEIGHT;
	}
}

/*
*/
