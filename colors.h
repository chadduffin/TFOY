#ifndef __COLORS__
#define __COLORS__

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

extern const color black;
extern const color white;
extern const color red;
extern const color yellow;
extern const color magenta;
extern const color green;
extern const color blue;
extern const color aqua;

extern const color dblue;
extern const color grey;
extern const color brown;

/*
*/

#endif /* __COLORS__ */
