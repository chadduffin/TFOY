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
#define STAT_BAR_WIDTH 19

#define WORLD_COLS 512
#define WORLD_ROWS 256

#define MIN_DUNGEON_DEPTH 1
#define MAX_DUNGEON_DEPTH 64

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

enum ComponentTypes {
	CREATURE_COMPONENT = 1,
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
	EMPTY = 0,
	SOLID,
	LIQUID,
	GROUND,
	CHARACTER,
	COLOR,

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

enum TileIndexes {
	NOTHING = 256,
	DIRT,
	WOODEN_WALL,

	HUMAN,

	AQUA,
	BLACK,
	BLUE,
	GREEN,
	MAGENTA,
	RED,
	WHITE,
	YELLOW,

	END_TILE,
	TILE_TYPE_COUNT = (END_TILE-EMPTY)
};

enum KeybindingTypes {
	NO_BINDING = 0,
	ACCEPT,
	CANCEL,
	LEFT,
	LEFT_UP,
	LEFT_DOWN,
	RIGHT,
	RIGHT_UP,
	RIGHT_DOWN,
	UP,
	DOWN,

	KEYBINDING_TYPE_COUNT
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
		*fg,
		*bg;
} dtile;

typedef struct ctile {
	unsigned char
		effect,
		duration;
	short tile;
} ctile;

typedef struct dcell {
	short
		alpha,
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

typedef struct render_component {
	short
		x,
		y,
		z,
		tile;
} render_component;

typedef struct player_component {
	entity *myself;
} player_component;

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

typedef struct mouse_state {
	int
		x,
		y,
		lb,
		rb;
} mouse_state;

/*
** functions
*/

int initializeSDL();
void exitSDL(int status);
int frameCap(int last_update);
int pollEvents();
void updateRenderingInfo();
void update();
void focusView();
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
void moveEntity(entity *target, scene *src, scene *dest, int x, int y);

void initializeMenu();
void initializeOverworld();
void cleanupScene(scene *target);
void addEntity(scene *dest, entity *target);
void delEntity(scene *dest, entity *target);
void popEntity(scene *dest, entity *target);
entity* getEntities(scene *source);
void moveObject(entity *target, scene *src, scene *dest, int x, int y);

void initializeKeybindings();

void gameStep();

/*
*/
