#include "yendor.h"
#include "globals.h"

/*
** FUNCTIONS
*/

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
		printf("> dport x .............. %d.\n", game_info.display_y);
		printf("> dport y .............. %d.\n", game_info.display_x);
		printf("> dport width .......... %d.\n", game_info.display_w);
		printf("> dport height ......... %d.\n", game_info.display_h);
		printf("> tile width ........... %d.\n", game_info.tile_w);
		printf("> tile height .......... %d.\n", game_info.tile_h);
		printf("Initializing graphics .. SUCCESS.\n");
	}
}

void G_Render(void) {
	SDL_SetRenderTarget(game_info.renderer, game_info.buffers[game_info.target_buffer]);
	SDL_RenderClear(game_info.renderer);

	if (location != menu) {
		G_RenderSalvage();
		
		if (location->focus != NULL) {
			int
				x,
				y;

			G_EntityPos(&(location->focus), &x, &y);
			G_GenerateFOV(x, y, NULL, &G_MarkVisible);
		}

		G_ClearAndDecrement();
	} else {
		SDL_RenderCopy(game_info.renderer, game_info.buffers[!game_info.target_buffer], NULL, NULL);
	}	

	G_LoopEntities(ANY_ENTITY, &G_EntityRender);
  G_CheckTileTransitions(&location);
	G_RenderChanges();

	SDL_SetRenderTarget(game_info.renderer, NULL);	
	SDL_RenderCopy(game_info.renderer, game_info.buffers[game_info.target_buffer], NULL, NULL);
	game_info.target_buffer = (game_info.target_buffer == 0) ? 1 : 0;	

	if (location != menu) {
		if (SDL_GetTicks()-last_flicker > FLICKER_RATE) {
			G_RenderFlicker(0.0001);
			last_flicker += rand()%FLICKER_RATE;
		}
		G_RenderLightmap();
	}

	SDL_RenderPresent(game_info	.renderer);
}

void G_LightRender(void) {
	SDL_RenderCopy(game_info.renderer, game_info.buffers[!game_info.target_buffer], NULL, NULL);

	if (location != menu) {
		if (SDL_GetTicks()-last_flicker > FLICKER_RATE) {
			G_RenderFlicker(0.0001);
			last_flicker += rand()%FLICKER_RATE;
		}

		G_RenderLightmap();
	}

	SDL_RenderPresent(game_info.renderer);
}

