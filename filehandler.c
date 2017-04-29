#include "yendor.h"
#include "globals.h"

int G_LoadChunks(void *data) {
  while (game_info.running) {
    SDL_LockMutex(fmutex);

    if (G_TreeSize(&chunks) > 0) {
      int i = 0, *chunk_list = (int*)malloc(sizeof(int)*G_TreeSize(&chunks));
      G_TreeNode *min = G_TreeNodeMinimum(&chunks);

      while (min != chunks->nil) {
        chunk_list[i] = (int)(min->key);

        printf("%i added to list. 'chunks' size is now %i.\n", (int)(min->key), G_TreeSize(&chunks));

        G_TreeNodeDelete(&chunks, &min);
        min = G_TreeNodeMinimum(&chunks);
        i += 1;
      }
      
      SDL_UnlockMutex(fmutex);

      /* load chunks here */

      free(chunk_list);
    } else {
      SDL_UnlockMutex(fmutex);
    }

    SDL_Delay(250);
  }

  return 0;
}

int G_CharToInt(char input[2]) {
  return ((input[0] << 8) | input[1]);
}

void G_IntToChar(int input, char output[2]) {
  assert(input >= 0);

  output[0] = (input & 0xFF00) >> 8;
  output[1] = input & 0x00FF;
}
