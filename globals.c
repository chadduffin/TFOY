#include "yendor.h"

/*
** externs
*/

G_Info game_info;
G_Cell dmatrix[COLS][ROWS];

int phys_keys[SDL_NUM_SCANCODES];
int virt_keys[KEYBINDING_COUNT];
int last_flicker = 0;
int port = 32768;
int G_ID = 0;

IPaddress ipaddress;
TCPsocket socket;

char *name = "The Fellowships Of Yendor";
char *images[] = {"images.png"};
char *server_name = "";

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
	"           BBBBB         BB        B   BB   BB     BB   BB           BB  BB              BB  BB       BB    ",
	"            BBB          BBBBBBBBBBB   BB    BB    BB   BB           BB  BB              BB  BB     BBB     ",
	"            BBB          BB        B   BB     BB   BB   BB           BB  BB              BB  BBBBBBBB       ",
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

/*
*/

