#include "yendor.h"
#include "globals.h"

/*
** EXTERNS
*/

G_Scene *menu;
G_Scene *overworld;
G_Scene *location;

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
			overworld->tiles[i].tile = (rand()%64 < 1) ? WALL : DIRT;

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
	l->light.red = 255;
	l->light.green = 255;
	l->light.blue = 255;
	l->light.intensity = 32;
	G_AddEntity(&overworld, &player);
	G_Entity *t = G_CreateEntity();
	r = (G_RenderComponent*)G_AddComponent(&t, RENDER_COMPONENT);
	r->tile = HUMAN;
	r->x = 480;
	r->y = 230;
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
	l->light.green = 255;
	l->light.blue = 0;
	l->light.intensity = 32;
	G_AddEntity(&overworld, &x);
}

void G_CleanupScene(G_Scene **scene) {
	G_Entity *entity = (*scene)->head;
	
	while (entity != NULL) {
		(*scene)->head = (G_Entity*)(entity->next);
		free(entity);
		entity = (*scene)->head;
	}

	free((*scene)->tiles);
}

void G_AddEntity(G_Scene **scene, G_Entity **entity) {
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
	if ((*scene != NULL) && (*entity != NULL)) {
		G_PopEntity(scene, entity);
		free(*entity);
	}
}

void G_PopEntity(G_Scene **scene, G_Entity **entity) {
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
}

G_View* G_SceneView(G_Scene **scene) {
	if ((scene != NULL) && (*scene != NULL)) {
		return &((*scene)->view);
	}

	return NULL;
}

G_Entity* G_GetEntities(G_Scene **scene) {
	if ((scene != NULL) && (*scene != NULL)) {
		return (*scene)->head;
	}

	return NULL;
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
