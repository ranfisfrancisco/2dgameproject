#include "random.h"
#include <time.h>
#include <stdlib.h>

int RAND_INIT;

void rand_init() {
	time_t t;
	srand((unsigned)time(&t));

	RAND_INIT = 1;
}

int rand_int(int low, int high) {
	int i;

	if (!RAND_INIT)
		rand_init();

	i = rand() % (high - low) + low;
	return i;
}