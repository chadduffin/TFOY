#include "yendor.h"
#include "globals.h"

/*
** EXTERNS
*/

G_Attribute attributes[ATTRIBUTE_COUNT] = {
	{"Might",						"Description of might."},
	{"Intellect", 			"Description of intellect."},
	{"Constitution", 		"Description of constitution."},
	
	{"Attack Power", 		"Description of ap."},
	{"Spell Power", 		"Description of sp."},
	{"Haste", 					"Description of haste."},
	{"Dexterity", 			"Description of dexterity."},
	{"Critical Strike", "Description of critical strike."},

	{"Dodge", 					"Description of dodge."},
	{"Block", 					"Description of block."},
	{"Parry", 					"Description of parry."},
	{"Resolve", 				"Description of resolve."},
	{"Fortitude", 			"Description of fortitude."},

	{"Armor", 					"Description of armor."},
	{"Level", 					"Description of level."},
	{"Experience", 			"Description of experience."},

	{"Health",					"Description of health."},
	{"Energy", 					"Description of energy."},
	{"Mana", 						"Description of mana."},
	{"Rage", 						"Description of rage."},

	{"Hunger", 					"Description of strength."},
	{"Thirst", 					"Description of strength."},
	{"Wetness", 				"Description of strength."},
	{"Temperature", 		"Description of strength."}
};

/*
** FUNCTIONS
*/

const char* G_GetAttributeName(Attribute attribute) {
	assert((attribute >= MIGHT) && (attribute < ATTRIBUTE_COUNT));
	return attributes[attribute].name;
}

const char* G_GetAttributeDescription(Attribute attribute) {
	assert((attribute >= MIGHT) && (attribute < ATTRIBUTE_COUNT));
	return attributes[attribute].description;
}

int G_GetExperienceNeeded(int level) {
	assert(level >= 1);
	return (level == 1) ? EXPERIENCE_OFFSET : 10*((EXPERIENCE_OFFSET+(2 << (level+3)))/10);
}

/*
*/
