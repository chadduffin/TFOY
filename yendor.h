#ifndef __YENDOR__
#define __YENDOR__

#include <math.h>
#include <time.h>
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_net.h>
#include <SDL2/SDL_image.h>

/*
** DEFINES
*/

#define TITLE "The Fellowships Of Yendor"
#define VERSION "v0.0.1"

#define FPS 30
#define TEXTURE_COUNT 1
// #define NDEBUG

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
#define UI_COLS 0
#define MESSAGE_ROWS 0

#define DCOLS COLS
#define DROWS ROWS
#define DCOLS_OFFSET 0
#define DROWS_OFFSET 0

#define WORLD_COLS 512
#define WORLD_ROWS 256

#define MIN_DUNGEON_DEPTH 1
#define MAX_DUNGEON_DEPTH 64

#define LEVEL_CAP 20
#define EXPERIENCE_OFFSET 42

#define FLICKER_RATE 512
#define LIGHT_DISTANCE 128

#define boolean unsigned char

/*
** ENUMS
*/

typedef enum Attribute {
	MIGHT,
	INTELLECT,
	CONSTITUTION,

	ATTACK_POWER,
	SPELL_POWER,

	HASTE,
	DEXTERITY,
	CRITICAL_STRIKE,
	
	DODGE,
	BLOCK,
	PARRY,
	RESOLVE,
	FORTITUDE,

	ARMOR,
	LEVEL,
	EXPERIENCE,

	HEALTH,
	ENERGY,
	MANA,
	RAGE,

	HUNGER,
	THIRST,
	WETNESS,
	TEMPERATURE,

	ATTRIBUTE_COUNT
} Attribute;

typedef enum EntityType {
	NULL_ENTITY = 0,
	GAME_ENTITY = 1,
	UI_ENTITY = 2,

	ANY_ENTITY = GAME_ENTITY | UI_ENTITY,
} EntityType;

typedef enum Component {
	CONTROLLER_COMPONENT = 0,
	CREATURE_COMPONENT,
	RENDER_COMPONENT,
	LIGHT_COMPONENT,

	UI_COMPONENT,

	COMPONENT_COUNT
} Component;

typedef enum CreatureFlag {
	IS_INVULNERABLE = 1,
	IS_INVISIBLE = 2,

	CREATURE_FLAG_COUNT
} CreatureFlag;

typedef enum CreatureCategory {
	ANIMAL = 0,
	DRAGON,
	UNDEAD,
	MAGICAL,
	HUMANOID,
	INFERNAL,
	AIRBORNE,
	FIREBORNE,
	WATERBORNE,
	EARTHBORNE,
	ABOMINATION,

	CREATURE_CATEGORY_COUNT
} CreatureCategory;

typedef enum TileCategory {
	EMPTY = 0,
	SOLID,
	LIQUID,
	GROUND,
	ENTITY,
	COLOR,

	TILE_CATEGORY_COUNT
} TileCategory;

typedef enum TileFlag {
	OBSTRUCTS = 1,
	FLAMMABLE = 2,
	FREEZABLE = 4,
	BREAKABLE = 8,

	FLICKERS = 16,
	FLICKER_ONCE = 32,
} TileFlag;

typedef enum Tile {
	NOTHING = 256,
	DIRT,
	GRASS,
	WATER,
	WALL,

	BALL,

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
	TILE_COUNT = (END_TILE-NOTHING)
} Tile;

typedef enum Keybinding {
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

	KEYBINDING_COUNT
} Keybinding;

typedef enum UIState {
	INACTIVE = 0,
	ACTIVE = 1,
	HOVER = 2,
	PRESSED = 4,
	CHANGED = 8,

} UIState;

typedef enum TileLayer {
	CREATURE_LAYER = 0,
	ORNAMENT_LAYER,
	EFFECT_LAYER,
	ITEM_LAYER,
	GAS_LAYER,
	UI_LAYER,

	TILE_LAYER_COUNT
} TileLayer;

/*
** TYPEDEFS
*/

typedef struct G_Color {
	int
		red,
		green,
		blue,
		red_rand,
		green_rand,
		blue_rand;
} G_Color;

typedef struct G_Light {
	int
		red,
		green,
		blue,
		intensity;
} G_Light;

typedef struct G_LightNode {
	int
		x,
		y,
		id,
		count;
	G_Light light;
} G_LightNode;

typedef struct G_Attribute {
	char
		*name,
		*description;
} G_Attribute;

typedef struct G_TileDescriptor {
	char
		*name,
		*description;
	int
		x,
		y;
	const G_Color
		*fg,
		*bg;
	TileFlag flags;
	TileCategory base;
} G_TileDescriptor;

typedef struct G_Tile {
	Tile tile;
} G_Tile;

typedef struct G_Cell {
	int layers[TILE_LAYER_COUNT];
	boolean
		changed,
		visible;
	Tile
		tile;
	G_Color
		fg,
		bg;
	G_LightNode light;
} G_Cell;

typedef struct G_Entity {
	unsigned int id;
	void
		*components[COMPONENT_COUNT],
		*prev,
		*next;
	EntityType type;
}	G_Entity;

typedef struct G_ControllerComponent {
	G_Entity *entity;
} G_ControllerComponent;

