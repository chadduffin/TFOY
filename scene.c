#include "yendor.h"
#include "globals.h"

/*
** EXTERNS
*/

G_Scene *menu = NULL;
G_Scene *overworld = NULL;
G_Scene *location = NULL;

/*
** FUNCTIONS
*/

void G_InitializeMenu(void) {
	int i;
	menu = (G_Scene*)malloc(sizeof(G_Scene));
	menu->w = COLS;
	menu->h = ROWS;
	menu->l = COLS*ROWS;
	menu->entity_count = 0;
	menu->head = NULL;
	menu->tail = NULL;
	menu->focus = NULL;
	menu->view.x = 0;
	menu->view.y = 0;
	menu->view.xp = 0;
	menu->view.yp = 0;
	menu->view.w = COLS;
	menu->view.h = ROWS;
	menu->view.unchanged = 0;
	menu->ambient.red = 255;
	menu->ambient.green = 255;
	menu->ambient.blue = 255;
	menu->ambient.intensity = 255;

	menu->tiles = (G_Tile*)malloc(sizeof(G_Tile)*menu->l);

	for (i = 0; i < menu->l; i += 1) {
		if (title[i/COLS][i%COLS] == 'B') {
			menu->tiles[i].tile = BLACK;
		} else if ((title[i/COLS][i%COLS] != 'B') && (title[i/COLS][i%COLS] != ' ')) {
			menu->tiles[i].tile = title[i/COLS][i%COLS];
		} else if ((i > COLS) && (title[i/COLS][(i-1)%COLS] == 'B')) {
			menu->tiles[i].tile = WHITE;
		} else {
			menu->tiles[i].tile = MAGENTA;
		}
	}
}

void G_InitializeOverworld(void) {
	int i;
	overworld = (G_Scene*)malloc(sizeof(G_Scene));
	overworld->w = WORLD_COLS;
	overworld->h = WORLD_ROWS;
	overworld->l = WORLD_COLS*WORLD_ROWS;
	overworld->entity_count = 0;
	overworld->head = NULL;
	overworld->tail = NULL;
	overworld->focus = NULL;
	overworld->view.x = 0;
	overworld->view.y = 0;
	overworld->view.xp = 0;
	overworld->view.yp = 0;
	overworld->view.w = DCOLS;
	overworld->view.h = DROWS;
	overworld->view.unchanged = 0;
	overworld->ambient.red = 0;
	overworld->ambient.green = 0;
	overworld->ambient.blue = 0;
	overworld->ambient.intensity = 255;

	overworld->tiles = (G_Tile*)malloc(sizeof(G_Tile)*overworld->l);

	for (i = 0; i < overworld->l; i += 1) {
		if ((i%WORLD_COLS == 0) || ((i+1)%WORLD_COLS == 0) ||
				(i < WORLD_COLS) || (i > overworld->l-WORLD_COLS)) {
			overworld->tiles[i].tile = WALL;
		} else {
			overworld->tiles[i].tile = (rand()%256 < 1) ? WALL : DIRT;
			if ((i%WORLD_COLS == WORLD_COLS-64) || (i%WORLD_COLS == WORLD_COLS-12) || (i/WORLD_COLS == 200)) {
				overworld->tiles[i].tile = WALL;
			}

			int
				x = i%WORLD_COLS,	
				y = i/WORLD_COLS;
			if ((x+rand()%4 > WORLD_COLS-24) && (y+rand()%4 > WORLD_ROWS-24)) {
				overworld->tiles[i].tile = WATER;
			}
		}
	}

	G_Entity *player = G_CreateEntity();
	G_RenderComponent *r = (G_RenderComponent*)G_AddComponent(&player, RENDER_COMPONENT);
	r->tile = HUMAN;
	r->x = 460;
	r->y = 230;
	r->x_previous = r->x;
	r->y_previous = r->y;
	G_LightComponent *l = (G_LightComponent*)G_AddComponent(&player, LIGHT_COMPONENT);
	l->light.red = 0;
	l->light.green = 255;
	l->light.blue = 0;
	l->light.intensity = 16;
	G_AddComponent(&player, CONTROLLER_COMPONENT);
	G_AddEntity(&overworld, &player);
	G_Entity *t = G_CreateEntity();
	r = (G_RenderComponent*)G_AddComponent(&t, RENDER_COMPONENT);
	r->tile = HUMAN;
	r->x = 480;
	r->y = 210;
	r->x_previous = r->x;
	r->y_previous = r->y;
	l = (G_LightComponent*)G_AddComponent(&t, LIGHT_COMPONENT);
	l->light.red = 255;
	l->light.green = 0;
	l->light.blue = 0;
	l->light.intensity = 32;
	G_AddEntity(&overworld, &t);
	G_Entity *x = G_CreateEntity();
	r = (G_RenderComponent*)G_AddComponent(&x, RENDER_COMPONENT);
	r->tile = HUMAN;
	r->x = 480;
	r->y = 240;
	r->x_previous = r->x;
	r->y_previous = r->y;
	l = (G_LightComponent*)G_AddComponent(&x, LIGHT_COMPONENT);
	l->light.red = 0;
	l->light.green = 0;
	l->light.blue = 255;
	l->light.intensity = 32;
	G_AddEntity(&overworld, &x);
}

