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
  {"                                                                                                    "},
  {"                                                                                                    "},
  {"BBBBBB         BBBBBBB                T H E   F E L L O W S H I P S   O F                           "},
  {" BBB             BBB                                                                                "},
  {"  BBB           BBB BBBBBBBBBBBBB BBB           BBBB BBBBBBBBB             BBBB      BBBBBBBB       "},
  {"   BBB         BBB   BBBBBBBBBBBB  BBB           BB   BBBBBBBBBBB        BBBBBBBB     BBBBBBBBB     "},
  {"    BBB       BBB    BB         B  BBBB          BB   BB       BBB     BBB      BBB   BB     BBB    "},
  {"     BBB     BBB     BB            BB BB         BB   BB         BB   BB          BB  BB       BB   "},
  {"      BBB   BBB      BB            BB  BB        BB   BB          BB BB            BB BB       BB   "},
  {"       BBB BBB       BB            BB   BB       BB   BB          BB BB            BB BB       BB   "},
  {"        BBBBB        BB        B   BB    BB      BB   BB          BB BB            BB BB     BBB    "},
  {"         BBB         BBBBBBBBBBB   BB     BB     BB   BB          BB BB            BB BBBBBBBB      "},
  {"         BBB         BB        B   BB      BB    BB   BB          BB BB            BB BB    BB      "},
  {"         BBB         BB            BB       BB   BB   BB          BB BB            BB BB     BB     "},
  {"         BBB         BB            BB        BB  BB   BB          BB BB            BB BB      BB    "},
  {"         BBB         BB            BB         BB BB   BB         BB   BB          BB  BB       BB   "},
  {"         BBB         BB         B  BB          BBBB   BB       BBB     BBB      BBB   BB        BB  "},
  {"         BBB         BBBBBBBBBBBB  BB           BBB   BBBBBBBBBBB        BBBBBBBB     BB         BB "},
  {"         BBB        BBBBBBBBBBBBB BBBB           BB  BBBBBBBBB             BBBB      BBBB         BB"},
  {"         BBB                                                                                        "},
  {"       BBBBBBB                                                                                      "},
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
