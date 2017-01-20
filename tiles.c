#include "tiles.h"
#include <stdlib.h>

/*
** externs
*/

dtile descriptor_tiles[TILE_TYPE_COUNT] = {
	{"empty", "description", EMPTY_TILE, 0, 0, 0, 0},
		{"chasm", "description", EMPTY_TILE, 0, 0, 0, 0},

	{"ground", "the ground", GROUND_TILE, 0, 0, 14, 2},
		{"mud", "description", GROUND_TILE, 0, 0, 0, 0},
		{"dirt", "description", GROUND_TILE, 0, 0, 0, 0},
		{"soil", "description", GROUND_TILE, 0, 0, 0, 0},
		{"moss", "description", GROUND_TILE, FLAMMABLE | FREEZABLE, 0, 0, 0},
		{"light snow", "description", GROUND_TILE, 0, IS_FROZEN, 0, 0},
		{"medium snow", "description", GROUND_TILE, 0, IS_FROZEN, 0, 0},
		{"heavy snow", "description", GROUND_TILE, 0, IS_FROZEN, 0, 0},
		{"rock", "description", GROUND_TILE, 0, 0, 0, 0},
		{"mossy rock", "description", GROUND_TILE, FLAMMABLE | FREEZABLE, 0, 0, 0},
		{"grass", "description", GROUND_TILE, FLAMMABLE | FREEZABLE, 0, 0, 0},
		{"dead grass", "description", GROUND_TILE, FLAMMABLE, 0, 0, 0},
		{"stone", "description", GROUND_TILE, 0, 0, 0, 0},
			{"mossy stone", "description", GROUND_TILE, FLAMMABLE | FREEZABLE, 0, 0, 0},
			{"cracked stone", "description", GROUND_TILE, 0, 0, 0, 0},
		{"fungus", "description", GROUND_TILE, FLAMMABLE | FREEZABLE, 0, 0, 0},
			{"bioluminescent fungus", "description", GROUND_TILE, FLAMMABLE | FREEZABLE, 0, 0, 0},
		{"gravel", "description", GROUND_TILE, 0, 0, 0, 0},
		{"wood", "description", GROUND_TILE, 0, 0, 0, 0},
		{"rotting wood", "description", GROUND_TILE, 0, 0, 0, 0},
		{"polished wood", "description", GROUND_TILE, 0, 0, 0, 0},
		{"tile", "description", GROUND_TILE, 0, 0, 0, 0},
		{"smooth tile", "description", GROUND_TILE, 0, 0, 0, 0},
		{"cracked tile", "description", GROUND_TILE, 0, 0, 0, 0},
		{"carpet", "description", GROUND_TILE, FLAMMABLE, 0, 0, 0},
		{"torn carpet", "description", GROUND_TILE, FLAMMABLE, 0, 0, 0},
		{"marble", "description", GROUND_TILE, 0, 0, 0, 0},
		{"cracked marble", "description", GROUND_TILE, 0, 0, 0, 0},
		{"polished marble", "description", GROUND_TILE, 0, 0, 0, 0},
		{"granite", "description", GROUND_TILE, 0, 0, 0, 0},
		{"cracked granite", "description", GROUND_TILE, 0, 0, 0, 0},
		{"polished granite", "description", GROUND_TILE, 0, 0, 0, 0},

		{"wall", "a wall", SOLID_TILE, 0, 0, 3, 2},

		{"player", "the player", CHARACTER_TILE, 0, 0, 2, 0},
};

/*
*/
