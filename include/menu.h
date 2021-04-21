#ifndef __MENU_H__
#define __MENU_H_

#include "font.h"

typedef enum {
	MENU_ACTION_CONTINUE, MENU_ACTION_CLOSE, MENU_ACTION_QUIT
} Menu_Action;

typedef struct Menu_s{
	char** texts;
	int numOfOptions;
	int highlightIndex;
	Font* font;
	struct Menu_s* backMenu;
} Menu;

/**
 * @brief Initialize the Menu. Must be run before using any other Menu elements
 */
void menu_init();

/**
 * @brief resets menu variables for when the menu is opened
 */
void menu_open();

/**
 * @brief Update the HUD.
 * @return GAME_STATE to transition to
 */
int menu_update();

/**
 * @brief Draw the Menu.
 */
void menu_draw();

#endif