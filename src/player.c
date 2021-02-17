#include "simple_logger.h"
#include "player.h"
#include "input.h"

enum player_state {PLAYER_IDLE, PLAYER_WALK, PLAYER_PUNCH, PLAYER_KICK, PLAYER_QCFP};
enum facing_side {FACE_LEFT, FACE_RIGHT};

void player_update(Entity* self);

Entity* player_spawn(Vector2D position) {
	Entity* ent;
	ent = entity_new();
	if (!ent) {
		slog("failed to create entity for player");
		return NULL;
	}
	ent->sprite = gf2d_sprite_load_all("images/kyo_1.png", 112, 128, 12);
	vector2d_copy(ent->position, position);
	ent->frameRate = 0.1;
	ent->frameCount = 16;
	ent->maxHealth = 100;
	ent->health = ent->maxHealth;
	ent->type = PLAYER_TYPE;
	ent->rotation = vector3d(0,0,0);
	ent->update = player_update;
	ent->speed = 2;
	ent->flip = vector2d(FACE_RIGHT, 0);
	ent->scale = vector2d(2, 2);
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

void player_update_side(Entity* self, const Uint8* keys) {
	int left, right;

	left = keys[SDL_SCANCODE_A];
	right = keys[SDL_SCANCODE_D];

	if (left && !right) {
		self->side = FACE_LEFT;
		self->flip.x = FACE_LEFT;
	}
	else if (right && !left) {
		self->side = FACE_RIGHT;
		self->flip.x = FACE_RIGHT;
	}
}

void player_change_state(Entity* self, enum player_state state) {
	self->frame = 0;
	self->statePos = 0;
	self->state = state;
}

//TODO: QCB detection
void player_input(Entity* self, const Uint8* keys) {
	enum player_directional_input raw_input = NO_INPUT;
	enum player_move move = 0;
	int up, down, left, right;
	int punch, kick;
	int startFrame, endFrame;

	up = keys[SDL_SCANCODE_W];
	right = keys[SDL_SCANCODE_D];
	down = keys[SDL_SCANCODE_S];
	left = keys[SDL_SCANCODE_A];
	punch = keys[SDL_SCANCODE_J];
	kick = keys[SDL_SCANCODE_K];


	//check for inputs
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
	
	switch (self->state) 
	{
	case PLAYER_IDLE:
		startFrame = 1;
		endFrame = 10;
		self->frame += 0.05;

		if (self->frame > endFrame || self->frame < startFrame)
			self->frame = startFrame;

		if (move == QCF_MOVE && punch) {
			player_change_state(self, PLAYER_QCFP);
		}
		else if (punch) {
			player_change_state(self, PLAYER_PUNCH);
		}
		else if (kick) {
			player_change_state(self, PLAYER_KICK);
		}
		else if (right || left || up || down) {
			player_change_state(self, PLAYER_WALK);
			player_update_side(self, keys);
			player_movement(self, keys);
		}
		else {
			
		}
		break;

	case PLAYER_WALK:
		startFrame = 11;
		endFrame = 12;

		self->frame += 0.20;
		if (self->frame > endFrame || self->frame < startFrame)
			self->frame = startFrame;

		player_movement(self, keys);
		player_update_side(self, keys);

		if (move == QCF_MOVE && punch) {
			player_change_state(self, PLAYER_QCFP);
		} else if (move == QCF_MOVE && punch) {
			player_change_state(self, PLAYER_QCFP);
		}
		else if (punch) {
			player_change_state(self, PLAYER_PUNCH);
		}
		else if (kick) {
			player_change_state(self, PLAYER_KICK);
		}
		else if (!right && !left && !up && !down) {
			player_change_state(self, PLAYER_IDLE);
		} 
		
		break;


	case PLAYER_PUNCH:
		startFrame = 23;
		endFrame = 23;
		self->statePos++;

		if (self->frame < startFrame)
			self->frame = startFrame;

		if (self->statePos > 12) 
			player_change_state(self, PLAYER_IDLE);
		
		break;

	case PLAYER_KICK:
		startFrame = 18;
		endFrame = 22;
		self->statePos++;
		
		if (self->frame < startFrame)
			self->frame = startFrame;

		else if (self->frame == endFrame) {
			
		}
		else {
			self->frame++;
		}

		if (self->statePos > 15)
			player_change_state(self, PLAYER_IDLE);

		break;

	case PLAYER_QCFP:
		startFrame = 14;
		endFrame = 16;

		self->statePos++;

		if (self->frame < startFrame)
			self->frame = startFrame;
		else if (self->frame == endFrame) {

		}
		else {
			self->frame++;
		}

		if (self->statePos > 60)
			player_change_state(self, PLAYER_IDLE);

		break;
	default:
		player_change_state(self, PLAYER_WALK);
	}
}

void player_update(Entity* self) {
	const Uint8* keys;
	keys = SDL_GetKeyboardState(NULL); // get the keyboard state for this frame
	player_input(self, keys);
}