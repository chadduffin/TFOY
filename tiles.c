#include "yendor.h"
#include "globals.h"

G_TileInformation tile_info[TILE_COUNT] = {
  {"Nothing", "", 0, 0, &bad_color, &bad_color, 0},

  {"Ground", "", 14, 2, &brown, &dblue, 0},
  {"Stone", "", 14, 2, &dgrey, &grey, 0},
  {"Sand", "", 14, 2, &dyellow, &yellow, 0},
  {"Wall", "", 3, 2, &dred, &grey, OBSTRUCTS},
  {"Grass", "", 2, 2, &green, &dblue, HIGHLY_FLAMMABLE | FREEZABLE},
  {"Tall Grass", "", 2, 14, &green, &dblue, FLAMMABLE},
  {"Burnt Grass", "", 2, 2, &brown, &dblue, 0},
  {"Fungus", "", 2, 2, &teal, &dblue, FLAMMABLE | LUMINESCENT},
  {"Water", "", 14, 7, &dblue, &scott_blue, FLICKERS_REGULAR | DISABLES_ACTIONS},
  {"Frozen Grass", "", 2, 2, &white, &scott_blue, 0},

  {"Ice", "", 10, 2, &white, &scott_blue, 0},
  {"Lava", "", 14, 7, &red, &orange, FLICKERS_SLOW | DISABLES_ACTIONS | ILLUMINATING},

  {"Fire", "", 14, 1, &orange, &bad_color, ILLUMINATING | FLICKERS_QUICK | DISABLES_ACTIONS},
  {"Green Fire", "", 14, 1, &dgreen, &bad_color, FLICKERS_QUICK | DISABLES_ACTIONS},

  {"Human", "", 0, 4, &white, &bad_color, OBSTRUCTS_MOVEMENT},

  {"solid", "", 0, 0, &magenta, &black, 0},
  {"white", "", 0, 0, &white, &white, 0},
  {"black", "", 0, 0, &black, &black, 0},
  {"red", "", 0, 0, &red, &red, 0},
  {"green", "", 0, 0, &green, &green, 0},
  {"blue", "", 0, 0, &blue, &blue, 0},
  {"magenta", "", 0, 0, &magenta, &magenta, 0},

  {"error", "", 1, 11, &magenta, &black, OBSTRUCTS},
};

void G_TileUpdate(Tile tile, int x, int y) {
	if (tile > 255) {
	  int scene_x = x+active_scene->view.x, scene_y = y+active_scene->view.y;

	  G_LightNode node;
	  G_TileInformation info = tile_info[tile-NOTHING];

	  if (G_TileFlagCompare(tile, LUMINESCENT)) {
  		node.x = scene_x;
  		node.y = scene_y;
  		node.r = (info.fg->r)/3;
  		node.g = (info.fg->g)/3;
  		node.b = (info.fg->b)/3;
  		node.intensity = 2;
 	    node.id.value = game_info.id.value+scene_x+scene_y*active_scene->tw;

      G_GenerateFOVSimple(scene_x, scene_y, &node, &G_AddLight);
	  } else if (G_TileFlagCompare(tile, ILLUMINATING)) {
  		node.x = scene_x;
  		node.y = scene_y;
  		node.r = 255;
  		node.g = 255;
  		node.b = 255;
  		node.intensity = 2;
 	    node.id.value = game_info.id.value+scene_x+scene_y*active_scene->tw;

      G_GenerateFOVSimple(scene_x, scene_y, &node, &G_AddLight);
    }
	}
}

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

boolean G_TileFlagCompare(Tile tile, TileFlag flag) {
  return (G_TileFlags(tile) & flag) == flag;
}
