#include "main_menu.h"

/*
** externs
*/

int title_width = 106;
int title_height = 27;
gameWidget mmWidgets[M_WIDGET_COUNT];

/*
** functions
*/

void initMainMenu() {
	mmWidgets[USERNAME_LABEL].type = W_LABEL;
	mmWidgets[USERNAME_LABEL].x = (window_width/2)-tile_width*4;
	mmWidgets[USERNAME_LABEL].y = (title_height+9)*tile_height;
	mmWidgets[USERNAME_LABEL].hotkey = '\0';
	strncpy(mmWidgets[USERNAME_LABEL].text, "Username\0", 140);

	mmWidgets[PASSWORD_LABEL].type = W_LABEL;
	mmWidgets[PASSWORD_LABEL].x = (window_width/2)-tile_width*4;
	mmWidgets[PASSWORD_LABEL].y = (title_height+15)*tile_height;
	mmWidgets[PASSWORD_LABEL].hotkey = '\0';
	strncpy(mmWidgets[PASSWORD_LABEL].text, "Password\0", 140);

	mmWidgets[USERNAME_FIELD].type = W_FIELD;
	mmWidgets[USERNAME_FIELD].x = (window_width/2)-tile_width*16;
	mmWidgets[USERNAME_FIELD].y = (title_height+11)*tile_height;
	mmWidgets[USERNAME_FIELD].min = 2;
	mmWidgets[USERNAME_FIELD].max = 32;
	mmWidgets[USERNAME_FIELD].current = 8;
	mmWidgets[USERNAME_FIELD].hotkey = '\0';
	strncpy(mmWidgets[USERNAME_FIELD].text, "abcdabcdabcdabcdabcdabcdabcdabcd\0", 140);

	mmWidgets[PASSWORD_FIELD].type = W_FIELD;
	mmWidgets[PASSWORD_FIELD].x = (window_width/2)-tile_width*16;
	mmWidgets[PASSWORD_FIELD].y = (title_height+17)*tile_height;
	mmWidgets[PASSWORD_FIELD].min = 2;
	mmWidgets[PASSWORD_FIELD].max = 32;
	mmWidgets[PASSWORD_FIELD].current = 8;
	mmWidgets[PASSWORD_FIELD].hotkey = '\0';
	strncpy(mmWidgets[PASSWORD_FIELD].text, "abcdabcdabcdabcdabcdabcdabcdabcd\0", 140);

	mmWidgets[PLAY_BUTTON].type = W_BUTTON;
	mmWidgets[PLAY_BUTTON].x = (window_width/2)-tile_width*2;
	mmWidgets[PLAY_BUTTON].y = (title_height+23)*tile_height;
	mmWidgets[PLAY_BUTTON].hotkey = 'P';
	strncpy(mmWidgets[PLAY_BUTTON].text, "Play\0", 140);

	mmWidgets[EXIT_BUTTON].type = W_BUTTON;
	mmWidgets[EXIT_BUTTON].x = (window_width/2)-tile_width*2;
	mmWidgets[EXIT_BUTTON].y = (title_height+27)*tile_height;
	mmWidgets[EXIT_BUTTON].hotkey = 'E';
	strncpy(mmWidgets[EXIT_BUTTON].text, "Exit\0", 140);
}

