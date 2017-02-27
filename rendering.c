#include "yendor.h"
#include "globals.h"

/*
** FUNCTIONS
*/

void G_UpdateRenderingInfo() {
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
			G_DecrementFOV();
		}
	} else {
		SDL_RenderCopy(game_info.renderer, game_info.buffers[!game_info.target_buffer], NULL, NULL);
	}

	G_LoopEntities(&G_EntityRender);

	G_RenderChanges();

	SDL_SetRenderTarget(game_info.renderer, NULL);	
	SDL_RenderCopy(game_info.renderer, game_info.buffers[game_info.target_buffer], NULL, NULL);

	if (location != menu) {
		G_RenderLightmap();
	}

	SDL_RenderPresent(game_info.renderer);
	game_info.target_buffer = (game_info.target_buffer == 0) ? 1 : 0;	
}

void G_LightRender(void) {
	if (SDL_GetTicks()-last_flicker > FLICKER_RATE) {
		G_RenderFlicker(0.0001);
		last_flicker += rand()%FLICKER_RATE;
	}

	SDL_RenderCopy(game_info.renderer, game_info.buffers[!game_info.target_buffer], NULL, NULL);

	if (location != menu) {
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
	src.x = 0;
	src.y = 0;

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
			to_draw = (dmatrix[x][y].entity == NOTHING) ? dmatrix[x][y].tile : dmatrix[x][y].entity;
			G_Color
				fg = (to_draw < 256) ? white : *(descriptor_tiles[to_draw-256].fg),
				bg = (to_draw < 256) ? black : *(descriptor_tiles[to_draw-256].bg);

			if (dmatrix[x][y].changed == 1) {
				dst.x = game_info.display_x+(x*game_info.tile_w);
				dst.y = game_info.display_y+(y*game_info.tile_h);
	
				if ((dmatrix[x][y].visible) || (location == menu)) {
					int
						r,
						g,
						b;
	
					G_EvaluateRGB(bg, &r, &g, &b, G_TileFlickers(to_draw));
	
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
			if ((dmatrix[x][y].visible == 0) || (dmatrix[x][y].entity != NOTHING)) {
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

	SDL_SetRenderDrawColor(game_info.renderer, 0, 0, 0, 255);
	SDL_SetRenderTarget(game_info.renderer, NULL);	
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
	SDL_Rect dst;
	dst.w = game_info.tile_w;
	dst.h = game_info.tile_h;

	for (y = DROWS_OFFSET; y < DROWS_OFFSET+DROWS; y += 1) {
		for (x = DCOLS_OFFSET; x < DCOLS_OFFSET+DCOLS; x += 1) {
			if (dmatrix[x][y].visible) {
				r = dmatrix[x][y].light.light.red;
				g = dmatrix[x][y].light.light.green;
				b = dmatrix[x][y].light.light.blue;
				intensity = dmatrix[x][y].light.light.intensity;
				dst.x = x*game_info.tile_w+game_info.display_x;
				dst.y = y*game_info.tile_h+game_info.display_y;

				max = (r > g) ? r : g;
				max = (max > b) ? max : b;
				max = (max%255 != 0) ? ((max/255)+1) : (max/255);

				if (max != 0) {
					r = r/max;
					g = g/max;
					b = b/max;
				}
	
				intensity = (intensity < 0) ? 0 : intensity;
				intensity = (intensity > 255) ? 255 : intensity;

				SDL_SetRenderDrawColor(game_info.renderer, r, g, b, 255);
				SDL_SetRenderDrawBlendMode(game_info.renderer, SDL_BLENDMODE_MOD);
				SDL_RenderFillRect(game_info.renderer, &dst);

				SDL_SetRenderDrawColor(game_info.renderer, 0, 0, 0, 255-intensity);
				SDL_SetRenderDrawBlendMode(game_info.renderer, SDL_BLENDMODE_BLEND);
				SDL_RenderFillRect(game_info.renderer, &dst);
	
				SDL_SetRenderDrawBlendMode(game_info.renderer, SDL_BLENDMODE_NONE);
			}

			dmatrix[x][y].light.id = -1;
		}
	}
}

void G_EvaluateRGB(G_Color col, int *r, int *g, int *b, boolean flickers) {
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
			} else {
				x_adj = x+(i*dx);
				y_adj = y+(j*dy);
				nx_adj = (i < x_bound-1) ? x+((i+1)*dx) : x_adj;
				ny_adj = y+(j*dy);
			}

			started = (started == 0) ? 1 : started;

			if ((current <= start) && (current >= end)) {
				if (G_TileObstructs(G_SceneTile(x_adj, y_adj))) {
					if ((was_blocked == 0) && (started != 1)) {
						G_CastShadow(distance+1, x, y, invert, dx, dy, start, ((float)(j+0.5))/(float)(i-0.5), light, func);
						start = ((float)(j-0.5))/((float)(i+0.5));
					}
					if ((j > 0) && (G_TileObstructs(G_SceneTile(nx_adj, ny_adj)))) {
						start = ((float)(j))/((float)(i+0.5));
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

void G_DecrementFOV() {
	if (location != menu) {
		int
			x,
			y;

		for (y = DROWS_OFFSET; y < (DROWS+DROWS_OFFSET); y += 1) {
			for (x = DCOLS_OFFSET; x < (DCOLS+DCOLS_OFFSET); x += 1) {
				if (dmatrix[x][y].visible == 1) {
					dmatrix[x][y].visible = 0;
					dmatrix[x][y].changed = 1;
				} else if (dmatrix[x][y].visible == 2) {
					dmatrix[x][y].visible = 1;
				}
			}
		}
	}
}

void G_MarkVisible(int *x, int *y, void *data) {
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
	G_LightNode *light = (G_LightNode*)data;
	int
		lx = *x,
		ly = *y,
		dx = (lx)-(light->x-location->view.x),
		dy = (ly)-(light->y-location->view.y);
	float intensity = (1.0)/(1.0+(sqrt(dx*dx+dy*dy))/16.0);
	intensity -= (1.0)/(1.0+(sqrt(light->light.intensity*light->light.intensity)));

	lx += DCOLS_OFFSET;
	ly += DROWS_OFFSET;

	if (intensity > 1.0) {
		intensity  = 1.0;
	} else if (intensity < 0.01) {
		return;
	}

	if ((lx >= DCOLS_OFFSET) && (lx < DCOLS_OFFSET+DCOLS) &&
			(ly >= DROWS_OFFSET) && (ly < DROWS_OFFSET+DROWS)) {
		if (dmatrix[lx][ly].light.id != light->id) {
			if (dmatrix[lx][ly].light.id == -1) {
				dmatrix[lx][ly].light.light.red = 0;
				dmatrix[lx][ly].light.light.green = 0;
				dmatrix[lx][ly].light.light.blue = 0;
				dmatrix[lx][ly].light.light.intensity = 0;
			}
			dmatrix[lx][ly].light.light.red += (light->light.red*intensity);
			dmatrix[lx][ly].light.light.green += light->light.green*intensity;
			dmatrix[lx][ly].light.light.blue += light->light.blue*intensity;
			dmatrix[lx][ly].light.light.intensity += intensity*255;
			dmatrix[lx][ly].light.id = light->id;
		}
	}
}

/*
*/
