#ifndef __HUD_H__
#define __HUD_H_

/**
 * @brief Initialize the HUD. Must be run before using any other HUD elements
 */
void hud_init();

/**
 * @brief Draw the HUD.
 * @param comboCounter NULL if not applicable, otherwise a pointer to an integer for the combo counter
 */
void hud_draw(int* comboCounter);

/**
 * @brief Draw the Level Transition.
 */
void hud_draw_level_transition();

/**
 * @brief Draw the beat game screen.
 */
void hud_draw_beat_game();

#endif