typedef struct G_CreatureComponent {
	int base_attributes[ATTRIBUTE_COUNT];
	CreatureFlag flags;
	CreatureCategory category;
} G_CreatureComponent;

typedef struct G_RenderComponent {
	int
		x,
		y,
		z,
		x_previous,
		y_previous;
	Tile tile;
} G_RenderComponent;

typedef struct G_LightComponent {
	G_Light light;
} G_LightComponent;
 
typedef struct G_UIComponent {
	char
		hotkey,
		*name;
	int
		x,
		y,
		l,
		focus;
	void
		**data,
		(*on_click)(void**),
		(*on_hover)(void**);
	boolean border;
	UIState state;
} G_UIComponent;

typedef struct G_View {
	int
		x,
		y,
		xp,
		yp,
		w,
		h;
	boolean unchanged;
} G_View;

typedef struct G_Scene {
	int
		w,
		h,
		l,
		entity_count;
	G_Tile *tiles;
	G_Entity
		*head,
		*tail,
		*focus,
		*inspect;
	G_View view;
	G_Light ambient;
} G_Scene;

typedef struct G_Info {
	int
		tile_w,
		tile_h,
		mouse_x,
		mouse_y,
		mouse_lb,
		mouse_rb,
		window_w,
		window_h,
		display_x,
		display_y,
		display_w,
		display_h,
		target_buffer,
		phys[SDL_NUM_SCANCODES];
	SDL_Event event;
	SDL_Window *window;
	SDL_Renderer *renderer;
	SDL_Texture
		*buffers[2],
		*textures[TEXTURE_COUNT];
	SDL_Scancode virt[KEYBINDING_COUNT];
	boolean running;
} G_Info;

/*
** FUNCTIONS
*/

int G_Init(void);
int G_Exit(int status);
void G_Quit(void **data);
int G_FrameCap(int last_update);
int G_PollEvents(void);
int G_HandleEvents(void);
void G_FocusView(void);
void G_Update(void);
void G_LightUpdate(void);
void G_LoopEntities(EntityType type, void (*func)(G_Entity**));
void G_InitializeKeybindings(void);
int G_CheckBound(Keybinding key);
int G_CheckPhysical(SDL_Scancode key);
int G_IsPointWithin(int x, int y, G_View *view);
unsigned int G_GetID(void);
char* G_IntToChar(int value);
char* G_FloatToChar(float value);

// rendering.c
void G_UpdateRenderingInfo(void);
void G_Render(void);
void G_LightRender(void);
void G_RenderSalvage(void);
void G_RenderChanges(void);
void G_RenderFlicker(float frequency);
void G_RenderLightmap(void);
void G_ClearScreen(void);
void G_EvaluateRGB(G_Color col, int *r, int *g, int *b, boolean flicker);
void G_GenerateFOV(int x, int y, void *light, void (*func)(int*, int*, void*));
void G_CastShadow(
	int distance, int x, int y,
	int invert, int dx, int dy,
	float start, float end,
	void *light, void (*func)(int*, int*, void*));
void G_MarkVisible(int *x, int *y, void *data);
void G_AddLight(int *x, int *y, void *data);
void G_ClearAndDecrement(void);
void G_InvalidateView(void);
boolean G_LightCanShine(int fx, int fy, int lx, int ly, int dx, int dy);
Tile G_CellToTile(int x, int y);

// tiles.c
const char* G_TileName(Tile tile);
const char* G_TileDescription(Tile tile);
G_Color G_TileForeground(Tile tile);
G_Color G_TileBackground(Tile tile);
TileFlag G_TileFlags(Tile tile);
boolean G_TileSolid(Tile tile);
boolean G_TileFlickers(Tile tile);
boolean G_TileFlickerOnce(Tile tile);
boolean G_TileObstructs(Tile tile);
void G_TileSource(Tile tile, SDL_Rect *source);

// attributes.c
const char* G_GetAttributeName(Attribute attribute);
const char* G_GetAttributeDescription(Attribute attribute);
int G_GetExperienceNeeded(int level);

// entity.c
G_Entity* G_CreateEntity(EntityType type);
void* G_AddComponent(G_Entity **entity, Component component);
void* G_GetComponent(G_Entity **entity, Component component);
void G_RemoveComponent(G_Entity **entity, Component component);
void G_EntityPos(G_Entity **entity, int *x, int *y);
void G_EntityMov(G_Entity **entity, G_Scene **src, G_Scene **dst);
void G_EntityUpdate(G_Entity **entity);
void G_EntityRender(G_Entity **entity);
EntityType G_GetEntityType(G_Entity **entity);
Tile G_EntityIDToTile(int ID);

// scene.c
void G_InitializeMenu(void);
void G_InitializeOverworld(void);
void G_ChangeScene(void **scene);
void G_InitializeUI(G_Scene **scene);
void G_CleanupScene(G_Scene **scene);
void G_AddEntity(G_Scene **scene, G_Entity **entity);
void G_DelEntity(G_Scene **scene, G_Entity **entity);
void G_PopEntity(G_Scene **scene, G_Entity **entity);
G_View* G_SceneView(G_Scene **scene);
G_Entity* G_GetEntities(G_Scene **scene);
G_Entity* G_FindEntity(G_Scene **scene, int ID);
Tile G_SceneTile(int x, int y);

/*
*/

#endif /* YENDOR */
