#include "simple_logger.h"
#include "enemy.h"
#include "player.h"
#include <stdlib.h>

void enemy_think();
void enemy_hurt();
void enemy_update(Entity* self);

Entity *enemy_spawn(Vector2D position) {
	Entity* ent;

	ent = entity_new();
	if (!ent) {
		slog("failed to create entity for player");
		return NULL;
	}
	ent->sprite = gf2d_sprite_load_all("images/cat.png", 50, 50, 10);
	vector2d_copy(ent->position, position);
	gfc_rect_set(ent->hurtbox, ent->position.x, ent->position.y, ent->sprite->frame_w, ent->sprite->frame_h);
	ent->frameRate = 0.1;
	ent->frameCount = 16;
	ent->maxHealth = 100;
	ent->health = ent->maxHealth;
	ent->type = ENEMY_TYPE;
	ent->rotation = vector3d(0, 0, 0);
	ent->update = enemy_update;
	ent->think = enemy_think;
	ent->hurt = enemy_hurt;
	ent->speed = 2;
	ent->flip = vector2d(FACE_RIGHT, 0);
	ent->scale = vector2d(4, 4);

	return ent;
}

void enemy_update_side(Entity* self) {
	Vector2D playerPosition;

	playerPosition = player_get_position();

	if (playerPosition.x - self->position.x > 0) {
		self->flip.x = FACE_RIGHT;
	}
	else if (playerPosition.x - self->position.x < 0) {
		self->flip.x = FACE_LEFT;
	}
}

//Moves enemy closer to player and returns current distance from enemy to player
Vector2D enemy_move_to_player(Entity* self) {
	Vector2D playerPosition;
	float xchange, ychange;

	playerPosition = player_get_position();
	xchange = 1;
	ychange = 1;

	if (playerPosition.x - self->position.x > 0) {
		self->position.x += xchange;
	}
	else if (playerPosition.x - self->position.x < 0) {
		self->position.x -= xchange;
	}

	if (playerPosition.y - self->position.y > 0) {
		self->position.y += ychange;
	}
	else if (playerPosition.y - self->position.y < 0) {
		self->position.y -= ychange;
	}

	enemy_update_side(self);

	return vector2d(abs(playerPosition.x - self->position.x), abs(playerPosition.y - self->position.y));
}

void enemy_change_state(Entity* self, enum enemy_state state) {
	self->frame = 0;
	self->statePos = 0;
	self->state = state;
}

//TODO: Add randomness to movement
void enemy_think(Entity* self) {
	int startFrame, endFrame;
	Vector2D distToPlayer;

	switch (self->state) {
	case ENEMY_IDLE:
		distToPlayer = enemy_move_to_player(self);

		if (distToPlayer.x < 10 && distToPlayer.y < 3)
			enemy_change_state(self, ENEMY_ATTACK);

		break;

	case ENEMY_HURT:
		startFrame = 13;
		endFrame = 13;
		
		self->statePos += 1;

		if (self->frame < startFrame)
			self->frame = startFrame;
		else if (self->frame >= endFrame) {

		}
		else {
			self->frame++;
		}

		if (self->statePos > 30) {
			enemy_change_state(self, ENEMY_IDLE);
		}
		break;
			
	case ENEMY_ATTACK:
		startFrame = 10;
		endFrame = 19;

		self->statePos += 1;

		if (self->frame < startFrame)
			self->frame = startFrame;
		else if (self->frame >= endFrame) {

		}
		else {
			self->frame++;
		}

		if (self->statePos > 15) {
			enemy_change_state(self, ENEMY_IDLE);
		}
		break;

	default:
		break;
	}
	
}

void enemy_die(Entity* self) {
	entity_free(self);
	return;
}

void enemy_update(Entity* self) {
	if (self->health <= 0) {
		enemy_die(self);
		return;
	}

	gfc_rect_set(self->hurtbox, self->position.x, self->position.y, self->sprite->frame_w, self->sprite->frame_h);
}


void enemy_hurt(Entity* self, int damage) {
	enemy_change_state(self, ENEMY_HURT);
	self->health -= damage;
}