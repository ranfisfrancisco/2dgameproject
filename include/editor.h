#ifndef __EDITOR_H__
#define __EDITOR_H__

#include "gfc_vector.h"
#include "entity.h"
#include "fight.h"

typedef struct {
	Vector2D position;
	Entity* shadow;
	EntityType currentType;
	LevelFightData* fightData;
	clock_t clock_start;
	double time_elapsed;
} Editor;

void editor_init();

void editor_update();

#endif