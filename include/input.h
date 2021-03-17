#ifndef __INPUT_H__
#define __INPUT_H__

#include "gfc_types.h"
#include "gfc_vector.h"
#include "gf2d_sprite.h"
#define NUM_OF_PLAYER_MOVES 3

enum player_move {NO_MOVE, BACK_FORWARD_MOVE, QC_MOVE};
enum player_directional_input { NO_INPUT, DOWN_BACK_INPUT, DOWN_INPUT, DOWN_FORWARD_INPUT, BACK_INPUT, NEUTRAL_INPUT, FORWARD_INPUT, UP_BACK_INPUT, UP_INPUT, UP_FORWARD_INPUT };

/**
 * @brief Initialize the input buffer. Must be run before using any other input buffer methods
 */
void input_buffer_init();

/**
 * @brief Free the input buffer
 */
void input_buffer_free();

/**
 * @brief Feed the next directional input to the input buffer
 * @param input The directional input the user inputted
 * @return The player move the player performed. Returns NO_MOVE if they have not inputted a move.
 */
enum player_move feed_input(enum player_directional_input input);

#endif