#include "yendor.h"

int DCOLS = 78;
int DROWS = 43;
int DCOLS_OFFSET = 1;
int DROWS_OFFSET = 1;
int TILE_SOURCE_WIDTH = 16;
int TILE_SOURCE_HEIGHT = 16;

int permutations[512];

SDL_mutex *fmutex;
SDL_Thread *threads[GAME_THREAD_COUNT];

boolean redraw_lockable = 0;

boolean vismap[COLS][ROWS];
G_TileCell tilemap[COLS][ROWS];
G_LightNode lightmap[COLS][ROWS];

G_Id menu_id;
G_Id overworld_id;

G_Console console;
G_GameInformation game_info;

G_Tree *chunks = NULL;
G_Tree *scenes = NULL;
G_Scene *active_scene = NULL;

char title[ROWS][COLS] = {
  {"                                                                                "},
  {"                       T H E   F E L L O W S H I P S   O F                      "},
  {"                                                                                "},
  {"  BBBBBB   BBBBBB                                                               "},
  {"    BBB     BBB                                                                 "},
  {"     BBB   BBB  BBBBBBBBBB  BBB    BBBB  BBBBBBBB        BBBBB    BBBBBBB       "},
  {"      BBB BBB    BBBBBBBBB   BBB    BB    BBBBBBBBB    BBBBBBBBB   BBBBBBBB     "},
  {"       BBBBB     BB      B   BBBB   BB    BB     BBB  BBB     BBB  BB    BBB    "},
  {"        BBB      BB          BBBBB  BB    BB      BB  BB       BB  BB    BBB    "},
  {"        BBB      BBBBBBBB    BB BBB BB    BB      BB  BB       BB  BBBBBBBB     "},
  {"        BBB      BB          BB  BBBBB    BB      BB  BB       BB  BBBBBBB      "},
  {"        BBB      BB      B   BB   BBBB    BB     BBB  BBB     BBB  BB   BBB     "},
  {"        BBB      BBBBBBBBB   BB    BBB    BBBBBBBBB    BBBBBBBBB   BB    BBB    "},
  {"        BBB     BBBBBBBBBB  BBBB    BB   BBBBBBBB        BBBBB    BBBB    BBB   "},
  {"       BBBBB                                                                    "},
  {"                                                                                "},
  {"                                                                                "},
  {"                                                                                "},
  {"                                                                                "},
  {"                                                                                "},
  {"                                                                                "},
  {"                                                                                "},
  {"                                                                                "},
  {"                                                                                "},
  {"                                                                                "},
  {"                                                                                "},
  {"                                                                                "},
  {"                                                                                "},
  {"                                                                                "},
  {"                                                                                "},
  {"                                                                                "},
  {"                                                                                "},
  {"                                                                                "},
  {"                                                                                "},
  {"                                                                                "},
  {"                                                                                "},
  {"                                                                                "},
  {"                                                                                "},
  {"                                                                                "},
  {"                                                                                "},
  {"                                                                                "},
  {"                                                                                "},
  {"                                                                                "},
  {"                                                                                "},
  {"                                                                                "},
};
