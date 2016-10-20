#include "tiles.h"
#include <stdlib.h>

/*
** functions
*/

tile* getTile(unsigned int value) {
	tile *to_return = (tile*)malloc(sizeof(tile));

	to_return->base = (value & BASE_MASK) >> 30;

	to_return->depth = 0;
	to_return->dir = 0;
	to_return->flam = 0;

	switch (to_return->base) {
		case LIQUID:
			{
				to_return->depth = (value & L_DPTH_MASK) >> 28;
				to_return->dir = (value & L_DIRS_MASK) >> 25;
				to_return->type = (value & L_TYPE_MASK) >> 19;
			}
			break;
		case GROUND:
			{
				to_return->flam = (value & G_FLAM_MASK) >> 29;
				to_return->type = (value & G_TYPE_MASK) >> 19;
			}
			break;
		case NOTHING:
		case WALL:
			{
				to_return->type = (value & TYPE_MASK) >> 19;
			}
			break;
	}

	to_return->perm = (value & PERM_MASK) >> 13;
	to_return->temp = (value & TEMP_MASK) >> 7;
	to_return->duration = value & TIME_MASK;

	return to_return;
}

unsigned int makeTile(
	unsigned int base,
	unsigned int type,
	unsigned int depth,
	unsigned int dir,
	unsigned int flam,
	unsigned int perm,
	unsigned int temp,
	unsigned int duration) {
	switch (base) {
		case LIQUID:
			{
				depth = (depth << 28) & L_DPTH_MASK;
				dir = (dir << 25) & L_DIRS_MASK;
				type = (type << 19) & L_TYPE_MASK;
			}
			break;
		case GROUND:
			{
				flam = (flam << 29) & G_FLAM_MASK;
				type = (type << 18) & G_TYPE_MASK;
			}
			break;
		case NOTHING:
		case WALL:
			{
				type = (type << 19) & TYPE_MASK;
			}
			break;
	}

	base = (base << 30) & BASE_MASK;
	return (base | type | depth | dir | flam | perm | temp | duration);
}

/*
*/
