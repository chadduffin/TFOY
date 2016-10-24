#include "tiles.h"
#include <stdlib.h>

/*
** functions
*/

tile* getTile(unsigned int value) {
	tile *to_return = (tile*)malloc(sizeof(tile));

	to_return->base = (value & BASE_MASK) >> 30;
	to_return->type = (value & TYPE_MASK) >> 17;
	to_return->perm = (value & PERM_MASK) >> 12;
	to_return->temp = (value & TEMP_MASK) >> 7;
	to_return->duration = value & TIME_MASK;

	return to_return;
}

unsigned int makeTile(
	unsigned int base,
	unsigned int type,
	unsigned int perm,
	unsigned int temp,
	unsigned int duration) {

	base = (base << 30) & BASE_MASK;
	type = (type << 17) & TYPE_MASK;
	perm = (perm << 12) & PERM_MASK;
	temp = (temp << 7) & TEMP_MASK;
	duration = duration & TIME_MASK;

	return (base | type | perm | temp | duration);
}

/*
*/
