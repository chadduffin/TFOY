#include "yendor.h"
#include "globals.h"

int main(int argc, char **argv) {
  int status = 0;

  G_LoadSettings(NULL);
  G_Init(NULL);

  threads[INITIALIZE_THREAD] = SDL_CreateThread(G_NetworkingInit, "networking-initialization", NULL);
  SDL_DetachThread(threads[INITIALIZE_THREAD]);

  while (game_info.running) {
    if (G_PollEvents(NULL) < 0) {
      game_info.running = 0;
    }

    G_UpdateBegin();

    threads[UPDATE_THREAD] = SDL_CreateThread(G_Update, "game-updating", NULL);

    G_Render(NULL);

    SDL_WaitThread(threads[UPDATE_THREAD], &status);

    G_UpdateEnd();
  };

  G_Quit();
  G_WriteSettings(NULL);

  return 0;
}
