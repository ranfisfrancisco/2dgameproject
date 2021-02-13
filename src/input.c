#include "simple_logger.h"
#include "input.h"

static int* input_buffer = { 0 };
static int input_index = 0;
const static int buffer_len = 60;
enum player_move finished_input_buffer = 0;

//init frame buffer
void input_buffer_init() {
	input_buffer = gfc_allocate_array(sizeof(int), buffer_len);
	if (input_buffer == NULL) {
		slog("Failed to allocate input buffer");
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

// takes player directional input and returns a move if one was performed
enum player_move feed_input(enum player_directional_input input) {
	input_buffer[input_index] = input;
	input_index++;
	if (input_index >= buffer_len)
		input_index = 0;

	//check for back forward input
	if (get_buffer_value(1) == FORWARD_INPUT) {
		for (int i = 1; i < 11; i++) {
			if (get_buffer_value(i) == BACK_INPUT) {
				return BACK_FORWARD_MOVE;
			}
		}
	}

	return NO_MOVE;
}