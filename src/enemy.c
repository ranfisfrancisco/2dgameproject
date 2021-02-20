#include "simple_logger.h"
#include "enemy.h"
#include "player.h"
#include <stdlib.h>

void enemy_think();
void enemy_hurt();

Entity *enemy_spawn(Vector2D position) {
	Entity* ent;

	ent = entity_new();
	if (!ent) {
		slog("failed to create entity for player");
		return NULL;
	}
	ent->sprite = gf2d_sprite_load_all("images/cat.png", 50, 50, 10);
	vector2d_copy(ent->position, position);
	ent->frameRate = 0.1;
	ent->frameCount = 16;
	ent->maxHealth = 100;
	ent->health = ent->maxHealth;
	ent->type = ENEMY_TYPE;
	ent->rotation = vector3d(0, 0, 0);
	//ent->update = player_update;
	ent->think = enemy_think;
	ent->hurt = enemy_hurt;
	ent->speed = 2;
	ent->flip = vector2d(FACE_RIGHT, 0);
	ent->scale = vector2d(4, 4);

	return ent;
}



//Moves enemy closer to player and returns current distance from enemy to player
Vector2D enemy_move_to_player(struct Entity_s* self) {
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

	return vector2d(abs(playerPosition.x - self->position.x), (playerPosition.y - self->position.y));
}

void enemy_change_state(struct Entity_s* self, enum enemy_state state) {
	self->frame = 0;
	self->statePos = 0;
	self->state = state;
}

//TODO: Add randomness to movement
void enemy_think(struct Entity_s* self) {
	int startFrame, endFrame;
	Vector2D distToPlayer;

	switch (self->state) {
	case ENEMY_IDLE:
		distToPlayer = enemy_move_to_player(self);

		if (distToPlayer.x < 10 && distToPlayer.y < 10)
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

		if (self->statePos > 20) {
			enemy_change_state(self, ENEMY_IDLE);
		}
			
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

		if (self->statePos > 20) {
			enemy_change_state(self, ENEMY_IDLE);
		}

	default:
		break;
	}
	
}


void enemy_hurt(struct Entity_s* self) {
	self->state = ENEMY_HURT;
}