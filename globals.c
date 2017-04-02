#include "yendor.h"

int DCOLS = 98;
int DROWS = 68;
int DCOLS_OFFSET = 1;
int DROWS_OFFSET = 1;
int TILE_SOURCE_WIDTH = 12;
int TILE_SOURCE_HEIGHT = 12;

SDL_Thread *threads[GAME_THREAD_COUNT];

boolean vismap[COLS][ROWS];
G_TileCell tilemap[COLS][ROWS];
G_LightNode lightmap[COLS][ROWS];

G_FPS fps;
G_Console console;
G_GameInformation game_info;

G_Tree *scenes = NULL;
G_Scene *active_scene = NULL;
