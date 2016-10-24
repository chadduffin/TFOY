#include <stdio.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_net.h>
#include <SDL2/SDL_image.h>

#include "globals.h"


int main(int argc, char **argv) {
	int status = initializeSDL(),
		last_update = SDL_GetTicks();

	if (status > GRPHCS_OK) {
		while (1) {
			if (pollEvents() == -1) {
				break;
			}
			last_update = frameCap(last_update);
		}
	}

	exitSDL(status);
  return 0;
}
