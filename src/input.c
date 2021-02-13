#include "simple_logger.h"
#include "input.h"

static int* input_buffer = { 0 };
static int input_index = 0;
const static int buffer_len = 60;
static enum player_move* move_buffer = { 0 };

//init frame buffer
void input_buffer_init() {
	input_buffer = gfc_allocate_array(sizeof(int), buffer_len);
	if (input_buffer == NULL) {
		slog("Failed to allocate input buffer");
		return;
	}

	move_buffer = gfc_allocate_array(sizeof(int), NUM_OF_PLAYER_MOVES);
	if (move_buffer == NULL) {
		slog("Failed to allocate move buffer");
		return;
	}
}


//free frame buffer
void input_buffer_free() {
	if (input_buffer != NULL) {
		free(input_buffer);
	}
	memset(&input_buffer, 0, sizeof(input_buffer));
	slog("input buffer closed");

	if (move_buffer != NULL) {
		free(move_buffer);
	}
	memset(&move_buffer, 0, sizeof(move_buffer));
	slog("move buffer closed");
}

//returns player input offset frames ago
enum player_directional_input get_buffer_value(int offset) {
	int index;

	if (offset >= buffer_len || offset < 0) {
		return NO_INPUT;
	}

	index = input_index - offset;
	if (index < 0) {
		index += buffer_len;
	}
	else if (index >= buffer_len) {
		index -= buffer_len;
	}

	return input_buffer[index];
}

//reduces all move buffer by 1 down to 0
void reduce_move_buffer() {
	for (int i = 0; i < NUM_OF_PLAYER_MOVES; i++) {
		if (move_buffer[i] > 0)
			move_buffer[i]--;
	}
}

//set all move buffer values to 0
void clear_move_buffer() {
	for (int i = 0; i < NUM_OF_PLAYER_MOVES; i++) {
		move_buffer[i] = 0;
	}
}

enum player_move get_next_move_buffer_value() {
	int max = 0;
	int max_index = 0;

	for (int i = 0; i < NUM_OF_PLAYER_MOVES; i++) {
		if (move_buffer[i] > max) {
			max = move_buffer[i];
			max_index = i;
		}
	}

	return max_index;
}

// takes player directional input and returns a move if one was performed
enum player_move feed_input(enum player_directional_input input) {
	input_buffer[input_index] = input;
	input_index++;
	if (input_index >= buffer_len)
		input_index = 0;

	reduce_move_buffer();

	if (get_buffer_value(1) == FORWARD_INPUT) {
		int flag = 0;

		for (int i = 1; i < 11; i++) {
			if (flag == 0 && get_buffer_value(i) == DOWN_FORWARD_INPUT) {
				flag = 1;
			}
			else if (flag == 1 && get_buffer_value(i) == DOWN_INPUT) {
				move_buffer[QCF_MOVE] = 5;
				return QCF_MOVE;
			}
		}
	}

	if (get_buffer_value(1) == FORWARD_INPUT) {
		for (int i = 1; i < 11; i++) {
			if (get_buffer_value(i) == BACK_INPUT) {
				move_buffer[QCF_MOVE] = 7;
				return BACK_FORWARD_MOVE;
			}
		}
	}

	return get_next_move_buffer_value();
}