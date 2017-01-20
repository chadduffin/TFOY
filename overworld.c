#include "overworld.h"

/*
** externs
*/

ctile overworld[WORLD_COLS][WORLD_ROWS];

/*
** functions
*/

void seedWorldTest() {
	int
		x,
		y;

	for (y = 0; y < WORLD_ROWS; y += 1) {
		for (x = 0; x < WORLD_COLS; x += 1) {
			if ((x == 0) || (y == 0) || (x == WORLD_COLS-1) || (y == WORLD_ROWS-1)) {
				overworld[x][y].tile = WALL;
				overworld[x][y].effect = 0;
				overworld[x][y].duration = 0;
			} else {
				overworld[x][y].tile = GROUND;
				overworld[x][y].effect = 0;
				overworld[x][y].duration = 0;
			}
		}
	}
}

/*
*/