void G_RenderSalvage(void) {
	G_View view = *(G_SceneView(&location));
	int
		x,
		y,
		xdiff = location->view.x-location->view.xp,
		ydiff = location->view.y-location->view.yp;
	SDL_Rect
		src,
		dst;
	dst.x = src.x = game_info.display_x;
	dst.y = src.y = game_info.display_y;
	dst.w = src.w = DCOLS_OFFSET*game_info.tile_w;
	dst.h = src.h = ROWS*game_info.tile_h;
	SDL_RenderCopy(game_info.renderer, game_info.buffers[!game_info.target_buffer], &src, &dst);
	dst.x = src.x = game_info.display_x+DCOLS_OFFSET*game_info.tile_w;
	dst.y = src.y = game_info.display_y;
	dst.w = src.w = (COLS-DCOLS_OFFSET)*game_info.tile_w;
	dst.h = src.h = DCOLS_OFFSET*game_info.tile_h;
	SDL_RenderCopy(game_info.renderer, game_info.buffers[!game_info.target_buffer], &src, &dst);
	dst.x = src.x = game_info.display_x+DCOLS_OFFSET*game_info.tile_w;
	dst.y = src.y = game_info.display_y+(DROWS_OFFSET+DROWS)*game_info.tile_h;
	dst.w = src.w = (COLS-DCOLS_OFFSET)*game_info.tile_w;
	dst.h = src.h = (ROWS-DROWS-DROWS_OFFSET)*game_info.tile_h;
	SDL_RenderCopy(game_info.renderer, game_info.buffers[!game_info.target_buffer], &src, &dst);
	dst.x = src.x = game_info.display_x+(DCOLS+DCOLS_OFFSET)*game_info.tile_w;
	dst.y = src.y = game_info.display_y+(DROWS_OFFSET)*game_info.tile_h;
	dst.w = src.w = (COLS-DCOLS-DCOLS_OFFSET)*game_info.tile_w;
	dst.h = src.h = (DROWS)*game_info.tile_h;
	SDL_RenderCopy(game_info.renderer, game_info.buffers[!game_info.target_buffer], &src, &dst);

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
		src.x *= game_info.tile_w;
		src.y *= game_info.tile_h;
		src.w *= game_info.tile_w;
		src.h *= game_info.tile_h;
		
		src.x += game_info.display_x+(DCOLS_OFFSET)*game_info.tile_w;
		src.y += game_info.display_y+(DROWS_OFFSET)*game_info.tile_h;

		dst.x = (xdiff < 0) ? (-xdiff) : 0;
		dst.y = (ydiff < 0) ? (-ydiff) : 0;
		dst.x *= game_info.tile_w;
		dst.y *= game_info.tile_h;
		dst.w = src.w;
		dst.h = src.h;

		dst.x += game_info.display_x+(DCOLS_OFFSET)*game_info.tile_w;
		dst.y += game_info.display_y+(DROWS_OFFSET)*game_info.tile_h;

		SDL_RenderCopy(game_info.renderer, game_info.buffers[!game_info.target_buffer], &src, &dst);

		src.x = (src.x-game_info.display_x)/game_info.tile_w;
		src.y = (src.y-game_info.display_y)/game_info.tile_h;
		dst.x = (dst.x-game_info.display_x)/game_info.tile_w;
		dst.y = (dst.y-game_info.display_y)/game_info.tile_h;
		dst.w /= game_info.tile_w;
		dst.h /= game_info.tile_h;

		if ((xdiff != 0) || (ydiff != 0)) {
			int
				x_start = (xdiff > 0) ? 0 : dst.w-1,
				y_start = (ydiff > 0) ? 0 : dst.h-1,
				x_end = (xdiff > 0) ? dst.w : -1,
				y_end = (ydiff > 0) ? dst.h : -1,
				x_it = (xdiff > 0) ? 1 : -1,
				y_it = (ydiff > 0) ? 1 : -1;

			for (y = y_start; y != y_end; y += y_it) {
				for (x = x_start; x != x_end; x += x_it) {
					dmatrix[dst.x+x][dst.y+y] = dmatrix[src.x+x][src.y+y];
				}
			}
		}
	} else {
		dst.x = 0;
		dst.y = 0;
		dst.w = 0;
		dst.h = 0;
	}

	int
		x_offset = DCOLS_OFFSET-view.x,
		y_offset = DROWS_OFFSET-view.y;

	for (y = DROWS_OFFSET; y < (DROWS+DROWS_OFFSET); y += 1) {
		for (x = DCOLS_OFFSET; x < (DCOLS+DCOLS_OFFSET); x += 1) {
			if ((x < dst.x) || (x >= dst.x+dst.w) ||
					(y < dst.y) || (y >= dst.y+dst.h)) {
				dmatrix[x][y].changed = 1;
				dmatrix[x][y].tile = G_SceneTile(x-x_offset, y-y_offset);
			}
		}
	}
}

