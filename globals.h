#ifndef __GLOBALS__
#define __GLOBALS__

#include <SDL2/SDL.h>
#include <SDL2/SDL_net.h>
#include <SDL2/SDL_image.h>

#include "attributes.h"
#include "creatures.h"
#include "tiles.h"
#include "items.h"

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

/*
** typedefs
*/

typedef struct color {
	short rgb[3];
} color;

typedef struct lightSource {
	short 
		flickering,
		intensity,
		range,
		light[3];
} lightSource;

typedef struct lightValue {
	short intensity,
		light[3];
} lightValue;

typedef struct dcell {
	short changed;
	unsigned int tile;
	lightValue value;
	lightSource source;
} dcell;

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
extern int next_actor_id;

extern SDL_Rect dport;
extern SDL_Event event;
extern SDL_Window *window;
extern SDL_Renderer *renderer;
extern SDL_Texture *textures[TEXTURE_COUNT];

extern IPaddress ipaddress;
extern TCPsocket socket;

/*
** functions
*/

int initializeSDL();
void exitSDL(int status);
int frameCap(int last_update);
int pollEvents();
void updateRenderingInfo();

/*
*/

#endif /* __GLOBALS__ */
