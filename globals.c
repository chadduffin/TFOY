#include "globals.h"

/*
** externs
*/

char *name = "The Fellowships Of Yendor";
char *images[] = {"images.png"};
char *server_name = "";
char buffer[MAX_BUFFER];
int image_count = 1;
int port = 32768;
int tile_width = 16;
int tile_height = 24;
int window_width = 300;
int window_height = 300;
int next_actor_id = 0;

SDL_Rect dview;
SDL_Rect dport;
SDL_Event event;
SDL_Window *window = NULL;
SDL_Renderer *renderer = NULL;
SDL_Texture *textures[TEXTURE_COUNT];

IPaddress ipaddress;
TCPsocket socket;

/*
** functions
*/

int initializeSDL() {
	int status = NOT_OK;

	if ((status & NOT_OK) == NOT_OK) {
		if (SDL_Init(SDL_INIT_EVERYTHING) == 0) {
			status = status | SDL_OK;
			printf("Initializing SDL2 ...... SUCCESS.\n");
		} else {
			printf("Initializing SDL2 ...... FAILURE.\n");
		}
		
		if ((status & SDL_OK) == SDL_OK) {
			window = SDL_CreateWindow(name, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, window_width, window_height, SDL_WINDOW_FULLSCREEN | SDL_WINDOW_FULLSCREEN_DESKTOP);
			
			if (window != NULL) {
				status = status | WINDOW_OK;
				printf("Opening window ......... SUCCESS.\n");
			} else {
				printf("Opening window ......... FAILURE.\n");
			}
		}

		if ((status & WINDOW_OK) == WINDOW_OK) {
			renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

			if (SDL_GetRendererOutputSize(renderer, &window_width, &window_height) == 0) {
				updateRenderingInfo(1);

				if (renderer != NULL) {
					status = status | RENDER_OK;
					SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
					SDL_RenderClear(renderer);

					printf("Creating renderer ...... SUCCESS.\n");
				} else {
					printf("Creating renderer ...... FAILURE.\n");
				}
			}
		}

		if ((status & RENDER_OK) == RENDER_OK) {
			int i;
			IMG_Init(IMG_INIT_PNG);
			SDL_Surface *holder = NULL;

			for (i = 0; i < TEXTURE_COUNT; i += 1) {
				holder = IMG_Load(images[0]);
				textures[i] = SDL_CreateTextureFromSurface(renderer, holder);
				SDL_FreeSurface(holder);
			}	

			IMG_Quit();
			status = status | GRPHCS_OK;
			printf("Loading textures ....... SUCCESS.\n");

			drawMainMenu();
		}

		if ((status & GRPHCS_OK) == GRPHCS_OK) {
			if (SDLNet_Init() == 0) {
				printf("Initializing SDLNet .... SUCCESS.\n");

				if (SDLNet_ResolveHost(&ipaddress, server_name, port) == 0) {
					socket = SDLNet_TCP_Open(&ipaddress);
					printf("Connecting to server ... SUCCESS.\n");
					
					if (socket) {
						status = status | NETWRK_OK;
						printf("Opening socket ......... SUCCESS.\n");
					} else {
						printf("Opening socket ......... FAILURE.\n");
					}
				} else {
					printf("Connecting to server ... FAILURE.\n");
				} 
			} else {
				printf("Initializing SDLNet .... FAILURE.\n");
			}
		}
	}

	return status;
}

void exitSDL(int status) {
	if ((status & NETWRK_OK) == NETWRK_OK) {
		SDLNet_TCP_Close(socket);
		SDLNet_Quit();
		status = status ^ NETWRK_OK;
	}
	if ((status & GRPHCS_OK) == GRPHCS_OK) {
		int i;
		for (i = 0; i < TEXTURE_COUNT; i += 1) {
			SDL_DestroyTexture(textures[i]);
		}
	}
	if ((status & RENDER_OK) == RENDER_OK) {
		SDL_DestroyRenderer(renderer);
	}
	if ((status & WINDOW_OK) == WINDOW_OK) {
		SDL_DestroyWindow(window);
	}
	if ((status & SDL_OK) == SDL_OK) {
		SDL_Quit();
	}
}

