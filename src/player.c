#include "simple_logger.h"
#include "player.h"

enum player_state {playerneutral, playerspinning};

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
	ent->type = playertype;
	ent->rotation = vector3d(0,0,0);
	ent->update = player_update;
	return ent;
}

void player_update(Entity* self) {
	self->frame++;
	if (self->frame > 16)
		self->frame = 0;
}

void player_input(Entity* self, const Uint8* keys) {
	switch (self->state) 
	{
	case playerneutral:
		if (keys[SDL_SCANCODE_LEFT]) {
			self->position.x -= 2;
			if (self->position.x < 0)
				self->position.x = 0;

		}
		else if (keys[SDL_SCANCODE_RIGHT]) {
			self->position.x += 2;
			if (self->position.x > 1200)
				self->position.x = 1200;
		}
		if (keys[SDL_SCANCODE_UP]) {
			self->position.y -= 2;
			if (self->position.y < 0)
				self->position.y = 0;
		}
		else if (keys[SDL_SCANCODE_DOWN]) {
			self->position.y += 2;
			if (self->position.y > 1200)
				self->position.y = 1200;
		}
		if (keys[SDL_SCANCODE_S]) {
			self->state = playerspinning;
			self->statePos = 0;
		}
		break;

	case playerspinning:
		self->statePos++;
		if (self->statePos > 360) {
			self->state = playerneutral;
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