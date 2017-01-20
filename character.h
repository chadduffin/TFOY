#ifndef __CHARACTER__
#define __CHARACTER__

#include "attributes.h"

/*
** enums
*/

enum CharacterCategories {
	ANIMAL = 0,
	DRAGON,
	UNDEAD,
	MAGICAL,
	HUMANOID,
	INFERNAL,
	FIREBORNE,
	WATERBORNE,
	ABOMINATION,

	CATEGORY_COUNT
};

enum CharacterAlignment {
	LAWFUL = 0,
	CHAOTIC = 1,
	
	GOOD = 2,
	EVIL = 4,

	NEUTRAL = 8,
	
	ALIGNMENT_COUNT
};

enum CharacterFlags {
	IS_PLAYER = 0,
	IS_HOSTILE,
	
	INVULNERABLE,

	FLAG_COUNT
};

/*
** typedefs
*/

typedef struct character {
	short
		x,
		y,
		z;
	unsigned int
		id,
		flags,
		category,
		alignment,
		
		tile;
	int attributes[ATTRIBUTE_TYPE_COUNT];
} character;

/*
*/

#endif /* __CHARACTER__ */
