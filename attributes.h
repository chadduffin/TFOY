#ifndef __ATTRIBUTES__
#define __ATTRIBUTES__

/*
** defines
*/

#define LEVEL_CAP 20
#define EXPERIENCE_OFFSET 42

/*
** enums
*/

enum AttributeTypes {
	STRENGTH,
	AGILITY,
	INTELLECT,
	CONSTITUTION,

	ATTACK_POWER,
	SPELL_POWER,
	CRITICAL_STRIKE,
	HASTE,
	DEXTERITY,
	
	DODGE,
	BLOCK,
	PARRY,
	RESOLVE,
	FORTITUDE,

	ARMOR,
	LEVEL,
	EXPERIENCE,

	HEALTH,
	MANA,
	ENERGY,
	RAGE,

	HUNGER,
	THIRST,
	WETNESS,
	TEMPERATURE,

	ATTRIBUTE_TYPE_COUNT
};

/*
** typedefs
*/

typedef struct attribute {
	char
		*name,
		*description;
} attribute;

/*
** externs
*/

extern attribute attributes[ATTRIBUTE_TYPE_COUNT];

/*
** functions
*/

const char* getAttributeName(int index);
const char* getAttributeDescription(int index);
int getExperienceNeeded(int level);

/*
*/

#endif /* __ATTRIBUTES__ */
