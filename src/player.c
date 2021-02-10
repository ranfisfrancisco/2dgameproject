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

}

void player_input(Entity* self, const Uint8* keys) {
	switch (self->state) {
	case playerneutral:
		break;
	case playerspinning:
		break;
	default:
		break;
	}



	if (keys[SDL_SCANCODE_LEFT]) {
		self->position.x -= 1;
		if (self->position.x < 0)
			self->position.x = 0;

	}
	else if (keys[SDL_SCANCODE_RIGHT]) {
		self->position.x += 1;
		if (self->position.x > 1200)
			self->position.x = 1200;
	}
	else if (keys[SDL_SCANCODE_UP]) {
		self->state = playerspinning;
	}

	if (self->state == playerspinning) {
		
	}
}