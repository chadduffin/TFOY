#include "yendor.h"
#include "globals.h"

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
			renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

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
				{
					if (event.key.keysym.scancode == SDL_SCANCODE_ESCAPE) {
						return -1;
					} else {
						if (phys_keys[event.key.keysym.scancode] == 0) {
							phys_keys[event.key.keysym.scancode] = SDL_GetTicks();
						}
					}
				}
				break;
			case SDL_KEYUP:
				{
					phys_keys[event.key.keysym.scancode] = 0;
				}
				break;
			case SDL_MOUSEMOTION:
				{
					cursor.x = event.motion.x;
					cursor.y = event.motion.y;
				}
				break;
			case SDL_MOUSEBUTTONDOWN:
				{
					if (event.button.button == SDL_BUTTON_LEFT) {
						cursor.lb = (cursor.lb == 0) ? SDL_GetTicks() : cursor.lb;
					} else if (event.button.button == SDL_BUTTON_RIGHT) {
						cursor.rb = (cursor.rb == 0) ? SDL_GetTicks() : cursor.rb;
					}
				}
				break;
			case SDL_MOUSEBUTTONUP:
				{
					if (event.button.button == SDL_BUTTON_LEFT) {
						cursor.lb = 0;
					} else if (event.button.button == SDL_BUTTON_RIGHT) {
						cursor.rb = 0;
					}
				}
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
	view.w = tile_width*DCOLS;
	view.h = tile_height*DROWS;
	view_previous.x = view.w;
	view_previous.y = view.h;

	if (initial == 1) {
		printf("Initializing graphics .. PENDING.\n");
		printf("> window width ......... %d.\n", window_width);
		printf("> window height ........ %d.\n", window_height);
		printf("> dport x .............. %d.\n", dport.x);
		printf("> dport y .............. %d.\n", dport.y);
		printf("> dport width .......... %d.\n", dport.w);
		printf("> dport height ......... %d.\n", dport.h);
		printf("> tile width ........... %d.\n", tile_width);
		printf("> tile height .......... %d.\n", tile_height);
		printf("Initializing graphics .. SUCCESS.\n");
	}
}

void focusView() {
	entity *target = (focus == NULL) ? player : focus;
	
	if (target != NULL) {
		render_component *comp = (render_component*)getComponent(target, RENDER_COMPONENT);

		view.x = comp->x;
		view.y = comp->y;
		view.x -= ((DCOLS/2)+1)*tile_width;
		view.y -= ((DROWS/2)+1)*tile_height;

		if (view.x < 0) {
			view.x = 0;
		} else if (view.x+view.w > (location->w)*tile_width) {
			view.x = ((location->w)*tile_width)-view.w;
		}
		if (view.y < 0) {
			view.y = 0;
		} else if (view.y+view.h > (location->h)*tile_height) {
			view.y = ((location->h)*tile_height)-view.h;
		}
	}
}

void update() {
	focusView();

	if (phys_keys[SDL_SCANCODE_SPACE]) {
		(location == &menu) ? changeScene(&overworld) : changeScene(&menu);
		phys_keys[SDL_SCANCODE_SPACE] = 0;
	}
}

void render() {
	// clear new buffer to be drawn to
	SDL_SetRenderTarget(renderer, render_buffers[target_buffer]);
	SDL_RenderClear(renderer);

	// attempt to salvage data from previous screen
	if (location != &menu) {
		renderSalvage();
	} else {
		SDL_RenderCopy(renderer, render_buffers[!target_buffer], NULL, NULL);
	}
	renderChanges();
	
	// render new buffer to screen
	SDL_SetRenderTarget(renderer, NULL);	
	SDL_RenderCopy(renderer, render_buffers[target_buffer], NULL, NULL);

	view_previous.x = view.x;
	view_previous.y = view.y;

	SDL_RenderPresent(renderer);
	target_buffer = (target_buffer == 0) ? 1 : 0;	
}

