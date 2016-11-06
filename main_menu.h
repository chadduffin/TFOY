#ifndef __MAIN_MENU__
#define __MAIN_MENU__

#include <SDL2/SDL.h>
#include "globals.h"

/*
** enums
*/

enum mainMenuWidgets {
	USERNAME_LABEL = 0,
	USERNAME_FIELD,

	PASSWORD_LABEL,
	PASSWORD_FIELD,
	
	PLAY_BUTTON,
	EXIT_BUTTON,

	M_WIDGET_COUNT
};

/*
** externs
*/

extern int title_width;
extern int title_height;
extern gameWidget mmWidgets[M_WIDGET_COUNT];

/*
** functions
*/

void initMainMenu();
void drawMainMenu();
void drawMainMenuUI();

/*
*/

#endif /* __MAIN_MENU__ */
