#include "random.h"
#include <time.h>

static int rand_int(int low, int high) {
	time_t t;

	/* Intializes random number generator */
	srand((unsigned)time(&t));

	return rand() % (high-low) + low;
}