void G_ChangeScene(G_Scene **scene) {
	int
		x,
		y;
	location = *scene;
	location->view.unchanged = 0;

	if (location == menu) {
		for (y = 0; y < ROWS; y += 1) {
			for (x = 0; x < COLS; x += 1) {
				dmatrix[x][y].changed = 1;
				dmatrix[x][y].visible = 1;
				dmatrix[x][y].entity = NOTHING;
				dmatrix[x][y].tile = G_SceneTile(x, y);

				dmatrix[x][y].light.id = -1;
				dmatrix[x][y].light.count = 0;
				dmatrix[x][y].light.light.red = 0;
				dmatrix[x][y].light.light.green = 0;
				dmatrix[x][y].light.light.blue = 0;
				dmatrix[x][y].light.light.intensity = 0;
			}
		}
	} else {
		int
			x_offset = location->view.x-DCOLS_OFFSET,
			y_offset = location->view.y-DROWS_OFFSET;

		for (y = 0; y < ROWS; y += 1) {
			for (x = 0; x < COLS; x += 1) {
				dmatrix[x][y].changed = 1;
				dmatrix[x][y].entity = NOTHING;

				if ((x >= DCOLS_OFFSET) && (x < DCOLS+DCOLS_OFFSET) &&
						(y >= DROWS_OFFSET) && (y < DROWS+DROWS_OFFSET)) {
					dmatrix[x][y].visible = 0;
					dmatrix[x][y].tile = G_SceneTile(x+x_offset, y+y_offset);
				} else {
					dmatrix[x][y].visible = 1;
					dmatrix[x][y].tile = BLACK;
				}

				dmatrix[x][y].light.id = -1;
				dmatrix[x][y].light.count = 0;
				dmatrix[x][y].light.light.red = 0;
				dmatrix[x][y].light.light.green = 0;
				dmatrix[x][y].light.light.blue = 0;
				dmatrix[x][y].light.light.intensity = 0;
			}
		}
	}
}

void G_CleanupScene(G_Scene **scene) {
	assert((scene != NULL) && (*scene != NULL));

	G_Entity *entity = (*scene)->head;
	
	while (entity != NULL) {
		(*scene)->head = (G_Entity*)(entity->next);
		free(entity);
		entity = (*scene)->head;
	}

	free((*scene)->tiles);
}

void G_AddEntity(G_Scene **scene, G_Entity **entity) {
	assert((scene != NULL) && (*scene != NULL) && (entity != NULL) && (*entity != NULL));

	if ((*scene)->head == NULL) {
		(*scene)->head = (*entity);
		(*scene)->tail = (*entity);
	} else {
		(*entity)->prev = (void*)((*scene)->tail);
		(*scene)->tail->next = (void*)(*entity);
		(*scene)->tail = (*entity);
	}
	
	if ((*scene)->focus == NULL) {
		(*scene)->focus = *entity;
	}
	
	(*scene)->entity_count += 1;
}

void G_DelEntity(G_Scene **scene, G_Entity **entity) {
	assert((scene != NULL) && (*scene != NULL) && (entity != NULL) && (*entity != NULL));

	if ((*scene != NULL) && (*entity != NULL)) {
		G_PopEntity(scene, entity);
		free(*entity);
	}
}

void G_PopEntity(G_Scene **scene, G_Entity **entity) {
	assert((scene != NULL) && (*scene != NULL) && (entity != NULL) && (*entity != NULL));

	if ((*entity != (*scene)->head) && (*entity != (*scene)->tail)) {
		if ((*entity)->next != NULL) {
			((G_Entity*)((*entity)->next))->prev = (*entity)->prev;
		}
		if ((*entity)->prev != NULL) {
			((G_Entity*)((*entity)->prev))->next = (*entity)->next;
		}
	} else {
		(*scene)->head = ((*entity) == (*scene)->head) ? (G_Entity*)((*scene)->head->next) : (*scene)->head;
		(*scene)->tail = ((*entity) == (*scene)->tail) ? (G_Entity*)((*scene)->tail->prev) : (*scene)->tail;

		if ((*entity)->next != NULL) {
			((G_Entity*)((*entity)->next))->prev = NULL;
		}
		if ((*entity)->prev != NULL) {
			((G_Entity*)((*entity)->prev))->next = NULL;
		}
	}

	(*scene)->entity_count -= 1;
}

G_View* G_SceneView(G_Scene **scene) {
	assert((scene != NULL) && (*scene != NULL));

	return &((*scene)->view);
}

G_Entity* G_GetEntities(G_Scene **scene) {
	assert((scene != NULL) && (*scene != NULL));

	return (*scene)->head;
}

Tile G_SceneTile(int x, int y) {
	if ((x < 0) || (x >= location->w) ||
			(y < 0) || (y >= location->h) ||
			(location == NULL)) {
		return WALL;
	}

	return (location->tiles[x+(y*location->w)].tile);
}

/*
*/
