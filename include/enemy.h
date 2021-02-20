#ifndef __ENEMY_H__
#define __ENEMY_H__

#include "entity.h"
#include "gfc_vector.h"

enum enemy_state { ENEMY_IDLE, ENEMY_HURT, ENEMY_WALK, ENEMY_ATTACK };

Entity* enemy_spawn(Vector2D position);

#endif