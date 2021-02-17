#ifndef __PLAYER_H__
#define __PLAYER_H__

#include "entity.h"
#include "gfc_vector.h"

typedef struct {
	Entity* entity;

}PlayerEntity;

void player_spawn(Vector2D position);

#endif