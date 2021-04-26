#ifndef __ENEMY_H__
#define __ENEMY_H__

#include "entity.h"
#include "gfc_vector.h"
#include "gfc_types.h"

enum enemy_state { ENEMY_SEEK, ENEMY_HURT, ENEMY_ATTACK };

typedef struct {
	Sound* attackSound;
	Sound* hurtSound;
	int waitTime;
}EnemyData;

/**
 * @brief Spawns an entity of a given type
 * @return Pointer to new entity construct if it was made, NULL otherwise
 */
Entity* enemy_spawn(Vector2D position, enum entity_type type);

#endif