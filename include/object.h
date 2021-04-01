#ifndef __OBJECT_H__
#define __OBJECT_H__

#include "entity.h"

/**
 * @brief Spawns a pickup of a given type
 * @return Pointer to new entity construct if it was made, NULL otherwise
 */
Entity* object_spawn(Vector2D position, enum enemy_type type);

#endif