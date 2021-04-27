#include "simple_logger.h"
#include "director.h"
#include "enemy.h"
#include "player.h"
#include "camera.h"
#include "random.h"
#include <stdlib.h>

void cat_enemy_think(Entity* self);
void dog_enemy_1_think(Entity* self);
void dog_enemy_2_think(Entity* self);
void cat_enemy_hurt(Entity* self, int damage);
void dog_enemy_1_hurt(Entity* self, int damage);
void dog_enemy_2_hurt(Entity* self, int damage);
void enemy_update(Entity* self);
void enemy_die(Entity* self);

Entity* enemy_spawn(Vector2D position, enum entity_type type) {
	Entity* ent;
	EnemyData* data;

	if (!entity_is_enemy(type)) {
		slog("Attempted to spawn enemy as non-enemy type");
		return NULL;
	}

	ent = entity_new(type);
	if (!ent) {
		slog("failed to create entity for player");
		return NULL;
	}

	data = malloc(sizeof(EnemyData));
	if (!data) {
		slog("failed to create data for Enemy");
		entity_free(ent);
		return NULL;
	}
	memset(data, 0, sizeof(EnemyData));
	ent->data = data;
	data->attackSound = gfc_sound_load("sounds/punch1.mp3", -1, -1);
	data->hurtSound = gfc_sound_load("sounds/mc_hurt.mp3", -1, -1);

	if (type == BOSS_TYPE_1 || type == BOSS_TYPE_2) {
		ent->sprite = gf2d_sprite_load_image("images/dog.PNG");
	}
	else if (type == ENEMY_TYPE_1 || type == ENEMY_TYPE_2 || type == ENEMY_TYPE_3 || type == ENEMY_TYPE_4 || type == ENEMY_TYPE_5) {
		ent->sprite = gf2d_sprite_load_all("images/cat.png", 50, 50, 10);
		ent->think = cat_enemy_think;
		ent->hurt = cat_enemy_hurt;
	} 

	if (ent->sprite == NULL) {
		slog("Failed to load sprite for enemy!");
		entity_free(ent);
		return NULL;
	}

	vector2d_copy(ent->drawPosition, position);
	ent->deathScore = 100;
	ent->flip = vector2d(FACE_RIGHT, 0);
	ent->rotation = vector3d(0, 0, 0);
	ent->defaultColorShift = vector4d(255, 255, 255, 255);
	ent->update = enemy_update;	
	ent->die = enemy_die;

	switch (type) {
	case ENEMY_TYPE_1:
		ent->maxHealth = 100;
		ent->speed = 2;
		ent->scale = vector2d(4, 4);
		break;
	case ENEMY_TYPE_2:
		ent->maxHealth = 150;
		ent->speed = 1;
		ent->scale = vector2d(8, 8);
		break;
	case ENEMY_TYPE_3:
		ent->maxHealth = 50;
		ent->speed = 3;
		ent->scale = vector2d(1, 1);
		break;
	case ENEMY_TYPE_4:
		ent->maxHealth = 100;
		ent->speed = 2;
		ent->defaultColorShift = vector4d(255, 255, 255, 100);
		ent->scale = vector2d(4, 4);
		break;
	case ENEMY_TYPE_5:
		ent->maxHealth = 100;
		ent->speed = 2;
		ent->defaultColorShift = vector4d(255, 100, 255, 255);
		ent->scale = vector2d(4, 4);
		break;
	case BOSS_TYPE_1:
		ent->maxHealth = 200;
		ent->speed = 2;
		ent->scale = vector2d(4, 4);
		ent->think = dog_enemy_1_think;
		ent->hurt = dog_enemy_1_hurt;
		break;
	case BOSS_TYPE_2:
		ent->maxHealth = 200;
		ent->speed = 4;
		ent->defaultColorShift = vector4d(0, 0, 0, 0);
		ent->scale = vector2d(4, 4);
		ent->think = dog_enemy_2_think;
		ent->hurt = dog_enemy_2_hurt;
		break;
	}

	ent->health = ent->maxHealth;
	ent->colorShift = ent->defaultColorShift;

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
	self->colorShift = self->defaultColorShift;
}

