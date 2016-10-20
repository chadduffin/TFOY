#ifndef __CREATURES__
#define __CREATURES__

/*
** enums
*/

enum CreatureCategories {
	BEAST,
	DEMON,
	DRAGON,
	HUMANOID,
	MAGICAL,
	UNDEAD,
	
	CREATURE_CATEGORY_COUNT
};

/*
** typedefs
*/

typedef struct creatureInfo {
	char name,
		description;
	int base_attributes[ATTRIBUTE_TYPE_COUNT];
	unsigned int category;
} creatureInfo;

typedef struct creature {
	int
		x,
		y,
		z;
	creatureInfo info;
} creature;

/*
*/

#endif /* __CREATURES__ */
