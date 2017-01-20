#ifndef __COLORS__
#define __COLORS__

/*
** defines
*/

#define BLACK {0, 0, 0, 0, 0, 0, 0}
#define WHITE {255, 255, 255, 0, 0, 0, 0}
#define RED {255, 0, 0, 0, 0, 0, 0}
#define GREEN {0, 255, 0, 0, 0, 0, 0}
#define BLUE {0, 0, 255, 0, 0, 0, 0}
#define YELLOW {255, 255, 0, 0, 0, 0, 0}
#define MAGENTA {255, 0, 255, 0, 0, 0, 0}
#define AQUA {0, 255, 255, 0, 0, 0, 0}

#define BROWN {105, 90, 70, 24, 20, 16, 0}
#define GREY {225, 225, 225, 24, 24, 24, 0}
#define DARK_BLUE {0, 0, 75, 8, 8, 12, 0}

/*
** typedefs
*/

typedef struct color {
	short
		red,
		green,
		blue,
		redRand,
		greenRand,
		blueRand,

		flickers;
} color;

/*
** externs
*/

extern const color red;
extern const color blue;
extern const color aqua;
extern const color black;
extern const color white;
extern const color green;
extern const color yellow;
extern const color magenta;

extern const color brown;
extern const color grey;
extern const color dark_blue;

/*
*/

#endif /* __COLORS__ */