void G_RenderChanges(void) {
	int
		x,
		y;
	SDL_Rect
		src,
		dst;
	Tile to_draw;
	dst.w = game_info.tile_w;
	dst.h = game_info.tile_h;
	src.w = TILE_SOURCE_WIDTH;
	src.h = TILE_SOURCE_HEIGHT;

	for (y = 0; y < ROWS; y += 1) {
		for (x = 0; x < COLS; x += 1) {
			to_draw = G_CellToTile(x, y);
			G_Color
				fg = (to_draw < 256) ? dmatrix[x][y].fg : *(descriptor_tiles[to_draw-256].fg),
				bg = (to_draw < 256) ? dmatrix[x][y].bg : *(descriptor_tiles[to_draw-256].bg);

			if (dmatrix[x][y].changed == 1) {
				dst.x = game_info.display_x+(x*game_info.tile_w);
				dst.y = game_info.display_y+(y*game_info.tile_h);
	
				if (dmatrix[x][y].visible) {
					int
						r,
						g,
						b;
	
					G_EvaluateRGB(bg, &r, &g, &b, G_TileFlickerOnce(to_draw));
	
					SDL_SetRenderDrawColor(game_info.renderer, r, g, b, 255);
					SDL_RenderFillRect(game_info.renderer, &dst);	
	
					if (&fg != &bg) {
						G_TileSource(to_draw, &src);
						G_EvaluateRGB(fg, &r, &g, &b, G_TileFlickers(to_draw));
						SDL_SetTextureColorMod(game_info.textures[0], r, g, b);
						SDL_RenderCopy(game_info.renderer, game_info.textures[0], &src, &dst);
						SDL_SetTextureColorMod(game_info.textures[0], 255, 255, 255);
					}
				} else {
					SDL_SetRenderDrawColor(game_info.renderer, 0, 0, 0, 255);
					SDL_RenderFillRect(game_info.renderer, &dst);	
				}
				dmatrix[x][y].changed = 0;
			}
		}
	}

	location->view.unchanged = 1;
	SDL_SetRenderDrawColor(game_info.renderer, 0, 0, 0, 255);
}

void G_RenderFlicker(float frequency) {
	int
		x,
		y,
		r,
		g,
		b;
	SDL_Rect
		dst,
		src;
	Tile tile;
	dst.w = src.w = game_info.tile_w;
	dst.h = src.h = game_info.tile_h;

  SDL_SetRenderTarget(game_info.renderer, game_info.buffers[!game_info.target_buffer]);

	for (y = DROWS_OFFSET; y < DROWS_OFFSET+DROWS; y += 1) {
		for (x = DCOLS_OFFSET; x < DCOLS_OFFSET+DCOLS; x += 1) {
			if ((dmatrix[x][y].visible == 0) || (dmatrix[x][y].tile != G_CellToTile(x, y))) {
				continue;
			}
			if (G_TileFlickers(dmatrix[x][y].tile)) {
				if (frequency > (float)(rand()%100)/100) {
					tile = dmatrix[x][y].tile;

					dst.x = x*game_info.tile_w+game_info.display_x;
					dst.y = y*game_info.tile_h+game_info.display_y;

					G_EvaluateRGB(G_TileBackground(tile), &r, &g, &b, 1);

					SDL_SetRenderDrawColor(game_info.renderer, r, g, b, 255);
					SDL_RenderFillRect(game_info.renderer, &dst);	

					G_EvaluateRGB(G_TileForeground(tile), &r, &g, &b, 1);

					G_TileSource(tile, &src);
					SDL_SetTextureColorMod(game_info.textures[0], r, g, b);
					SDL_RenderCopy(game_info.renderer, game_info.textures[0], &src, &dst);
					SDL_SetTextureColorMod(game_info.textures[0], 255, 255, 255);
				}
			}
		}
	}

  SDL_SetRenderTarget(game_info.renderer, NULL);
	SDL_SetRenderDrawColor(game_info.renderer, 0, 0, 0, 255);
}

