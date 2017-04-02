#include "yendor.h"
#include "globals.h"

int G_Init(void *data) {
	game_info.tile_w = 16;
	game_info.tile_h = 16;
	game_info.mouse_x = 0;
	game_info.mouse_y = 0;
	game_info.mouse_lb = 0;
	game_info.mouse_rb = 0;
	game_info.window_w = 320;
	game_info.window_h = 240;
	game_info.display_x = 0;
	game_info.display_y = 0;
	game_info.display_w = 320;
	game_info.display_h = 240;
	game_info.target_buffer = 0;
  game_info.id.value = 0;
	game_info.running = 0;
	game_info.full = 0;
  
	srand(time(NULL) & 2147483647);

  if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) == 0) {
		game_info.window = SDL_CreateWindow("TFOY", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 320, 240, SDL_WINDOW_FULLSCREEN | SDL_WINDOW_FULLSCREEN_DESKTOP);

    if (game_info.window != NULL) {
			game_info.renderer = SDL_CreateRenderer(game_info.window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
      
			if (SDL_GetRendererOutputSize(game_info.renderer, &(game_info.window_w), &(game_info.window_h)) == 0) {
				G_UpdateRenderingInfo();

        if (game_info.renderer != NULL) {
					game_info.buffers[0] = SDL_CreateTexture(game_info.renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, game_info.window_w, game_info.window_h);
					game_info.buffers[1] = SDL_CreateTexture(game_info.renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, game_info.window_w, game_info.window_h);

	        SDL_SetRenderTarget(game_info.renderer, NULL);
          SDL_SetRenderDrawColor(game_info.renderer, 0, 0, 0, 255);
          SDL_RenderClear(game_info.renderer);
	        SDL_SetRenderTarget(game_info.renderer, game_info.buffers[game_info.target_buffer]);
          SDL_SetRenderDrawColor(game_info.renderer, 0, 0, 0, 255);
          SDL_RenderClear(game_info.renderer);
	        SDL_SetRenderTarget(game_info.renderer, game_info.buffers[!game_info.target_buffer]);
          SDL_SetRenderDrawColor(game_info.renderer, 0, 0, 0, 255);
          SDL_RenderClear(game_info.renderer);
          
          SDL_RenderPresent(game_info.renderer);
        }
      }
    }
  }

  if (game_info.renderer != NULL) {
		int i;
		IMG_Init(IMG_INIT_PNG);
		SDL_Surface *holder = NULL;

		for (i = 0; i < TEXTURE_COUNT; i += 1) {
			holder = IMG_Load("images.png");
			game_info.textures[i] = SDL_CreateTextureFromSurface(game_info.renderer, holder);
			SDL_FreeSurface(holder);
		}	

		IMG_Quit();
    game_info.running = 1;

    G_ClearBuffers();
    G_InitializeKeybindings();
    
    scenes = G_TreeCreate();
    active_scene = G_SceneCreate(1024, 768);
    
    G_TestScene(&active_scene);
  }

  return 0;
}

int G_Update(void *data) {
  int status, x, y;

  if (active_scene != NULL) {
    active_scene->view.xp = active_scene->view.x;
    active_scene->view.yp = active_scene->view.y;

    threads[WORKER_THREAD_A] = SDL_CreateThread(G_UpdateTransitions, "game-transition-updating", NULL);

    if ((game_info.phys[SDL_SCANCODE_Z]) ||
        (game_info.phys[SDL_SCANCODE_L]) || (game_info.phys[SDL_SCANCODE_K]) ||
        (game_info.phys[SDL_SCANCODE_H]) || (game_info.phys[SDL_SCANCODE_J])) {
      game_info.phys[SDL_SCANCODE_Z] = 0;
      game_info.full = 1;
    }

    G_UpdateEntities(NULL);
    G_FocusView(&active_scene);
    G_EntityPos(&(active_scene->focus), &x, &y);
    G_GenerateFOV(x, y, NULL, &G_MakeVisible);
    G_UpdateLightmap(NULL);
    G_RenderEntities(NULL);

    SDL_WaitThread(threads[WORKER_THREAD_A], &status);

    for (y = 0; y < DROWS; y += 1) {
      for (x = 0; x < DCOLS; x += 1) {
        Tile tile = G_SceneGetTile(&active_scene, x+active_scene->view.x, y+active_scene->view.y);
        tilemap[x+DCOLS_OFFSET][y+DROWS_OFFSET].layers[BASE_LAYER] = tile;
        G_TileUpdate(tile, x, y);
      }
    }

    active_scene->step += game_info.full;
    game_info.full = 0;
  }

  return 0;
}

