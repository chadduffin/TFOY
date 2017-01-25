#include "yendor.h"

/*
** externs
*/

char title[ROWS][COLS] = {
		"                                                                                                            ",
		"                                   T H E    F E L L O W S H I P S    O F                                    ",
		"                                                                                                            ",
		"  BBBBBBB         BBBBBBB                                                                                   ",
		"    BBB             BBB                                                                                     ", 
		"     BBB           BBB                                                                                      ",
		"      BBB         BBB   BBBBBBBBBBBBB BBB         BBBB BBBBBBBBBB              BBBBBB       BBBBBBBB        ",
		"       BBB       BBB     BBBBBBBBBBBB  BBB         BB   BBBBBBBBBBBB         BBBBBBBBBB      BBBBBBBBB      ",
		"        BBB     BBB      BB         B  BBBB        BB   BB        BBB      BBB        BBB    BB     BBB     ",
		"         BBB   BBB       BB            BB BB       BB   BB          BB    BB            BB   BB       BB    ",
		"          BBB BBB        BB            BB  BB      BB   BB           BB  BB              BB  BB       BB    ",
		"           BBBBB         BB        B   BB   BB     BB   BB           BB  BB     BBBB     BB  BB       BB    ",
		"            BBB          BBBBBBBBBBB   BB    BB    BB   BB           BB  BB     BBBB     BB  BB     BBB     ",
		"            BBB          BB        B   BB     BB   BB   BB           BB  BB     BBBB     BB  BBBBBBBB       ",
		"            BBB          BB            BB      BB  BB   BB           BB  BB              BB  BB    BB       ",
		"            BBB          BB            BB       BB BB   BB          BB    BB            BB   BB     BB      ",
		"            BBB          BB         B  BB        BBBB   BB        BBB      BBB        BBB    BB      BB     ",
		"            BBB          BBBBBBBBBBBB  BB         BBB   BBBBBBBBBBBB         BBBBBBBBBB      BB       BB    ",
		"            BBB         BBBBBBBBBBBBB BBBB         BB  BBBBBBBBBB              BBBBBB       BBBB       BBB  ",
		"            BBB                                                                  BB                         ",
		"            BBB                                                                  BB                         ",
		"          BBBBBBB                                                          BBBBBBBBBBBBBB                   ",
		"                                                                                 BB                         ",
		"                                                                                 BB                         ",
		"                                                                                 BB                         ",
		"                                                                                                            ",
		"                                                                                                            ",
		"                                                                                                            ",
		"                                                                                                            ",
		"                                                                                                            ",
		"                                                                                                            ",
		"                                                                                                            ",
		"                                                                                                            ",
		"                                                                                                            ",
		"                                                                                                            ",
		"                                                                                                            ",
		"                                                                                                            ",
		"                                                                                                            ",
		"                                                                                                            ",
		"                                                                                                            ",
		"                                                                                                            ",
		"                                                                                                            ",
		"                                                                                                            ",
		"                                                                                                            ",
		"                                                                                                            ",
		"                                                                                                            ",
		"                                                                                                            ",
		"                                                                                                            ",
		"                                                                                                            ",
		"                                                                                                            ",
		"                                                                                                            ",
		"                                                                                                            ",
		"                                                                                                            ",
		"                                                                                                            ",
		"                                                                                                            ",
		"                                                                                                            ",
		"                                                                                                            ",
		"                                                                                                            ",
		"                                                                                                            ",
		"                                                                                                            ",
		"                                                                                                            ",
		"                                                                                                            ",
		"                                                                                                            ",
		"                                                                                                            "
	};

char *name = "The Fellowships Of Yendor";
char *images[] = {"images.png"};
char *server_name = "";
char buffer[MAX_BUFFER];
int image_count = 1;
int port = 32768;
int tile_width = 16;
int tile_height = 24;
int window_width = 320;
int window_height = 240;
int target_buffer = 0;
int mouse_x = 0;
int mouse_y = 0;

SDL_Rect view;
SDL_Rect view_previous;
SDL_Rect dport;
SDL_Event event;
SDL_Window *window = NULL;
SDL_Renderer *renderer = NULL;
SDL_Texture *textures[TEXTURE_COUNT];
SDL_Texture *render_buffers[2];

IPaddress ipaddress;
TCPsocket socket;

entity *player;

dcell dmatrix[COLS][ROWS];

keypress phys_keys[SDL_NUM_SCANCODES];

/*
*/

