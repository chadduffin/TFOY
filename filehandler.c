#include "yendor.h"
#include "globals.h"

int G_LoadSettings(void *data) {
  int val, i;
  char buf[32];
  FILE *config = fopen("config.tfoy", "r");

  if (config != NULL) {
    i = 0;

    while (fscanf(config, "%s %i\n", buf, &val) == 2) {
      memcpy(game_info.settings[i].key, buf, 32);
      game_info.settings[i].value = val;
      i += 1;
    }
  }
  
  fclose(config);
  
  return 0;
}

int G_WriteSettings(void *data) {
  Setting setting = WORLD_WIDTH;
  FILE *config = fopen("config.tfoy", "w");

  while (setting != SETTING_COUNT) {
    fprintf(config, "%s %i\n", game_info.settings[setting].key, game_info.settings[setting].value);
    setting += 1;
  }

  return 0;
}

char* G_EncodeChunk(G_SceneChunk *chunk, int *size) {
  int i, j, num, len = 0;
  char container[CHUNK_SIZE*CHUNK_SIZE*3], *encoded = NULL;

  for (i = 0, j = 0; i < CHUNK_SIZE*CHUNK_SIZE*3; i += 3) {
    num = 1;

    while ((j < CHUNK_SIZE*CHUNK_SIZE) && (chunk->tiles[j].tile == chunk->tiles[j+1].tile)) {
      j += 1;
      num += 1;
    }

    container[i] = (char)(num);
    container[i+1] = (chunk->tiles[j].tile)/256;
    container[i+2] = (chunk->tiles[j].tile)%256;

    j += 1;
    len += 3

    if (j == CHUNK_SIZE*CHUNK_SIZE) {
      break;
    }
  }

  encoded = (char*)malloc(sizeof(char)*len);

  for (i = 0; i < len; i += 1) {
    encoded[i] = container[i];
  }

  *size = len;

  return encoded;
}

G_SceneChunk* G_DecodeChunk(char *buf, int len) {
  int i, j, lim;
  G_SceneChunk *decoded = NULL;

  decoded = (G_SceneChunk*)malloc(sizeof(G_SceneChunk));

  for (i = 0, j = 0; i < len; i += 3) {
    lim = j+(int)(buf[i]);

    for (; j < lim; j += 1) {
      decoded->tiles[j].tile = (*(buf+i+1) << 8)+*(buf+i+2);
    }
  }

  return decoded;
}
