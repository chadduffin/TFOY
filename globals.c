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

SDL_Event event;
SDL_Window *window = NULL;
SDL_Renderer *renderer = NULL;
SDL_Texture *textures[TEXTURE_COUNT];

IPaddress ipaddress;
TCPsocket socket;

/*
** functions
*/

tile* getTile(unsigned int value) {
	tile *to_return = (tile*)malloc(sizeof(tile));

	to_return->base = (value & BASE_MASK) >> 30;

	to_return->depth = 0;
	to_return->dir = 0;
	to_return->flam = 0;

	switch (to_return->base) {
		case LIQUID:
			{
				to_return->depth = (value & L_DPTH_MASK) >> 28;
				to_return->dir = (value & L_DIRS_MASK) >> 25;
				to_return->type = (value & L_TYPE_MASK) >> 19;
			}
			break;
		case GROUND:
			{
				to_return->flam = (value & G_FLAM_MASK) >> 29;
				to_return->type = (value & G_TYPE_MASK) >> 19;
			}
			break;
		case NOTHING:
		case WALL:
			{
				to_return->type = (value & TYPE_MASK) >> 19;
			}
			break;
	}

	to_return->perm = (value & PERM_MASK) >> 13;
	to_return->temp = (value & TEMP_MASK) >> 7;
	to_return->duration = value & TIME_MASK;

	return to_return;
}

unsigned int makeTile(
	unsigned int base,
	unsigned int type,
	unsigned int depth,
	unsigned int dir,
	unsigned int flam,
	unsigned int perm,
	unsigned int temp,
	unsigned int duration) {
	switch (base) {
		case LIQUID:
			{
				depth = (depth << 28) & L_DPTH_MASK;
				dir = (dir << 25) & L_DIRS_MASK;
				type = (type << 19) & L_TYPE_MASK;
			}
			break;
		case GROUND:
			{
				flam = (flam << 29) & G_FLAM_MASK;
				type = (type << 18) & G_TYPE_MASK;
			}
			break;
		case NOTHING:
		case WALL:
			{
				type = (type << 19) & TYPE_MASK;
			}
			break;
	}

	base = (base << 30) & BASE_MASK;
	return (base | type | depth | dir | flam | perm | temp | duration);
}

int initializeSDL() {
	int status = NOT_OK;

	if ((status & NOT_OK) == NOT_OK) {
		if (SDL_Init(SDL_INIT_EVERYTHING) == 0) {
			status = status | SDL_OK;
		}
		
		if ((status & SDL_OK) == SDL_OK) {
			window = SDL_CreateWindow(name, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, COLS*16, ROWS*16, FULLSCREEN ? SDL_WINDOW_FULLSCREEN : 0);
			
			if (window != NULL) {
				status = status | WINDOW_OK;
			}
		}

		if ((status & WINDOW_OK) == WINDOW_OK) {
			renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

			if (renderer != NULL) {
				int width,
					height;
				SDL_GetRendererOutputSize(renderer, &width, &height);

				if ((width > 768) && (height > 576)) {
					if ((width >= 6144) && (height >= 4608)) {
						tile_width = 64;
						tile_height = 96;
						port_x = (width-6144)/2;
						port_y = (height-4608)/2;
					} else if ((width >= 3072) && (height >= 2304)) {
						tile_width = 32;
						tile_height = 48;
					} else if ((width >= 1536) && (height >= 1152)) {
						tile_width = 16;
						tile_height = 24;
					} else if ((width >= 768) && (height >= 576)) {
						tile_width = 8;
						tile_height = 12;
					}

					status = status | RENDER_OK;
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
		}

		if ((status & GRPHCS_OK) == GRPHCS_OK) {
			if (SDLNet_Init() == 0) {
				if (SDLNet_ResolveHost(&ipaddress, server_name, port) == 0) {
					socket = SDLNet_TCP_Open(&ipaddress);
					
					if (socket) {
						status = status | NETWRK_OK;
					}
				}
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
		}
	}

	return 0;
}
