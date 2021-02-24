#ifndef __ENTITY_H__
#define __ENTITY_H__

#include "gfc_types.h"
#include "gfc_vector.h"
#include "gf2d_sprite.h"

enum entity_type {NO_TYPE, PLAYER_TYPE, ENEMY_TYPE_1, ENEMY_TYPE_2, ENEMY_TYPE_3, ENEMY_TYPE_4, ENEMY_TYPE_5, PICKUP_TYPE_MEDKIT, PICKUP_TYPE_FMEDKIT, PICKUP_TYPE_KNIFE, PICKUP_TYPE_CROWBAR, PICKUP_TYPE_POWERUP};

enum facing_side { FACE_RIGHT, FACE_LEFT };

typedef struct Entity_s{
	Bool _inuse;
	Vector2D drawPosition;
	Vector2D realPosition;
	Sprite *sprite;
	Vector2D scale;
	SDL_Rect hurtbox;
	int attackHit;
	int baseDamage;
	float frame;
	Vector3D rotation;
	Vector4D colorShift;
	void (*update)(struct Entity_s* self);
	void (*think)(struct Entity_s* self);
	void (*draw)(struct Entity_s* self);
	void (*free)(struct Entity_s* self);
	void (*hurt)(struct Entity_s* self, int damage);
	void* data;
	enum entity_type type;
	int side;
	Vector2D flip;
	int state;
	int statePos;
	int health;
	int maxHealth;
	int defaultSpeed;
	int speed;
} Entity;

void entity_manager_init(Uint32 max_entities);

void entity_manager_free();

Vector2D entity_real_position(Entity* ent);

//calls update function on all entities.
void entity_manager_update_entities();

void entity_manager_think_entities();

void entity_manager_draw_entities();

int entity_manager_check_collison(SDL_Rect rect, int damage);

Entity *entity_new();

void entity_free(Entity *ent);

void entity_draw(Entity *ent);

#endif