#include "yendor.h"
#include "globals.h"

/*
** EXTERNS
*/

G_Attribute attributes[ATTRIBUTE_COUNT] = {
	{"Strength",				"Description of strength."},
	{"Agility", 				"Description of agility."},
	{"Intellect", 			"Description of intellect."},
	{"Constitution", 		"Description of constitution."},
	
	{"Attack Power", 		"Description of ap."},
	{"Spell Power", 		"Description of sp."},
	{"Critical Strike", "Description of cs."},
	{"Haste", 					"Description of haste."},
	{"Dexterity", 			"Description of dexterity."},

	{"Dodge", 					"Description of dodge."},
	{"Block", 					"Description of block."},
	{"Parry", 					"Description of parry."},
	{"Resolve", 				"Description of resolve."},
	{"Fortitude", 			"Description of fortitude."},

	{"Armor", 					"Description of armor."},
	{"Level", 					"Description of level."},
	{"Experience", 			"Description of experience."},

	{"Health",					"Description of health."},
	{"Mana", 						"Description of mana."},
	{"Energy", 					"Description of energy."},
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
	assert((attribute >= STRENGTH) && (attribute < ATTRIBUTE_COUNT));
	return attributes[attribute].name;
}

const char* G_GetAttributeDescription(Attribute attribute) {
	assert((attribute >= STRENGTH) && (attribute < ATTRIBUTE_COUNT));
	return attributes[attribute].description;
}

int G_GetExperienceNeeded(int level) {
	assert(level >= 1);
	return (level == 1) ? EXPERIENCE_OFFSET : 10*((EXPERIENCE_OFFSET+(2 << (level+3)))/10);
}

/*
*/
