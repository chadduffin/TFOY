#include "globals.h"

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

dcell dmatrix[COLS][ROWS];

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

	/*
	** colors
	**													r			g			b			rRand	gRand	bRand	flicker
	*/
	
	const color c_black 	= 		{0,		0,		0,		0,		0,		0,		0};
	const color c_white 	= 		{255,	255,	255,	0,		0,		0,		0};
	const color c_red 		= 		{255,	0,		0,		0,		0,		0,		0};
	const color c_yellow	= 		{255,	255,	0,		0,		0,		0,		0};
	const color c_magenta = 		{255,	0,		255,	0,		0,		0,		0};
	const color c_green 	= 		{0,		255,	0,		0,		0,		0,		0};
	const color c_blue 		= 		{0,		0,		255,	0,		0,		0,		0};
	const color c_aqua 		= 		{0,		255,	255,	0,		0,		0,		0};
	
	/*
	*/

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
					SDL_SetRenderTarget(renderer, render_buffers[target_buffer]);
					SDL_RenderClear(renderer);
					SDL_RenderPresent(renderer);

					render_buffers[0] = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, window_width, window_height);
					render_buffers[1] = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, window_width, window_height);

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
		SDL_DestroyTexture(render_buffers[0]);
		SDL_DestroyTexture(render_buffers[1]);
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
					return -1;
				}
				break;
			case SDL_KEYUP:
				break;
			case SDL_MOUSEMOTION:
				{
					mouse_x = event.motion.x;
					mouse_y = event.motion.y;
				}
				break;
			case SDL_MOUSEBUTTONDOWN:
				{
					switch (event.button.button) {
						case SDL_BUTTON_LEFT:
							break;
					}
				}
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

	tile_width = dport.w/COLS;
	tile_height = dport.h/ROWS;

	dport.w = tile_width*COLS;
	dport.h = tile_height*ROWS;

	dport.x = (window_width-(tile_width*COLS))/2;
	dport.y = (window_height-(tile_height*ROWS))/2;

	view.x = 0;
	view.y = 0;
	view.w = tile_width*COLS;
	view.h = tile_height*ROWS;
	view_previous.x = 0;
	view_previous.y = 0;

	if (initial == 1) {
		printf("Initializing graphics .. PENDING.\n");
		printf("> window width ......... %d.\n", window_width);
		printf("> window height ........ %d.\n", window_height);
		printf("> port x ............... %d.\n", dport.x);
		printf("> port y ............... %d.\n", dport.y);
		printf("> port width ........... %d.\n", dport.w);
		printf("> port height .......... %d.\n", dport.h);
		printf("> tile width ........... %d.\n", tile_width);
		printf("> tile height .......... %d.\n", tile_height);
		printf("Initializing graphics .. SUCCESS.\n");
	}
}

void render() {
	// clear new buffer to be drawn to
	SDL_SetRenderTarget(renderer, render_buffers[target_buffer]);
	SDL_RenderClear(renderer);

	// attempt to salvage data from previous screen
	renderSalvage();
	renderChanges();
	
	// render new buffer to screen
	SDL_SetRenderTarget(renderer, NULL);	
	SDL_RenderCopy(renderer, render_buffers[target_buffer], NULL, NULL);

	SDL_RenderPresent(renderer);
	target_buffer = (target_buffer == 0) ? 1 : 0;	
}

void renderSalvage() {
	int
		xdiff = view.x-view_previous.x,
		ydiff = view.y-view_previous.y;

	// ensures that the shift amount is not too great
	if (((xdiff == 0) ||
			((xdiff < 0) && (view.w+xdiff > 0)) ||
			((xdiff > 0) && (view.w-xdiff > 0))) &&
			((ydiff == 0) ||
			((ydiff < 0) && (view.h+ydiff > 0)) ||
			((ydiff > 0) && (view.h-ydiff > 0)))) {
		SDL_Rect
			src,
			dst;

		src.x = (xdiff < 0) ? 0 : xdiff;
		src.y = (ydiff < 0) ? 0 : ydiff;
		src.w = (xdiff < 0) ? (view.w+xdiff) : (view.w-xdiff);
		src.h = (ydiff < 0) ? (view.h+ydiff) : (view.h-ydiff);
		
		src.x += dport.x;
		src.y += dport.y;

		dst.x = (xdiff < 0) ? (-xdiff) : 0;
		dst.y = (ydiff < 0) ? (-ydiff) : 0;
		dst.w = src.w;
		dst.h = src.h;

		dst.x += dport.x;
		dst.y += dport.y;

		SDL_RenderCopy(renderer, render_buffers[!target_buffer], &src, &dst);
	}
}

void renderChanges() {
	int
		x,
		y;
	SDL_Rect
		src,
		dst;
	dst.w = tile_width;
	dst.h = tile_height;
	src.w = TILE_SOURCE_WIDTH;
	src.h = TILE_SOURCE_HEIGHT;

	for (y = 0; y < ROWS; y += 1) {
		for (x = 0; x < COLS; x += 1) {
			if (dmatrix[x][y].changed == 1) {
				short
					r = dmatrix[x][y].bg.red,
					g = dmatrix[x][y].bg.green,
					b = dmatrix[x][y].bg.blue;

				dst.x = dport.x+(x*tile_width);
				dst.y = dport.y+(y*tile_height);

				SDL_SetRenderDrawColor(renderer, r, g, b, 255);
				SDL_RenderFillRect(renderer, &dst);	

				if (dmatrix[x][y].empty != 1) {
					r = dmatrix[x][y].fg.red;
					g = dmatrix[x][y].fg.green;
					b = dmatrix[x][y].fg.blue;

					SDL_SetTextureColorMod(textures[0], r, g, b);
					lookupTile(&src, dmatrix[x][y].tile.tile);

					SDL_RenderCopy(renderer, textures[0], &src, &dst);
					SDL_SetTextureColorMod(textures[0], 255, 255, 255);
				}
				dmatrix[x][y].changed = dmatrix[x][y].fg.flickers | dmatrix[x][y].bg.flickers;
			}
		}
	}

	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
}

void clearScreen() {
	int
		x,
		y;
	dcell cell;
	cell.empty = 1;
	cell.changed = 1;
	cell.fg = c_black;
	cell.bg = c_black;

	for (y = 0; y < ROWS; y += 1) {
		for (x = 0; x < COLS; x += 1) {
			dmatrix[x][y] = cell;
		}
	}
}

void initializeScreen() {
	int
		x,
		y;
	dcell cell;
	cell.changed = 1;
	cell.fg = c_black;

	for (y = 0; y < ROWS; y += 1) {
		for (x = 0; x < COLS; x += 1) {
			cell.empty = 1;
			if (title[y][x] == 'B') {
				cell.bg = c_black;
			} else if ((title[y][x] != 'B') && (title[y][x] != ' ')) {
				cell.empty = 0;
				cell.fg = c_white;
				cell.bg = c_black;
				cell.tile.tile = title[y][x];
			} else if ((y > 0) && (title[y-1][x] == 'B')) {
				cell.bg = c_white;
				cell.bg.red = 255-y;
				cell.bg.green = 255-y;
				cell.bg.blue = 255-y;
			} else {
				cell.bg = c_magenta;
				cell.bg.red -= (y*1.5);
				cell.bg.blue -= (y*1.5);
			}
			dmatrix[x][y] = cell;
		}
	}
}

void lookupTile(SDL_Rect *source, unsigned int value) {
	source->x = (value%16)*TILE_SOURCE_WIDTH;
	source->y = (value/16)*TILE_SOURCE_HEIGHT;
}

/*
*/
