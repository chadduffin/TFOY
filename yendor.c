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
					cursor.x = event.motion.x-dport.x;
					cursor.y = event.motion.y-dport.y;
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

int handleEvents() {
	short to_return = -1;

	if (phys_keys[SDL_SCANCODE_SPACE]) {
		(location == &menu) ? changeScene(&overworld) : changeScene(&menu);
		phys_keys[SDL_SCANCODE_SPACE] = 0;
		to_return = 0;
	}

	entity *target = (focus == NULL) ? player : focus;
	render_component *value = getComponent(target, RENDER_COMPONENT);

	if (checkBoundKey(RIGHT)) {
		value->x += 1;
		phys_keys[virt_keys[RIGHT]] = 0;
		to_return = 0;
	}
	if (checkBoundKey(LEFT)) {
		value->x -= 1;
		phys_keys[virt_keys[LEFT]] = 0;
		to_return = 0;
	}
	if (checkBoundKey(UP)) {
		value->y -= 1;
		phys_keys[virt_keys[UP]] = 0;
		to_return = 0;
	}
	if (checkBoundKey(DOWN)) {
		value->y += 1;
		phys_keys[virt_keys[DOWN]] = 0;
		to_return = 0;
	}

	// return -1 to cancel gameStep() and focusView()
	return to_return;
}

void focusView() {
	if (location != &menu) {
		entity *target = (focus == NULL) ? player : focus;
	
		if (target != NULL) {
			render_component *comp = (render_component*)getComponent(target, RENDER_COMPONENT);
	
			view.x = comp->x;
			view.y = comp->y;
			view.x -= (DCOLS/2)+1;
			view.y -= (DROWS/2)+1;

			if (view.x < 0) {
				view.x = 0;
			} else if (view.x+view.w >= location->w) {
				view.x = location->w-view.w;
			}
			if (view.y < 0) {
				view.y = 0;
			} else if (view.y+view.h >= location->h) {
				view.y = location->h-view.h;
			}
		}
	} else {
		view.x = 0;
		view.y = 0;
	}
}

void update() {
	if (handleEvents() != -1) {
		// perform a full game step and re-focus view
		gameStep();
		focusView();
	} else if (location != &menu) {
		// refresh lighting instead of recalculating
		int
			x,
			y;

		for (y = 0; y < DROWS; y += 1) {
			for (x = 0; x < DCOLS; x += 1) {
				dmatrix[x+DCOLS_OFFSET][y+DROWS_OFFSET].visible = (dmatrix[x+DCOLS_OFFSET][y+DROWS_OFFSET].visible == 1) ? 2 : 0;
			}
		}
	}
}

void gameStep() {
	entity
		*head = getEntities(location);

	while (head != NULL) {
		entityUpdate(head);
		head = (entity*)(head->next);
	}

}

void changeScene(scene *dest) {
	int
		x,
		y;
	location = dest;

	view.x = 0;
	view.y = 0;
	view_previous.x = view.x+view.w;
	view_previous.y = view.y+view.h;

	if (location == &menu) {
		for (y = 0; y < ROWS; y += 1) {
			for (x = 0; x < COLS; x += 1) {
				dmatrix[x][y].changed = 1;
				dmatrix[x][y].visible = 1;
				dmatrix[x][y].entity = NOTHING;
				dmatrix[x][y].tile = getTileValue(x, y);
			}
		}
	} else {
		focusView();
		int
			x_offset = view.x-DCOLS_OFFSET,
			y_offset = view.y-DROWS_OFFSET;

		for (y = 0; y < ROWS; y += 1) {
			for (x = 0; x < COLS; x += 1) {
				dmatrix[x][y].changed = 1;
				dmatrix[x][y].entity = NOTHING;

				if ((x >= DCOLS_OFFSET) && (x < DCOLS+DCOLS_OFFSET) &&
						(y >= DROWS_OFFSET) && (y < DROWS+DROWS_OFFSET)) {
					dmatrix[x][y].tile = getTileValue(x+x_offset, y+y_offset);
				} else {
					dmatrix[x][y].tile = BLACK;
				}
			}
		}
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

int checkBoundKey(unsigned int keybinding) {
	return phys_keys[virt_keys[keybinding]];
}

int isPointWithin(int x, int y, SDL_Rect *dst) {
	return ((x >= dst->x) && (x < dst->x+dst->w) && (y >= dst->y) && (y < dst->y+dst->h));
}

/*
*/
