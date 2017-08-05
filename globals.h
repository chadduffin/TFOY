#ifndef __GLOBALS__
#define __GLOBALS__

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#ifdef __unix__
  #include <sys/stat.h>
  #include <sys/types.h>

	#include <SDL2/SDL.h>
	#include <SDL2/SDL_net.h>
	#include <SDL2/SDL_image.h>
#elif __linux__
  #include <sys/stat.h>
  #include <sys/types.h>

	#include <SDL2/SDL.h>
	#include <SDL2/SDL_net.h>
	#include <SDL2/SDL_image.h>
#elif __APPLE__
  #include <sys/stat.h>
  #include <sys/types.h>

	#include <SDL2/SDL.h>
	#include <SDL2_net/SDL_net.h>
	#include <SDL2_image/SDL_image.h>
#elif _WIN32
  #include "SDL2/SDL.h"
  #include "SDL2/SDL_net.h"
  #include "SDL2/SDL_image.h"
#endif

/* globals.c */

extern int FPS;
extern int DCOLS;
extern int DROWS;
extern int DCOLS_OFFSET;
extern int DROWS_OFFSET;
extern int TILE_SOURCE_WIDTH;
extern int TILE_SOURCE_HEIGHT;

extern int permutations[512];

extern SDL_mutex *fmutex;
extern SDL_Thread *threads[GAME_THREAD_COUNT];

extern boolean redraw_lockable;

extern boolean vismap[COLS][ROWS];
extern G_TileCell tilemap[COLS][ROWS];
extern G_LightNode lightmap[COLS][ROWS];

extern G_Id menu_id;
extern G_Id overworld_id;

extern G_Console console;
extern G_GameInformation game_info;

extern G_Tree *chunks;
extern G_Tree *scenes;
extern G_Scene *active_scene;

extern char title[ROWS][COLS];

/* tiles.c */

extern G_TileInformation tile_info[TILE_COUNT];

/* colors.c */

extern G_Color black;
extern G_Color white;

extern G_Color red;
extern G_Color grey;
extern G_Color teal;
extern G_Color blue;
extern G_Color brown;
extern G_Color green;
extern G_Color orange;
extern G_Color yellow;
extern G_Color violet;
extern G_Color purple;
extern G_Color magenta;

extern G_Color dred;
extern G_Color dgrey;
extern G_Color dteal;
extern G_Color dblue;
extern G_Color dbrown;
extern G_Color dgreen;
extern G_Color dorange;
extern G_Color dyellow;
extern G_Color dviolet;
extern G_Color dpurple;
extern G_Color dmagenta;

extern G_Color ui_fg_active;
extern G_Color ui_bg_active;
extern G_Color ui_fg_inactive;
extern G_Color ui_bg_inactive;

extern G_Color bad_color;

#endif /* GLOBALS */
