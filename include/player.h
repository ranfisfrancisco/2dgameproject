#ifndef __PLAYER_H__
#define __PLAYER_H__

#include "entity.h"
#include "gfc_vector.h"

Entity* player_spawn(Vector2D position);

void player_input(Entity* self, const Uint8* keys);

#endif