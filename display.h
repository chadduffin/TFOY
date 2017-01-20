#ifndef __DISPLAY__
#define __DISPLAY__

#include "tiles.h"
#include "globals.h"

/*
** typedefs
*/

typedef struct dcell {
	short
		empty,
		changed;
	ctile tile;
} dcell;

/*
** externs
*/

extern dcell dmatrix[COLS][ROWS];

/*
*/

#endif /* __DISPLAY__ */
