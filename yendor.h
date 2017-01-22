#include <stdio.h>
#include <stdlib.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_net.h>
#include <SDL2/SDL_image.h>

/*
** defines
*/

#define TITLE "The Fellowships Of Yendor"
#define VERSION "v0.0.1"

#define FULLSCREEN 0
#define DEBUGGING 0
#define ONLINE 0

#define DISABLE_LIGHT 0
#define DISABLE_COLOR_MOD 1

#define FPS 60
#define MAX_BUFFER 1024
#define TEXTURE_COUNT 1

#define NOT_OK 0
#define SDL_OK 1
#define WINDOW_OK 2
#define RENDER_OK 4
#define GRPHCS_OK 8
#define NETWRK_OK 16

#define TILE_SOURCE_WIDTH 16
#define TILE_SOURCE_HEIGHT 16

#define COLS 108
#define ROWS 64
#define DCOLS (COLS - STAT_BAR_WIDTH - 2)
#define DROWS (ROWS - MESSAGE_ROWS - 2)
// the offset of the actual game view
#define DCOLS_OFFSET (COLS - DCOLS)
#define DROWS_OFFSET (ROWS - DROWS)

#define MESSAGE_ROWS 5
#define STAT_BAR_WIDTH 20

#define WORLD_COLS 86
#define WORLD_ROWS 57

#define MIN_DUNGEON_DEPTH 1
#define MAX_DUNGEON_DEPTH 64

#define LEVEL_CAP 20
#define EXPERIENCE_OFFSET 42

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

enum ComponentTypes {
	CREATURE_COMPONENT = 1,
	LOCATION_COMPONENT,
	RENDER_COMPONENT,

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

enum TileCategories {
	EMPTY_TILE = 0,
	SOLID_TILE,
	LIQUID_TILE,
	GROUND_TILE,
	COLOR_TILE,

	TILE_CATEGORY_COUNT
};

enum TileFlags {
	// if the tile PROPOGATES fire, it is flammable.
	FLAMMABLE = 1,
	// if the tile CHANGES STATE when frozen, it is freezable.
	FREEZABLE = 2,
	// if the tile IS A WALL and CAN DISAPPEAR, it is breakable.
	BREAKABLE = 4,
};

enum TileEffects {
	IS_BURNT = 1,
	IS_FROZEN = 2,
	IS_MELTED = 4,
	IS_CHARRED = 8,
	IS_FROSTED = 16,
	IS_STEAMING = 32,
	IS_EXTINGUISED = 64,
	IS_EXPLOSIVE_GAS = 128,
	IS_POISONOUS_GAS = 256,
};

enum TileIndexs {
	EMPTY = 256,
	GROUND,
	WALL,

	SOLID_BLACK,
	SOLID_WHITE,
	SOLID_RED,
	SOLID_GREEN,
	SOLID_BLUE,
	SOLID_YELLOW,
	SOLID_MAGENTA,
	SOLID_AQUA,

	END_TILE,
	TILE_TYPE_COUNT = (END_TILE-EMPTY)
};

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

typedef struct attribute {
	char
		*name,
		*description;
} attribute;

// descriptor tile
typedef struct dtile {
	char
		*name,
		*description;
	short
		base,
		flags,
		effect,
	
		// x & y of tile display source
		x,
		y;
	const color
		fg,
		bg;
} dtile;

// compressed tile
typedef struct ctile {
	unsigned char
		effect,
		duration;
	short tile;
} ctile;

typedef struct dcell {
	short
		empty,
		changed;
	ctile tile;
} dcell;

typedef struct entity {
	unsigned int id;
	void
		*components[COMPONENT_TYPE_COUNT],
		*prev,
		*next;
}	entity;

typedef struct creature_component {
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

typedef struct render_component {
	short tile;
} render_component;

typedef struct scene {
	short
		w,
		h,
		entity_count;
	ctile **tiles;
	entity
		*head,
		*tail;
} scene;

/*
** functions
*/

int initializeSDL();
void exitSDL(int status);
int frameCap(int last_update);
int pollEvents();
void updateRenderingInfo();
void update();
void render();
void renderSalvage();
void renderChanges();
void clearScreen();
void lookupTile(SDL_Rect *source, unsigned int value);
void evaluateRGB(color col, short *r, short *g, short *b);
void changeScene(scene *dest);

const char* getAttributeName(int index);
const char* getAttributeDescription(int index);
int getExperienceNeeded(int level);

entity* createEntity(unsigned int id);
void* addComponent(entity *target, int component_type);
void* getComponent(entity *target, int component_type);
void removeComponent(entity *target, int component_type);

void initializeMenu();
void initializeOverworld();
void cleanupScene(scene *target);
void addEntity(scene *dest, entity *target);
void delEntity(scene *dest, entity *target);
entity* getEntities(scene *source);

/*
*/
