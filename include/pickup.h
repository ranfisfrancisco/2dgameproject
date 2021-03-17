#ifndef __PICKUP_H__
#define __PICKUP_H__

#include "entity.h"

/**
 * @brief Spawns a pickup of a given type
 * @return Pointer to new entity construct if it was made, NULL otherwise
 */
Entity* pickup_spawn(Vector2D position, enum enemy_type type);

#endif