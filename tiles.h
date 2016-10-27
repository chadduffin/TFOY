#ifndef __TILES__
#define __TILES__

/*
** defines
*/

#define BASE_MASK 0xC0000000
#define TYPE_MASK 0x3D000000
#define ORNM_MASK 0x01FD0000
#define PERM_MASK 0x0001F000
#define TEMP_MASK 0x00000F80
#define TIME_MASK 0x0000007F

/*
** enums
*/

enum TileCategory {
	NOTHING = 0,
	LIQUID,
	GROUND,
	WALL,

	TILE_CATEGORY_COUNT
};

enum LiquidTileTypes {
	L_WATER_SHALLOW = 0,
	L_WATER_MEDIUM,
	L_WATER_DEEP,

	L_SWAMP_SHALLOW,
	L_SWAMP_MEDIUM,
	L_SWAMP_DEEP,

	L_OCEAN_SHALLOW,
	L_OCEAN_MEDIUM,
	L_OCEAN_DEEP,

	L_LAVA,

	LIQUID_TYPE_COUNT
};

enum LiquidOrnamentTypes {
	L_LILYPAD = 0,

	L_ORNAMENT_COUNT
};

enum GroundTileTypes {
	G_DIRT = 0,
	G_ROCK,
	G_SAND,
	G_GRAVEL,
	G_OBSIDIAN,
	G_SANDSTONE,

	G_TILE,
	G_WOOD,
	G_STONE,
	G_MARBLE,
	G_BRICKS,

	GROUND_TILE_COUNT
};

enum GroundOrnamentTypes {
	G_GRASS,
	G_SHRUB,
	G_FLOWERS,
	G_FOLIAGE,
	G_SAPLING,

	G_MOSS,
	G_FUNGUS,
	G_FUNGUS_BIO,

	G_RUBBLE,

	G_SKULL,
	G_BONES,

	G_ICE,
	G_SNOW,
	G_PUDDLE,

	G_ORNAMENT_COUNT
};

enum WallTileTypes {
	W_STONE = 0,

	WALL_TILE_COUNT
};

enum WallOrnamentTypes {
	W_TORCH = 0,

	WALL_ORNAMENT_COUNT
};

enum TileEffectTypes {
	E_NO_EFFECT = 0,

	E_NOTHING,
	E_LIQUID,
	E_GROUND,
	E_WALL,

	E_BURNT,
	E_BURNING,
	
	E_HIDDEN,

	E_EFFECT_COUNT
};

/*
** typedefs
*/

typedef struct tile {
	unsigned int
		base,
		type,
		ornm,
		perm,
		temp,
		time;
} tile;

/*
** functions
*/

tile* getTile(unsigned int value);
unsigned int makeTile(
	unsigned int base,
	unsigned int type,
	unsigned int ornm,
	unsigned int perm,
	unsigned int temp,
	unsigned int time);

/*
*/

#endif /* __TILES__ */
