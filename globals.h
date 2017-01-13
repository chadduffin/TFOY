#ifndef __GLOBALS__
#define __GLOBALS__

#include <SDL2/SDL.h>
#include <SDL2/SDL_net.h>
#include <SDL2/SDL_image.h>

#include "tiles.h"

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
#define TILE_SOURCE_HEIGHT 16

#define COLS 108
#define ROWS 42
#define DCOLS (COLS - STAT_BAR_WIDTH - 2)
#define DROWS (ROWS - MESSAGE_ROWS - 2)

#define MESSAGE_ROWS 5
#define STAT_BAR_WIDTH 20

#define MIN_DUNGEON_DEPTH 1
#define MAX_DUNGEON_DEPTH 64

/*
** typedefs
*/

typedef struct color {
	short rgb[3];
} color;

typedef struct dcell {
	short
		empty,
		changed;
	etile tile;
	color
		fg,
		bg;
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
extern int target_buffer;
extern int mouse_x;
extern int mouse_y;

extern dcell dmatrix[COLS][ROWS];

// view is the in world x & y
extern SDL_Rect view;
extern SDL_Rect view_previous;
// dport is the net area to be drawn and still maintain aspect ratio
extern SDL_Rect dport;
extern SDL_Event event;
extern SDL_Window *window;
extern SDL_Renderer *renderer;
extern SDL_Texture *textures[TEXTURE_COUNT];
extern SDL_Texture *render_buffers[2];

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
SDL_Rect getCharTile(unsigned char value);
void render();
void renderSalvage();
void renderChanges();
void initializeScreen();

/*
*/

#endif /* __GLOBALS__ */
