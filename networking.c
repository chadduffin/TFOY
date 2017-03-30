#include "yendor.h"
#include "globals.h"

int G_NetworkingInit(void *data) {
  SDLNet_Init();

  return 0;
}

int G_Networking(void *data) {
  return 0;
}