//TODO: Add randomness to movement
void cat_enemy_think(Entity* self) {
	int startFrame, endFrame;
	SDL_Rect hitbox;
	Vector2D distToPlayer;

	switch (self->state) {
	case ENEMY_SEEK:
		if (((EnemyData*)self->data)->waitTime > 0) {
			((EnemyData*)self->data)->waitTime--;
			break;
		}

		if (rand_int(0, 1000) < 20) {
			((EnemyData*)self->data)->waitTime = rand_int(10, 30);
		}

		self->attackHit = 0;
		distToPlayer = enemy_move_to_player(self);

		if (distToPlayer.x < 10 && distToPlayer.y < 3 && rand_int(0, 100) > 10)
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
			enemy_change_state(self, ENEMY_SEEK);
		}
		break;
			
	case ENEMY_ATTACK:
		startFrame = 10;
		endFrame = 19;

		self->statePos += 1;

		gfc_rect_set(hitbox, self->hurtbox.x, self->hurtbox.y, self->hurtbox.w, self->hurtbox.y);

		if (!self->attackHit) {
			if (self->side == FACE_LEFT) {
				hitbox.x -= 2 * self->sprite->frame_w;
			}

			if (player_collison_check(hitbox)) {
				self->attackHit = 1;
				player_hurt(NULL, 2);
				gfc_sound_play(((EnemyData*)self->data)->attackSound, 0, 1, 1, 0);
				enemy_change_state(self, ENEMY_SEEK);
				((EnemyData*)self->data)->waitTime = rand_int(30, 60);
			}
		}

		if (self->frame < startFrame)
			self->frame = startFrame;
		else if (self->frame >= endFrame) {

		}
		else {
			self->frame++;
		}

		/*if (self->statePos > 15) {
			enemy_change_state(self, ENEMY_SEEK);
			((EnemyData*)self->data)->waitTime = rand_int(0, 10);
		}*/
		break;

	default:
		break;
	}
	
}

void dog_enemy_1_think(Entity* self) {
	SDL_Rect hitbox;
	Vector2D distToPlayer;

	switch (self->state) {
	case ENEMY_SEEK:
		if (((EnemyData*)self->data)->waitTime > 0) {
			((EnemyData*)self->data)->waitTime--;
			break;
		}

		if (rand_int(0, 1000) < 20) {
			((EnemyData*)self->data)->waitTime = rand_int(10, 30);
		}

		self->attackHit = 0;
		distToPlayer = enemy_move_to_player(self);

		if (distToPlayer.x < 10 && distToPlayer.y < 3)
			enemy_change_state(self, ENEMY_ATTACK);

		break;

	case ENEMY_HURT:
		self->statePos += 1;
		self->colorShift = vector4d(255, 0, 0, 255);

		if (self->statePos > 30) {
			enemy_change_state(self, ENEMY_SEEK);
		}
		break;

	case ENEMY_ATTACK:
		self->statePos += 1;
		self->colorShift = vector4d(0, 255, 0, 255);

		gfc_rect_set(hitbox, self->drawPosition.x, self->drawPosition.y, self->sprite->frame_w * 1.2, self->sprite->frame_h);

		if (!self->attackHit) {
			if (self->side == FACE_LEFT) {
				hitbox.x -= 2 * self->sprite->frame_w;
			}

			if (player_collison_check(hitbox)) {
				self->attackHit = 1;
				player_hurt(NULL, 2);
				gfc_sound_play(((EnemyData*)self->data)->attackSound, 0, 1, 1, 0);
				enemy_change_state(self, ENEMY_SEEK);
				((EnemyData*)self->data)->waitTime = rand_int(30, 60);
			}
		}

		if (self->statePos > 40) {
			enemy_change_state(self, ENEMY_SEEK);
		}
		break;

	default:
		break;
	}

}

