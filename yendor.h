#ifndef __YENDOR__
#define __YENDOR__

#include <math.h>
#include <time.h>
#include <stdio.h>
#include <limits.h>
#include <stdlib.h>
#include <assert.h>

#ifdef __linux__
  #include <sys/stat.h>
  #include <sys/types.h>

	#include <SDL2/SDL.h>
	#include <SDL2/SDL_net.h>
	#include <SDL2/SDL_image.h>
#elif __APPLE__
  #include <sys/stat.h>
  #include <sys/types.h>

	#include <SDL2/SDL.h>
	#include <SDL2_net/SDL_net.h>
	#include <SDL2_image/SDL_image.h>
#elif __unix__
  #include <sys/stat.h>
  #include <sys/types.h>

	#include <SDL2/SDL.h>
	#include <SDL2_net/SDL_net.h>
	#include <SDL2_image/SDL_image.h>
#elif _WIN32
  #include "SDL2/SDL.h"
  #include "SDL2/SDL_net.h"
  #include "SDL2/SDL_image.h"
#endif

/*
 * DEFINES
 */

#define TITLE "The Fellowships of Yendor"
#define VERSION "v0.0.1"

/* #define NDEBUG */
#define TIMING

#define TEXTURE_COUNT 1

#define COLS 80
#define ROWS 45

#define CHUNK_SIZE 256
#define FILE_CHUNK_SIZE 16

#define WORLD_WIDTH 512
#define WORLD_HEIGHT 512

#define LEVEL_CAP 20
#define EXPERIENCE_OFFSET 42

#define FRAME_CAP
#define UPDATE_DELAY 1014
#define FLICKER_RANGE 48
#define MAX_VIEW_DISTANCE (DCOLS/2)
#define MAX_LIGHT_DISTANCE 128

#define ELEMENT_THRESHOLD 16

#define boolean unsigned char

/*
 * ENUMS
 */

typedef enum Thread {
  FILE_THREAD = 0,
  UPDATE_THREAD,
  RENDER_THREAD,
  NETWORK_THREAD,
  INITIALIZE_THREAD,

  WORKER_THREAD_A,
  WORKER_THREAD_B,
  WORKER_THREAD_C,
  WORKER_THREAD_D,

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
  STONE,
  SAND,
  WALL,
  GRASS,
  TALL_GRASS,
  BURNT_GRASS,
  FROZEN_GRASS,
  FUNGUS,
  WATER,

  ICE,
  LAVA,
  
  BASIC_FIRE,
  GREEN_FIRE,

  HUMAN,

  SOLID_COLOR,
  SOLID_WHITE,
  SOLID_BLACK,
  SOLID_RED,
  SOLID_GREEN,
  SOLID_BLUE,
  SOLID_MAGENTA,

  ERROR_TILE,
  END_TILE,
  TILE_COUNT = (END_TILE-NOTHING)
} Tile;

typedef enum ElementFlag {
  SPREADS_FLUID = 0,
  SPREADS_EXPLODE,
  SPREADS_DIFFUSE,
  SPREADS_PROPOGATE,

  ELEMENT_FLAG_COUNT
} ElementFlag;

typedef enum TileFlag {
  FLICKERS_REGULAR    = 0x0001,
  FLICKERS_QUICK      = 0x0002,
  FLICKERS_SLOW       = 0x0004,
  FLICKERS = FLICKERS_REGULAR | FLICKERS_QUICK | FLICKERS_SLOW,

  BARELY_FLAMMABLE    = 0x0008,
  NORMAL_FLAMMABLE    = 0x0010,
  HIGHLY_FLAMMABLE    = 0x0020,
  FLAMMABLE = BARELY_FLAMMABLE | NORMAL_FLAMMABLE | HIGHLY_FLAMMABLE,

  FREEZABLE           = 0x0040,
  BREAKABLE           = 0x0080,
  LUMINESCENT         = 0x0100,
  ILLUMINATING        = 0x0200,

  IS_BURNING          = 0x0400,
  IS_FREEZING         = 0x0800,
  IS_EXTINGUISHING    = 0x1000,

  OBSTRUCTS_VISION    = 0x2000,
  OBSTRUCTS_MOVEMENT  = 0x4000,
  OBSTRUCTS = OBSTRUCTS_VISION | OBSTRUCTS_MOVEMENT,

  DISABLES_ACTIONS    = 0x8000,
} TileFlag;

