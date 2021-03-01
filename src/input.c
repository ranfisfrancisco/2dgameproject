#include "simple_logger.h"
#include "input.h"

typedef struct {
	int* input_buffer;
	int input_len;
	int input_index;
	int move_buffer_hold_window;
	enum player_move* move_buffer;
} InputManager;

static InputManager input_manager = { 0 };

//init frame buffer
void input_buffer_init() {
	input_manager.input_len = 60;
	input_manager.move_buffer_hold_window = 6;

	input_manager.input_buffer = gfc_allocate_array(sizeof(int), input_manager.input_len);
	if (input_manager.input_buffer == NULL) {
		slog("Failed to allocate input buffer");
		return;
	}

	input_manager.move_buffer = gfc_allocate_array(sizeof(int), NUM_OF_PLAYER_MOVES);
	if (input_manager.move_buffer == NULL) {
		slog("Failed to allocate move buffer");
		return;
	}
}

//free frame buffer
void input_buffer_free() {
	if (input_manager.input_buffer != NULL) {
		free(input_manager.input_buffer);
	}
	memset(&input_manager.input_buffer, 0, sizeof(input_manager.input_buffer));
	slog("input buffer closed");

	if (input_manager.move_buffer != NULL) {
		free(input_manager.move_buffer);
	}
	memset(&input_manager.move_buffer, 0, sizeof(input_manager.move_buffer));
	slog("move buffer closed");
}

//returns player input offset frames ago
enum player_directional_input get_buffer_value(int offset) {
	int index;

	if (offset >= input_manager.input_len || offset < 0) {
		return NO_INPUT;
	}

	index = input_manager.input_index - offset;
	if (index < 0) {
		index += input_manager.input_len;
	}
	else if (index >= input_manager.input_len) {
		index -= input_manager.input_len;
	}

	return input_manager.input_buffer[index];
}

//reduces all move buffer by 1 down to 0
void reduce_move_buffer() {
	for (int i = 0; i < NUM_OF_PLAYER_MOVES; i++) {
		if (input_manager.move_buffer[i] > 0)
			input_manager.move_buffer[i]--;
	}
}

//set all move buffer values to 0
void clear_move_buffer() {
	for (int i = 0; i < NUM_OF_PLAYER_MOVES; i++) {
		input_manager.move_buffer[i] = 0;
	}
}

enum player_move get_next_move_buffer_value() {
	int max = 0;
	int max_index = 0;

	for (int i = 0; i < NUM_OF_PLAYER_MOVES; i++) {
		if (input_manager.move_buffer[i] > max) {
			max = input_manager.move_buffer[i];
			max_index = i;
		}
	}

	return max_index;
}

// takes player directional input and returns a move if one was performed
enum player_move feed_input(enum player_directional_input input) {
	input_manager.input_buffer[input_manager.input_index] = input;
	input_manager.input_index++;
	if (input_manager.input_index >= input_manager.input_len)
		input_manager.input_index = 0;

	reduce_move_buffer();

	//detect quarter circle
	if (get_buffer_value(1) == FORWARD_INPUT) {
		int flag = 0;

		for (int i = 1; i < 11; i++) {
			if (flag == 0 && get_buffer_value(i) == DOWN_FORWARD_INPUT) {
				flag = 1;
			}
			else if (flag == 1 && get_buffer_value(i) == DOWN_INPUT) {
				input_manager.move_buffer[QC_MOVE] = input_manager.move_buffer_hold_window;
				return QC_MOVE;
			}
		}
	}

	if (get_buffer_value(1) == BACK_INPUT) {
		int flag = 0;

		for (int i = 1; i < 11; i++) {
			if (flag == 0 && get_buffer_value(i) == DOWN_BACK_INPUT) {
				flag = 1;
			}
			else if (flag == 1 && get_buffer_value(i) == DOWN_INPUT) {
				input_manager.move_buffer[QC_MOVE] = input_manager.move_buffer_hold_window;
				return QC_MOVE;
			}
		}
	}

	//detect back forward
	if (get_buffer_value(1) == FORWARD_INPUT) {
		for (int i = 1; i < 11; i++) {
			if (get_buffer_value(i) == BACK_INPUT) {
				input_manager.move_buffer[BACK_FORWARD_MOVE] = 7;
				return BACK_FORWARD_MOVE;
			}
		}
	}

	if (get_buffer_value(1) == BACK_INPUT) {
		for (int i = 1; i < 11; i++) {
			if (get_buffer_value(i) == FORWARD_INPUT) {
				input_manager.move_buffer[BACK_FORWARD_MOVE] = 7;
				return BACK_FORWARD_MOVE;
			}
		}
	}

	return get_next_move_buffer_value();
}