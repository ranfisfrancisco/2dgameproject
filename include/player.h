#ifndef __PLAYER_H__
#define __PLAYER_H__

#include "entity.h"
#include "gfc_vector.h"
#include "gfc_types.h"

typedef struct {
	Entity* weapon;
	int powerUpTime;
}PlayerData;


void player_spawn(Vector2D position);

Vector2D player_get_position();

int player_get_health();

int player_get_max_health();

void player_attatch_weapon(Entity* ent);

Vector2D player_get_weapon_position();

void player_power_up(int frameTime);

int player_collison_check(SDL_Rect rect);

void player_change_health(int amount);

#endif