#include "yendor.h"
#include "globals.h"

#include <sys/time.h>
#include <sys/resource.h>

int main(int argc, char **argv) {
  int status = 0;

  #ifndef NDEBUG
    struct rusage usage;
  #endif
  
  G_Init(NULL);

  threads[FILE_THREAD] = SDL_CreateThread(G_LoadChunks, "filehandler", NULL);
  SDL_DetachThread(threads[FILE_THREAD]);

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

  #ifndef NDEBUG
    getrusage(RUSAGE_SELF, &usage);

    printf("Resident Set Size: %liMB\n", usage.ru_maxrss/1048576);
    printf("Minor Page Faults: %li\n", usage.ru_minflt);
    printf("Major Page Faults: %li\n", usage.ru_majflt);
  #endif

  return 0;
}