typedef enum TileLayer {
  UI_LAYER,
  CREATURE_LAYER,
  PROPOGATE_LAYER,
  ITEM_LAYER,
  ORNAMENT_LAYER,
  BASE_LAYER,

  DIFFUSE_LAYER,
  EXPLODE_LAYER,

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

typedef enum UIFlag {
  MOUSE_PRESSED = 1,
  KEY_PRESSED = 2,
  VISIBLE = 4,
  ACTIVE = 8,
  HOVER = 16,
  FOCUS = 32,

  PRESSED = MOUSE_PRESSED | KEY_PRESSED,
} UIFlag;

typedef enum ChunkStatus {
  NOT_LOADED = 0,
  IS_MISSING,
  IS_LOADING,

  IS_LOADED,
} ChunkStatus;

typedef enum DirectionFlags {
  NA = 0, // no direction
  EE = 1,
  NE = 2,
  NN = 4,
  NW = 8,
  WW = 16,
  SW = 32,
  SS = 64,
  SE = 128,
  CA = EE | NN | WW | SS,
  DI = NE | NW | SE | SW,
  AL = EE | NE | NN | NW | WW | SW | SS | SE,

  DIRECTION_COUNT = 8
} DirectionFlags;

/*
 * TYPEDEFS
 */

typedef struct G_Id {
  long int value;
} G_Id;

typedef struct G_Setting {
  char key[32];
  int value;
} G_Setting;

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
  unsigned int
    timer, frame_count, last_update;
  G_Id id;
  SDL_Event event;
  SDL_Window *window;
  SDL_Renderer *renderer;
  SDL_Texture *buffers[2], *textures[TEXTURE_COUNT];
  SDL_Scancode virt[KEYBINDING_COUNT];
  boolean running, redraw, full;
} G_GameInformation;

typedef struct G_Position {
  int x, y;
} G_Position;

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
  unsigned short tile;
} G_Tile;

typedef struct G_UITile {
  unsigned short tile;
  G_Color fg;
} G_UITile;

typedef struct G_TileCell {
  Tile layers[TILE_LAYER_COUNT];
  G_Color *fg, *bg;
  boolean fchange;
} G_TileCell;

typedef struct G_TileInformation {
  char *name, *description;
  int x, y;
  G_Color *fg, *bg;
  TileFlag flags;
} G_TileInformation;

typedef struct G_TileTransition {
  int x, y;
  unsigned int when;
  Tile is, to;
} G_TileTransition;

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

typedef struct G_View {
  int x, y, xp, yp, w, h;
  boolean follows;
} G_View;

typedef struct G_UIWidget {
  int x, y, w, h, focus, depth, length;
  void (*func)(void*), *data;
  SDL_Scancode hotkey;
  G_Color fg, bg;
  G_UITile *tiles;
  boolean changed;
  UIFlag flags;
} G_UIWidget;

typedef struct G_UIWindow {
  int x, y, w, h, depth;
  boolean visible;

  G_Color fg, bg;
  G_Tree *widgets, *windows;
} G_UIWindow;

typedef struct G_Entity {
  G_Id id;
  void *components[COMPONENT_COUNT];
} G_Entity;

typedef struct G_UIComponent {
  G_UIWindow *root;
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
  int amount, intensity, dissipation;
  TileFlag tile_flags, target_flag;
  ElementFlag element_flags;
  DirectionFlags directions;
  void (*func)(int, int, unsigned int);
} G_ElementComponent;

typedef struct G_CreatureComponent {
	void *holder;
} G_CreatureComponent;
 
typedef struct G_ControllerComponent {
  G_Entity *entity;
} G_ControllerComponent;

