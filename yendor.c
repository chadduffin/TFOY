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
	game_info.timer = 0;
	game_info.frame_count = 0;
	game_info.last_update = 0;
	game_info.target_buffer = 0;
  game_info.id.value = 0;
	game_info.running = 0;
	game_info.redraw = 1;
	game_info.full = 0;
  
	srand(time(NULL) & 2147483647);

  if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) == 0) {
    SDL_ShowCursor(SDL_DISABLE);

		game_info.window = SDL_CreateWindow("TFOY", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 320, 240, SDL_WINDOW_FULLSCREEN | SDL_WINDOW_FULLSCREEN_DESKTOP);

    if (game_info.window) {
			game_info.renderer = SDL_CreateRenderer(game_info.window, -1, SDL_RENDERER_ACCELERATED);
      
			if (SDL_GetRendererOutputSize(game_info.renderer, &(game_info.window_w), &(game_info.window_h)) == 0) {
				G_UpdateRenderingInfo();

        if (game_info.renderer) {
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

  if (game_info.renderer) {
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
    chunks = G_TreeCreate();

    /* creates overworld */
    G_Scene *scene = G_SceneCreate(WORLD_WIDTH, WORLD_HEIGHT, 1);
    scene->view.follows = 1;
    overworld_id = scene->id;

    /* creates menu */
    scene = G_SceneCreate(1, 1, 0);
    menu_id = scene->id;
    G_InitializeMenu(&scene);

    fmutex = SDL_CreateMutex();

    G_SceneChange(&scene);
/*
    unsigned char *encoded = NULL, elen[4];
    unsigned int len, q;
    G_SceneChunk chunk;
    chunk.tiles = (G_Tile*)malloc(sizeof(G_Tile)*CHUNK_SIZE*CHUNK_SIZE);
    FILE *file = fopen("data/c.000.tfoy", "wb");

    for (i = 0; i < 256; i += 1) {
      permutations[i] = permutations[i+255] = i;
    }
    
    for (i = 0; i < 1024; i += 1) {
      int ii = G_RandomNumber(0, 511);
      permutations[i%512] += permutations[ii];
      permutations[ii] = permutations[i%512]-permutations[ii];
      permutations[i%512] -= permutations[ii];	
    }

    for (q = 0; q < 257; q += 1) {
      if (q == 256) {
        fclose(file);
        file = fopen("data/c.001.tfoy", "wb");
      }
      for (i = 0; i < CHUNK_SIZE*CHUNK_SIZE; i += 1) {
        double e = G_NoiseOctaveSum(i%CHUNK_SIZE+(q%16)*CHUNK_SIZE, i/CHUNK_SIZE+(q/16)*CHUNK_SIZE);

        if (q == 256) {
          e = G_NoiseOctaveSum(CHUNK_SIZE*FILE_CHUNK_SIZE+i%CHUNK_SIZE, i/CHUNK_SIZE);
        }

        e *= 1.1;

        if (e > 0.9) {
          chunk.tiles[i].tile = LAVA;
        } else if (e > 0.7) {
          if (e > 0.86) {
            chunk.tiles[i].tile = WALL;
          } else if (e > 0.83) {
            chunk.tiles[i].tile = FUNGUS;
          } else {
            chunk.tiles[i].tile = STONE;
          }
        } else if (e > 0.5) {
          if (e > 0.65) {
            if (G_RandomNumber(0, 7) < 10*e) {
              chunk.tiles[i].tile = TALL_GRASS;
            } else {
              chunk.tiles[i].tile = GRASS;
            }
          } else {
            if (G_RandomNumber(0, 7) < 10*e) {
              chunk.tiles[i].tile = GRASS;
            } else {
              chunk.tiles[i].tile = GROUND;
            }
          }
        } else if (e > 0.4) {
          chunk.tiles[i].tile = GROUND;
        } else if (e > 0.37) {
          chunk.tiles[i].tile = SAND;
        } else {
          chunk.tiles[i].tile = WATER;
        }
      }

      encoded = G_EncodeChunk(chunk.tiles, &len);
      G_IntToChar(len, elen, 1);
      
      fwrite(elen, 1, 4, file);
      fwrite(encoded, 1, len, file);
      free(encoded);
    }

    fclose(file);
    */
  }

  return 0;
}

int G_Update(void *data) {
  int status, x, y, dx, dy;

  if (SDL_GetTicks()-game_info.timer > UPDATE_DELAY) {
    G_UpdateInfrequent();
  }

  if (active_scene) {
    active_scene->view.xp = active_scene->view.x;
    active_scene->view.yp = active_scene->view.y;

    threads[WORKER_THREAD_A] = SDL_CreateThread(G_UpdateTransitions, "game-transition-updating", NULL);

    if (game_info.phys[SDL_SCANCODE_Z] ||
        game_info.phys[SDL_SCANCODE_H] ||
        game_info.phys[SDL_SCANCODE_J] ||
        game_info.phys[SDL_SCANCODE_K] ||
        game_info.phys[SDL_SCANCODE_L]) {
      game_info.phys[SDL_SCANCODE_Z] = 0;
      game_info.full = 1;
    }

    G_UpdateEntities(NULL);
    G_FocusView(&active_scene);
    G_EntityPos(&(active_scene->focus), &x, &y);
    G_GenerateFOV(x, y, COLS, NULL, &G_MakeVisible);

    threads[WORKER_THREAD_B] = SDL_CreateThread(G_UpdateLightmap, "game-lightmap-updating", NULL);
    threads[WORKER_THREAD_C] = SDL_CreateThread(G_RenderEntities, "game-entity-render-updating", NULL);

    SDL_WaitThread(threads[WORKER_THREAD_A], &status);
    SDL_WaitThread(threads[WORKER_THREAD_B], &status);
    SDL_WaitThread(threads[WORKER_THREAD_C], &status);

    dx = active_scene->view.x-active_scene->view.xp;
    dy = active_scene->view.y-active_scene->view.yp;

    int
      bx = (dx < 0) ? DCOLS : 0,
      by = (dy < 0) ? DROWS : 0,
      ix = (dx < 0) ? -1 : 1,
      iy = (dy < 0) ? -1 : 1;
    boolean ex = 0, ey = 0;

    for (y = by; ey != 1; y += iy) {
      ex = 0;
      for (x = bx; ex != 1; x += ix) {
        Tile tile = ERROR_TILE;

        if (!(game_info.redraw) &&
            (x+dx >= 0) && (x+dx < DCOLS) &&
            (y+dy >= 0) && (y+dy < DROWS)) {
          tile = tilemap[x+dx+DCOLS_OFFSET][y+dy+DROWS_OFFSET].layers[BASE_LAYER];
        } else {
          tile = G_SceneGetTile(&active_scene, x+active_scene->view.x, y+active_scene->view.y);
        }

        tilemap[x+DCOLS_OFFSET][y+DROWS_OFFSET].layers[BASE_LAYER] = tile;

        if (tilemap[x+DCOLS_OFFSET][y+DROWS_OFFSET].layers[PROPOGATE_LAYER] != NOTHING) {
          G_TileUpdate(tilemap[x+DCOLS_OFFSET][y+DROWS_OFFSET].layers[PROPOGATE_LAYER], x, y);
        } else {
          G_TileUpdate(tile, x, y);
        }

        if (dx < 0) {
          if (x <= 0) {
            ex = 1;
          }
        } else {
          if (x >= DCOLS-1) {
            ex = 1;
          }
        }
      }

      if (dy < 0) {
        if (y <= 0) {
          ey = 1;
        }
      } else {
        if (y >= DROWS-1) {
          ey = 1;
        }
      }
    }

    active_scene->step += game_info.full;
    game_info.redraw = 0;
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
            // escape pressed
					} else {
						if (game_info.phys[game_info.event.key.keysym.scancode] == 0) {
							game_info.phys[game_info.event.key.keysym.scancode] = SDL_GetTicks();
						}
					}
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

          if (game_info.mouse_x < game_info.display_x) {
					  game_info.mouse_x = game_info.display_x;
          } else if (game_info.mouse_x >= game_info.display_x+game_info.display_w) {
					  game_info.mouse_x = game_info.display_x+game_info.display_w-game_info.tile_w;
          }

          if (game_info.mouse_y < game_info.display_y) {
					  game_info.mouse_y = game_info.display_y;
          } else if (game_info.mouse_y >= game_info.display_y+game_info.display_h) {
					  game_info.mouse_y = game_info.display_y+game_info.display_h-game_info.tile_h;
          }

          game_info.mouse_x = game_info.mouse_x-(game_info.mouse_x-game_info.display_x)%game_info.tile_w;
          game_info.mouse_y = game_info.mouse_y-(game_info.mouse_y-game_info.display_y)%game_info.tile_h;
				}
				break;
			case SDL_MOUSEBUTTONDOWN:
				{
					if (game_info.event.button.button == SDL_BUTTON_LEFT) {
						game_info.mouse_lb = (game_info.mouse_lb == 0) ? SDL_GetTicks() : game_info.mouse_lb;
            

            if (game_info.phys[SDL_SCANCODE_LCTRL]) {
              int x = (game_info.mouse_x-game_info.display_x)/game_info.tile_w-DCOLS_OFFSET+active_scene->view.x,
                  y = (game_info.mouse_y-game_info.display_y)/game_info.tile_h-DROWS_OFFSET+active_scene->view.y;
              G_Tile tile;
              tile.tile = WALL;

              //G_SceneSetGTile(&active_scene, tile, x, y);
              //G_ElementPropogateCreate(x, y, 200, 32, 1, BASIC_FIRE, IS_BURNING, FLAMMABLE, AL, &G_BurnTile);
              //G_ElementDiffuseCreate(x, y, G_RandomNumber(2048, 8192), 32, WATER, PROPOGATE_LAYER, IS_EXTINGUISHING, AL, NULL);
              G_ElementExplodeCreate(x, y, G_RandomNumber(8, 128), 4, BASIC_FIRE, 0, AL, &G_BurnTile);

            } else {
              int x = (game_info.mouse_x-game_info.display_x)/game_info.tile_w,
                  y = (game_info.mouse_y-game_info.display_y)/game_info.tile_h;

              Tile tile = G_GetTile(tilemap[x][y].layers);

              printf("Tile Information\n");
              printf("tile        %i  \n", tile);
              printf("is visible  %i  \n", vismap[x][y]);
              printf("light           \n");
              printf("+ intensity %i  \n", lightmap[x][y].count);
              printf("+ count     %i  \n", lightmap[x][y].count);
              printf("+ green     %i  \n", lightmap[x][y].r);
              printf("+ blue      %i  \n", lightmap[x][y].r);
              printf("+ red       %i  \n", lightmap[x][y].r);

              if (G_QTreeEntityFind(&(active_scene->collision), CREATURE_LAYER, x+active_scene->view.x-DCOLS_OFFSET, y+active_scene->view.y-DROWS_OFFSET)) {
                printf("Creature here.\n");
              }

              if (G_QTreeEntityFind(&(active_scene->collision), ORNAMENT_LAYER, x+active_scene->view.x-DCOLS_OFFSET, y+active_scene->view.y-DROWS_OFFSET)) {
                printf("Ornament here.\n");
              }
            }
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
      if (tilemap[x][y].layers[UI_LAYER] != NOTHING) {
        vismap[x][y] = 1;
      } else if ((lightmap[x][y].count == 1) &&
                (lightmap[x][y].r == 0) &&
                (lightmap[x][y].g == 0) &&
                (lightmap[x][y].b == 0)) {
        vismap[x][y] = 0;
      }

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

      console.tilemap[x][y].fg = tilemap[x][y].fg;
      console.tilemap[x][y].bg = tilemap[x][y].bg;

      vismap[x][y] = 0;
      lightmap[x][y].r = (active_scene) ? (active_scene->ambient.r) : 0;
      lightmap[x][y].g = (active_scene) ? (active_scene->ambient.g) : 0;
      lightmap[x][y].b = (active_scene) ? (active_scene->ambient.b) : 0;
      lightmap[x][y].count = 1;
      lightmap[x][y].id.value = -1;
      lightmap[x][y].intensity = 255;

      for (z = 0; z < BASE_LAYER; z += 1) {
        tilemap[x][y].layers[z] = NOTHING;
      }

      tilemap[x][y].fchange = 0;
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

    if (transition->is == tile.tile) {
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
  return game_info.frame_count/((SDL_GetTicks()-game_info.timer)/(1000.0));
}

float G_GetSlope(int scene_x, int scene_y, int x, int y, int dx, int dy, int invert, boolean top) {
  boolean above = 0, right = 0;

  if (top) {
    if (invert) {
      right = G_SceneTileObstructs(&active_scene, scene_x+y*dx, scene_y+(x+1)*dy);
    } else {
      right = G_SceneTileObstructs(&active_scene, scene_x+(x+1)*dx, scene_y+y*dy);
    }
  
    return (!(right) ? (float)(y)/(x+0.9) : (float)(y)/(x+1.0));
  } else {
    if (invert) {
      right = G_SceneTileObstructs(&active_scene, scene_x+y*dx, scene_y+(x+1)*dy);
      above = G_SceneTileObstructs(&active_scene, scene_x+(y-1)*dx, scene_y+x*dy);
    } else {
      above = G_SceneTileObstructs(&active_scene, scene_x+x*dx, scene_y+(y-1)*dy);
      right = G_SceneTileObstructs(&active_scene, scene_x+(x+1)*dx, scene_y+y*dy);
    }

    return (!(right || above) ? (float)(y+0.9)/(x) : (float)(y+1.0)/(x));
  }
}

void G_Quit(void) {
	int i;
  
  G_TreeDestroy(&chunks);
  G_TreeDestroy(&scenes);

	for (i = 0; i < TEXTURE_COUNT; i += 1) {
		SDL_DestroyTexture(game_info.textures[i]);
	}

  SDL_DestroyMutex(fmutex);
	SDL_DestroyTexture(game_info.buffers[0]);
	SDL_DestroyTexture(game_info.buffers[1]);
	SDL_DestroyRenderer(game_info.renderer);
  SDL_DestroyWindow(game_info.window);
	SDLNet_Quit();
	SDL_Quit();
}

void G_UpdateBegin(void) {
  game_info.frame_count += 1;

  if (game_info.phys[SDL_SCANCODE_T]) {
    G_Scene *scene;
    G_TreeNode *node;

    if (active_scene->id.value == menu_id.value) {
      node = G_TreeNodeFind(&scenes, overworld_id.value);
      
      if (node) {
        scene = (G_Scene*)(node->data);
        G_SceneChange(&scene);
      }
    } else {
      node = G_TreeNodeFind(&scenes, menu_id.value);

      if (node) {
        scene = (G_Scene*)(node->data);
        G_SceneChange(&scene);
      }
    }

    game_info.phys[SDL_SCANCODE_T] = 0;
  }

  G_CopyBuffer(NULL);
}

void G_UpdateEnd(void) {
  if (active_scene) {
    G_TreeNode *node = active_scene->del_buffer;

    while (node) {
      G_TreeNode *actual = G_TreeNodeFind(&(active_scene->entities), node->key);

      if (actual) {
        G_TreeNodeDelete(&(active_scene->entities), &actual);
      }

      active_scene->del_buffer = node->left;

      free(node);
      
      node = active_scene->del_buffer;
    }

    node = active_scene->ins_buffer;

/* insert all nodes that were created */

    while (node) {
      active_scene->ins_buffer = node->left;
      node->left = NULL;

      G_TreeNodeInsert(&(active_scene->entities), &node);

      node = active_scene->ins_buffer;
    }
  }
}

void G_UpdateInfrequent(void) {
  /* output frames per second */
  printf("FPS          %u\n", G_GetFPS());
  printf("Entity Count %i\n", active_scene->entities->size);

  game_info.frame_count = 0;
  game_info.timer = SDL_GetTicks();

  if ((active_scene) && (active_scene->persistent)) {
    int
      i, j, id,
      x = (active_scene->view.x+(active_scene->view.w/2))/CHUNK_SIZE,
      y = (active_scene->view.y+(active_scene->view.h/2))/CHUNK_SIZE;
    long int *data = NULL;
    G_TreeNode *node = NULL;

    SDL_LockMutex(fmutex);

    if (redraw_lockable) {
      redraw_lockable = 0;
      game_info.redraw = 1;
    }

    for (i = -2; i < 3; i += 1) {
      for (j = -2; j < 3; j += 1) {
        if ((x+j >= 0) && (x+j < active_scene->w) && (y+i >= 0) && (y+i < active_scene->h)) {
          id = x+j+(y+i)*active_scene->w;
          if (active_scene->chunks[id].status == NOT_LOADED) {
            active_scene->chunks[id].status = IS_LOADING;
            data = (long int*)malloc(sizeof(long int));
            node = (G_TreeNode*)malloc(sizeof(G_TreeNode));
            *data = active_scene->id.value;
            node->key = id;
            node->data = (void*)data;
            G_TreeNodeInsert(&chunks, &node);
          }
        }
      }
    }

    SDL_UnlockMutex(fmutex);

    G_QTreePurge(&(active_scene->collision));
  }
}

void G_ClearBuffers(void) {
  int x, y, z;

  for (y = 0; y < ROWS; y += 1) {
    for (x = 0; x < COLS; x += 1) {
      vismap[x][y] = 0;
      lightmap[x][y].r = (active_scene) ? (active_scene->ambient.r) : 0;
      lightmap[x][y].g = (active_scene) ? (active_scene->ambient.g) : 0;
      lightmap[x][y].b = (active_scene) ? (active_scene->ambient.b) : 0;
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

void G_GenerateFOV(int x, int y, int range, void *light, void (*func)(int*, int*, void*)) {
	if ((!active_scene) ||
      (x < 0) || (x >= active_scene->tw) ||
			(y < 0) || (y >= active_scene->th)) {
		return;
	}

	if (!G_SceneTileObstructs(&active_scene, x, y)) {
		G_Shadowcast(x, y, 1, 1, 1, range, 0, 0.0, 2.0, light, func);
		G_Shadowcast(x, y, 1, 1, 1, range, 1, 0.0, 2.0, light, func);
		G_Shadowcast(x, y, -1, 1, 1, range, 0, 0.0, 2.0, light, func);
		G_Shadowcast(x, y, -1, 1, 1, range, 1, 0.0, 2.0, light, func);
		G_Shadowcast(x, y, 1, -1, 1, range, 0, 0.0, 2.0, light, func);
		G_Shadowcast(x, y, 1, -1, 1, range, 1, 0.0, 2.0, light, func);
		G_Shadowcast(x, y, -1, -1, 1, range, 0, 0.0, 2.0, light, func);
		G_Shadowcast(x, y, -1, -1, 1, range, 1, 0.0, 2.0, light, func);
	}

	x -= active_scene->view.x;
	y -= active_scene->view.y;

	if ((x >= 0) && (x < active_scene->w) &&
			(y >= 0) && (y < active_scene->h)) {
		func(&x, &y, light);
	}
}

void G_GenerateFOVSimple(int x, int y, void *light, void (*func)(int*, int*, void*)) {
  int dx, dy, adj_x, adj_y;

  for (dy = -1; dy < 2; dy += 1) {
    for (dx = -1; dx < 2; dx += 1) {
      adj_x = x+dx-active_scene->view.x;
      adj_y = y+dy-active_scene->view.y;

      if ((adj_x >= 0) && (adj_x < active_scene->view.w) &&
          (adj_y >= 0) && (adj_y < active_scene->view.h)) {
        G_AddLight(&adj_x, &adj_y, light);
      }
    }
  }
}

void G_Shadowcast(int scene_x, int scene_y, int dx, int dy, int dist, int range, int invert, float start, float end, void *data, void (*func)(int*, int*, void*)) {
  int x, y, x_adj, y_adj;
  float top, mid, bot, width;
  boolean good = 0, is_solid = 0, was_solid = 0;

  while (dist <= range) {
    x = dist;
    y = 0;
    width = (1.0)/(dist);

    while (y <= dist) {
      top = y*width;
      mid = (y+0.5)*width;
      bot = (y+1.0)*width;

      if (((top <= end) || (mid <= end) || (bot <= end)) &&
          ((top >= start) || (mid >= start) || (bot >= start))) {
        if (invert) {
          x_adj = scene_x+(y*dx);
          y_adj = scene_y+(x*dy);
        } else {
          x_adj = scene_x+(x*dx);
          y_adj = scene_y+(y*dy);
        }

        was_solid = (y == 0) ? 0 : is_solid;
        is_solid = G_SceneTileObstructs(&active_scene, x_adj, y_adj);

        x_adj -= active_scene->view.x;
        y_adj -= active_scene->view.y;

        good = 1;

        if (is_solid) {
          if ((y == dist) || ((float)(y+1.0)/(x) > end)) {
            end = top;
          } else {
            if ((was_solid == 0) && (y < dist) && (y > 0)) {
              G_Shadowcast(scene_x, scene_y, dx, dy, dist+1, range, invert, start, top, data, func);
            }

            start = bot;
          }

          func(&x_adj, &y_adj, data);
        } else {
          if ((mid >= start) && (mid <= end) &&
              (((top >= start) && (top <= end)) ||
              ((bot >= start) && (bot <= end)))) {
            func(&x_adj, &y_adj, data);
          }
        }
      } else if (bot > end) {
        y = dist;
      }

      y += 1;
    }

    if (!good) {
      return;
    }
    
    dist = (start < end) ? dist+1 : range+1;
  }
}

void G_AddLight(int *x, int *y, void *data) {
	assert((x) && (y) && (data) && (active_scene));

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
	} else if (intensity < 0.01) {
		return;
	}

	G_EntityPos(&(active_scene->focus), &fx, &fy);

	if ((lx >= DCOLS_OFFSET) && (lx < DCOLS_OFFSET+DCOLS) &&
			(ly >= DROWS_OFFSET) && (ly < DROWS_OFFSET+DROWS)) {
		if ((vismap[lx][ly]) && (lightmap[lx][ly].id.value != light->id.value)) {
			if (!(G_SceneTileObstructs(&active_scene, lx_adj, ly_adj)) ||
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
 	node.id.value = game_info.id.value+x+y*active_scene->tw;

  G_GenerateFOV(x, y, node.intensity, &node, &G_AddLight);
}

void G_MakeVisible(int *x, int *y, void *data) {
	assert((x) && (y));

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

  if ((focus) && (s->view.follows)) {
    int x, y;

    G_EntityPos(&focus, &x, &y);

    s->view.x = x-s->view.w/2;
    s->view.y = y-s->view.h/2;

    s->view.x = (s->view.x < 0) ? 0 : ((s->view.x+s->view.w < s->tw) ? s->view.x : s->tw-s->view.w-1);
    s->view.y = (s->view.y < 0) ? 0 : ((s->view.y+s->view.h < s->th) ? s->view.y : s->th-s->view.h-1);
  }
}

void G_ResizeDPort(int x, int y, int w, int h) {
  if ((x+w <= COLS) && (y+h <= ROWS)) {
    DCOLS_OFFSET = x;
    DCOLS = w;
    DROWS_OFFSET = y;
    DROWS = h;

    if (active_scene) {
      active_scene->view.w = DCOLS;
      active_scene->view.h = DROWS;
    }

    G_ClearBuffers();
  }
}

void G_InitializeMenu(G_Scene **scene) {
  assert((scene) && (*scene));

  int x, y;
  G_Tile tile;
  G_Scene *s = *scene;
  G_Entity *entity = NULL;
  G_UIWidget *widget = NULL;
  G_UIComponent *ui = NULL;

  s->chunks[0].tiles = (G_Tile*)malloc(sizeof(G_Tile)*CHUNK_SIZE*CHUNK_SIZE);
  s->chunks[0].status = IS_LOADED;

  s->ambient.r = 255;
  s->ambient.g = 255;
  s->ambient.b = 255;

  for (y = 0; y < ROWS; y += 1) {
    for (x = 0; x < COLS; x += 1) {
      if (title[y][x] == 'B') {
        tile.tile = SOLID_BLACK;
      } else if ((x > 0) && (title[y][x-1] == 'B')) {
        tile.tile = SOLID_WHITE;
      } else if (title[y][x] == ' ') {
        tile.tile = SOLID_MAGENTA;
      } else {
        tile.tile = title[y][x];
      }

      G_SceneSetGTile(scene, tile, x, y);
    }
  }

  entity = G_EntityCreate();
  ui = (G_UIComponent*)G_EntityComponentInsert(&entity, UI_COMPONENT);

  ui->root = G_UIWindowCreate(35, 25, 11, 5, 0, 1);

  widget = G_UIWidgetCreate(36, 26, 9, 1, 0, 9, (void(*)(void*))(&G_SceneChange), &(G_TreeNodeFind(&scenes, 0)->data), "PLAY GAME");
  G_UIWidgetHotkey(&widget, SDL_SCANCODE_P, 0);
  G_UIWindowAddWidget(&(ui->root), &widget);

  widget = G_UIWidgetCreate(36, 28, 9, 1, 0, 9, &G_UIWidgetQuitGame, NULL, "EXIT GAME");
  G_UIWidgetHotkey(&widget, SDL_SCANCODE_E, 0);
  G_UIWindowAddWidget(&(ui->root), &widget);

  G_SceneEntityInsert(scene, &entity);
}

G_Position G_GetDirectionComponents(DirectionFlags flag) {
  G_Position position;

  position.x = ((flag & 0x83) ? 1 : 0)-((flag & 0x38) ? 1 : 0);
  position.y = ((flag & 0xE0) ? 1 : 0)-((flag & 0x0E) ? 1 : 0);

  return position;
}

boolean G_CellChanged(int x, int y, int a, int b) {
  int range;
  TileFlag flags;

  if (tilemap[x][y].fchange) {
    return 1;
  }

  if (vismap[x][y] != console.vismap[a][b]) {
    return 1;
  }

  if ((vismap[x][y] == 1) && (G_GetTile(tilemap[x][y].layers) != G_GetTile(console.tilemap[a][b].layers))) {
    return 1;
  }

  if (tilemap[x][y].layers[DIFFUSE_LAYER] != NOTHING) {
    return 1;
  }

  if (vismap[x][y] == 1) {
    flags = G_TileFlags(G_GetTile(tilemap[x][y].layers));

    if (flags & FLICKERS) {
      if (flags & FLICKERS_QUICK) {
        range = FLICKER_RANGE/2;
      } else if (flags & FLICKERS_SLOW) {
        range = FLICKER_RANGE*2;
      } else {
        range = FLICKER_RANGE;
      }

      if (G_RandomNumber(0, range) == 0) {
        return 1;
      }
    }
  }

  if ((tilemap[x][y].fg != console.tilemap[a][b].fg) ||
      (tilemap[x][y].bg != console.tilemap[a][b].bg)) {
    return 1;
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
  assert(active_scene);

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
  TileLayer layer = UI_LAYER;

  for (; layer != BASE_LAYER; layer += 1) {
    if (layers[layer] != NOTHING) {
      return layers[layer];
    }
  }

  return layers[BASE_LAYER];
}
