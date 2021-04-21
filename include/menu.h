#ifndef __MENU_H__
#define __MENU_H_

//typedef enum {
//	MENU_STATE_MAIN, MENU_STATE_EXIT
//} MenuState;

#define MENU_MAX_ITEMS 5

typedef struct {
	char** texts[MENU_MAX_ITEMS];
	int numOfOptions;
	int highlightIndex;
} Menu;

/**
 * @brief Initialize the Menu. Must be run before using any other Menu elements
 */
void menu_init();

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