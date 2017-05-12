#include "yendor.h"
#include "globals.h"

#include <sys/time.h>
#include <sys/resource.h>

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

/* TEMPORARY */
unsigned int a = SDL_GetTicks();
/*************/

    G_UpdateBegin();

    threads[UPDATE_THREAD] = SDL_CreateThread(G_Update, "game-updating", NULL);

    G_Render(NULL);

    SDL_WaitThread(threads[UPDATE_THREAD], &status);

    G_UpdateEnd();

/* TEMPORARY */
if (SDL_GetTicks()-a < 1000.0/60) {
  SDL_Delay((1000.0)/60-(SDL_GetTicks()-a));
}
/*************/
  };

  G_Quit();

  return 0;
}
