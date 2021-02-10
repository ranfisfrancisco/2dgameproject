#include "simple_logger.h"
#include "player.h"

enum player_state {PLAYER_NEUTRAL, PLAYER_SPINNING};

void player_update(Entity* self);

Entity* player_spawn(Vector2D position) {
	Entity* ent;
	ent = entity_new();
	if (!ent) {
		slog("failed to create entity for player");
		return NULL;
	}
	ent->sprite = gf2d_sprite_load_all("images/ed210_top.png", 128, 128, 16);
	vector2d_copy(ent->position, position);
	ent->frameRate = 0.1;
	ent->frameCount = 16;
	ent->maxHealth = 100;
	ent->health = ent->maxHealth;
	ent->type = PLAYER_TYPE;
	ent->rotation = vector3d(0,0,0);
	ent->update = player_update;
	ent->speed = 2;
	return ent;
}

void player_update(Entity* self) {
	self->frame++;
	if (self->frame > 16)
		self->frame = 0;
}

void player_movement(Entity* self, const Uint8* keys) {
	if (keys[SDL_SCANCODE_LEFT]) {
		self->position.x -= self->speed;
		if (self->position.x < 0)
			self->position.x = 0;

	}
	else if (keys[SDL_SCANCODE_RIGHT]) {
		self->position.x += self->speed;
		if (self->position.x > 1200)
			self->position.x = 1200;
	}
	if (keys[SDL_SCANCODE_UP]) {
		self->position.y -= self->speed;
		if (self->position.y < 0)
			self->position.y = 0;
	}
	else if (keys[SDL_SCANCODE_DOWN]) {
		self->position.y += self->speed;
		if (self->position.y > 1200)
			self->position.y = 1200;
	}
}

void player_input(Entity* self, const Uint8* keys) {
	switch (self->state) 
	{
	case PLAYER_NEUTRAL:
		player_movement(self, keys);
		if (keys[SDL_SCANCODE_S]) {
			self->state = PLAYER_SPINNING;
			self->statePos = 0;
		}
		break;

	case PLAYER_SPINNING:
		player_movement(self, keys);
		self->statePos++;
		if (self->statePos > 360) {
			self->state = PLAYER_NEUTRAL;
			self->statePos = 0;
			self->rotation = vector3d(0,0,0);
		}
		else {
			self->rotation = vector3d(64, 64, self->statePos);
		}
		break;
	default:
		break;
	}
}