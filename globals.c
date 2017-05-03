#include "yendor.h"

int DCOLS = 98;
int DROWS = 58;
int DCOLS_OFFSET = 1;
int DROWS_OFFSET = 1;
int TILE_SOURCE_WIDTH = 12;
int TILE_SOURCE_HEIGHT = 12;

int permutations[512];

SDL_mutex *fmutex;
SDL_Thread *threads[GAME_THREAD_COUNT];

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
  {"                                                                                                    "},
  {"                                       T H E   F E L L O W S H I P S   O F                          "},
  {"   BBBB              BBBB                                                                           "},
  {"     BB              BB                                                                             "},
  {"      BB            BB    BBBBBBBBBBBBB BB          BB BBBBBBB          BBBBBB     BBBBBBBB         "},
  {"       BB          BB      B          B  BB         B   B    BBBB     BBB    BBB    B     BBB       "},
  {"        BB        BB       B             BBB        B   B       BB   BB        BB   B       BB      "},
  {"         BB      BB        B             B BB       B   B        BB BB          BB  B        B      "},
  {"          BB    BB         B             B  BB      B   B         B B            B  B        B      "},
  {"           BB  BB          B             B   BB     B   B         B B            B  B       BB      "},
  {"            BBBB           BBBBBBBBBB    B    BB    B   B         B B            B  B     BBB       "},
  {"             BB            B             B     BB   B   B         B B            B  BBBBBBB         "},
  {"             BB            B             B      BB  B   B        BB BB          BB  B    BB         "},
  {"             BB            B             B       BB B   B       BB   BB        BB   B     BB        "},
  {"             BB            B          B  B        BBB   B    BBBB     BBB    BBB    B      BB       "},
  {"             BB           BBBBBBBBBBBBB BB         BBB BBBBBBB          BBBBBB     BBB      BBB     "},
  {"             BB                                                                                     "},
  {"            BBBB                                                                                    "},
  {"                                                                                                    "},
  {"                                                                                                    "},
  {"                                                                                                    "},
  {"                                                                                                    "},
  {"                                                                                                    "},
  {"                                                                                                    "},
  {"                                                                                                    "},
  {"                                                                                                    "},
  {"                                                                                                    "},
  {"                                                                                                    "},
  {"                                                                                                    "},
  {"                                                                                                    "},
  {"                                                                                                    "},
  {"                                                                                                    "},
  {"                                                                                                    "},
  {"                                                                                                    "},
  {"                                                                                                    "},
  {"                                                                                                    "},
  {"                                                                                                    "},
  {"                                                                                                    "},
  {"                                                                                                    "},
  {"                                                                                                    "},
  {"                                                                                                    "},
  {"                                                                                                    "},
  {"                                                                                                    "},
  {"                                                                                                    "},
  {"                                                                                                    "},
  {"                                                                                                    "},
  {"                                                                                                    "},
  {"                                                                                                    "},
  {"                                                                                                    "},
  {"                                                                                                    "},
  {"                                                                                                    "},
  {"                                                                                                    "},
  {"                                                                                                    "},
  {"                                                                                                    "},
  {"                                                                                                    "},
  {"                                                                                                    "},
  {"                                                                                                    "},
  {"                                                                                                    "},
  {"                                                                                                    "},
  {"                                                                                                    "},
};