typedef struct G_QTreeLeaf {
  struct G_QTree *parent;
  G_Entity *entities[TILE_LAYER_COUNT];
} G_QTreeLeaf;

typedef struct G_QTree {
  int size, count;
  void *children[4];
  struct G_QTree *parent;
} G_QTree;

typedef struct G_SceneChunk {
  G_Tile *tiles;
  ChunkStatus status;
} G_SceneChunk;

typedef struct G_Scene {
  int w, h, tw, th;
  unsigned int step;
  G_Id id;
  G_View view;
  G_Entity *focus;
  G_SceneChunk *chunks;
  G_Tree *entities, *transitions;
  G_TreeNode *ins_buffer, *del_buffer;
  G_QTree *collision;
  G_Light ambient;
  boolean persistent;
} G_Scene;

typedef struct G_Console {
  SDL_Rect src, dst;
  G_View view;
  G_TileCell tilemap[COLS][ROWS];
  G_LightNode lightmap[COLS][ROWS];
  boolean vismap[COLS][ROWS], changed[COLS][ROWS];
} G_Console;

/*
 * FUNCTIONS
 */

/* TEMPORARY */
void G_Print(void *data);
void G_FillChunk(G_SceneChunk* chunk, int id);
double G_Noise(double X, double Y);
double G_NoiseOctaveSum(int X, int Y);
/*************/

/* yendor.c */
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
float G_GetSlope(int scene_x, int scene_y, int x, int y, int dx, int dy, int invert, boolean top);
double G_GetTime(void);
void G_Quit(void);
void G_UpdateBegin(void);
void G_UpdateEnd(void);
void G_UpdateInfrequent(void);
void G_ClearBuffers(void);
void G_InitializeKeybindings(void);
void G_GenerateFOV(int x, int y, int range, void *light, void (*func)(int*, int*, void*));
void G_GenerateFOVSimple(int x, int y, void *light, void (*func)(int*, int*, void*));
void G_Shadowcast(int scene_x, int scene_y, int dx, int dy, int dist, int range, int invert, float start, float end, void *data, void (*func)(int*, int*, void*));
void G_AddLight(int *x, int *y, void *data);
void G_AddPointLight(int x, int y, int r, int g, int b, int intensity);
void G_MakeVisible(int *x, int *y, void *data);
void G_FocusView(G_Scene **view);
void G_ResizeDPort(int x, int y, int w, int h);
void G_InitializeMenu(G_Scene **scene);
boolean G_CellChanged(int x, int y, int a, int b);
boolean G_LightCanShine(int fx, int fy, int lx, int ly, int dx, int dy);
boolean G_PointWithinView(int x, int y);
G_Id G_GetId(void);
G_Position G_GetDirectionComponents(DirectionFlags flag);
Tile G_GetTile(Tile layers[TILE_LAYER_COUNT]);

/* rendering.c */
int G_Render(void *data);
void G_RenderLight(void);
void G_RenderSalvage(void);
void G_RenderChanges(void);
void G_RenderLightmap(void);
void G_UpdateRenderingInfo(void);
void G_EvaluateColor(G_Color color, int *r, int *g, int *b, boolean flicker);

/* filehandler.c */
int G_LoadChunks(void *data);
unsigned int G_CharToInt(unsigned char *input, boolean full);
unsigned char* G_EncodeChunk(G_Tile *chunk, unsigned int *length);
void G_IntToChar(unsigned int input, unsigned char *output, boolean full);
void G_LoadChunksInner(long int *chunks, long int *scenes, unsigned int length);
G_Tile* G_DecodeChunk(unsigned char *chunk, unsigned int length);

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
void G_EntityMove(G_Entity **entity, boolean relative, int x, int y);
void G_EntityDestroy(G_Entity **entity);
void G_EntityRender(void *entity);
void G_EntityRenderUI(G_UIComponent **ui);
void G_EntityUpdate(void *entity);
void G_EntityLightAdd(void *entity);
void G_UIComponentUpdate(G_Entity **entity);
void G_ControllerComponentUpdate(G_Entity **entity);
void G_ElementComponentUpdate(G_Entity **entity);
void G_ElementFluid(G_Entity **entity);
void G_ElementDiffuse(G_Entity **entity);
void G_ElementPropogate(G_Entity **entity);
void G_ElementExplode(G_Entity **entity);

