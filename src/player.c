#include "simple_logger.h"
#include "player.h"
#include "input.h"

enum player_state {PLAYER_IDLE, PLAYER_WALK, PLAYER_SPINNING};

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

void player_movement(Entity* self, const Uint8* keys) {
	int up, down, left, right;

	up = keys[SDL_SCANCODE_W];
	right = keys[SDL_SCANCODE_D];
	down = keys[SDL_SCANCODE_S];
	left = keys[SDL_SCANCODE_A];

	if (left) {
		self->position.x -= self->speed;
		if (self->position.x < 0)
			self->position.x = 0;

	}
	else if (right) {
		self->position.x += self->speed;
		if (self->position.x > 1200)
			self->position.x = 1200;
	}
	if (up) {
		self->position.y -= self->speed;
		if (self->position.y < 0)
			self->position.y = 0;
	}
	else if (down) {
		self->position.y += self->speed;
		if (self->position.y > 1200)
			self->position.y = 1200;
	}
}

void player_input(Entity* self, const Uint8* keys) {
	enum player_directional_input raw_input = NO_INPUT;
	enum player_move move = 0;
	int up, down, left, right;
	int attack;
	int startFrame, endFrame;

	up = keys[SDL_SCANCODE_W];
	right = keys[SDL_SCANCODE_D];
	down = keys[SDL_SCANCODE_S];
	left = keys[SDL_SCANCODE_A];
	attack = keys[SDL_SCANCODE_J];


	//check for too many inputs
	if (up + right + down + left > 3) {
		raw_input = NO_INPUT;
	}
	else if (up && right){
		raw_input = UP_FORWARD_INPUT;
	}
	else if (right && down) {
		raw_input = DOWN_FORWARD_INPUT;
	}
	else if (down && left) {
		raw_input = DOWN_BACK_INPUT;
	}
	else if (left && up) {
		raw_input = UP_BACK_INPUT;
	}
	else if (up) {
		raw_input = UP_INPUT;
	}
	else if (right) {
		raw_input = FORWARD_INPUT;
	}
	else if (down) {
		raw_input = DOWN_INPUT;
	}
	else if (left) {
		raw_input = BACK_INPUT;
	}

	move = feed_input(raw_input);
	printf("%d %d\n", raw_input, move);

	//PROBLEM based on statepos, decide what frame to show
	switch (self->state) 
	{
	case PLAYER_IDLE:
		startFrame = 0;
		endFrame = 16;
		self->frame += 0.1;
		if (self->frame > endFrame || self->frame < startFrame)
			self->frame = startFrame;

		if (move == BACK_FORWARD_MOVE) {
			
		}
		else if (move == QCF_MOVE && attack) {
			self->state = PLAYER_SPINNING;
			self->statePos = 0;
		}
		else {
			player_movement(self, keys);
		}
		break;

	case PLAYER_SPINNING:
		startFrame = 33;
		endFrame = 48;
		player_movement(self, keys);
		self->statePos += 1;

		if (self->statePos > 180) {
			startFrame = 17;
			endFrame = 17;
		}

		self->frame += 0.1;
		if (self->frame > endFrame || self->frame < startFrame)
			self->frame = startFrame;

		if (self->statePos > 360) {
			self->state = PLAYER_IDLE;
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

void player_update(Entity* self) {
	const Uint8* keys;
	keys = SDL_GetKeyboardState(NULL); // get the keyboard state for this frame
	player_input(self, keys);
}