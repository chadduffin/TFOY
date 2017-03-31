#include "yendor.h"
#include "globals.h"

G_TileInformation tile_info[TILE_COUNT] = {
  {"Nothing", "", 0, 0, &white, &black, 0},

  {"Ground", "", 14, 2, &brown, &dblue, 0},
  {"Wall", "", 3, 2, &dred, &grey, OBSTRUCTS},
  {"Grass", "", 2, 2, &green, &dblue, FLAMMABLE},
  {"Burnt Grass", "", 2, 2, &brown, &dblue, 0},
  {"Water", "", 14, 7, &dblue, &blue, FLICKERS_REGULAR | DISABLES_ACTIONS},

  {"Lava", "", 14, 7, &red, &orange, FLICKERS_SLOW | DISABLES_ACTIONS},

  {"Fire", "", 14, 1, &red, &orange, FLICKERS_QUICK | DISABLES_ACTIONS},

  {"Human", "", 0, 4, &white, &black, OBSTRUCTS_MOVEMENT},
};

TileFlag G_TileFlags(Tile tile) {
  if ((tile >= NOTHING) && (tile < END_TILE)) {
    return tile_info[tile-NOTHING].flags;
  }

  return 0;
}

SDL_Rect G_TileSource(Tile tile) {
	assert((tile >= 0) && (tile < END_TILE));

  SDL_Rect src;
  
  src.w = TILE_SOURCE_WIDTH;
  src.h = TILE_SOURCE_HEIGHT;

	if (tile < 256) {
		src.x = (tile%16)*TILE_SOURCE_WIDTH;
		src.y = (tile/16)*TILE_SOURCE_HEIGHT;
	} else {
		src.x = tile_info[tile-NOTHING].x*TILE_SOURCE_WIDTH;
		src.y = tile_info[tile-NOTHING].y*TILE_SOURCE_HEIGHT;
	}

  return src;
}
