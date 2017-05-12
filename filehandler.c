#include "yendor.h"
#include "globals.h"

int G_LoadChunks(void *data) {
  while (game_info.running) {
    SDL_LockMutex(fmutex);

    if (G_TreeSize(&chunks) > 0) {
      int i = 0, len = G_TreeSize(&chunks);
      long int
        *chunk_list = (long int*)malloc(sizeof(long int)*len),
        *scene_list = (long int*)malloc(sizeof(long int)*len);
      G_TreeNode *min = G_TreeNodeMinimum(&chunks);

      while (min != chunks->nil) {
        chunk_list[i] = min->key;
        scene_list[i] = *(long int*)(min->data);

        G_TreeNodeDelete(&chunks, &min);
        min = G_TreeNodeMinimum(&chunks);
        i += 1;
      }
      
      SDL_UnlockMutex(fmutex);
      G_LoadChunksInner(chunk_list, scene_list, len);
      free(chunk_list);
      free(scene_list);
      len = 0;
    } else {
      SDL_UnlockMutex(fmutex);
    }

    SDL_Delay(100);
  }

  return 0;
}

unsigned int G_CharToInt(unsigned char *input, boolean full) {
  if (full) {
    return ((input[0] << 24) | (input[1] << 16) | (input[2] << 8) | input[3]);
  } else {
    return ((input[0] << 8) | input[1]);
  }
}

unsigned char* G_EncodeChunk(G_Tile *chunk, unsigned int *length) {
  int i, j;
  unsigned int len = 0, num = 0;
  unsigned char container[CHUNK_SIZE*CHUNK_SIZE*4], *encoded = NULL;

  for (i = 0, j = 0; i < CHUNK_SIZE*CHUNK_SIZE*4; i += 4) {
    num = 1;

    while ((j < CHUNK_SIZE*CHUNK_SIZE-1) && (chunk[j].tile == chunk[j+1].tile)) {
      j += 1;
      num += 1;
    }

    G_IntToChar(num, container+i, 0);
    G_IntToChar(chunk[j].tile, container+i+2, 0);

    j += 1;
    len += 4;

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

void G_IntToChar(unsigned int input, unsigned char *output, boolean full) {
  assert(input >= 0);

  if (full) {
    output[0] = (input & 0xFF000000) >> 24;
    output[1] = (input & 0x00FF0000) >> 16;
    output[2] = (input & 0x0000FF00) >> 8;
    output[3] = (input & 0x000000FF);
  } else {
    output[0] = (input & 0xFF00) >> 8;
    output[1] = (input & 0x00FF);
  }
}

void G_LoadChunksInner(long int *chunk_list, long int *scene_list, unsigned int length) {
  assert((chunk_list != NULL) && (scene_list != NULL));

  char filename[16];
  unsigned char c[4], *chunk = NULL;
  unsigned int i, j, x, y, file_id, file_id_prev, file_id_offset, chunk_id, chunk_length;
  FILE *file = NULL;
  G_Scene *scene;
  G_TreeNode *node;

  for (i = 0, j = 0; i < length; i += 1) {
    chunk_id = chunk_list[i];

    x = chunk_list[i]%WORLD_WIDTH;
    y = chunk_list[i]/WORLD_WIDTH;

    file_id = (x/FILE_CHUNK_SIZE)+(y/FILE_CHUNK_SIZE)*(WORLD_WIDTH/FILE_CHUNK_SIZE);
    file_id_offset = (x%FILE_CHUNK_SIZE)+(y%FILE_CHUNK_SIZE)*FILE_CHUNK_SIZE;
    
    if ((file == NULL) || (file_id != file_id_prev)) {
      sprintf(filename, "data/c.%3i.tfoy", file_id);
      filename[7] = (file_id > 99) ? filename[7] : '0';
      filename[8] = (file_id > 9) ? filename[8] : '0';
      file = fopen(filename, "rb");
      file_id_prev = file_id;
      j = 0;
    } else {
      j += 1;
    }
  
    if (file != NULL) {
      fread(c, sizeof(unsigned char), 4, file);
      chunk_length = G_CharToInt(c, 1);

      while ((j != file_id_offset) && (!feof(file)))  {
        fseek(file, chunk_length, SEEK_CUR);
        fread(c, sizeof(unsigned char), 4, file);
        chunk_length = G_CharToInt(c, 1);
        j += 1;
      }

      if ((j == file_id_offset) && (!feof(file))) {
        printf("Chunk %i loaded for scene %li.\n", chunk_id, scene_list[i]);

        node = G_TreeNodeFind(&scenes, scene_list[i]);

        if (node != NULL) {
          scene = (G_Scene*)(node->data);

          chunk = (unsigned char*)malloc(sizeof(unsigned char)*chunk_length);
          
          fread(chunk, sizeof(unsigned char), chunk_length, file);
          G_Tile *tiles = G_DecodeChunk(chunk, chunk_length);

          scene->chunks[chunk_id].tiles = tiles;
          scene->chunks[chunk_id].status = IS_LOADED;

          if (scene == active_scene) {
            if (((scene->view.x+scene->view.w/2)/CHUNK_SIZE == x) &&
                ((scene->view.y+scene->view.h/2)/CHUNK_SIZE == y)) {
              SDL_LockMutex(fmutex);
              redraw_lockable = 1;
              SDL_UnlockMutex(fmutex);
            }
          }

          free(chunk);
          node = NULL;
          chunk = NULL;
          scene = NULL;
        } else {
          fseek(file, chunk_length, SEEK_CUR);
        }
      } else {
        printf("Could not find chunk %u.\n", chunk_id);
        active_scene->chunks[chunk_id].tiles = NULL;
        active_scene->chunks[chunk_id].status = IS_MISSING;
      }
    } else {
      active_scene->chunks[chunk_id].tiles = NULL;
      active_scene->chunks[chunk_id].status = IS_MISSING;
      fclose(file);
    }
  }

  fclose(file);
}

G_Tile* G_DecodeChunk(unsigned char *chunk, unsigned int length) {
  unsigned int i, j, lim;
  G_Tile *decoded = NULL;

  decoded = (G_Tile*)malloc(sizeof(G_Tile)*CHUNK_SIZE*CHUNK_SIZE);

  for (i = 0, j = 0; i < length; i += 4) {
    lim = j+G_CharToInt(chunk+i, 0);

    if (lim == j) {
      lim = j+65536;
    }

    for (; j < lim; j += 1) {
      decoded[j].tile = G_CharToInt(chunk+i+2, 0);
    }
  }

  return decoded;
}
