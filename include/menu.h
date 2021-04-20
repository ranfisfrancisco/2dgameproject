#ifndef __MENU_H__
#define __MENU_H_

/**
 * @brief Initialize the Menu. Must be run before using any other Menu elements
 */
void menu_init();

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