int G_PollEvents(void* data) {
	while (SDL_PollEvent(&(game_info.event))) {
		switch (game_info.event.type) {
			case SDL_QUIT:
				return -1;
			case SDL_KEYDOWN:
				{
					if (game_info.event.key.keysym.scancode == SDL_SCANCODE_ESCAPE) {
            return -1;
					} else {
						if (game_info.phys[game_info.event.key.keysym.scancode] == 0) {
							game_info.phys[game_info.event.key.keysym.scancode] = SDL_GetTicks();
						}
					}
          return 6;
				}
				break;
			case SDL_KEYUP:
				{
					game_info.phys[game_info.event.key.keysym.scancode] = 0;
				}
				break;
			case SDL_MOUSEMOTION:
				{
					game_info.mouse_x = game_info.event.motion.x;
					game_info.mouse_y = game_info.event.motion.y;
				}
				break;
			case SDL_MOUSEBUTTONDOWN:
				{
					if (game_info.event.button.button == SDL_BUTTON_LEFT) {
						game_info.mouse_lb = (game_info.mouse_lb == 0) ? SDL_GetTicks() : game_info.mouse_lb;
					} else if (game_info.event.button.button == SDL_BUTTON_RIGHT) {
						game_info.mouse_rb = (game_info.mouse_rb == 0) ? SDL_GetTicks() : game_info.mouse_rb;
					}
				}
				break;
			case SDL_MOUSEBUTTONUP:
				{
					if (game_info.event.button.button == SDL_BUTTON_LEFT) {
						game_info.mouse_lb = 0;
					} else if (game_info.event.button.button == SDL_BUTTON_RIGHT) {
						game_info.mouse_rb = 0;
					}
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

int G_CopyBuffer(void *data) {
  int x, y, z, dx, dy, adx, ady;
  SDL_Rect src, dst;
  G_View view = active_scene->view;

  dx = view.x-view.xp;
  dy = view.y-view.yp;
  adx = (dx < 0) ? (-1*dx) : dx;
  ady = (dy < 0) ? (-1*dy) : dy;

  if ((adx < view.w) && (ady < view.h)) {
    if (dx < 0) {
      src.x = 0;
      dst.x = adx;
      src.w = dst.w = view.w+dx;
    } else {
      src.x = adx;
      dst.x = 0;
      src.w = dst.w = view.w-dx;
    }

    if (dy < 0) {
      src.y = 0;
      dst.y = ady;
      src.h = dst.h = view.h+dy;
    } else {
      src.y = ady;
      dst.y = 0;
      src.h = dst.h = view.h-dy;
    }
  } else {
    src.x = dst.x = -1;
    src.y = dst.y = -1;
    src.w = dst.w = 0;
    src.h = dst.h = 0;
  }

  src.x += DCOLS_OFFSET;
  dst.x += DCOLS_OFFSET;
  src.y += DROWS_OFFSET;
  dst.y += DROWS_OFFSET;

  for (y = 0; y < ROWS; y += 1) {
    for (x = 0; x < COLS; x += 1) {
      if ((x >= dst.x) && (x < dst.x+dst.w) &&
          (y >= dst.y) && (y < dst.y+dst.h)) {
        console.changed[x][y] = G_CellChanged(x, y, x+dx, y+dy);
      } else if ((x < DCOLS_OFFSET) || (x >= DCOLS+DCOLS_OFFSET) ||
          (y < DROWS_OFFSET) || (y >= DROWS+DROWS_OFFSET)) {
        console.changed[x][y] = G_CellChanged(x, y, x, y);
      } else {
        console.changed[x][y] = 1;
      }
    }
  }

  for (y = 0; y < ROWS; y += 1) {
    for (x = 0; x < COLS; x += 1) {
      console.vismap[x][y] = vismap[x][y];
      console.lightmap[x][y] = lightmap[x][y];
      console.tilemap[x][y] = tilemap[x][y];

      vismap[x][y] = 0;
      lightmap[x][y].r = 0;
      lightmap[x][y].g = 0;
      lightmap[x][y].b = 0;
      lightmap[x][y].count = 1;
      lightmap[x][y].id.value = -1;
      lightmap[x][y].intensity = 255;

      for (z = 0; z < TILE_LAYER_COUNT; z += 1) {
        tilemap[x][y].layers[z] = NOTHING;
      }

      tilemap[x][y].fg = &bad_color;
      tilemap[x][y].bg = &bad_color;
    }
  }

  console.view = active_scene->view;

  src.x = game_info.display_x+(src.x*game_info.tile_w);
  dst.x = game_info.display_x+(dst.x*game_info.tile_w);
  src.y = game_info.display_y+(src.y*game_info.tile_h);
  dst.y = game_info.display_y+(dst.y*game_info.tile_h);
  src.w *= game_info.tile_w;
  dst.w *= game_info.tile_w;
  src.h *= game_info.tile_h;
  dst.h *= game_info.tile_h;

  console.src = src;
  console.dst = dst;

  return 0;
}

int G_RenderEntities(void *data) {
  G_Tree *tree = active_scene->entities;

  G_TreeNodeIterate(&tree, &(tree->root->left), &G_EntityRender);

  return 0;
}

int G_UpdateEntities(void *data) {
  G_Tree *tree = active_scene->entities;

  G_TreeNodeIterate(&tree, &(tree->root->left), &G_EntityUpdate);

  return 0;
}

int G_UpdateLightmap(void *data) {
  G_Tree *tree = active_scene->entities;

  G_TreeNodeIterate(&tree, &(tree->root->left), &G_EntityLightAdd);

  return 0;
}

int G_UpdateTransitions(void *data) {
  G_Tree *tree = active_scene->transitions;
  G_TreeNode *node = G_TreeNodeMinimum(&tree);
  G_TileTransition *transition;

  while ((node != tree->nil) && (node->key != -1) && (node->key < active_scene->step)) {
    transition = (G_TileTransition*)(node->data);
    G_Tile tile = G_SceneGetGTile(&active_scene, transition->x, transition->y);

    if (transition->id.value == tile.id.value) {
      tile.id.value = -1;
      tile.tile = transition->to;
      G_SceneSetGTile(&active_scene, tile, transition->x, transition->y);
    }

    G_TreeNodeSucceed(&tree, &node);
    node = G_TreeNodeMinimum(&tree);
  }

  return 0;
}

int G_CheckPhysical(SDL_Scancode key) {
	return game_info.phys[key];
}

int G_CheckBound(Keybinding keybinding) {
	return game_info.phys[game_info.virt[keybinding]];
}

int G_RandomNumber(int lower, int upper) {
  return lower+(rand()%(upper-lower));
}

unsigned int G_GetFPS(void) {
  return fps.frame_count/((SDL_GetTicks()-fps.last_tick)/(1000.0));
}

void G_Quit(void) {
	int i;

  G_TreeDestroy(&scenes);

	for (i = 0; i < TEXTURE_COUNT; i += 1) {
		SDL_DestroyTexture(game_info.textures[i]);
	}

	SDL_DestroyTexture(game_info.buffers[0]);
	SDL_DestroyTexture(game_info.buffers[1]);
	SDL_DestroyRenderer(game_info.renderer);
  SDL_DestroyWindow(game_info.window);
	SDLNet_Quit();
	SDL_Quit();
}

void G_UpdateBegin(void) {
  fps.frame_count += 1;

  G_CopyBuffer(NULL);
}

void G_UpdateEnd(void) {
  if (SDL_GetTicks()-fps.last_tick > FPS_LATENCY) {
    fps.frame_count = 0;
    fps.last_tick = SDL_GetTicks();
  }

  if (active_scene != NULL) {
    G_TreeNode *node = active_scene->del_buffer;

    while (node != NULL) {
      G_TreeNode *actual = G_TreeNodeFind(&(active_scene->entities), node->key);
      
      if (actual != NULL) {
        G_TreeNodeDelete(&(active_scene->entities), &actual);
      }

      active_scene->del_buffer = node->left;

      free(node);
      
      node = active_scene->del_buffer;
    }

    node = active_scene->ins_buffer;

    while (node != NULL) {
      active_scene->ins_buffer = node->left;
      node->left = NULL;

      G_TreeNodeInsert(&(active_scene->entities), &node);
      
      node = active_scene->ins_buffer;
    }
  }
}

void G_ClearBuffers(void) {
  int x, y, z;

  for (y = 0; y < ROWS; y += 1) {
    for (x = 0; x < COLS; x += 1) {
      vismap[x][y] = 0;
      lightmap[x][y].r = 0;
      lightmap[x][y].g = 0;
      lightmap[x][y].b = 0;
      lightmap[x][y].count = 1;
      lightmap[x][y].id.value = -1;
      lightmap[x][y].intensity = 255;

      for (z = 0; z < TILE_LAYER_COUNT; z += 1) {
        tilemap[x][y].layers[z] = NOTHING;
      }

      tilemap[x][y].fg = &bad_color;
      tilemap[x][y].bg = &bad_color;
    }
  }
}

void G_InitializeKeybindings(void) {
	int i;

	for (i = 0; i < SDL_NUM_SCANCODES; i += 1) {
		game_info.phys[i] = 0;
	}
	for (i = 0; i < KEYBINDING_COUNT; i += 1) {
		game_info.virt[i] = -1;
	}

	game_info.virt[LEFT] = SDL_SCANCODE_H;
	game_info.virt[RIGHT] = SDL_SCANCODE_L;
	game_info.virt[UP] = SDL_SCANCODE_J;
	game_info.virt[DOWN] = SDL_SCANCODE_K;
	game_info.virt[WAIT] = SDL_SCANCODE_Z;
}

void G_GenerateFOV(int x, int y, void *light, void (*func)(int*, int*, void*)) {
	if ((x < 0) || (x >= active_scene->w) ||
			(y < 0) || (y >= active_scene->h)) {
		return;
	}

	if (!G_SceneTileObstructs(&active_scene, x, y)) {
		G_CastShadow(1, x, y, 0, 1, 1, 1, 0, light, func);
		G_CastShadow(1, x, y, 1, 1, 1, 1, 0, light, func);
		G_CastShadow(1, x, y, 0, -1, 1, 1, 0, light, func);
		G_CastShadow(1, x, y, 1, -1, 1, 1, 0, light, func);
		G_CastShadow(1, x, y, 0, 1, -1, 1, 0, light, func);
		G_CastShadow(1, x, y, 1, 1, -1, 1, 0, light, func);
		G_CastShadow(1, x, y, 0, -1, -1, 1, 0, light, func);
		G_CastShadow(1, x, y, 1, -1, -1, 1, 0, light, func);
	}

	x -= active_scene->view.x;
	y -= active_scene->view.y;

	if ((x >= 0) && (x < active_scene->w) &&
			(y >= 0) && (y < active_scene->h)) {
		func(&x, &y, light);
	}
}

void G_CastShadow(
	int distance, int x, int y, int invert, int dx, int dy,
	float start, float end, void *light, void (*func)(int*, int*, void*)) {
	int
		i,
		j,
		x_adj,
		y_adj,
		nx_adj,	
		ny_adj,
		imm_nx_adj,	
		imm_ny_adj,
		x_bound,
		y_bound,
		started,
		was_blocked = 0,
    max = MAX_LIGHT_DISTANCE;
	float current;

  if (light != NULL) {
    max = ((G_LightNode*)light)->intensity;
    max = (max < MAX_LIGHT_DISTANCE) ? max : MAX_LIGHT_DISTANCE;
  }

	if (dx > 0) {
		if (x+max >= active_scene->w) {
			x_bound = active_scene->w-x;
		} else {
			x_bound = max;
		}
	} else {
		if (x-max < 0) {
			x_bound = x+1;
		} else {
			x_bound = max;
		}
	}
	if (dy > 0) {
		if (y+max >= active_scene->h) {
			y_bound = active_scene->h-y;
		} else {
			y_bound = max;
		}
	} else {
		if (y-max < 0) {
			y_bound = y+1;
		} else {
			y_bound = max;
		}
	}

	if (invert) {
		i = x_bound;
		x_bound = y_bound;
		y_bound = i;
	}

	for (i = distance; i < x_bound; i += 1) {
		started = 0;
		for (j = (distance > y_bound) ? y_bound : distance; j >= 0; j -= 1) {
			current = ((float)j)/((float)i);
			if (invert) {
				x_adj = x+(j*dx);
				y_adj = y+(i*dy);
				nx_adj = x+(j*dx);
				ny_adj = (i < x_bound-1) ? y+((i+1)*dy) : y_adj;
				imm_nx_adj = x+((j-1.0)*dx);
				imm_ny_adj = y+(i*dy);
			} else {
				x_adj = x+(i*dx);
				y_adj = y+(j*dy);
				nx_adj = (i < x_bound-1) ? x+((i+1)*dx) : x_adj;
				ny_adj = y+(j*dy);
				imm_nx_adj = x+(i*dx);
				imm_ny_adj = y+((j-1)*dy);
			}

			started = (started == 0) ? 1 : started;

			if ((current <= start) && (current >= end)) {
				if (G_SceneTileObstructs(&active_scene, x_adj, y_adj)) {
					if ((was_blocked == 0) && (started != 1)) {
						G_CastShadow(distance+1, x, y, invert, dx, dy, start, ((float)(j+0.5))/(float)(i-0.5), light, func);
						start = ((float)(j-0.5))/((float)(i+0.5));
					}
					if ((j > 0) && (G_SceneTileObstructs(&active_scene, nx_adj, ny_adj))) {
            if (G_SceneTileObstructs(&active_scene, imm_nx_adj, imm_ny_adj)) {
              start = ((float)(j-1.0))/((float)(i));
            } else {
              start = ((float)(j))/((float)(i+0.5));
            }
					} else {
						start = ((float)(j-0.5))/((float)(i+0.5));
					}
					was_blocked = 1;
				} else {
					was_blocked = 0;
				}

				started = 2;
			
				x_adj -= active_scene->view.x;
				y_adj -= active_scene->view.y;

				func(&x_adj, &y_adj, light);
			} else if ((started == 1) && (current < end)) {
				return;
      }
		}

		was_blocked = 0;
		distance += 1;
	}
}

void G_AddLight(int *x, int *y, void *data) {
	assert((x != NULL) && (y != NULL) && (data != NULL) && (active_scene != NULL));

	G_LightNode *light = (G_LightNode*)data;
	int
		fx,
		fy,
		lx = *x,
		ly = *y,
		lx_adj,
		ly_adj,
		dx = (lx)-(light->x-active_scene->view.x),
		dy = (ly)-(light->y-active_scene->view.y);
	float
		a = 0.97969,
		b = 3.49689,
		c = 0.34968,
		d = -0.071786,
		intensity = (sqrt(dx*dx+dy*dy))/(light->intensity);
	intensity = (d)+(a-d)/(1.0+pow((intensity/c), b));

	lx += DCOLS_OFFSET;
	ly += DROWS_OFFSET;
	lx_adj = lx+active_scene->view.x-DCOLS_OFFSET;
	ly_adj = ly+active_scene->view.y-DROWS_OFFSET;

	if (intensity > 1.0) {
		intensity  = 1.0;
	} else if (intensity < 0.001) {
		return;
	}

	G_EntityPos(&(active_scene->focus), &fx, &fy);

	if ((lx >= DCOLS_OFFSET) && (lx < DCOLS_OFFSET+DCOLS) &&
			(ly >= DROWS_OFFSET) && (ly < DROWS_OFFSET+DROWS)) {
		if ((vismap[lx][ly]) && (lightmap[lx][ly].id.value != light->id.value)) {
			if (!(G_TileFlags(tilemap[lx][ly].layers[BASE_LAYER]) & OBSTRUCTS_VISION) ||
					(G_LightCanShine(fx, fy, light->x, light->y, lx_adj, ly_adj))) {
				lightmap[lx][ly].r += light->r*intensity;
				lightmap[lx][ly].g += light->g*intensity;
				lightmap[lx][ly].b += light->b*intensity;
				lightmap[lx][ly].intensity += intensity*255;
				lightmap[lx][ly].id = light->id;
				lightmap[lx][ly].count += 1;
			}
		}
	}
}

void G_AddPointLight(int x, int y, int r, int g, int b, int intensity) {
  G_LightNode node;

  node.x = x;
  node.y = y;
  node.r = r/2;
 	node.g = g/2;
 	node.b = b/2;
  node.intensity = intensity;
 	node.id.value = game_info.id.value+x+y*active_scene->w;

  G_GenerateFOV(x, y, &node, &G_AddLight);
}

void G_MakeVisible(int *x, int *y, void *data) {
	assert((x != NULL) && (y != NULL));

	int
		lx = *x+DCOLS_OFFSET,
		ly = *y+DROWS_OFFSET;

	if ((lx >= DCOLS_OFFSET) && (lx < DCOLS_OFFSET+DCOLS) &&
			(ly >= DROWS_OFFSET) && (ly < DROWS_OFFSET+DROWS)) {
		vismap[lx][ly] = 1;
	}	
}

void G_FocusView(G_Scene **scene) {
  G_Scene *s = *scene;
  G_Entity *focus = s->focus;

  if (focus != NULL) {
    int x, y;

    G_EntityPos(&focus, &x, &y);

    s->view.x = x-s->view.w/2;
    s->view.y = y-s->view.h/2;

    s->view.x = (s->view.x < 0) ? 0 : ((s->view.x+s->view.w < s->w) ? s->view.x : s->w-s->view.w-1);
    s->view.y = (s->view.y < 0) ? 0 : ((s->view.y+s->view.h < s->h) ? s->view.y : s->h-s->view.h-1);
  }
}

boolean G_CellChanged(int x, int y, int a, int b) {
  int range;
  TileFlag flags;

  if (vismap[x][y] != console.vismap[a][b]) {
    return 1;
  }

  if ((vismap[x][y] == 1) && (G_GetTile(tilemap[x][y].layers) != G_GetTile(console.tilemap[a][b].layers))) {
    return 1;
  }

  if (vismap[x][y] == 1) {
    flags = G_TileFlags(G_GetTile(tilemap[x][y].layers));

    if (flags & FLICKERS) {
      if (flags & FLICKERS_QUICK) {
        range = FLICKER_RANGE/2;
      } else if (flags & FLICKERS_SLOW) {
        range = FLICKER_RANGE*8;
      } else {
        range = FLICKER_RANGE;
      }


      if (G_RandomNumber(0, range) == 0) {
        return 1;
      }
    }
  }

  return 0;
}

boolean G_LightCanShine(int fx, int fy, int lx, int ly, int dx, int dy) {
  fx = (fx-dx < 0) ? -1 : ((fx-dx > 0) ? 1 : 0);
  fy = (fy-dy < 0) ? -1 : ((fy-dy > 0) ? 1 : 0);
  lx = (lx-dx < 0) ? -1 : ((lx-dx > 0) ? 1 : 0);
  ly = (ly-dy < 0) ? -1 : ((ly-dy > 0) ? 1 : 0);
  fx = (fx == 0) ? lx : fx;
  fy = (fy == 0) ? ly : fy;

  return (!(((fx != lx) && (fy != ly)) ||
          ((fx == lx) && (fy != ly) && (G_SceneTileObstructs(&active_scene, dx+fx, dy))) ||
          ((fx != lx) && (fy == ly) && (G_SceneTileObstructs(&active_scene, dx, dy+fy)))));
}

boolean G_PointWithinView(int x, int y) {
  assert(active_scene != NULL);

  return ((x >= active_scene->view.x) && (x < active_scene->view.x+active_scene->view.w) &&
          (y >= active_scene->view.y) && (y < active_scene->view.y+active_scene->view.h));
}

G_Id G_GetId(void) {
  G_Id id = game_info.id;

  // get the next id to be assigned
  game_info.id.value += 1;

  return id;
}

Tile G_GetTile(Tile *layers) {
  if (layers[UI_LAYER] != NOTHING) {
    return layers[UI_LAYER];
  } else if (layers[CREATURE_LAYER] != NOTHING) {
    return layers[CREATURE_LAYER];
  } else if (layers[ITEM_LAYER] != NOTHING) {
    return layers[ITEM_LAYER];
  } else if (layers[ORNAMENT_LAYER] != NOTHING) {
    return layers[ORNAMENT_LAYER];
  }

  return layers[BASE_LAYER];
}
