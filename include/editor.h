#ifndef __EDITOR_H__
#define __EDITOR_H__

#include "gfc_vector.h"
#include "entity.h"

typedef struct {
	Vector2D position;
	Entity* shadow;
	EntityType current_type;
} Editor;

void editor_init();

void editor_update();

#endif