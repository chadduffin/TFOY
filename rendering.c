#include "yendor.h"
#include "globals.h"

int G_Render(void *data) {
	SDL_SetRenderTarget(game_info.renderer, game_info.buffers[game_info.target_buffer]);
  SDL_RenderClear(game_info.renderer);

  SDL_RenderCopy(game_info.renderer, game_info.buffers[!game_info.target_buffer], &(console.src), &(console.dst));

  G_RenderChanges();

	SDL_SetRenderTarget(game_info.renderer, NULL);	
	SDL_RenderCopy(game_info.renderer, game_info.buffers[game_info.target_buffer], NULL, NULL);
	game_info.target_buffer = (game_info.target_buffer == 0) ? 1 : 0;	

  G_RenderLightmap();

	SDL_RenderPresent(game_info.renderer);

  return 0;
}

void G_RenderChanges(void) {
  int x, y, r, g, b;
  SDL_Rect src, dst;
  Tile tile;
  G_Color fg, bg;

	dst.w = game_info.tile_w;
	dst.h = game_info.tile_h;
	src.w = TILE_SOURCE_WIDTH;
	src.h = TILE_SOURCE_HEIGHT;

  for (y = 0; y < ROWS; y += 1) {
    for (x = 0; x < COLS; x += 1) {
      if (console.changed[x][y]) {
        tile = G_GetTile(console.tilemap[x][y].layers);
				dst.x = game_info.display_x+(x*game_info.tile_w);
				dst.y = game_info.display_y+(y*game_info.tile_h);

        if ((console.vismap[x][y]) && (tile != NOTHING)) {
  				if (console.tilemap[x][y].fg == &bad_color) {
            fg = (tile < NOTHING) ? white : *(tile_info[tile-NOTHING].fg);
          } else {
            fg = *(console.tilemap[x][y].fg);
          }

  				if (console.tilemap[x][y].bg == &bad_color) {
            bg = (tile < NOTHING) ? black : *(tile_info[tile-NOTHING].bg);

            if (tile < NOTHING) {
              bg = black;
            } else {
              if (tile_info[tile-NOTHING].bg == &bad_color) {
                bg = *(tile_info[console.tilemap[x][y].layers[BASE_LAYER]-NOTHING].bg);
              } else {
                bg = *(tile_info[tile-NOTHING].bg);
              }
            }
          } else {
            bg = *(console.tilemap[x][y].bg);
          }

          G_EvaluateColor(bg, &r, &g, &b, G_TileFlags(tile) & FLICKERS);

					SDL_SetRenderDrawColor(game_info.renderer, r, g, b, 255);
					SDL_RenderFillRect(game_info.renderer, &dst);	

          if (&fg != &bg) {
						src = G_TileSource(tile);
						G_EvaluateColor(fg, &r, &g, &b, G_TileFlags(tile) & FLICKERS);
						SDL_SetTextureColorMod(game_info.textures[0], r, g, b);
						SDL_RenderCopy(game_info.renderer, game_info.textures[0], &src, &dst);
						SDL_SetTextureColorMod(game_info.textures[0], 255, 255, 255);
          }
        } else {
					SDL_SetRenderDrawColor(game_info.renderer, 0, 0, 0, 255);
					SDL_RenderFillRect(game_info.renderer, &dst);	
        }
      }
    }
  }

	SDL_SetRenderDrawColor(game_info.renderer, 0, 0, 0, 255);
}

