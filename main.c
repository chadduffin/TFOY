#include "yendor.h"
#include "globals.h"


int main(int argc, char **argv) {
	int status = initializeSDL(),
		last_update = SDL_GetTicks();

	clearScreen();

	if (status > GRPHCS_OK) {
		initializeMenu();
		initializeOverworld();
		initializeKeybindings();
		changeScene(&menu);

		while (1) {
			if (pollEvents() == -1) {
				break;
			}

			update();
			render();
			last_update = frameCap(last_update);
		}
	}

	cleanupScene(&menu);
	cleanupScene(&overworld);
	exitSDL(status);
  return 0;
}
