#ifndef __YENDOR__
#define __YENDOR__

#include <math.h>
#include <time.h>
#include <stdio.h>
#include <limits.h>
#include <stdlib.h>
#include <assert.h>

#ifdef __unix__
	#include <SDL2/SDL.h>
	#include <SDL2_net/SDL_net.h>
	#include <SDL2_image/SDL_image.h>
#elif __linux__
	#include <SDL2/SDL.h>
	#include <SDL2/SDL_net.h>
	#include <SDL2/SDL_image.h>
#elif __APPLE__
	#include <SDL2/SDL.h>
	#include <SDL2_net/SDL_net.h>
	#include <SDL2_image/SDL_image.h>
#elif _WIN32
	#ifdef _WIN64
		/* 64-Bit Windows */
	#else
		/* 32-Bit Windows */
	#endif
#endif

/*
* DEFINES
*/

#define TITLE "Yendor"
#define VERSION "v0.0.1"

#define FPS_LATENCY 250
#define TEXTURE_COUNT 1

#define COLS 100
#define ROWS 70
#define UI_COLS 0
#define MESSAGE_ROWS 0

#define DCOLS 98
#define DROWS 68
#define DCOLS_OFFSET 1
#define DROWS_OFFSET 1

#define WORLD_COLS 512
#define WORLD_ROWS 256

#define LEVEL_CAP 20
#define EXPERIENCE_OFFSET 42

#define TILE_SOURCE_WIDTH 12
#define TILE_SOURCE_HEIGHT 12

#define FLICKER_RANGE 128
#define MAX_VIEW_DISTANCE (DCOLS/2)
#define MAX_LIGHT_DISTANCE 128

#define boolean unsigned char

/**************
**** ENUMS ****
**************/

typedef enum Thread {
  FILE_THREAD = 0,
  UPDATE_THREAD,
  RENDER_THREAD,
  NETWORK_THREAD,
  INITIALIZE_THREAD,

  WORKER_THREAD_A,
  WORKER_THREAD_B,
  WORKER_THREAD_C,

  GAME_THREAD_COUNT,
} Thread;

typedef enum Component {
	UI_COMPONENT = 0,
	LIGHT_COMPONENT,
	RENDER_COMPONENT,
  ELEMENT_COMPONENT,
	CREATURE_COMPONENT,
	CONTROLLER_COMPONENT,

	COMPONENT_COUNT
} Component;

typedef enum Tile {
  NOTHING = 256,

  GROUND,
  WALL,
  GRASS,
  BURNT_GRASS,
  WATER,
  
  BASIC_FIRE,

  HUMAN,

  END_TILE,
  TILE_COUNT = (END_TILE-NOTHING)
} Tile;

typedef enum ElementFlag {
  SPREADS_DIFFUSE = 1,
  SPREADS_PROPOGATE = 2,
} ElementFlag;

typedef enum TileFlag {
  FLICKERS_REGULAR = 1,
  FLICKERS_QUICK = 2,
  FLICKERS = FLICKERS_REGULAR | FLICKERS_QUICK,

  FLAMMABLE = 4,
  FREEZABLE = 8,
  BREAKABLE = 16,

  IS_BURNING = 32,
  IS_FREEZING = 64,

  OBSTRUCTS_VISION = 128,
  OBSTRUCTS_MOVEMENT = 256,
  OBSTRUCTS = OBSTRUCTS_VISION | OBSTRUCTS_MOVEMENT,

  DISABLES_ACTIONS = 512,
} TileFlag;

typedef enum TileLayer {
  UI_LAYER,
  CREATURE_LAYER,
  ITEM_LAYER,
  ORNAMENT_LAYER,
  BASE_LAYER,
  EFFECT_LAYER,
  GAS_LAYER,

  TILE_LAYER_COUNT
} TileLayer;

typedef enum Keybinding {
	NO_BINDING = 0,
	ACCEPT,
	CANCEL,
	LEFT,
	RIGHT,
	UP,
	DOWN,
  WAIT,

	KEYBINDING_COUNT
} Keybinding;

/*************
** TYPEDEFS **
*************/

typedef struct G_Id {
  long int value;
} G_Id;

