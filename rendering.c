#include "yendor.h"
#include "globals.h"

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

	if (initial == 1) {
		view.x = 0;
		view.y = 0;
		view.w = DCOLS;
		view.h = DROWS;
		view_previous.x = view.w;
		view_previous.y = view.h;

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

void render() {
	int
		x,
		y;
	entity *target = (focus == NULL) ? player : focus;

	// clear new buffer to be drawn to
	SDL_SetRenderTarget(renderer, render_buffers[target_buffer]);
	SDL_RenderClear(renderer);

	// attempt to salvage data from previous screen
	if (location != &menu) {
		renderSalvage();
	} else {
		SDL_RenderCopy(renderer, render_buffers[!target_buffer], NULL, NULL);
	}

	if ((target != NULL) && (location != &menu)) {
		entityPos(target, &x, &y);
		x -= view.x;
		y -= view.y;

		generateFOV(x, y);
	}

	renderChanges();
	
	// render new buffer to screen
	SDL_SetRenderTarget(renderer, NULL);	
	SDL_RenderCopy(renderer, render_buffers[target_buffer], NULL, NULL);

	view_previous.x = view.x;
	view_previous.y = view.y;

	SDL_RenderPresent(renderer);
	target_buffer = (target_buffer == 0) ? 1 : 0;	
	
	decrementVis();
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
		src.x *= tile_width;
		src.y *= tile_height;
		src.w *= tile_width;
		src.h *= tile_height;
		
		src.x += dport.x+(DCOLS_OFFSET)*tile_width;
		src.y += dport.y+(DROWS_OFFSET)*tile_height;

		dst.x = (xdiff < 0) ? (-xdiff) : 0;
		dst.y = (ydiff < 0) ? (-ydiff) : 0;
		dst.x *= tile_width;
		dst.y *= tile_height;
		dst.w = src.w;
		dst.h = src.h;

		dst.x += dport.x+(DCOLS_OFFSET)*tile_width;
		dst.y += dport.y+(DROWS_OFFSET)*tile_height;

		SDL_RenderCopy(renderer, render_buffers[!target_buffer], &src, &dst);

		src.x = (src.x-dport.x)/tile_width;
		src.y = (src.y-dport.y)/tile_height;
		dst.x = (dst.x-dport.x)/tile_width;
		dst.y = (dst.y-dport.y)/tile_height;
		dst.w /= tile_width;
		dst.h /= tile_height;

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
			int to_draw = (dmatrix[x][y].entity == NOTHING) ? dmatrix[x][y].tile : dmatrix[x][y].entity;
			color
				fg = (to_draw < 256) ? white : *(descriptor_tiles[to_draw-256].fg),
				bg = (to_draw < 256) ? black : *(descriptor_tiles[to_draw-256].bg);

			if (dmatrix[x][y].changed == 1) {
				dst.x = dport.x+(x*tile_width);
				dst.y = dport.y+(y*tile_height);
	
				if (dmatrix[x][y].visible) {
					int
						r,
						g,
						b;
	
					evaluateRGB(bg, &r, &g, &b);
	
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
				} else {
					SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
					SDL_RenderFillRect(renderer, &dst);	
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

void evaluateRGB(color col, int *r, int *g, int *b) {
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

void generateFOV(int x, int y) {
	//take rendering port coordinates and adjust them
	x += DCOLS_OFFSET;
	y += DROWS_OFFSET;

	if ((lookupTile(dmatrix[x][y].tile)->flags & OBSTRUCTS) != OBSTRUCTS) {
		//cast shadows into each octant
		castLight(1, x, y, 0, 1, 1, 1, 0);
		castLight(1, x, y, 1, 1, 1, 1, 0);
		castLight(1, x, y, 0, -1, 1, 1, 0);
		castLight(1, x, y, 1, -1, 1, 1, 0);
		castLight(1, x, y, 0, 1, -1, 1, 0);
		castLight(1, x, y, 1, 1, -1, 1, 0);
		castLight(1, x, y, 0, -1, -1, 1, 0);
		castLight(1, x, y, 1, -1, -1, 1, 0);
	}

	//make at least the exact spot of the player visible
	dmatrix[x][y].visible = 2;
	dmatrix[x][y].changed = 1;
}

void castLight(
	int distance, int x, int y,
	int invert, int dx, int dy,
	float start, float end) {
	int
		i,
		j,
		x_adj,
		y_adj,
		nx_adj,
		ny_adj,
		bound,
		started,
		was_blocked = 0;
	float current;

	int last = -1;

	if (invert) {
		bound = (dy < 0) ? (y-DROWS_OFFSET+1) : (DROWS+DROWS_OFFSET-y);
	} else {
		bound = (dx < 0) ? (x-DCOLS_OFFSET+1) : (DCOLS+DCOLS_OFFSET-x);
	}

	for (i = distance; i < bound; i += 1) {
		started = 0;
		for (j = distance; j >= 0; j -= 1) {
			// slope of the current block
			current = ((float)j)/((float)i);
			// translate algorithm coordinates to dmatrix coordinates
			if (invert) {
				x_adj = x+(j*dx);
				y_adj = y+(i*dy);
				nx_adj = x+(j*dx);
				ny_adj = y+((i+1)*dy);
			} else {
				x_adj = x+(i*dx);
				y_adj = y+(j*dy);
				nx_adj = x+((i+1)*dx);
				ny_adj = y+(j*dy);
			}
			if ((x_adj < DCOLS_OFFSET) || (x_adj >= DCOLS_OFFSET+DCOLS) ||
					(y_adj < DROWS_OFFSET) || (y_adj >= DROWS_OFFSET+DROWS)) {
				continue;
			} else {
				started = (started == 0) ? 1 : started;
			}
			if ((current <= start) && (current >= end)) {
				if ((lookupTile(dmatrix[x_adj][y_adj].tile)->flags & OBSTRUCTS) == OBSTRUCTS) {
					if ((was_blocked == 0) && (started != 1)) {
						castLight(distance+1, x, y, invert, dx, dy, start, ((float)(j+0.5))/(float)(i-0.5));
						start = ((float)(j-0.5))/((float)(i+0.5));
					}
					if (last == -1) {
						last = j;
					}
					if ((j > 0) && ((lookupTile(dmatrix[nx_adj][ny_adj].tile)->flags & OBSTRUCTS) == OBSTRUCTS)) {
						start = ((float)(j))/((float)(i+1.0));
					} else {
						start = ((float)(j-0.5))/((float)(i+0.5));
					}
					was_blocked = 1;
				} else {
					was_blocked = 0;
				}

				started = 2;
				if (dmatrix[x_adj][y_adj].visible == 0) {
					dmatrix[x_adj][y_adj].changed = 1;
				}
				dmatrix[x_adj][y_adj].visible = 2;
			} else if ((started == 1) && (current < end)) {
				return;
			}
		}
		was_blocked = 0;
		distance += 1;
	}
}

void decrementVis() {
	if (location != &menu) {
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

/*
*/
