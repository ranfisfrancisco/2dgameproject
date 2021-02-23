#ifndef __PLAYER_H__
#define __PLAYER_H__

#include "entity.h"
#include "gfc_vector.h"
#include "gfc_types.h"

typedef struct {
	Entity* entity;

}PlayerEntity;

void player_spawn(Vector2D position);

Vector2D player_get_position();

int player_collison_check(SDL_Rect rect);

void player_change_health(int amount);

#endif