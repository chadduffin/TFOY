#include "yendor.h"
#include "globals.h"

int G_LoadChunks(void *data) {
  while (game_info.running) {
    SDL_LockMutex(fmutex);

    if (G_TreeSize(&chunks) > 0) {
      int i = 0, len = G_TreeSize(&chunks), *chunk_list = (int*)malloc(sizeof(int)*len);
      G_TreeNode *min = G_TreeNodeMinimum(&chunks);

      while (min != chunks->nil) {
        chunk_list[i] = (int)(min->key);

        G_TreeNodeDelete(&chunks, &min);
        min = G_TreeNodeMinimum(&chunks);
        i += 1;
      }
      
      SDL_UnlockMutex(fmutex);
      G_LoadChunksInner(chunk_list, len);
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

void G_LoadChunksInner(int *list, int length) {
  char c, *filename = (char*)malloc(sizeof(char)*16);
  int i, j, id, id_offset;
  FILE *file = NULL;

  for (i = 0, j = 0; i < length; i += 1) {
    id = list[i];
    id_offset = id-(id%256);;
    sprintf(filename, "data/c.%3i.tfoy", id/256);
    filename[7] = ((id/256) > 99) ? filename[2] : '0';
    filename[8] = ((id/256) > 9) ? filename[3] : '0';
    file = fopen(filename, "rb");

    if (file != NULL) {
      printf("'%s' opened.\n", filename);
   
      c = fgetc(file);

      while ((j+id_offset != id) && (!feof(file)))  {
        printf("Skipping %i bytes head.\n", (int)c);
        fseek(file, (int)c, SEEK_CUR);
        c = fgetc(file);
        j += 1;
      }

      if ((j+id_offset == id) && (!feof(file))) {
        printf("%c, %c.\n", c, fgetc(file));
      } else {
        printf("Chunk #%i could not be located.\n", id);
      }
    } else {
      printf("'%s' could not be opened.\n", filename);
    }

    fclose(file);
  }
}