void dog_enemy_2_think(Entity* self) {
	SDL_Rect hitbox;
	Vector2D distToPlayer;

	switch (self->state) {
	case ENEMY_SEEK:
		if (((EnemyData*)self->data)->waitTime > 0) {
			((EnemyData*)self->data)->waitTime--;
			break;
		}

		if (rand_int(0, 1000) < 20) {
			((EnemyData*)self->data)->waitTime = rand_int(10, 30);
		}

		self->attackHit = 0;
		distToPlayer = enemy_move_to_player(self);

		if (distToPlayer.x < 10 && distToPlayer.y < 3)
			enemy_change_state(self, ENEMY_ATTACK);

		break;

	case ENEMY_HURT:
		self->statePos += 1;
		self->colorShift = vector4d(255, 0, 0, 255);

		if (self->statePos > 30) {
			enemy_change_state(self, ENEMY_SEEK);
		}
		break;

	case ENEMY_ATTACK:
		self->statePos += 1;
		self->colorShift = vector4d(0, 255, 0, 255);

		gfc_rect_set(hitbox, self->drawPosition.x, self->drawPosition.y, self->sprite->frame_w * 1.2, self->sprite->frame_h);

		if (!self->attackHit) {
			if (self->side == FACE_LEFT) {
				hitbox.x -= 2 * self->sprite->frame_w;
			}

			if (player_collison_check(hitbox)) {
				self->attackHit = 1;
				player_hurt(NULL, 2);
				gfc_sound_play(((EnemyData*)self->data)->attackSound, 0, 1, 1, 0);
				enemy_change_state(self, ENEMY_SEEK);
				((EnemyData*)self->data)->waitTime = rand_int(30, 60);
			}
		}

		if (self->statePos > 40) {
			enemy_change_state(self, ENEMY_SEEK);
		}
		break;

	default:
		break;
	}

}

void enemy_die(Entity* self) {
	director_add_score(self->deathScore);
	gfc_sound_free(((EnemyData*)self->data)->attackSound);
	gfc_sound_free(((EnemyData*)self->data)->hurtSound);
	entity_free(self);
	return;
}

void enemy_set_hurtbox(Entity* self) {
	Vector2D cameraOffset;

	cameraOffset = camera_get_offset();

	gfc_rect_set(self->hurtbox, self->drawPosition.x + cameraOffset.x, self->drawPosition.y + cameraOffset.y, self->sprite->frame_w * self->scale.x, self->sprite->frame_h * self->scale.y);
}

void enemy_update(Entity* self) {
	if (self->health <= 0) {
		enemy_die(self);
		return;
	}

	self->realPosition = entity_real_position(self);

	enemy_set_hurtbox(self);
}


void cat_enemy_hurt(Entity* self, int damage) {
	if (self->type == ENEMY_TYPE_4) {
		if (damage > 30) {
			enemy_change_state(self, ENEMY_HURT);
			self->health -= damage;
		}
		else {

		}
	}
	else if (self->type == ENEMY_TYPE_5) {
		if (damage < 30) {
			enemy_change_state(self, ENEMY_HURT);
			self->health -= damage;
		}
		else {

		}
	}
	else {
		enemy_change_state(self, ENEMY_HURT);
		self->health -= damage;
	}

	gfc_sound_play(((EnemyData*)self->data)->hurtSound, 0, 1, 1, 0);
}

void dog_enemy_1_hurt(Entity* self, int damage) {
	if (self->state != ENEMY_ATTACK)
		return;
	enemy_change_state(self, ENEMY_HURT);
	self->health -= damage;

	gfc_sound_play(((EnemyData*)self->data)->hurtSound, 0, 1, 1, 0);
}

void dog_enemy_2_hurt(Entity* self, int damage) {
	enemy_change_state(self, ENEMY_HURT);
	self->health -= damage;

	gfc_sound_play(((EnemyData*)self->data)->hurtSound, 0, 1, 1, 0);
}