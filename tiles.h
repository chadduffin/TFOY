#ifndef __TILES__
#define __TILES__

#include "colors.h"

/*
** enums
*/

enum TileCategories {
	EMPTY_TILE = 0,
	SOLID_TILE,
	LIQUID_TILE,
	GROUND_TILE,
	COLOR_TILE,

	TILE_CATEGORY_COUNT
};

enum TileFlags {
	// if the tile PROPOGATES fire, it is flammable.
	FLAMMABLE = 1,
	// if the tile CHANGES STATE when frozen, it is freezable.
	FREEZABLE = 2,
	// if the tile IS A WALL and CAN DISAPPEAR, it is breakable.
	BREAKABLE = 4,
};

enum TileEffects {
	IS_BURNT = 1,
	IS_FROZEN = 2,
	IS_MELTED = 4,
	IS_CHARRED = 8,
	IS_FROSTED = 16,
	IS_STEAMING = 32,
	IS_EXTINGUISED = 64,
	IS_EXPLOSIVE_GAS = 128,
	IS_POISONOUS_GAS = 256,
};

enum TileIndexs {
	EMPTY = 256,

	GROUND,

	WALL,

	SOLID_BLACK,
	SOLID_WHITE,
	SOLID_RED,
	SOLID_GREEN,
	SOLID_BLUE,
	SOLID_YELLOW,
	SOLID_MAGENTA,
	SOLID_AQUA,

	END_TILE,
	TILE_TYPE_COUNT = (END_TILE-EMPTY)
};

/*
** typedefs
*/

// descriptor tile
typedef struct dtile {
	char
		*name,
		*description;
	short
		base,
		flags,
		effect,
	
		// x & y of tile display source
		x,
		y;
	const color
		fg,
		bg;
} dtile;

// compressed tile
typedef struct ctile {
	unsigned char
		effect,
		duration;
	short tile;
} ctile;

/*
** externs
*/

extern dtile descriptor_tiles[TILE_TYPE_COUNT];

/*
*/

#endif /* __TILES__ */
