#include <math.h>
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
#define DCOLS_OFFSET (COLS - DCOLS - 1)
#define DROWS_OFFSET (ROWS - DROWS - 1)

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
	LIGHT_COMPONENT,

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
	ENTITY,
	COLOR,

	TILE_CATEGORY_COUNT
};

enum TileFlags {
	OBSTRUCTS = 1,
	// if the tile PROPOGATES fire, it is flammable.
	FLAMMABLE = 2,
	// if the tile CHANGES STATE when frozen, it is freezable.
	FREEZABLE = 4,
	// if the tile IS A WALL and CAN DISAPPEAR, it is breakable.
	BREAKABLE = 8,
};

enum TileIndexes {
	NOTHING = 256,
	DIRT,
	WALL,

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
	int
		red,
		green,
		blue,
		redRand,
		greenRand,
		blueRand,
	
		flickers;
} color;

typedef struct light {
	int intensity;
	color value;
} light;

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
	int
		base,
		flags,
		// x & y of tile display source
		x,
		y;
	const color
		*fg,
		*bg;
} dtile;

// compressed tile
typedef struct ctile {
	short tile;
} ctile;

typedef struct dcell {
	int
		tile,
		//the alpha value of the foreground image
		alpha,
		//the entity tile, if an entity is present
		entity,
		//whether or not this tile needs to be redrawn
		changed,
		//whether or not this tile needs to be updated
		visible,
		discovered;
	color
		fg,
		bg;
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
	int
		x,
		y,
		z,
		x_previous,
		y_previous,
		x_drawn,
		y_drawn,
		tile;
} render_component;

typedef struct light_component {
	light light_value;
} light_component;

typedef struct scene {
	int
		w,
		h,
		entity_count;
	ctile **tiles;
	entity
		*head,
		*tail;
	light ambient_light;
} scene;

typedef struct mouse_state {
	int
		x,
		y,
		lb,
		rb;
} mouse_state;

typedef struct lightmap_node {
	int
		red,
		green,
		blue,
		alpha,
		light_count;
	
} lightmap_node;

/*
** functions
*/

int initializeSDL();
void exitSDL(int status);
int frameCap(int last_update);
int pollEvents();
int handleEvents();
void focusView();
void update();
void gameStep();
void changeScene(scene *dest);
void initializeKeybindings();
int checkBoundKey(unsigned int keybinding);
int isPointWithin(int x, int y, SDL_Rect *dst);

// rendering.c
void updateRenderingInfo();
void render();
void renderSalvage();
void renderChanges();
void clearScreen();
void evaluateRGB(color col, int *r, int *g, int *b);
void generateFOV(int x, int y);
void castLight(
	int distance, int x, int y,
	int invert, int dx, int dy,
	float start, float end);
void decrementVis();
void addLight(int x, int y, light light_value);
void clearLightmap();
color mixColor(color first, color second);
light normalizeLight(lightmap_node node);
void renderLightmap();

// tiles.c
int getTileTime(int x, int y);
int getTileValue(int x, int y);
int getTileChanged(int x, int y);
const dtile* lookupTile(unsigned int value);
void lookupTileSource(SDL_Rect *source, unsigned int value);

// attributes.c
const char* getAttributeName(int index);
const char* getAttributeDescription(int index);
int getExperienceNeeded(int level);

// entity.c
entity* createEntity(unsigned int id);
void* addComponent(entity *target, int component_type);
void* getComponent(entity *target, int component_type);
void removeComponent(entity *target, int component_type);
void entityPos(entity *target, int *x, int *y);
void entityMov(entity *target, scene *src, scene *dest, int x, int y, int relative);
void entityUpdate(entity *target);

// scene.c
void initializeMenu();
void initializeOverworld();
void cleanupScene(scene *target);
void addEntity(scene *dest, entity *target);
void delEntity(scene *dest, entity *target);
void popEntity(scene *dest, entity *target);
entity* getEntities(scene *source);

/*
*/
