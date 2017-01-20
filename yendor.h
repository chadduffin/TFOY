#include <stdio.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_net.h>
#include <SDL2/SDL_image.h>

#include "scene.h"
#include "display.h"
#include "globals.h"
#include "character.h"
#include "attributes.h"

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

/*
*/
