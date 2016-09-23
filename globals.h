#ifndef __GLOBALS__
#define __GLOBALS__

#include <SDL2/SDL.h>
#include <SDL2/SDL_net.h>
#include <SDL2/SDL_image.h>

/*
*/

#define TITLE "The Fellowships Of Yendor"
#define VERSION "v0.0.1"

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

#define COLS 128
#define ROWS 64
#define DCOLS (COLS - STAT_BAR_WIDTH - 1)
#define DROWS (ROWS - CHAT_ROWS - MESSAGE_ROWS - 2)

#define CHAT_ROWS (ONLINE * 8)
#define MESSAGE_ROWS 4
#define STAT_BAR_WIDTH 32

#define MIN_DUNGEON_DEPTH 1
#define MAX_DUNGEON_DEPTH 64

#define WORLD_COLS 10240
#define WORLD_ROWS 8192

#define BASE_MASK 3221225472
#define TYPE_MASK 1073217536
#define PERM_MASK 516096
#define TEMP_MASK 8064
#define TIME_MASK 127

#define L_DPTH_MASK 805306368
#define L_DIRS_MASK 234881024
#define L_TYPE_MASK 33030144

/*
** externs
*/

extern char *name;
extern char *images[];
extern char *server_name;
extern char buffer[MAX_BUFFER];
extern int image_count;
extern int port;

extern SDL_Event event;
extern SDL_Window *window;
extern SDL_Renderer *renderer;
extern SDL_Texture *textures[TEXTURE_COUNT];

extern IPaddress ipaddress;
extern TCPsocket socket;

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

typedef struct tile {
	unsigned int base,
		type,
		depth,
		dir,
		perm,
		temp,
		duration;
} tile;

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
** functions
*/

tile* getTile(unsigned int value);
unsigned int makeTile(
	unsigned int base,
	unsigned int type,
	unsigned int depth,
	unsigned int dir,
	unsigned int perm,
	unsigned int temp,
	unsigned int duration);
int initializeSDL();
void exitSDL(int status);
int frameCap(int last_update);
int pollEvents();

/*
*/

#endif /* __GLOBALS__ */
