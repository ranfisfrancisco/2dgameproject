#include "simple_logger.h"
#include "enemy.h"
#include "player.h"
#include <stdlib.h>

void enemy_think(Entity* self);
void enemy_hurt(Entity* self, int damage);
void enemy_update(Entity* self);

Entity* enemy_spawn(Vector2D position, enum enemy_type type) {
	Entity* ent;

	ent = entity_new();
	if (!ent) {
		slog("failed to create entity for player");
		return NULL;
	}
	if (type == ENEMY_TYPE_1 || type == ENEMY_TYPE_2 || type == ENEMY_TYPE_3 || type == ENEMY_TYPE_4 || type == ENEMY_TYPE_5) {
		ent->sprite = gf2d_sprite_load_all("images/cat.png", 50, 50, 10);
	}
	else {
		slog("Attempted to spawn enemy as non-enemy type");
		entity_free(ent);
		return NULL;
	}
	vector2d_copy(ent->drawPosition, position);
	gfc_rect_set(ent->hurtbox, ent->drawPosition.x, ent->drawPosition.y, ent->sprite->frame_w, ent->sprite->frame_h);
	if (type == ENEMY_TYPE_1)
		ent->maxHealth = 100;
	else if (type == ENEMY_TYPE_2)
		ent->maxHealth = 150;
	else if (type == ENEMY_TYPE_3)
		ent->maxHealth = 50;
	else if (type == ENEMY_TYPE_4)
		ent->maxHealth = 100;
	else if (type == ENEMY_TYPE_5)
		ent->maxHealth = 100;
	ent->health = ent->maxHealth;
	ent->type = type;
	ent->rotation = vector3d(0, 0, 0);
	ent->update = enemy_update;
	ent->think = enemy_think;
	ent->hurt = enemy_hurt;
	if (type == ENEMY_TYPE_1)
		ent->speed = 2;
	else if (type == ENEMY_TYPE_2)
		ent->speed = 1;
	else if (type == ENEMY_TYPE_3)
		ent->speed = 3;
	else if (type == ENEMY_TYPE_4)
		ent->speed = 2;
	else if (type == ENEMY_TYPE_5)
		ent->speed = 2;
	ent->flip = vector2d(FACE_RIGHT, 0);

	if (type == ENEMY_TYPE_1)
		ent->scale = vector2d(4, 4);
	else if (type == ENEMY_TYPE_2)
		ent->scale = vector2d(8, 8);
	else if (type == ENEMY_TYPE_3)
		ent->scale = vector2d(1, 1);
	else if (type == ENEMY_TYPE_4)
		ent->scale = vector2d(1, 1);
	else if (type == ENEMY_TYPE_5)
		ent->scale = vector2d(1, 1);

	return ent;
}

void enemy_update_side(Entity* self) {
	Vector2D playerPosition;

	playerPosition = player_get_position();

	if (playerPosition.x - self->drawPosition.x > 0) {
		self->flip.x = FACE_RIGHT;
	}
	else if (playerPosition.x - self->drawPosition.x < 0) {
		self->flip.x = FACE_LEFT;
	}
}

//Moves enemy closer to player and returns current distance from enemy to player
Vector2D enemy_move_to_player(Entity* self) {
	Vector2D playerPosition;
	Vector2D currentPosition;

	playerPosition = player_get_position();
	currentPosition = entity_real_position(self);
	
	if (playerPosition.x - currentPosition.x > 0) {
		self->drawPosition.x += self->speed;
	}
	else if (playerPosition.x - currentPosition.x < 0) {
		self->drawPosition.x -= self->speed;
	}

	if (playerPosition.y -currentPosition.y > 0) {
		self->drawPosition.y += self->speed;
	}
	else if (playerPosition.y - currentPosition.y < 0) {
		self->drawPosition.y -= self->speed;
	}

	enemy_update_side(self);

	return vector2d(abs(playerPosition.x - currentPosition.x), abs(playerPosition.y - currentPosition.y));
}

void enemy_change_state(Entity* self, enum enemy_state state) {
	self->frame = 0;
	self->statePos = 0;
	self->state = state;
}

//TODO: Add randomness to movement
void enemy_think(Entity* self) {
	int startFrame, endFrame;
	SDL_Rect hitbox;
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

		gfc_rect_set(hitbox, self->drawPosition.x + self->sprite->frame_w, self->drawPosition.y, self->sprite->frame_w, self->sprite->frame_h);

		if (!self->attackHit) {
			if (self->side == FACE_LEFT) {
				hitbox.x -= 2 * self->sprite->frame_w;
			}

			if (entity_manager_check_collison(hitbox, 4))
				self->attackHit = 1;
		}

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

	self->realPosition = entity_real_position(self);

	gfc_rect_set(self->hurtbox, self->drawPosition.x, self->drawPosition.y, self->sprite->frame_w * self->scale.x, self->sprite->frame_h * self->scale.y);
}


void enemy_hurt(Entity* self, int damage) {
	enemy_change_state(self, ENEMY_HURT);
	self->health -= damage;
}