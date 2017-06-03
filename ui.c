#include "yendor.h"
#include "globals.h"

G_UIWindow* G_UIWindowCreate(void) {
  G_UIWindow *window = (G_UIWindow*)malloc(sizeof(G_UIWindow));

  window->x = 0;
  window->y = 0;
  window->w = 0;
  window->h = 0;
  window->visible = 1;
  window->widget = NULL;

  return window;
}

G_UIWidget* G_UIWidgetCreate(void) {
  G_UIWidget *widget = (G_UIWidget*)malloc(sizeof(G_UIWidget));

  widget->x = 0;
  widget->y = 0;
  widget->w = 0;
  widget->h = 0;
  widget->length = 0;
  widget->hotkey = 0;
  widget->func = NULL;
  widget->data = NULL;
  widget->tiles = NULL;
  widget->flags = VISIBLE | ACTIVE;

  return widget;
}

void G_UpdateUIWindow(G_UIWindow **window) {
  G_UIWindow *w = *window;

  if (w->widget != NULL) {
    G_UpdateUIWidget(&(w->widget));
  }
}

void G_UpdateUIWidget(G_UIWidget **widget) {
  G_UIWidget *w = *widget;

  int mouse_x = (game_info.mouse_x-game_info.display_x)/game_info.tile_w,
      mouse_y = (game_info.mouse_y-game_info.display_y)/game_info.tile_h;

  if ((mouse_x >= w->x) && (mouse_x < w->x+w->w) &&
      (mouse_y >= w->y) && (mouse_y < w->y+w->h)) {
    w->flags = w->flags | HOVER;

    if (game_info.mouse_lb) {
      w->func(w->data);
    }

    if (w->focus < 255) {
      w->focus += 1;
      w->changed = 1;
    } else {
      w->changed = 0;
    }
  } else {
    w->flags = (w->flags | HOVER) ^ HOVER;

    if (w->focus > 0) {
      w->focus -= 1;
      w->changed = 1;
    } else {
      w->changed = 0;
    }
  }

  w->bg.r = w->focus;
  w->bg.g = w->focus;
  w->bg.b = w->focus;
}

void G_RenderUIWindow(G_UIWindow **window) {
  int x, y, x_lim, y_lim;
  G_UIWindow *w = *window;

  x = w->x;
  y = w->y;
  x_lim = x+w->w;
  y_lim = y+w->h;

  if (w->widget != NULL) {
    G_RenderUIWidget(&(w->widget));
  }

  for (; y < y_lim; y += 1) {
    for (x = w->x; x < x_lim; x += 1) {
      if ((x >= 0) && (x < COLS) && (y >= 0) && (y < ROWS)) {
        if (tilemap[x][y].layers[UI_LAYER] == NOTHING) {
          tilemap[x][y].fg = &ui_dark_a;
          tilemap[x][y].bg = &ui_dark_b;

          if ((x == w->x) || (x == w->x+w->w-1) ||
              (y == w->y) || (y == w->y+w->h-1)) {
            tilemap[x][y].layers[UI_LAYER] = GROUND;
          } else {
            tilemap[x][y].layers[UI_LAYER] = SOLID_COLOR;
          }
        }
      }
    }
  }
}

void G_RenderUIWidget(G_UIWidget **widget) {
  int x, y, x_lim, y_lim, len;
  boolean newline = 0;
  G_UIWidget *w = *widget;

  x = w->x;
  y = w->y;
  x_lim = x+w->w;
  y_lim = y+w->h;
  len = w->length;

  for (; y < y_lim; y += 1) {
    newline = 0;

    for (x = w->x; x < x_lim; x += 1) {
      if ((x >= 0) && (x < COLS) && (y >= 0) && (y < ROWS)) {
        tilemap[x][y].fg = &(w->fg);
        tilemap[x][y].bg = &(w->bg);
        tilemap[x][y].fchange = w->changed;

        if ((len > 0) && (newline == 0)) {
          if (w->tiles[w->length-len].tile != 0) {
            tilemap[x][y].layers[UI_LAYER] = w->tiles[w->length-len].tile;
          } else {
            tilemap[x][y].layers[UI_LAYER] = SOLID_COLOR;
            newline = 1;
          }

          len -= 1;
        } else {
          tilemap[x][y].layers[UI_LAYER] = SOLID_COLOR;
        }
      }
    }
  }
}

void G_UIWindowDestroy(G_UIWindow **window) {
  assert((window != NULL) && (*window != NULL));

  G_UIWindow *w = *window;

  if (w->widget != NULL) {
    G_UIWidgetDestroy(&(w->widget));
  }
}

void G_UIWidgetDestroy(G_UIWidget **widget) {
  free((*widget)->tiles);
  free((*widget));
}