int frameCap(int last_update) {
	int delay = (1000.0/FPS)-(SDL_GetTicks()-last_update+0.5);

	if (delay > 0) {
		SDL_Delay(delay);
	}

	return SDL_GetTicks();
}

int pollEvents() {
	while (SDL_PollEvent(&event)) {
		switch (event.type) {
			case SDL_QUIT:
				return -1;
			case SDL_KEYDOWN:
				if (event.key.keysym.scancode == SDL_SCANCODE_ESCAPE) {
					printf("Quit pressed.\n");
					fflush(stdout);
					return -1;
				}
				break;
			case SDL_KEYUP:
				break;
			case SDL_MOUSEMOTION:
				break;
			case SDL_MOUSEBUTTONDOWN:
				break;
			case SDL_MOUSEBUTTONUP:
				break;
			case SDL_WINDOWEVENT:
				{
					switch (event.window.event) {
						case SDL_WINDOWEVENT_RESIZED:
						case SDL_WINDOWEVENT_SIZE_CHANGED:
						case SDL_WINDOWEVENT_MAXIMIZED:
							updateRenderingInfo(0);
							break;
					}
				}
				break;
		}
	}

	return 0;
}

void updateRenderingInfo(int initial) {
	double tile_ratio = ((double)(COLS*TILE_SOURCE_WIDTH))/(ROWS*TILE_SOURCE_HEIGHT),
		window_ratio = ((double)window_width)/window_height;

	if (tile_ratio > window_ratio) {
		dport.w = window_width;
		dport.h = window_width/tile_ratio;
	} else if (tile_ratio < window_ratio) {
		dport.w = window_height*tile_ratio;
		dport.h = window_height;
	} else {
		dport.w = window_width;
		dport.h = window_height;
	}

	dport.x = (window_width-dport.w)/2;
	dport.y = (window_height-dport.h)/2;

	tile_width = dport.w/COLS;
	tile_height = dport.h/ROWS;

	dview.w = tile_width*DCOLS;
	dview.h = tile_height*DROWS;
	dview.x = dport.x+dport.w-dview.w-tile_width;
	dview.y = dport.y+dport.h-dview.h-tile_height;

	if (initial == 1) {
		printf("Initializing graphics .. PENDING.\n");
		printf("> window width ......... %d.\n", window_width);
		printf("> window height ........ %d.\n", window_height);
		printf("> port width ........... %d.\n", dport.w);
		printf("> port height .......... %d.\n", dport.h);
		printf("> tile width ........... %d.\n", tile_width);
		printf("> tile height .......... %d.\n", tile_height);
		printf("Initializing graphics .. SUCCESS.\n");
	}
}

SDL_Rect getCharTile(unsigned char value) {
	SDL_Rect to_return;

	if ((value < 32) || (value > 126)) {
		value = 'X';
	}

	to_return.x = (value%16)*TILE_SOURCE_WIDTH;
	to_return.y = (1+(value/16))*TILE_SOURCE_HEIGHT;
	to_return.w = TILE_SOURCE_WIDTH;
	to_return.h = TILE_SOURCE_HEIGHT;

	return to_return;
}

void drawMainMenu() {
	const int
		title_width = 106,
		title_height = 23;

	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
	SDL_RenderClear(renderer);
	
	const char title_characters[23][106] = {
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
		"         BBBBBBB                                                                                          ",
		"         WWWWWWW                                                                                          ",
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
		dest.x = (tile_width/2)+dport.x;
	
	for (x = 0; x < title_width; x += 1) {
			dest.x += tile_width;

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
		}
		dest.y += tile_height;
	}
	SDL_RenderPresent(renderer);
}

/*
*/
