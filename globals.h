#ifndef __GLOBALS__
#define __GLOBALS__

#include <SDL2/SDL.h>
#include <SDL2/SDL_net.h>
#include <SDL2/SDL_image.h>

/*
** EXTERNS
*/

// globals.c
extern G_Info game_info;
extern G_Cell dmatrix[COLS][ROWS];
extern G_TreeNode *NIL;
extern long long G_ID;

extern int phys_keys[SDL_NUM_SCANCODES];
extern int virt_keys[KEYBINDING_COUNT];
extern int last_flicker;
extern int port;
extern IPaddress ipaddress;
extern TCPsocket socket;
extern char *name;
extern char *images[];
extern char *server_name;
extern char title[ROWS][COLS];

// attributes.c
extern G_Attribute attributes[ATTRIBUTE_COUNT];

// tiles.c
extern G_TileDescriptor descriptor_tiles[TILE_COUNT];

// scenes.c
extern G_Scene *menu;
extern G_Scene *overworld;
extern G_Scene *location;

// colors.c
extern const G_Color red;
extern const G_Color blue;
extern const G_Color aqua;
extern const G_Color black;
extern const G_Color white;
extern const G_Color green;
extern const G_Color yellow;
extern const G_Color magenta;
extern const G_Color brown;
extern const G_Color grey;
extern const G_Color dark_grey;
extern const G_Color orange;
extern const G_Color dark_blue;
extern const G_Color scott_blue;
extern const G_Color dark_green;
extern const G_Color forest_green;

/*
*/

#endif /* GLOBALS */
