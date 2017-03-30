#include "yendor.h"
#include "globals.h"

int main(int argc, char **argv) {
  int status = 0;

  G_Init(NULL);

  threads[INITIALIZE_THREAD] = SDL_CreateThread(G_NetworkingInit, "networking-initialization", NULL);
  SDL_DetachThread(threads[INITIALIZE_THREAD]);

  while (game_info.running) {
unsigned int a = SDL_GetTicks();
    G_UpdateBegin();

    threads[UPDATE_THREAD] = SDL_CreateThread(G_Update, "game-updating", NULL);
    threads[RENDER_THREAD] = SDL_CreateThread(G_Render, "game-renderering", NULL);
    
    SDL_WaitThread(threads[UPDATE_THREAD], &status);
    SDL_WaitThread(threads[RENDER_THREAD], &status);

    G_UpdateEnd();
printf("%u elapsed.\n", SDL_GetTicks()-a);
  };

  G_Quit();
  return 0;
}