void G_RenderLightmap(void) {
	int
		x,
		y,
		r,
		g,
		b,
		max,
		intensity;
	float scale;
	SDL_Rect dst;
	dst.w = game_info.tile_w;
	dst.h = game_info.tile_h;

	for (y = DROWS_OFFSET; y < DROWS_OFFSET+DROWS; y += 1) {
		for (x = DCOLS_OFFSET; x < DCOLS_OFFSET+DCOLS; x += 1) {
			if (dmatrix[x][y].light.count != 0) {
				max = (dmatrix[x][y].light.light.red > dmatrix[x][y].light.light.green) ? dmatrix[x][y].light.light.red : dmatrix[x][y].light.light.green;
				max = (max > dmatrix[x][y].light.light.blue) ? max : dmatrix[x][y].light.light.blue;
				scale = (float)(max)/255;

				if (scale > 1.0) {
					dmatrix[x][y].light.light.red *= (1.0)/(scale);
					dmatrix[x][y].light.light.green *= (1.0)/(scale);
					dmatrix[x][y].light.light.blue *= (1.0)/(scale);
				}
			}

			if (dmatrix[x][y].visible) {
				r = dmatrix[x][y].light.light.red;
				g = dmatrix[x][y].light.light.green;
				b = dmatrix[x][y].light.light.blue;
				intensity = dmatrix[x][y].light.light.intensity;
				dst.x = x*game_info.tile_w+game_info.display_x;
				dst.y = y*game_info.tile_h+game_info.display_y;
	
				intensity = (intensity < 0) ? 0 : intensity;
				intensity = (intensity > 255) ? 255 : intensity;

				if ((intensity == 0) && (r == 0) && (g == 0) && (b == 0)) {
					SDL_SetRenderDrawColor(game_info.renderer, 0, 0, 0, 255);
					SDL_RenderFillRect(game_info.renderer, &dst);
					dmatrix[x][y].visible = 0;
				} else {	
					SDL_SetRenderDrawColor(game_info.renderer, r, g, b, 255-intensity);
					SDL_SetRenderDrawBlendMode(game_info.renderer, SDL_BLENDMODE_MOD);
					SDL_RenderFillRect(game_info.renderer, &dst);
			
					SDL_SetRenderDrawColor(game_info.renderer, 0, 0, 0, 255);
					SDL_SetRenderDrawBlendMode(game_info.renderer, SDL_BLENDMODE_NONE);
				}
			}

			dmatrix[x][y].light.id = -1;
			dmatrix[x][y].light.count = 0;
		}
	}
}

