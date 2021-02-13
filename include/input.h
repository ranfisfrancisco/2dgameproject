#ifndef __INPUT_H__
#define __INPUT_H__

#include "gfc_types.h"
#include "gfc_vector.h"
#include "gf2d_sprite.h"

enum player_move {NO_MOVE, FORWARD_MOVE};
enum player_directional_input { INIT_INPUT, DOWN_BACK_INPUT, DOWN_INPUT, DOWN_FORWARD_INPUT, BACK_INPUT, NEUTRAL_INPUT, FORWARD_INPUT, UP_BACK_INPUT, UP_INPUT, UP_FORWARD_INPUT };

void input_buffer_init();
void input_buffer_free();

#endif