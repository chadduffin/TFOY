#include "yendor.h"

SDL_Thread *threads[GAME_THREAD_COUNT];

boolean vismap[COLS][ROWS];
G_TileCell tilemap[COLS][ROWS];
G_LightNode lightmap[COLS][ROWS];

G_Console console;
G_GameInformation game_info;

G_Tree *scenes = NULL;
G_Scene *active_scene = NULL;
