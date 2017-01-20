#include "yendor.h"


int main(int argc, char **argv) {
	int status = initializeSDL(),
		last_update = SDL_GetTicks();

	seedWorldTest();

	if (status > GRPHCS_OK) {
		initializeMenu();

		while (1) {
			if (pollEvents() == -1) {
				break;
			}
			update();
			render();
			last_update = frameCap(last_update);
		}
	}

	exitSDL(status);
  return 0;
}
