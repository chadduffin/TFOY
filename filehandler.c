#include "yendor.h"
#include "globals.h"

int G_LoadChunks(void *data) {
  while (game_info.running) {
    SDL_LockMutex(fmutex);

    if (G_TreeSize(&chunks) > 0) {
      int i = 0, len = G_TreeSize(&chunks);
      long int *chunk_list = (long int*)malloc(sizeof(long int)*len);
      G_TreeNode *min = G_TreeNodeMinimum(&chunks);

      while (min != chunks->nil) {
        chunk_list[i] = min->key;

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

unsigned int G_CharToInt(unsigned char input[2]) {
  return ((input[0] << 8) | input[1]);
}

unsigned int G_CharToFullInt(unsigned char input[4]) {
  return ((input[0] << 24) | (input[1] << 16) | (input[2] << 8) | input[3]);
}

void G_IntToChar(unsigned int input, unsigned char output[2]) {
  assert(input >= 0);

  output[0] = (input & 0xFF00) >> 8;
  output[1] = (input & 0x00FF);
}

void G_FullIntToChar(unsigned int input, unsigned char output[4]) {
  assert(input >= 0);

  output[0] = (input & 0xFF000000) >> 24;
  output[1] = (input & 0x00FF0000) >> 16;
  output[2] = (input & 0x0000FF00) >> 8;
  output[3] = (input & 0x000000FF);
}

void G_LoadChunksInner(long int *list, unsigned int length) {
  char *filename = NULL;
  unsigned char c[4], *chunk = NULL;
  unsigned int i, j, chunk_length, id, id_offset;
  FILE *file = NULL;

  filename = (char*)malloc(sizeof(char)*16);

  for (i = 0, j = 0; i < length; i += 1) {
    id = list[i];
    id_offset = id-(id%256);
    sprintf(filename, "data/c.%3i.tfoy", id/256);
    filename[7] = ((id/256) > 99) ? filename[2] : '0';
    filename[8] = ((id/256) > 9) ? filename[3] : '0';
    file = fopen(filename, "rb");

    if (file != NULL) {
      fread(c, sizeof(unsigned char), 4, file);
      chunk_length = G_CharToFullInt(c);

      while ((j+id_offset != id) && (!feof(file)))  {
        fseek(file, chunk_length, SEEK_CUR);
        fread(c, sizeof(unsigned char), 4, file);
        chunk_length = G_CharToFullInt(c);
        j += 1;
      }

      if ((j+id_offset == id) && (!feof(file))) {
        chunk = (unsigned char*)malloc(sizeof(unsigned char)*chunk_length);
        
        fread(chunk, sizeof(unsigned char), chunk_length, file);
        G_SceneChunk *test = G_DecodeChunk(chunk, chunk_length);

        free(chunk);
      } else {
        printf("Chunk #%i could not be located.\n", id);
      }
    } else {
      printf("'%s' could not be opened.\n", filename);
    }

    fclose(file);
  }
}

unsigned char* G_EncodeChunk(G_SceneChunk *chunk, unsigned int *length) {
  int i, j, num;
  unsigned int len = 0;
  unsigned char container[CHUNK_SIZE*CHUNK_SIZE*3], *encoded = NULL;

  for (i = 0, j = 0; i < CHUNK_SIZE*CHUNK_SIZE*3; i += 3) {
    num = 1;

    while ((j < CHUNK_SIZE*CHUNK_SIZE-1) && (chunk->tiles[j].tile == chunk->tiles[j+1].tile)) {
      j += 1;
      num += 1;
    }

    container[i] = (unsigned char)(num);
    G_IntToChar(chunk->tiles[j].tile, container+i+1);
    //container[i+1] = (chunk->tiles[j].tile & 0xFF00) >> 8;
    //container[i+2] = (chunk->tiles[j].tile * 0x00FF);

    j += 1;
    len += 3;

    if (j == CHUNK_SIZE*CHUNK_SIZE) {
      break;
    }
  }

  encoded = (unsigned char*)malloc(sizeof(unsigned char)*len);

  for (i = 0; i < len; i += 1) {
    encoded[i] = container[i];
  }

  *length = len;

  return encoded;
}

G_SceneChunk* G_DecodeChunk(unsigned char *chunk, unsigned int length) {
  unsigned int i, j, lim;
  G_SceneChunk *decoded = NULL;

  decoded = (G_SceneChunk*)malloc(sizeof(G_SceneChunk));
  decoded->tiles = (G_Tile*)malloc(sizeof(G_Tile)*CHUNK_SIZE*CHUNK_SIZE);

  for (i = 0, j = 0; i < length; i += 3) {
    lim = j+(unsigned int)(chunk[i]);

    for (; j < lim; j += 1) {
      decoded->tiles[j].tile = G_CharToInt(chunk+i+1);
    }
  }

  return decoded;
}
