#include "yendor.h"
#include "globals.h"

G_UIWindow* G_UIWindowCreate(int x, int y, int w, int h, int depth, boolean visible) {
  G_UIWindow *window = (G_UIWindow*)malloc(sizeof(G_UIWindow));

  window->x = x;
  window->y = y;
  window->w = w;
  window->h = h;
  window->depth = depth;
  window->visible = visible;
  window->fg = white;
  window->bg = black;
  window->widgets = G_TreeCreate();
  window->windows = G_TreeCreate();

  return window;
}

G_UIWidget* G_UIWidgetCreate(int x, int y, int w, int h, int depth, int length, void(*func)(void*), void *data, char *text) {
  int i;
  G_UIWidget *widget = (G_UIWidget*)malloc(sizeof(G_UIWidget));

  widget->x = x;
  widget->y = y;
  widget->w = w;
  widget->h = h;
  widget->focus = 0;
  widget->depth = depth;
  widget->length = length;
  widget->func = func;
  widget->data = data;
  widget->hotkey = 0;
  widget->fg = white;
  widget->bg = black;
  widget->tiles = (G_UITile*)malloc(sizeof(G_UITile)*(length));

  for (i = 0; i < length; i += 1) {
    widget->tiles[i].fg = white;
    widget->tiles[i].tile = text[i];
  }

  widget->changed = 1;
  widget->flags = VISIBLE | ACTIVE;

  return widget;
}

void G_UIWindowAddWidget(G_UIWindow **window, G_UIWidget **widget) {
  assert((window != NULL) && (*window != NULL) && (widget != NULL) && (*widget != NULL));

  G_UIWidget *wid = *widget;
  G_UIWindow *win = *window;
  G_TreeNode *node = (G_TreeNode*)malloc(sizeof(G_TreeNode));

  node->key = wid->depth;
  node->data = wid;

  G_TreeNodeInsert(&(win->widgets), &node);
}

void G_UIWindowAddWindow(G_UIWindow **window, G_UIWindow **subwindow) {
  assert((window != NULL) && (*window != NULL) && (subwindow != NULL) && (*subwindow != NULL));

  G_UIWindow *win = *window, *sub = *subwindow;
  G_TreeNode *node = (G_TreeNode*)malloc(sizeof(G_TreeNode));

  node->key = sub->depth;
  node->data = sub;

  G_TreeNodeInsert(&(win->windows), &node);
}

void G_UIWidgetHotkey(G_UIWidget **widget, SDL_Scancode hotkey, int index) {
  assert((widget != NULL) && (*widget != NULL));

  G_UIWidget *w = *widget;

  if ((index != -1) && (index < w->length)) {
    w->tiles[index].fg = yellow;
  }

  w->hotkey = hotkey;
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
        focus = 64;
      }
    } else {
      w->flags = (w->flags | HOVER | MOUSE_PRESSED) ^ (HOVER | MOUSE_PRESSED);
    }

    if (game_info.phys[w->hotkey] != 0) {
      w->flags = w->flags | KEY_PRESSED;
      focus = 64;
    } else if (((w->flags & HOVER) == HOVER) || (game_info.phys[w->hotkey] > 0)) {
      focus = (w->focus < 64) ? w->focus+4 : 64;
    } else {
      focus = (w->focus > 0) ? w->focus-2 : 0;
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
      w->bg.r += focus-w->focus;
      w->bg.g += focus-w->focus;
      w->bg.b += focus-w->focus;

      w->focus = focus;
      w->changed = 1;
    }
  } else {
    w->bg.r -= w->focus;
    w->bg.g -= w->focus;
    w->bg.b -= w->focus;
    w->focus = 0;
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
            tilemap[x][y].fg = &(w->fg);
            tilemap[x][y].bg = &(w->bg);

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
          tilemap[x][y].bg = (active) ? &(w->bg) : &grey;
          tilemap[x][y].fchange = w->changed;

          if ((len > 0) && (newline == 0)) {
            tilemap[x][y].fg = (active) ? &(w->tiles[w->length-len].fg) : &dgrey;

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

  G_TreeDestroy(&(w->windows));
  G_TreeDestroy(&(w->widgets));
}

void G_UIWidgetDestroy(G_UIWidget **widget) {
  free((*widget)->tiles);
  free((*widget));
}

void G_UIWidgetQuitGame(void *empty) {
  game_info.running = 0;
}
