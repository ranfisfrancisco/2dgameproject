#ifndef __INPUT_H__
#define __INPUT_H__

#include "gfc_types.h"
#include "gfc_vector.h"
#include "gf2d_sprite.h"
#define NUM_OF_PLAYER_MOVES 3

enum player_move {NO_MOVE, BACK_FORWARD_MOVE, QC_MOVE};
enum player_directional_input { NO_INPUT, DOWN_BACK_INPUT, DOWN_INPUT, DOWN_FORWARD_INPUT, BACK_INPUT, NEUTRAL_INPUT, FORWARD_INPUT, UP_BACK_INPUT, UP_INPUT, UP_FORWARD_INPUT };

void input_buffer_init();
void input_buffer_free();
enum player_move feed_input(enum player_directional_input input);

#endif