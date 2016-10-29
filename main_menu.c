#include "main_menu.h"

void drawMainMenu() {
	const int
		title_width = 106,
		title_height = 26;

	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
	SDL_RenderClear(renderer);
	
	const char title_characters[26][106] = {
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
		"           BBB         WWWWWWWWWWWWW WWWW         WW  WWWWWWWWWW              WWWWWW       WWWW       WWW ",
		"           BBB                                                                                            ",
		"         BBBBBBB                                                                                          ",
		"         WWWWWWW                                                                                          ",
		"                                                                                                          ",
		"                                                                                                          "
	};

	int
		x = 0,
		y = 0;
	SDL_Rect dest;

	dest.y = 4*tile_height;
	dest.w = tile_width;
	dest.h = tile_height;

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
				SDL_SetTextureColorMod(textures[0], 255, 255, 255);
				SDL_Rect src = getCharTile(title_characters[y][x]);
				SDL_RenderCopy(renderer, textures[0], &src, &dest);
			} else {
				SDL_RenderFillRect(renderer, &dest);	
			}

			dest.x += tile_width;
		}
		dest.y += tile_height;
	}
	SDL_RenderPresent(renderer);
}

