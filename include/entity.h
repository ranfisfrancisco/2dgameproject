#ifndef __ENTITY_H__
#define __ENTITY_H__

#include "gfc_types.h"
#include "gfc_vector.h"
#include "gf2d_sprite.h"

enum entity_type {notype, player, enemy};
enum entity_state {nostate, seeking, attack1, attack2};

typedef struct{
	Bool _inuse;
	Vector2D position;
	Sprite *sprite;
	float frame;
	enum entity_type type;
	enum entity_state state;
} Entity;

void entity_manager_init(Uint32 max_entities);

void entity_manager_free();

Entity *entity_new();

void entity_free(Entity *ent);

void entity_draw(Entity *ent);

#endif