void renderSalvage() {
	int
		x,
		y,
		xdiff = view.x-view_previous.x,
		ydiff = view.y-view_previous.y;
	SDL_Rect
		src,
		dst;

	// ensures that the shift amount is not too great
	if (((xdiff == 0) ||
			((xdiff < 0) && (view.w+xdiff > 0)) ||
			((xdiff > 0) && (view.w-xdiff > 0))) &&
			((ydiff == 0) ||
			((ydiff < 0) && (view.h+ydiff > 0)) ||
			((ydiff > 0) && (view.h-ydiff > 0)))) {

		src.x = (xdiff < 0) ? 0 : xdiff;
		src.y = (ydiff < 0) ? 0 : ydiff;
		src.w = (xdiff < 0) ? (view.w+xdiff) : (view.w-xdiff);
		src.h = (ydiff < 0) ? (view.h+ydiff) : (view.h-ydiff);
		
		src.x += dport.x+(DCOLS_OFFSET)*tile_width;
		src.y += dport.y+(DROWS_OFFSET)*tile_height;

		dst.x = (xdiff < 0) ? (-xdiff) : 0;
		dst.y = (ydiff < 0) ? (-ydiff) : 0;
		dst.w = src.w;
		dst.h = src.h;

		dst.x += dport.x+(DCOLS_OFFSET)*tile_width;
		dst.y += dport.y+(DROWS_OFFSET)*tile_height;

		SDL_RenderCopy(renderer, render_buffers[!target_buffer], &src, &dst);
		
		dst.x = (dst.x-dport.x)/tile_width;
		dst.y = (dst.y-dport.y)/tile_height;
		dst.w /= tile_width;
		dst.h /= tile_height;
	} else {
		dst.x = 0;
		dst.y = 0;
		dst.w = 0;
		dst.h = 0;
	}

	int
		x_offset = DCOLS_OFFSET+(view.x/tile_width),
		y_offset = DROWS_OFFSET+(view.y/tile_height);

	for (y = DROWS_OFFSET; y < (DROWS+DROWS_OFFSET); y += 1) {
		for (x = DCOLS_OFFSET; x < (DCOLS+DCOLS_OFFSET); x += 1) {
				if ((x < dst.x) || (x >= dst.x+dst.w) ||
						(y < dst.y) || (y >= dst.y+dst.h)) {
					dmatrix[x][y].changed = 1;
					dmatrix[x][y].tile = getTileValue(x-x_offset, y-y_offset);
				}
		}
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
			int to_draw = dmatrix[x][y].tile;
			color
				fg = (to_draw < 256) ? white : *(descriptor_tiles[to_draw-256].fg),
				bg = (to_draw < 256) ? black : *(descriptor_tiles[to_draw-256].bg);

			if (dmatrix[x][y].changed == 1) {
				short
					r,
					g,
					b;

				evaluateRGB(bg, &r, &g, &b);

				if (dmatrix[x][y].visible == 0) {
					r = 0;
					g = 0;
					b = 0;
					fg = bg;
				}

				dst.x = dport.x+(x*tile_width);
				dst.y = dport.y+(y*tile_height);

				SDL_SetRenderDrawColor(renderer, r, g, b, 255);
				SDL_RenderFillRect(renderer, &dst);	

				if (&fg != &bg) {
					lookupTileSource(&src, to_draw);
					evaluateRGB(fg, &r, &g, &b);
					SDL_SetTextureColorMod(textures[0], r, g, b);
					SDL_SetTextureAlphaMod(textures[0], dmatrix[x][y].alpha);

					SDL_RenderCopy(renderer, textures[0], &src, &dst);

					SDL_SetTextureColorMod(textures[0], 255, 255, 255);
					SDL_SetTextureAlphaMod(textures[0], 255);
				}
				dmatrix[x][y].changed = 0;
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
	cell.tile = BLACK;
	cell.alpha = 255;
	cell.entity = NOTHING;
	cell.changed = 1;
	cell.visible = 1;
	cell.discovered = 1;
	cell.light_value.intensity = 255;
	cell.light_value.value = white;

	for (y = 0; y < ROWS; y += 1) {
		for (x = 0; x < COLS; x += 1) {
			dmatrix[x][y] = cell;
		}
	}
}

void evaluateRGB(color col, short *r, short *g, short *b) {
	*r = col.red;
	*g = col.green;
	*b = col.blue;

	if (!DISABLE_COLOR_MOD) {
		if (col.redRand != 0) {
			*r += (rand()%(col.redRand))-(rand()%(col.redRand));
		}
		if (col.greenRand != 0) {
			*g += (rand()%(col.greenRand))-(rand()%(col.greenRand));
		}
		if (col.blueRand != 0) {
			*b += (rand()%(col.blueRand))-(rand()%(col.blueRand));
		}

		if (*r < 0) {
			*r = 0;
		} else if (*r > 255) {
			*r = 255;
		}
	
		if (*g < 0) {
			*g = 0;
		} else if (*g > 255) {
			*g = 255;
		}
	
		if (*b < 0) {
			*b = 0;
		} else if (*b > 255) {
			*b = 255;
		}
	}
}

void changeScene(scene *dest) {
	int
		x,
		y;
	location = dest;

	view.x = 0;
	view.y = 0;
	view_previous.x = view.w;
	view_previous.y = view.h;

	if (location == &menu) {
		for (y = 0; y < ROWS; y += 1) {
			for (x = 0; x < COLS; x += 1) {
				dmatrix[x][y].changed = 1;
				dmatrix[x][y].tile = getTileValue(x, y);
			}
		}
	} else {
		int
			x_offset = DCOLS_OFFSET+(view.x/tile_width),
			y_offset = DROWS_OFFSET+(view.y/tile_height);
		for (y = 0; y < ROWS; y += 1) {
			for (x = 0; x < COLS; x += 1) {
				dmatrix[x][y].changed = 1;
				dmatrix[x][y].visible = 0;

				if ((x >= DCOLS_OFFSET) && (x < DCOLS+DCOLS_OFFSET) &&
						(y >= DROWS_OFFSET) && (y < DROWS+DROWS_OFFSET)) {
					dmatrix[x][y].tile = getTileValue(x-x_offset, y-y_offset);
				} else {
					dmatrix[x][y].tile = BLACK;
				}
			}
		}
		generateFOV(1+(DCOLS/2), 1+(DROWS/2));
	}
}

void initializeKeybindings() {
	int x;

	for (x = 0; x < SDL_NUM_SCANCODES; x += 1) {
		phys_keys[x] = 0;
	}
	for (x = 0; x < KEYBINDING_TYPE_COUNT; x += 1) {
		virt_keys[x] = -1;
	}

	virt_keys[LEFT] = SDL_SCANCODE_H;
	virt_keys[LEFT_UP] = SDL_SCANCODE_Y;
	virt_keys[LEFT_DOWN] = SDL_SCANCODE_B;
	virt_keys[RIGHT] = SDL_SCANCODE_L;
	virt_keys[RIGHT_UP] = SDL_SCANCODE_U;
	virt_keys[RIGHT_DOWN] = SDL_SCANCODE_N;
	virt_keys[UP] = SDL_SCANCODE_J;
	virt_keys[DOWN] = SDL_SCANCODE_K;
}

void generateFOV(short x, short y) {
	x += DCOLS_OFFSET;
	y += DROWS_OFFSET;
	castLight(1, x, y, 0, 1, 1, 1, 0);
	castLight(1, x, y, 1, 1, 1, 1, 0);
	castLight(1, x, y, 0, -1, 1, 1, 0);
	castLight(1, x, y, 1, -1, 1, 1, 0);
	castLight(1, x, y, 0, 1, -1, 1, 0);
	castLight(1, x, y, 1, 1, -1, 1, 0);
	castLight(1, x, y, 0, -1, -1, 1, 0);
	castLight(1, x, y, 1, -1, -1, 1, 0);
}

void castLight(
	short distance, short x, short y,
	short invert, short dx, short dy,
	float start, float end) {
	short
		i,
		j,
		x_adj,
		y_adj,
		was_blocked = 0;
	float
		current,
		next;

	for (i = distance; i < 28; i += 1) {
		for (j = distance; j >= 0; j -= 1) {
			next = ((float)(j-0.5))/((float)(i+0.5));
			current = ((float)(j+0.5))/((float)(i+0.5));
			if (current > start) {
				continue;
			}
			if (current >= end) {
				if (invert) {
					x_adj = x+(j*dy);
					y_adj = y+(i*dx);
				} else {
					x_adj = x+(i*dx);
					y_adj = y+(j*dy);
				}
				if (lookupTile(dmatrix[x_adj][y_adj].tile)->base == SOLID) {
					if ((was_blocked == 0) && (j != distance)) {
						if ((j == 0) || (next <= end)) {
							end = ((float)(j+1.0))/(float)(i);
						}
						castLight(distance+1, x, y, invert, dx, dy, start, ((float)(j+1.0))/(float)(i));
					}
					was_blocked = 1;
				} else {
					if (was_blocked) {
						start = (float)(j+1.0)/(float)(i);
					}
					was_blocked = 0;
				}
				
				dmatrix[x_adj][y_adj].visible = 1;
			} else {
				j = -1;
			}
		}
		distance += 1;
		was_blocked = 0;
	}
}

/*
*/
