#ifndef __GLOBALS__
#define __GLOBALS__

#include <SDL2/SDL.h>
#include <SDL2/SDL_net.h>
#include <SDL2/SDL_image.h>

/*
** defines
*/

#define TITLE "The Fellowships Of Yendor"
#define VERSION "v0.0.1"

#define FULLSCREEN 0
#define DEBUGGING 0
#define ONLINE 0

#define FPS 60
#define MAX_BUFFER 1024
#define TEXTURE_COUNT 1

#define NOT_OK 0
#define SDL_OK 1
#define WINDOW_OK 2
#define RENDER_OK 4
#define GRPHCS_OK 8
#define NETWRK_OK 16

#define TILE_SOURCE_WIDTH 16
#define TILE_SOURCE_HEIGHT 24

#define COLS 108
#define ROWS 42
#define DCOLS (COLS - STAT_BAR_WIDTH - 2)
#define DROWS (ROWS - MESSAGE_ROWS - 2)

#define MESSAGE_ROWS 8
#define STAT_BAR_WIDTH 20

#define MIN_DUNGEON_DEPTH 1
#define MAX_DUNGEON_DEPTH 64

#define WORLD_COLS 10240
#define WORLD_ROWS 8192

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

#define LEVEL_CAP 16
#define EXPERIENCE_OFFSET 42

/*
** typedefs
*/

typedef struct color {
	short rgb[3];
} color;

typedef struct lightSource {
	short flickering,
		intensity,
		range,
		light[3];
} lightSource;

typedef struct lightValue {
	short intensity,
		light[3];
} lightValue;

typedef struct pcell {
	unsigned int tile;
} pcell;

typedef struct dcell {
	short changed;
	pcell tile;
	lightValue value;
	lightSource source;
} dcell;

// struct tile offers an easier way to access attributes.
typedef struct tile {
	unsigned int base,
		type,
		depth,
		dir,
		flam,
		perm,
		temp,
		duration;
} tile;

/*
** externs
*/

extern char *name;
extern char *images[];
extern char *server_name;
extern char buffer[MAX_BUFFER];
extern int image_count;
extern int port;
extern int tile_width;
extern int tile_height;
extern int window_width;
extern int window_height;

extern SDL_Rect dport;
extern SDL_Event event;
extern SDL_Window *window;
extern SDL_Renderer *renderer;
extern SDL_Texture *textures[TEXTURE_COUNT];

extern IPaddress ipaddress;
extern TCPsocket socket;


/*
** enumerations
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

enum WeaponTypes {
	AXE,
	DAGGER,
	MACE,
	SPEAR,
	SWORD,
	WAND,

	BROADSWORD,
	POLEARM,
	STAFF,
	WARAXE,
	WARHAMMER,

	BOW,
	CROSSBOW,
	MUSKET,

	SHIELD,
	OFFHAND,

	WEAPON_TYPE_COUNT
};

enum ArmourTypes {
	CLOTH,
	LEATHER,
	SCALE,
	CHAIN,
	SPLINT,
	PLATE,

	AMULET,
	RING_ONE,
	RING_TWO,
	TRINKET,

	ARMOR_TYPE_COUNT
};

enum EquipmentSlots {
	HEAD,
	CLOAK,
	CHEST,
	HANDS,
	LEGS,
	FEET,

	NECK,
	FINGER_ONE,
	FINGER_TWO,
	
	MAIN_HAND,
	OFF_HAND,

	EQUIPMENT_SLOT_COUNT
};

enum AttributeTypes {
	STRENGTH,
	AGILITY,
	INTELLECT,
	CONSTITUTION,
	LEVEL,

	HEALTH,
	EMR,
	
	ATTACK_POWER,
	SPELL_POWER,
	CRITICAL_STRIKE,
	HASTE,
	DEXTERITY,
	
	DODGE,
	BLOCK,
	PARRY,
	RESOLVE,
	FORTITUDE,

	EXPERIENCE,
	
	ATTRIBUTE_TYPE_COUNT
};

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
int initializeSDL();
void exitSDL(int status);
int frameCap(int last_update);
int pollEvents();
void updateRenderingInfo();
int getExperienceNeeded(int level);

/*
*/

#endif /* __GLOBALS__ */