void drawMainMenu() {
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
	SDL_RenderClear(renderer);
	
	const char
		title_characters[27][106] = {
			"                                                                                                          ",
			"                                  T H E    F E L L O W S H I P S    O F                                   ",
			"                                                                                                          ",
			" BBBBBBB         BBBBBBB                                                                                  ",
			" WWBBBWW         WWBBBWW                                                                                  ", 
			"   WBBB           BBBW                                                                                    ",
			"    WBBB         BBBW  BBBBBBBBBBBBB BBB         BBBB BBBBBBBBBB              BBBBBB       BBBBBBBB       ",
			"     WBBB       BBBW   WBBBBBBBBBBBB WBBB        WBBW WBBBBBBBBBBBB         BBBBBBBBBB     WBBBBBBBBB     ",
			"      WBBB     BBBW     BBWWWWWWWWWB  BBBB        BB   BBWWWWWWWWBBB      BBBWWWWWWWWBBB    BBWWWWWBBB    ",
			"       WBBB   BBBW      BB         W  BBWBB       BB   BB        WWBB    BBWW        WWBB   BB     WWBB   ",
			"        WBBB BBBW       BB            BB WBB      BB   BB          WBB  BBW            WBB  BB       BB   ",
			"         WBBBBBW        BB        B   BB  WBB     BB   BB           BB  BB              BB  BB       BB   ",
			"          WBBBW         BBBBBBBBBBB   BB   WBB    BB   BB           BB  BB              BB  BB     BBBW   ",
			"           BBB          BBWWWWWWWWB   BB    WBB   BB   BB           BB  BB              BB  BBBBBBBBWW    ",
			"           BBB          BB        W   BB     WBB  BB   BB           BB  BB              BB  BBWWWWBB      ",
			"           BBB          BB            BB      WBB BB   BB          BBW  WBB            BBW  BB    WBB     ",
			"           BBB          BB         B  BB       WBBBB   BB        BBBW    WBBB        BBBW   BB     WBB    ",
			"           BBB          BBBBBBBBBBBB  BB        WBBB   BBBBBBBBBBBBW      WWBBBBBBBBBBWW    BB      WBB   ",
			"           BBB         BBBBBBBBBBBBB BBBB        WBB  BBBBBBBBBBWWW         WWBBBBBBWW     BBBB      WBBB ",
			"           BBB         WWWWWWWWWWWWW WWWW         WW  WWWWWWWWWW              WWBBWW       WWWW       WWW ",
			"           BBB                                                                  BB                        ",
			"         BBBBBBB                                                          BBBBBBBBBBBBBB                  ",
			"         WWWWWWW                                                          WWWWWWBBWWWWWW                  ",
			"                                                                                BB                        ",
			"                                                                                BB                        ",
			"                                                                                WW                        "
		},
		title_form[21][34] = {
			"1111111111111111111111111111111111",
			"1111111111111000000001111111111111",
			"1111111111111111111111111111111111",
			"1000000000000000000000000000000001",
			"1111111111111111111111111111111111",
			"1111111111111111111111111111111111",
			"1111111111111111111111111111111111",
			"1111111111111000000001111111111111",
			"1111111111111111111111111111111111",
			"1000000000000000000000000000000001",
			"1111111111111111111111111111111111",
			"0000000000000000000000000000000000",
			"0000000000000000000000000000000000",
			"0000000000000000000000000000000000",
			"1111111111111111111111111111111111",
			"1111111111111110000111111111111111",
			"1111111111111111111111111111111111",
			"0000000000000000000000000000000000",
			"1111111111111111111111111111111111",
			"1111111111111110000111111111111111",
			"1111111111111111111111111111111111"
		};

	int
		x = 0,
		y = 0;
	SDL_Rect dest;

	dest.y = 4*tile_height;
	dest.w = tile_width;
	dest.h = tile_height;

	SDL_SetTextureColorMod(textures[0], 255, 255, 255);

	for (y = 0; y < title_height; y += 1) {
		dest.x = (window_width-(tile_width*title_width))/2;
	
		for (x = 0; x < title_width; x += 1) {
			switch (title_characters[y][x]) {
				case 'B':
					SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
					break;
				case 'W':
					SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
					break;
				default:
					SDL_SetRenderDrawColor(renderer, 255, 0, 225, 255);
					break;
			}

			if ((y == 1) && (title_characters[y][x] != ' ')) {
				SDL_Rect src = getCharTile(title_characters[y][x]);
				SDL_RenderCopy(renderer, textures[0], &src, &dest);
			} else {
				SDL_RenderFillRect(renderer, &dest);	
			}

			dest.x += tile_width;
		}
		dest.y += tile_height;
	}

	y = 0;
	dest.y = (title_height+8)*tile_height;
	SDL_SetRenderDrawColor(renderer, 255, 0, 225, 255);
	
	for (;y < 24; y += 1) {
		dest.x = (window_width/2)-tile_width*17;
		for (x = 0; x < 34; x += 1) {
			if (title_form[y][x] == '1') {
				SDL_RenderFillRect(renderer, &dest);	
			}
			dest.x += tile_width;
		}
		dest.y += tile_height;
	}

	SDL_SetRenderDrawColor(renderer, 255, 0, 225, 255);

	drawMainMenuUI();
	SDL_RenderPresent(renderer);
}

void drawMainMenuUI() {
	int i;

	for (i = 0; i < M_WIDGET_COUNT; i += 1) {
		drawTextToScreen(mmWidgets[i].text, mmWidgets[i].hotkey, mmWidgets[i].x, mmWidgets[i].y);
	}
}

/*
*/
