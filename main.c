#include "yendor.h"
#include "globals.h"

int main(int argc, char **argv) {
  int status = 0;
  
  G_Init(NULL);

  threads[FILE_THREAD] = SDL_CreateThread(G_LoadChunks, "filehandler", NULL);
  SDL_DetachThread(threads[FILE_THREAD]);

  threads[INITIALIZE_THREAD] = SDL_CreateThread(G_NetworkingInit, "networking-initialization", NULL);
  SDL_DetachThread(threads[INITIALIZE_THREAD]);

  while (game_info.running) {
    if (G_PollEvents(NULL) < 0) {
      game_info.running = 0;
    }

    #ifndef FRAME_CAP
      if ((1000.0/FPS) <= (SDL_GetTicks()-game_info.last_update)) {
    #endif

      G_UpdateBegin();

      threads[UPDATE_THREAD] = SDL_CreateThread(G_Update, "game-updating", NULL);

      G_Render(NULL);

      SDL_WaitThread(threads[UPDATE_THREAD], &status);

      G_UpdateEnd();

    #ifdef FRAME_CAP
      int delay = (1000.0/FPS)-(SDL_GetTicks()-game_info.last_update);

      if (delay > 0) {
        SDL_Delay(delay);
      }
    #endif

      game_info.last_update = SDL_GetTicks();

    #ifndef FRAME_CAP
      } else {
        game_info.frame_count += 1;
        G_RenderLight();
      }
    #endif
  };

  G_Quit();

  return 0;
}