void G_RenderLightmap(void) {
	int x, y, r, g, b, max, intensity;
	float scale;
	SDL_Rect dst;

	dst.w = game_info.tile_w;
	dst.h = game_info.tile_h;

	for (y = DROWS_OFFSET; y < DROWS_OFFSET+DROWS; y += 1) {
		for (x = DCOLS_OFFSET; x < DCOLS_OFFSET+DCOLS; x += 1) {
			if (console.lightmap[x][y].count != 0) {
				max = (console.lightmap[x][y].r > console.lightmap[x][y].g) ? console.lightmap[x][y].r : console.lightmap[x][y].g;
				max = (max > console.lightmap[x][y].b) ? max : console.lightmap[x][y].b;
				scale = (float)(max)/255;

				if (scale > 1.0) {
					console.lightmap[x][y].r *= (1.0)/(scale);
					console.lightmap[x][y].g *= (1.0)/(scale);
					console.lightmap[x][y].b *= (1.0)/(scale);
				}
			}

			if (console.vismap[x][y]) {
				r = console.lightmap[x][y].r;
				g = console.lightmap[x][y].g;
				b = console.lightmap[x][y].b;
				intensity = console.lightmap[x][y].intensity;
				dst.x = x*game_info.tile_w+game_info.display_x;
				dst.y = y*game_info.tile_h+game_info.display_y;
	
				intensity = (intensity < 0) ? 0 : intensity;
				intensity = (intensity > 255) ? 255 : intensity;

				if ((intensity == 0) && (r == 0) && (g == 0) && (b == 0)) {
					SDL_SetRenderDrawColor(game_info.renderer, 0, 0, 0, 255);
					SDL_RenderFillRect(game_info.renderer, &dst);
				} else {	
					SDL_SetRenderDrawColor(game_info.renderer, r, g, b, 255-intensity);
					SDL_SetRenderDrawBlendMode(game_info.renderer, SDL_BLENDMODE_MOD);
					SDL_RenderFillRect(game_info.renderer, &dst);
			
					SDL_SetRenderDrawColor(game_info.renderer, 0, 0, 0, 255);
					SDL_SetRenderDrawBlendMode(game_info.renderer, SDL_BLENDMODE_NONE);
				}
			}
		}
	}
}

void G_UpdateRenderingInfo(void) {
	float
		tile_ratio = ((float)(COLS*TILE_SOURCE_WIDTH))/(ROWS*TILE_SOURCE_HEIGHT),
		window_ratio = ((float)game_info.window_w)/game_info.window_h;

	if (tile_ratio > window_ratio) {
		game_info.display_w = game_info.window_w;
		game_info.display_h = game_info.window_w/tile_ratio;
	} else if (tile_ratio < window_ratio) {
		game_info.display_w = game_info.window_h*tile_ratio;
		game_info.display_h = game_info.window_h;
	} else {
		game_info.display_w = game_info.window_w;
		game_info.display_h = game_info.window_h;
	}

	game_info.tile_w = game_info.display_w/COLS;
	game_info.tile_h = game_info.display_h/ROWS;

	game_info.display_w = game_info.tile_w*COLS;
	game_info.display_h = game_info.tile_h*ROWS;

	game_info.display_x = (game_info.window_w-game_info.display_w)/2;
	game_info.display_y = (game_info.window_h-game_info.display_h)/2;

	if (game_info.running == 0) {
		printf("Initializing graphics .. PENDING.\n");
		printf("> window width ......... %d.\n", game_info.window_w);
		printf("> window height ........ %d.\n", game_info.window_h);
		printf("> dport x .............. %d.\n", game_info.display_x);
		printf("> dport y .............. %d.\n", game_info.display_y);
		printf("> dport width .......... %d.\n", game_info.display_w);
		printf("> dport height ......... %d.\n", game_info.display_h);
		printf("> tile width ........... %d.\n", game_info.tile_w);
		printf("> tile height .......... %d.\n", game_info.tile_h);
		printf("Initializing graphics .. SUCCESS.\n");
	}
}

void G_EvaluateColor(G_Color color, int *r, int *g, int *b, boolean flicker) {
  *r = color.r;
  *g = color.g;
  *b = color.b;

  if (flicker) {
    int r_rand, g_rand, b_rand;

    r_rand = (color.r_rand == 0) ? 1 : color.r_rand;
    g_rand = (color.g_rand == 0) ? 1 : color.g_rand;
    b_rand = (color.b_rand == 0) ? 1 : color.b_rand;

    *r = (*r)+(2*G_RandomNumber(0, r_rand))-r_rand;
    *g = (*g)+(2*G_RandomNumber(0, g_rand))-g_rand;
    *b = (*b)+(2*G_RandomNumber(0, b_rand))-b_rand;
  }

  *r = (*r > 255) ? (255) : ((*r < 0) ? (0) : (*r));
  *g = (*g > 255) ? (255) : ((*g < 0) ? (0) : (*g));
  *b = (*b > 255) ? (255) : ((*b < 0) ? (0) : (*b));
}