void G_EvaluateRGB(G_Color col, int *r, int *g, int *b, boolean flickers) {
	assert((r != NULL) && (g != NULL) && (b != NULL));

	*r = col.red;
	*g = col.green;
	*b = col.blue;

	if (flickers) {
		*r += 2*(rand()%col.red_rand)-(col.red_rand);
		*g += 2*(rand()%col.green_rand)-(col.green_rand);
		*b += 2*(rand()%col.blue_rand)-(col.blue_rand);
	
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

void G_GenerateFOV(int x, int y, void *light, void (*func)(int*, int*, void*)) {
	if ((x < 0) || (x >= location->w) ||
			(y < 0) || (y >= location->h)) {
		return;
	}

	if (!G_TileObstructs(G_SceneTile(x, y))) {
		G_CastShadow(1, x, y, 0, 1, 1, 1, 0, light, func);
		G_CastShadow(1, x, y, 1, 1, 1, 1, 0, light, func);
		G_CastShadow(1, x, y, 0, -1, 1, 1, 0, light, func);
		G_CastShadow(1, x, y, 1, -1, 1, 1, 0, light, func);
		G_CastShadow(1, x, y, 0, 1, -1, 1, 0, light, func);
		G_CastShadow(1, x, y, 1, 1, -1, 1, 0, light, func);
		G_CastShadow(1, x, y, 0, -1, -1, 1, 0, light, func);
		G_CastShadow(1, x, y, 1, -1, -1, 1, 0, light, func);
	}

	x -= location->view.x;
	y -= location->view.y;

	if ((x >= 0) && (x < location->w) &&
			(y >= 0) && (y < location->h)) {
		func(&x, &y, light);
	}
}

void G_CastShadow(
	int distance, int x, int y,
	int invert, int dx, int dy,
	float start, float end,
	void *light, void (*func)(int*, int*, void*)) {
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
		was_blocked = 0;
	float current;

	if (dx > 0) {
		if (x+LIGHT_DISTANCE >= location->w) {
			x_bound = location->w-x;
		} else {
			x_bound = LIGHT_DISTANCE;
		}
	} else {
		if (x-LIGHT_DISTANCE < 0) {
			x_bound = x+1;
		} else {
			x_bound = LIGHT_DISTANCE;
		}
	}
	if (dy > 0) {
		if (y+LIGHT_DISTANCE >= location->h) {
			y_bound = location->h-y;
		} else {
			y_bound = LIGHT_DISTANCE;
		}
	} else {
		if (y-LIGHT_DISTANCE < 0) {
			y_bound = y+1;
		} else {
			y_bound = LIGHT_DISTANCE;
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
				if (G_TileObstructs(G_SceneTile(x_adj, y_adj))) {
					if ((was_blocked == 0) && (started != 1)) {
						G_CastShadow(distance+1, x, y, invert, dx, dy, start, ((float)(j+0.5))/(float)(i-0.5), light, func);
						start = ((float)(j-0.5))/((float)(i+0.5));
					}
					if ((j > 0) && (G_TileObstructs(G_SceneTile(nx_adj, ny_adj)))) {
            if (G_TileObstructs(G_SceneTile(imm_nx_adj, imm_ny_adj))) {
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
			
				x_adj -= location->view.x;
				y_adj -= location->view.y;

				func(&x_adj, &y_adj, light);
			} else if ((started == 1) && (current < end)) {
				return;
      }
		}

		was_blocked = 0;
		distance += 1;
	}
}

void G_MarkVisible(int *x, int *y, void *data) {
	assert((x != NULL) && (y != NULL));

	int
		lx = *x+DCOLS_OFFSET,
		ly = *y+DROWS_OFFSET;

	if ((lx >= DCOLS_OFFSET) && (lx < DCOLS_OFFSET+DCOLS) &&
			(ly >= DROWS_OFFSET) && (ly < DROWS_OFFSET+DROWS)) {
		if (dmatrix[lx][ly].visible == 0) {
			dmatrix[lx][ly].changed = 1;
		}
	
		dmatrix[lx][ly].visible = 2;
	}	
}

void G_AddLight(int *x, int *y, void *data) {
	assert((x != NULL) && (y != NULL) && (data != NULL) && (location != NULL) && (location->focus != NULL));

	G_LightNode *light = (G_LightNode*)data;
	int
		fx,
		fy,
		lx = *x,
		ly = *y,
		lx_adj,
		ly_adj,
		dx = (lx)-(light->x-location->view.x),
		dy = (ly)-(light->y-location->view.y);
	float
		a = 0.97969,
		b = 3.49689,
		c = 0.34968,
		d = -0.071786,
		intensity = (sqrt(dx*dx+dy*dy))/(light->light.intensity);
	intensity = (d)+(a-d)/(1.0+pow((intensity/c), b));

	lx += DCOLS_OFFSET;
	ly += DROWS_OFFSET;
	lx_adj = lx+location->view.x-DCOLS_OFFSET;
	ly_adj = ly+location->view.y-DROWS_OFFSET;

	if (intensity > 1.0) {
		intensity  = 1.0;
	} else if (intensity < 0.001) {
		return;
	}

	G_EntityPos(&(location->focus), &fx, &fy);

	if ((lx >= DCOLS_OFFSET) && (lx < DCOLS_OFFSET+DCOLS) &&
			(ly >= DROWS_OFFSET) && (ly < DROWS_OFFSET+DROWS)) {
		if ((dmatrix[lx][ly].visible) && (dmatrix[lx][ly].light.id != light->id)) {
			if ((!G_TileObstructs(dmatrix[lx][ly].tile)) ||
					(G_LightCanShine(fx, fy, light->x, light->y, lx_adj, ly_adj))) {
				dmatrix[lx][ly].light.light.red += light->light.red*intensity;
				dmatrix[lx][ly].light.light.green += light->light.green*intensity;
				dmatrix[lx][ly].light.light.blue += light->light.blue*intensity;
				dmatrix[lx][ly].light.light.intensity += intensity*255;
				dmatrix[lx][ly].light.id = light->id;
				dmatrix[lx][ly].light.count += 1;
			}
		}
	}
}

void G_ClearAndDecrement(void) {
	int
		x,
		y;

	for (y = DROWS_OFFSET; y < DROWS_OFFSET+DROWS; y += 1) {
		for (x = DCOLS_OFFSET; x < DCOLS_OFFSET+DCOLS; x += 1) {
			if (location != NULL) {
				dmatrix[x][y].light.count = 1;
				dmatrix[x][y].light.light.red = location->ambient.red;
				dmatrix[x][y].light.light.green = location->ambient.green;
				dmatrix[x][y].light.light.blue = location->ambient.blue;
				dmatrix[x][y].light.light.intensity = location->ambient.intensity;
			} else {
				dmatrix[x][y].light.count = 0;
				dmatrix[x][y].light.light.red = 0;
				dmatrix[x][y].light.light.green = 0;
				dmatrix[x][y].light.light.blue = 0;
				dmatrix[x][y].light.light.intensity = 0;
			}
			if (dmatrix[x][y].visible == 1) {
				dmatrix[x][y].visible = 0;
				dmatrix[x][y].changed = 1;
			} else if (dmatrix[x][y].visible == 2) {
				dmatrix[x][y].visible = 1;
			}
		}
	}
}

boolean G_LightCanShine(int fx, int fy, int lx, int ly, int dx, int dy) {
	fx = (fx-dx < 0) ? -1 : ((fx-dx > 0) ? 1 : 0);
	fy = (fy-dy < 0) ? -1 : ((fy-dy > 0) ? 1 : 0);
	lx = (lx-dx < 0) ? -1 : ((lx-dx > 0) ? 1 : 0);
	ly = (ly-dy < 0) ? -1 : ((ly-dy > 0) ? 1 : 0);
	fx = (fx == 0) ? lx : fx;
	fy = (fy == 0) ? ly : fy;

	return (!(((fx != lx) && (fy != ly)) ||
					((fx == lx) && (fy != ly) && (G_TileObstructs(G_SceneTile(dx+fx, dy)))) ||
					((fx != lx) && (fy == ly) && (G_TileObstructs(G_SceneTile(dx, dy+fy))))));
}

Tile G_CellToTile(int x, int y) {
	assert ((x >= 0) && (x < COLS) && (y >= 0) && (y < ROWS));

	if (dmatrix[x][y].layers[UI_LAYER] != -1) {
		return G_EntityIDToTile(dmatrix[x][y].layers[UI_LAYER]);
	} else if (dmatrix[x][y].layers[CREATURE_LAYER] != -1) {
		return G_EntityIDToTile(dmatrix[x][y].layers[CREATURE_LAYER]);
	} else if (dmatrix[x][y].layers[ITEM_LAYER] != -1) {
		return G_EntityIDToTile(dmatrix[x][y].layers[ITEM_LAYER]);
	} else if (dmatrix[x][y].layers[ORNAMENT_LAYER] != -1) {
		return G_EntityIDToTile(dmatrix[x][y].layers[ORNAMENT_LAYER]);
	} else if (dmatrix[x][y].layers[EFFECT_LAYER] != -1) {
		return G_EntityIDToTile(dmatrix[x][y].layers[EFFECT_LAYER]);
	}

	return dmatrix[x][y].tile;
}

/*
*/