typedef struct G_FPS {
  unsigned int frame_count, last_tick;
} G_FPS;

typedef struct G_GameInformation {
  int
    tile_w, tile_h,
    mouse_x, mouse_y,
    mouse_lb, mouse_rb,
    display_x, display_y,
    display_w, display_h,
    window_w, window_h,
    target_buffer,
    phys[SDL_NUM_SCANCODES];
  G_Id id;
  SDL_Event event;
  SDL_Window *window;
  SDL_Renderer *renderer;
  SDL_Texture *buffers[2], *textures[TEXTURE_COUNT];
  SDL_Scancode virt[KEYBINDING_COUNT];
  boolean running, full;
} G_GameInformation;

typedef struct G_Position {
  int x, y;
} G_Position;

typedef struct G_TreeNode {
  char color;
  void *data;
  long int key;
  struct G_TreeNode *left, *right, *parent;
} G_TreeNode;

typedef struct G_Tree {
  int size;
  G_TreeNode *root, *nil;
} G_Tree;

typedef struct G_TreeNodeContainer {
  G_Tree **tree;
  G_TreeNode **node;
  void (*func)(void*);
} G_TreeNodeContainer;

typedef struct G_Color {
  int r, g, b, r_rand, g_rand, b_rand;
} G_Color;

typedef struct G_Light {
  int r, g, b, intensity;
} G_Light;

typedef struct G_LightNode {
  int x, y, r, g, b, intensity, count;
  G_Id id;
} G_LightNode;

typedef struct G_Tile {
  G_Id id;
  Tile tile;
} G_Tile;

typedef struct G_TileCell {
  Tile layers[TILE_LAYER_COUNT];
} G_TileCell;

typedef struct G_TileInformation {
  char *name, *description;
  int x, y;
  G_Color *fg, *bg;
  TileFlag flags;
} G_TileInformation;

typedef struct G_TileTransition {
  int x, y;
  long int when;
  G_Id id;
  Tile is, to;
} G_TileTransition;

typedef struct G_View {
  int x, y, xp, yp, w, h;
} G_View;

typedef struct G_Entity {
  G_Id id, parent;
  void *components[COMPONENT_COUNT];
} G_Entity;

typedef struct G_UIComponent {
  void *holder;
} G_UIComponent;

typedef struct G_LightComponent {
	G_Light light;
} G_LightComponent;

typedef struct G_RenderComponent {
	int x, y, xp, yp;
	Tile tile;
  TileLayer layer;
} G_RenderComponent;

typedef struct G_ElementComponent {
  int volume, spread;
  TileFlag tile_flags;
  ElementFlag element_flags;
} G_ElementComponent;

typedef struct G_CreatureComponent {
	void *holder;
} G_CreatureComponent;
 
typedef struct G_ControllerComponent {
  G_Entity *entity;
} G_ControllerComponent;

typedef struct G_Scene {
  int w, h;
  long int step;
  G_Id id;
  G_View view;
  G_Tile *tiles;
  G_Tree *entities, *transitions;
  G_TreeNode *ins_buffer, *del_buffer;
  G_Entity *focus;
} G_Scene;

typedef struct G_Console {
  SDL_Rect src, dst;
  G_View view;
  G_TileCell tilemap[COLS][ROWS];
  G_LightNode lightmap[COLS][ROWS];
  boolean vismap[COLS][ROWS], changed[COLS][ROWS];
} G_Console;

/**************
** FUNCTIONS **
**************/

/* roguecraft.c */
int G_Init(void *data);
int G_Update(void *data);
int G_PollEvents(void* data);
int G_CopyBuffer(void *data);
int G_RenderEntities(void *data);
int G_UpdateEntities(void *data);
int G_UpdateLightmap(void *data);
int G_UpdateTransitions(void *data);
int G_CheckPhysical(SDL_Scancode key);
int G_CheckBound(Keybinding keybinding);
int G_RandomNumber(int lower, int upper);
unsigned int G_GetFPS(void);
void G_Quit(void);
void G_UpdateBegin(void);
void G_UpdateEnd(void);
void G_ClearBuffers(void);
void G_InitializeKeybindings(void);
void G_GenerateFOV(int x, int y, void *light, void (*func)(int*, int*, void*));
void G_CastShadow(int distance, int x, int y, int invert, int dx, int dy,
	float start, float end, void *light, void (*func)(int*, int*, void*));