/* scene.c */
G_Scene* G_SceneCreate(int w, int h, boolean persistent);
G_TileTransition* G_TileTransitionCreate(int x, int y, unsigned int when, Tile to);
void G_SceneChange(G_Scene **scene);
void G_SceneDestroy(G_Scene **scene);
void G_SceneChunkLoad(G_Scene **scene);
void G_SceneEntityInsert(G_Scene **scene, G_Entity **entity);
void G_SceneEntityDelete(G_Scene **scene, G_Entity **entity);
void G_SceneTransitionInsert(G_Scene **scene, G_TileTransition **transition);
void G_SceneSetGTile(G_Scene **scene, G_Tile tile, int x, int y);
void G_BurnTile(int x, int y, unsigned int when);
void G_FreezeTile(int x, int y, unsigned int when);
Tile G_SceneGetTile(G_Scene **scene, int x, int y);
G_Tile G_SceneGetGTile(G_Scene **scene, int x, int y);
boolean G_SceneTileObstructs(G_Scene **scene, int x, int y);
boolean G_SceneTilePropogate(G_Scene **scene, G_Entity **entity, int x, int y, boolean sentinel);

/* tiles.c */
void G_TileUpdate(Tile tile, int x, int y);
TileFlag G_TileFlags(Tile tile);
SDL_Rect G_TileSource(Tile tile);
boolean G_TileFlagCompare(Tile tile, TileFlag flag);

/* ui.c */
G_UIWindow* G_UIWindowCreate(int x, int y, int w, int h, int depth, boolean visible);
G_UIWidget* G_UIWidgetCreate(int x, int y, int w, int h, int depth, int length, void(*func)(void*), void *data, char *text);
void G_UIWindowAddWidget(G_UIWindow **window, G_UIWidget **widget);
void G_UIWindowAddWindow(G_UIWindow **window, G_UIWindow **subwindow);
void G_UIWidgetHotkey(G_UIWidget **widget, SDL_Scancode hotkey, int index);
void G_AddUIWindow(G_UIWindow **parent, G_UIWindow **child);
void G_AddUIWidget(G_UIWindow **parent, G_UIWidget **child);
void G_UpdateUIWindow(G_UIWindow **window);
void G_UpdateUIWidget(G_UIWidget **widget);
void G_RenderUIWindow(G_UIWindow **window);
void G_RenderUIWidget(G_UIWidget **widget);
void G_UIWindowToggleVisible(G_UIWindow **window);
void G_UIWidgetToggleVisible(G_UIWidget **widget);
void G_UIWidgetToggleActive(G_UIWidget **widget);
void G_UIWindowDestroy(G_UIWindow **window);
void G_UIWidgetDestroy(G_UIWidget **widget);
void G_UIWidgetQuitGame(void *empty);

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
G_TreeNode* G_TreeNodeFind(G_Tree **tree, long int key);
G_TreeNode* G_TreeNodeMinimum(G_Tree **tree);
G_TreeNode* G_TreeNodeSuccessor(G_Tree **tree, G_TreeNode **node);

/* qtree.c */

G_QTree* G_QTreeCreate(void);
int G_QTreeQuadrant(int *x, int *y, int *size);
void G_QTreeDestroy(G_QTree **tree);
void G_QTreeNodeMove(G_QTree **tree, G_Entity **entity, TileLayer layer, int x, int y);
void G_QTreeNodeInsert(G_QTree **tree, G_Entity **entity, TileLayer layer);
void G_QTreeNodeDelete(G_QTree **tree, G_Entity **entity, TileLayer layer);
void G_QTreePurge(G_QTree **tree);
G_Entity* G_QTreeEntityFind(G_QTree **tree, TileLayer layer, int x, int y);
G_QTreeLeaf* G_QTreeNodeFind(G_QTree **tree, int x, int y);

#endif /* YENDOR */
