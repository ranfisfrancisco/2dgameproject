#ifndef __DIRECTOR_H__
#define __DIRECTOR_H__

#include "level.h"

typedef struct {
	Level* currentLevel;
	int score;
	int highScore;
} GameVarsStruct;

void director_add_score(int amount);

int director_get_score();

void director_snap_camera();

void director_init_game();

int director_run_game();

void director_end_game();

#endif