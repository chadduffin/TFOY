#include "yendor.h"
#include "globals.h"


int main(int argc, char **argv) {
	int status = G_Init(),
		last_update = SDL_GetTicks();

	if (status > GRPHCS_OK) {
		G_InitializeMenu();
		G_InitializeOverworld();
		G_InitializeKeybindings();
		G_ChangeScene(&menu);

		G_Render();
		location->view.unchanged = 0;

		while (game_info.running) {
			if (G_PollEvents() == -1) {
				break;
			}

			G_Update();

			if (location != NULL) {
				if (G_SceneView(&location)->unchanged == 0) {
					G_Render();
				} else {
					G_LightRender();
				}
			}

			last_update = G_FrameCap(last_update);
		}
	}

	G_CleanupScene(&menu);
	G_CleanupScene(&overworld);
  return G_Exit(status);
}
