#ifndef __COMPONENTS__
#define __COMPONENTS__

#include "attributes.h"

/*
** enums
*/

enum ComponentTypes {
	CREATURE_COMPONENT = 1,
	LOCATION_COMPONENT,

	COMPONENT_TYPE_COUNT
};

enum CreatureFlags {
	IS_INVULNERABLE = 1,
	IS_INVISIBLE = 2,

	FLAG_COUNT
};

enum CreatureCategories {
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

/*
** typedefs
*/

typedef struct creature_component {
	short tile;
	unsigned int
		flags,
		category;
	int base_attributes[ATTRIBUTE_TYPE_COUNT];
} creature_component;

typedef struct location_component {
	short
		x,
		y,
		z;
} location_component;

/*
*/

#endif /* __COMPONENTS__ */
