#ifndef __TILES__
#define __TILES__

#include <SDL/SDL.h>

/*
** enums
*/

enum TileCategories {
	EMPTY_TILE = 0,
	SOLID_TILE,
	LIQUID_TILE,
	GROUND_TILE,

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
	EMPTY = 0,
		CHASM,

	GROUND,
		MUD,
		DIRT,
		SOIL,
		MOSS,
		SNOW,
			LIGHT_SNOW,
			HEAVY_SNOW,
		ROCK,
			MOSSY_ROCK,
		GRASS,
			DEAD_GRASS,
		STONE,
			MOSSY_STONE,
			CRACKED_STONE,
		FUNGUS,
			BIO_FUNGUS,
		GRAVEL,
		WOOD,
			ROTTING_WOOD,
			POLISHED_WOOD,
		TILE,
			SMOOTH_TILE,
			CRACKED_TILE,
		CARPET,
			TORN_CARPET,
		MARBLE,
			CRACKED_MARBLE,
			POLISHED_MARBLE,
		GRANITE,
			CRACKED_GRANITE,
			POLISHED_GRANITE,

	TILE_TYPE_COUNT
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
		effect;
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
