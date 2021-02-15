#ifndef __ENTITY_H__
#define __ENTITY_H__

#include "gfc_types.h"
#include "gfc_vector.h"
#include "gf2d_sprite.h"

enum entity_type {NO_TYPE, PLAYER_TYPE, ENEMY_TYPE};

typedef struct Entity_s{
	Bool _inuse;
	Vector2D position;
	Sprite *sprite;
	float frame;
	float frameRate;
	int frameCount;
	Vector3D rotation;
	void (*update)(struct Entity_s* self);
	void (*think)(struct Entity_s* self);
	void (*draw)(struct Entity_s* self);
	void (*free)(struct Entity_s* self);
	void* data;
	enum entity_type type;
	int side;
	int state;
	int statePos;
	int health;
	int maxHealth;
	int speed;
} Entity;

void entity_manager_init(Uint32 max_entities);

void entity_manager_free();

//calls update function on all entities.
void entity_manager_update_entities();

void entity_manager_draw_entities();

Entity *entity_new();

void entity_free(Entity *ent);

void entity_draw(Entity *ent);

#endif