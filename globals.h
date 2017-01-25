#ifndef __GLOBALS__
#define __GLOBALS__

#include <SDL2/SDL.h>
#include <SDL2/SDL_net.h>
#include <SDL2/SDL_image.h>

/*
** externs
*/

extern char title[ROWS][COLS];
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

// view is the in-world x & y
extern SDL_Rect view;
extern SDL_Rect view_previous;
// dport is the net screen area to be drawn and still maintain aspect ratio
extern SDL_Rect dport;
extern SDL_Event event;
extern SDL_Window *window;
extern SDL_Renderer *renderer;
extern SDL_Texture *textures[TEXTURE_COUNT];
extern SDL_Texture *render_buffers[2];

extern IPaddress ipaddress;
extern TCPsocket socket;

extern dcell dmatrix[COLS][ROWS];
extern dtile descriptor_tiles[TILE_TYPE_COUNT];
extern attribute attributes[ATTRIBUTE_TYPE_COUNT];

extern scene menu;
extern scene overworld;
extern scene *location;

extern entity *player;

extern const color red;
extern const color blue;
extern const color aqua;
extern const color black;
extern const color white;
extern const color green;
extern const color yellow;
extern const color magenta;

extern const color brown;
extern const color grey;
extern const color dark_blue;

extern keypress phys_keys[SDL_NUM_SCANCODES];

/*
*/

#endif /* __GLOBALS__ */
