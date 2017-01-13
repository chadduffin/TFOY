#ifndef __TILES__
#define __TILES__

/*
** defines
*/

#define BASE_MASK 0xC0000000
#define TYPE_MASK 0x3D000000
#define ORNM_MASK 0x01FD0000
#define PERM_MASK 0x0001F000
#define TEMP_MASK 0x00000F80
#define TIME_MASK 0x0000007F

/*
** typedefs
*/

// compressed tile
typedef struct ctile {
	unsigned int tile;
} ctile;

// expanded tile
typedef struct etile {
	unsigned int
		base,
		type,
		ornm,
		perm,
		temp,
		time;
} etile;

/*
** functions
*/

etile* expandTile(unsigned int value);
ctile* compressTile(
	unsigned int base,
	unsigned int type,
	unsigned int ornm,
	unsigned int perm,
	unsigned int temp,
	unsigned int time);

/*
*/

#endif /* __TILES__ */
