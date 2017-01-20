#include <stdio.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_net.h>
#include <SDL2/SDL_image.h>

#include "globals.h"

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
void initializeMenu();
void initializeWorld();
void lookupTile(SDL_Rect *source, unsigned int value);
void evaluateRGB(color col, short *r, short *g, short *b);
