#include "simple_logger.h"
#include "input.h"

static int* input_buffer = { 0 };
static int input_index = 0;
const static int buffer_len = 60;
enum player_move finished_input_buffer = 0;

void input_buffer_init() {
	input_buffer = gfc_allocate_array(sizeof(int), buffer_len);
	if (input_buffer == NULL) {
		slog("Failed to allocate input buffer");
		return;
	}
}

void input_buffer_free() {
	if (input_buffer != NULL) {
		free(input_buffer);
	}
	memset(&input_buffer, 0, sizeof(input_buffer));
	slog("input buffer closed");
}

enum player_move feed_input(enum player_directional_input input) {
	input_buffer[input_index] = input;
	input_index++;
	if (input_index >= buffer_len)
		input_index = 0;

	if (input == FORWARD_INPUT)
		return FORWARD_MOVE;

	return NO_MOVE;
}