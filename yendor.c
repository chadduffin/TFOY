#include "yendor.h"
#include "globals.h"

int G_Init() {
	int status = NOT_OK;
	game_info.tile_w = 16;
	game_info.tile_h = 16;
	game_info.window_w = 320;
	game_info.window_h = 240;
	game_info.display_x = 0;
	game_info.display_y = 0;
	game_info.display_w = 320;
	game_info.display_h = 240;
	game_info.target_buffer = 0;
	game_info.running = 0;

	srand(time(NULL) & 2147483647);

	if ((status & NOT_OK) == NOT_OK) {
		if (SDL_Init(SDL_INIT_EVERYTHING) == 0) {
			status = status | SDL_OK;
			printf("Initializing SDL2 ...... SUCCESS.\n");
		} else {
			printf("Initializing SDL2 ...... FAILURE.\n");
		}
		
		if ((status & SDL_OK) == SDL_OK) {
			game_info.window = SDL_CreateWindow(name, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 320, 240, SDL_WINDOW_FULLSCREEN | SDL_WINDOW_FULLSCREEN_DESKTOP);
			
			if (game_info.window != NULL) {
				status = status | WINDOW_OK;
				printf("Opening window ......... SUCCESS.\n");
			} else {
				printf("Opening window ......... FAILURE.\n");
			}
		}

		if ((status & WINDOW_OK) == WINDOW_OK) {
			game_info.renderer = SDL_CreateRenderer(game_info.window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

			if (SDL_GetRendererOutputSize(game_info.renderer, &(game_info.window_w), &(game_info.window_h)) == 0) {
				G_UpdateRenderingInfo();

				if (game_info.renderer != NULL) {
					status = status | RENDER_OK;

					game_info.buffers[0] = SDL_CreateTexture(game_info.renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, game_info.window_w, game_info.window_h);
					game_info.buffers[1] = SDL_CreateTexture(game_info.renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, game_info.window_w, game_info.window_h);

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
				game_info.textures[i] = SDL_CreateTextureFromSurface(game_info.renderer, holder);
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

			SDL_SetRenderDrawColor(game_info.renderer, 0, 0, 0, 255);
			SDL_SetRenderTarget(game_info.renderer, game_info.buffers[game_info.target_buffer]);
			SDL_RenderClear(game_info.renderer);
			SDL_SetRenderTarget(game_info.renderer, NULL);	
			SDL_RenderCopy(game_info.renderer, game_info.buffers[game_info.target_buffer], NULL, NULL);
			game_info.running = 1;
		}
	}

	SDL_PumpEvents();

	return status;
}

int G_Exit(int status) {
	if ((status & NETWRK_OK) == NETWRK_OK) {
		SDLNet_TCP_Close(socket);
		SDLNet_Quit();
		status = status ^ NETWRK_OK;
	}
	if ((status & GRPHCS_OK) == GRPHCS_OK) {
		int i;
		for (i = 0; i < TEXTURE_COUNT; i += 1) {
			SDL_DestroyTexture(game_info.textures[i]);
		}
	}
	if ((status & RENDER_OK) == RENDER_OK) {
		SDL_DestroyTexture(game_info.buffers[0]);
		SDL_DestroyTexture(game_info.buffers[1]);
		SDL_DestroyRenderer(game_info.renderer);
	}
	if ((status & WINDOW_OK) == WINDOW_OK) {
		SDL_DestroyWindow(game_info.window);
	}
	if ((status & SDL_OK) == SDL_OK) {
		SDL_Quit();
	}

	return 0;
}

int G_FrameCap(int last_update) {
	int delay = (1000.0/FPS)-(SDL_GetTicks()-last_update+0.5);

	if (delay > 0) {
		SDL_Delay(delay);
	}

	return SDL_GetTicks();
}

int G_PollEvents() {
	while (SDL_PollEvent(&(game_info.event))) {
		switch (game_info.event.type) {
			case SDL_QUIT:
				return -1;
			case SDL_KEYDOWN:
				{
					if (game_info.event.key.keysym.scancode == SDL_SCANCODE_ESCAPE) {
						return -1;
					} else {
						if (phys_keys[game_info.event.key.keysym.scancode] == 0) {
							phys_keys[game_info.event.key.keysym.scancode] = SDL_GetTicks();
						}
					}
				}
				break;
			case SDL_KEYUP:
				{
					phys_keys[game_info.event.key.keysym.scancode] = 0;
				}
				break;
			case SDL_MOUSEMOTION:
				{
				}
				break;
			case SDL_MOUSEBUTTONDOWN:
				{
				}
				break;
			case SDL_MOUSEBUTTONUP:
				{
				}
				break;
			case SDL_WINDOWEVENT:
				{
					switch (game_info.event.window.event) {
						case SDL_WINDOWEVENT_RESIZED:
						case SDL_WINDOWEVENT_SIZE_CHANGED:
						case SDL_WINDOWEVENT_MAXIMIZED:
							G_UpdateRenderingInfo();
							break;
						case SDL_WINDOWEVENT_FOCUS_LOST:
						case SDL_WINDOWEVENT_FOCUS_GAINED:
							break;
						default:
							break;
					}
				}
				break;
			default:
				break;
		}
	}

	return 0;
}

int G_HandleEvents() {
	short to_return = -1;

	if (phys_keys[SDL_SCANCODE_SPACE]) {
		(location == menu) ? G_ChangeScene(&overworld) : G_ChangeScene(&menu);
		phys_keys[SDL_SCANCODE_SPACE] = 0;
		to_return = 0;
	}

	if (G_CheckBound(RIGHT)) {
		to_return = 0;
	}
	if (G_CheckBound(LEFT)) {
		to_return = 0;
	}
	if (G_CheckBound(UP)) {
		to_return = 0;
	}
	if (G_CheckBound(DOWN)) {
		to_return = 0;
	}

	// return -1 to cancel gameStep() and focusView()
	return to_return;
}

void G_FocusView(void) {
	G_View *view = G_SceneView(&location);
	G_Entity *entity = location->focus;
	
	if (entity != NULL) {
		G_RenderComponent *render = (G_RenderComponent*)G_GetComponent(&entity, RENDER_COMPONENT);
	
		if (render != NULL) {
			view->x = render->x;
			view->y = render->y;
			view->x -= (DCOLS/2)+1;
			view->y -= (DROWS/2)+1;
			view->w = DCOLS;
			view->h = DROWS;
	
			if (view->x < 0) {
				view->x = 0;
			} else if (view->x+view->w >= location->w) {
				view->x = location->w-view->w;
			}
			if (view->y < 0) {
				view->y = 0;
			} else if (view->y+view->h >= location->h) {
				view->y = location->h-view->h;
			}
		}
	}
}

void G_Update(void) {
	if (location != NULL) {
		location->view.xp = location->view.x;
		location->view.yp = location->view.y;
	}

	if (G_HandleEvents() != -1) {
		// perform a full game step and re-focus view
		G_LoopEntities(&G_EntityUpdate);
		G_FocusView();
	}
}

void G_LightUpdate(void) {
}

void G_LoopEntities(void (*func)(G_Entity**)) {
	G_Entity *entity = G_GetEntities(&location);

	while (entity != NULL) {
		func(&entity);
		entity = (G_Entity*)(entity->next);
	}
}

void G_ChangeScene(G_Scene **scene) {
	int
		x,
		y;
	location = *scene;
	location->view.unchanged = 0;

	if (location == menu) {
		for (y = 0; y < ROWS; y += 1) {
			for (x = 0; x < COLS; x += 1) {
				dmatrix[x][y].changed = 1;
				dmatrix[x][y].visible = 1;
				dmatrix[x][y].entity = NOTHING;
				dmatrix[x][y].tile = G_SceneTile(x, y);

				dmatrix[x][y].light.id = -1;
				dmatrix[x][y].light.count = 0;
				dmatrix[x][y].light.light.red = 0;
				dmatrix[x][y].light.light.green = 0;
				dmatrix[x][y].light.light.blue = 0;
				dmatrix[x][y].light.light.intensity = 0;
			}
		}
	} else {
		int
			x_offset = location->view.x-DCOLS_OFFSET,
			y_offset = location->view.y-DROWS_OFFSET;

		for (y = 0; y < ROWS; y += 1) {
			for (x = 0; x < COLS; x += 1) {
				dmatrix[x][y].changed = 1;
				dmatrix[x][y].entity = NOTHING;

				if ((x >= DCOLS_OFFSET) && (x < DCOLS+DCOLS_OFFSET) &&
						(y >= DROWS_OFFSET) && (y < DROWS+DROWS_OFFSET)) {
					dmatrix[x][y].visible = 0;
					dmatrix[x][y].tile = G_SceneTile(x+x_offset, y+y_offset);
				} else {
					dmatrix[x][y].visible = 1;
					dmatrix[x][y].tile = BLACK;
				}

				dmatrix[x][y].light.id = -1;
				dmatrix[x][y].light.count = 0;
				dmatrix[x][y].light.light.red = 0;
				dmatrix[x][y].light.light.green = 0;
				dmatrix[x][y].light.light.blue = 0;
				dmatrix[x][y].light.light.intensity = 0;
			}
		}
	}
}

void G_InitializeKeybindings() {
	int x;

	for (x = 0; x < SDL_NUM_SCANCODES; x += 1) {
		phys_keys[x] = 0;
	}
	for (x = 0; x < KEYBINDING_COUNT; x += 1) {
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

int G_CheckBound(Keybinding keybinding) {
	return phys_keys[virt_keys[keybinding]];
}

int G_CheckPhysical(SDL_Scancode key) {
	return phys_keys[key];
}

int G_IsPointWithin(int x, int y, G_View *view) {
	return ((x >= view->x) && (x < view->x+view->w) && (y >= view->y) && (y < view->y+view->h));
}

unsigned int G_GetID(void) {
	G_ID += 1;
	return G_ID-1;
}

/*
*/
