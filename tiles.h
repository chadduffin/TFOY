#ifndef __TILES__
#define __TILES__

/*
** defines
*/

#define BASE_MASK 3221225472
#define TYPE_MASK 1073217536
#define PERM_MASK 516096
#define TEMP_MASK 8064
#define TIME_MASK 127

#define G_FLAM_MASK 536870912
#define G_TYPE_MASK 536346624

#define L_DPTH_MASK 805306368
#define L_DIRS_MASK 234881024
#define L_TYPE_MASK 33030144

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
		depth,
		dir,
		flam,
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
	unsigned int depth,
	unsigned int dir,
	unsigned int flam,
	unsigned int perm,
	unsigned int temp,
	unsigned int duration);

/*
*/

#endif /* __TILES__ */
