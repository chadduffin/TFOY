#include "yendor.h"
#include "globals.h"

/*
** EXTERNS
*/

G_TileDescriptor descriptor_tiles[TILE_COUNT] = {
	{"empty", "description", 0, 0, &black, &black, 0, EMPTY},
	{"ground", "the ground", 14, 2, &grey, &dark_blue, 0, GROUND},
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
	tile -= NOTHING;
	return (descriptor_tiles[tile].name);
}

const char* G_TileDescription(Tile tile) {
	tile -= NOTHING;
	return (descriptor_tiles[tile].description);
}

G_Color G_TileForeground(Tile tile) {
	tile -= NOTHING;
	return *(descriptor_tiles[tile].fg);
}

G_Color G_TileBackground(Tile tile) {
	tile -= NOTHING;
	return *(descriptor_tiles[tile].bg);
}

TileFlag G_TileFlags(Tile tile) {
	tile -= NOTHING;
	return (descriptor_tiles[tile].flags);
}	

boolean G_TileSolid(Tile tile) {
	tile -= NOTHING;
	return (descriptor_tiles[tile].base == SOLID);
}

boolean G_TileObstructs(Tile tile) {
	return ((descriptor_tiles[tile-NOTHING].flags & OBSTRUCTS) == OBSTRUCTS);
}

void G_TileSource(Tile tile, SDL_Rect *source) {
	if (tile < 256) {
		source->x = (tile%16)*TILE_SOURCE_WIDTH;
		source->y = (tile/16)*TILE_SOURCE_HEIGHT;
	} else {
		tile -= 256;
		source->x = descriptor_tiles[tile].x*TILE_SOURCE_WIDTH;
		source->y = descriptor_tiles[tile].y*TILE_SOURCE_HEIGHT;
	}
}

/*
*/