void G_AddLight(int *x, int *y, void *data);
void G_MakeVisible(int *x, int *y, void *data);
void G_FocusView(G_Scene **view);
boolean G_CellChanged(int x, int y, int a, int b);
boolean G_LightCanShine(int fx, int fy, int lx, int ly, int dx, int dy);
boolean G_PointWithinView(int x, int y);
G_Id G_GetId(void);
Tile G_GetTile(Tile layers[TILE_LAYER_COUNT]);

/* rendering.c */
int G_Render(void *data);
void G_RenderChanges(void);
void G_RenderLightmap(void);
void G_UpdateRenderingInfo(void);
void G_EvaluateColor(G_Color color, int *r, int *g, int *b, boolean flicker);

/* networking.c */
int G_NetworkingInit(void *data);
int G_Networking(void *data);

/* entity.c */
G_Entity* G_EntityCreate(void);
void* G_EntityComponentFind(G_Entity **entity, Component component);
void* G_EntityComponentInsert(G_Entity **entity, Component component);
void G_EntityComponentDelete(G_Entity **entity, Component component);
void G_EntitySetLayer(G_Entity **entity, TileLayer layer);
void G_EntityPos(G_Entity **entity, int *x, int *y);
void G_EntityDestroy(G_Entity **entity);
void G_EntityRender(void *entity);
void G_EntityUpdate(void *entity);
void G_ElementComponentUpdate(G_Entity **entity);
void G_ControllerComponentUpdate(G_Entity **entity);
void G_EntityLightAdd(void *entity);

/* scene.c */
G_Scene* G_SceneCreate(int w, int h);
G_TileTransition* G_TileTransitionCreate(int x, int y, long long when, Tile to);
void G_SceneChange(G_Scene **scene);
void G_SceneDestroy(G_Scene **scene);
void G_SceneEntityInsert(G_Scene **scene, G_Entity **entity);
void G_SceneEntityDelete(G_Scene **scene, G_Entity **entity);
void G_SceneTransitionInsert(G_Scene **scene, G_TileTransition **transition);
void G_SceneSetGTile(G_Scene **scene, G_Tile tile, int x, int y);
Tile G_SceneGetTile(G_Scene **scene, int x, int y);
G_Tile G_SceneGetGTile(G_Scene **scene, int x, int y);
boolean G_SceneTileObstructs(G_Scene **scene, int x, int y);
boolean G_SceneTileExpose(G_Scene **scene, G_Entity **entity, int x, int y);

void G_TestScene(G_Scene **scene);

/* tiles.c */

TileFlag G_TileFlags(Tile tile);
SDL_Rect G_TileSource(Tile tile);

/* tree.c */
G_Tree* G_TreeCreate(void);
void G_TreeDestroy(G_Tree **tree);
void G_TreeNodeInsert(G_Tree **tree, G_TreeNode **node);
void G_TreeNodeInsertInner(G_Tree **tree, G_TreeNode **node);
void G_TreeNodeDelete(G_Tree **tree, G_TreeNode **node);
void G_TreeNodeSucceed(G_Tree **tree, G_TreeNode **node);
void G_TreeNodeRotateLeft(G_Tree **tree, G_TreeNode **node);
void G_TreeNodeRotateRight(G_Tree **tree, G_TreeNode **node);
void G_TreeDeleteFix(G_Tree **tree, G_TreeNode **node);
void G_TreeNodeIterate(G_Tree **tree, G_TreeNode **node, void (*func)(void*));
int G_TreeSize(G_Tree **tree);
G_TreeNode* G_TreeNodeRoot(G_Tree **tree);
G_TreeNode* G_TreeNodeFind(G_Tree **tree, long int key);
G_TreeNode* G_TreeNodeMinimum(G_Tree **tree);
G_TreeNode* G_TreeNodeSuccessor(G_Tree **tree, G_TreeNode **node);

#endif /* YENDOR */
