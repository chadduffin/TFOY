#ifndef __TILES__
#define __TILES__

/*
** defines
*/

#define BASE_MASK 0xC0000000
#define TYPE_MASK 0x3FFD0000
#define PERM_MASK 0x0001F000
#define TEMP_MASK 0x00000F80
#define TIME_MASK 0x0000007F

/*
** enums
*/

enum Directions {
	RIGHT = 0,
	UP,
	LEFT,
	DOWN,

	UP_RIGHT,
	UP_LEFT,
	DOWN_RIGHT,
	DOWN_LEFT,

	NO_DIR,

	DIRECTION_COUNT
};

enum TileCategory {
	NOTHING = 0,
	LIQUID,
	GROUND,
	WALL,

	TILE_CATEGORY_COUNT
};

enum LiquidTileTypes {
	L_WATER = 0,
	L_WATER_SWAMP,
	L_WATER_OCEAN,

	L_LAVA,
	L_MAGMA,

	LIQUID_TYPE_COUNT
};

enum GroundTileTypes {
	G_DIRT = 0,
	G_GRASS_SHORT,
	G_GRASS_MEDIUM,
	G_GRASS_TALL,
	
	GROUND_TILE_COUNT
};

enum WallTileTypes {
	W_STONE = 0,

	WALL_TILE_COUNT
};

/*
** typedefs
*/

typedef struct tile {
	unsigned int
		base,
		type,
		perm,
		temp,
		duration;
} tile;

/*
** functions
*/

tile* getTile(unsigned int value);
unsigned int makeTile(
	unsigned int base,
	unsigned int type,
	unsigned int perm,
	unsigned int temp,
	unsigned int duration);

/*
*/

#endif /* __TILES__ */
