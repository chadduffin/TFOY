#include "yendor.h"
#include "globals.h"

G_UIWindow* G_UIWindowCreate(void) {
  G_UIWindow *window = (G_UIWindow*)malloc(sizeof(G_UIWindow));

  window->x = 0;
  window->y = 0;
  window->w = 0;
  window->h = 0;
  window->depth = 0;
  window->visible = 1;
  window->widget = NULL;
  window->widgets = G_TreeCreate();
  window->windows = G_TreeCreate();

  return window;
}

G_UIWidget* G_UIWidgetCreate(void) {
  G_UIWidget *widget = (G_UIWidget*)malloc(sizeof(G_UIWidget));

  widget->x = 0;
  widget->y = 0;
  widget->w = 0;
  widget->h = 0;
  widget->depth = 0;
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

  if (w->visible) {
    G_TreeNodeIterate(&(w->windows), &(w->windows->root->left), (void(*)(void*))(&G_UpdateUIWindow));
    G_TreeNodeIterate(&(w->widgets), &(w->widgets->root->left), (void(*)(void*))(&G_UpdateUIWidget));
  }
}

void G_UpdateUIWidget(G_UIWidget **widget) {
  int focus = 0;
  G_UIWidget *w = *widget;

  if ((w->flags & ACTIVE) == ACTIVE) {
    int mouse_x = (game_info.mouse_x-game_info.display_x)/game_info.tile_w,
        mouse_y = (game_info.mouse_y-game_info.display_y)/game_info.tile_h;

    if ((mouse_x >= w->x) && (mouse_x < w->x+w->w) &&
        (mouse_y >= w->y) && (mouse_y < w->y+w->h)) {
      w->flags = w->flags | HOVER;

      if (game_info.mouse_lb != 0) {
        w->flags = w->flags | MOUSE_PRESSED;
        focus = 32;
      }
    } else {
      w->flags = (w->flags | HOVER | MOUSE_PRESSED) ^ (HOVER | MOUSE_PRESSED);
    }

    if (game_info.phys[w->hotkey] != 0) {
      w->flags = w->flags | KEY_PRESSED;
      focus = 32;
    } else if (((w->flags & HOVER) == HOVER) || (game_info.phys[w->hotkey] > 0)) {
      focus = (w->focus < 32) ? w->focus+1 : 32;
    } else {
      focus = (w->focus > 0) ? w->focus-1 : 0;
    }

    if ((w->flags & PRESSED) > 0) {
      if ((game_info.mouse_lb == 0) &&
          ((w->flags & MOUSE_PRESSED) == MOUSE_PRESSED)) {
        w->func(w->data);
        w->flags = w->flags ^ MOUSE_PRESSED;
      } else if ((game_info.phys[w->hotkey] == 0) &&
                 ((w->flags & KEY_PRESSED) == KEY_PRESSED)) {
        w->func(w->data);
        w->flags = w->flags ^ KEY_PRESSED;
      }
    }

    if (focus != w->focus) {
      w->focus = focus;
      w->changed = 1;

      w->bg.r = ui_bg_active.r+focus;
      w->bg.g = ui_bg_active.g+focus;
      w->bg.b = ui_bg_active.b+focus;
    }
  } else {
    w->focus = 0;
    w->bg = ui_bg_inactive;
  }
}

void G_RenderUIWindow(G_UIWindow **window) {
  int x, y, x_lim, y_lim;
  G_UIWindow *w = *window;

  if (w->visible) {
    x = w->x;
    y = w->y;
    x_lim = x+w->w;
    y_lim = y+w->h;

    G_TreeNodeIterate(&(w->windows), &(w->windows->root->left), (void(*)(void*))(&G_RenderUIWindow));
    G_TreeNodeIterate(&(w->widgets), &(w->widgets->root->left), (void(*)(void*))(&G_RenderUIWidget));

    for (; y < y_lim; y += 1) {
      for (x = w->x; x < x_lim; x += 1) {
        if ((x >= 0) && (x < COLS) && (y >= 0) && (y < ROWS)) {
          if (tilemap[x][y].layers[UI_LAYER] == NOTHING) {
            tilemap[x][y].fg = &ui_bg_active;
            tilemap[x][y].bg = &ui_bg_active;

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
}

void G_RenderUIWidget(G_UIWidget **widget) {
  int x, y, x_lim, y_lim, len;
  boolean newline = 0, active;
  G_UIWidget *w = *widget;

  active = ((w->flags & ACTIVE) == ACTIVE);

  if ((w->flags & VISIBLE) == VISIBLE) {
    x = w->x;
    y = w->y;
    x_lim = x+w->w;
    y_lim = y+w->h;
    len = w->length;

    for (; y < y_lim; y += 1) {
      newline = 0;

      for (x = w->x; x < x_lim; x += 1) {
        if ((x >= 0) && (x < COLS) && (y >= 0) && (y < ROWS)) {
          tilemap[x][y].bg = (active) ? &(w->bg) : &ui_bg_inactive;
          tilemap[x][y].fchange = w->changed;

          if ((len > 0) && (newline == 0)) {
            tilemap[x][y].fg = (active) ? &(w->tiles[w->length-len].fg) : &ui_fg_inactive;

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
}

void G_UIWindowToggleVisible(G_UIWindow **window) {
  G_UIWindow *w = *window;

  w->visible = !(w->visible);
}

void G_UIWidgetToggleVisible(G_UIWidget **widget) {
  G_UIWidget *w = *widget;

  w->flags = w->flags ^ VISIBLE;
}

void G_UIWidgetToggleActive(G_UIWidget **widget) {
  G_UIWidget *w = *widget;

  w->flags = w->flags ^ ACTIVE;
}

void G_UIWindowDestroy(G_UIWindow **window) {
  assert((window != NULL) && (*window != NULL));

  G_UIWindow *w = *window;

  if (w->widget != NULL) {
    G_UIWidgetDestroy(&(w->widget));
  }

  G_TreeDestroy(&(w->windows));
  G_TreeDestroy(&(w->widgets));
}

void G_UIWidgetDestroy(G_UIWidget **widget) {
  free((*widget)->tiles);
  free((*widget));
}
