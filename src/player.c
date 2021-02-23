#include "simple_logger.h"
#include "player.h"
#include "input.h"
#include "camera.h"

enum player_state {PLAYER_IDLE, PLAYER_HURT, PLAYER_WALK, PLAYER_PUNCH, PLAYER_KICK, PLAYER_QCFP, PLAYER_QCFK, PLAYER_BFP, PLAYER_BFK, PLAYER_PK};

void player_update(Entity* self);
void player_hurt(Entity* self);

static PlayerEntity player = { 0 };

void player_spawn(Vector2D position) {
	if (player.entity != NULL)
		entity_free(player.entity);

	player.entity = entity_new();
	if (!player.entity) {
		slog("failed to create entity for player");
		return NULL;
	}
	player.entity->sprite = gf2d_sprite_load_all("images/kyo_2.png", 128, 144, 13);
	gfc_rect_set(player.entity->hurtbox, player.entity->drawPosition.x, player.entity->drawPosition.y, player.entity->sprite->frame_w, player.entity->sprite->frame_h);
	vector2d_copy(player.entity->drawPosition, position);
	player.entity->maxHealth = 100;
	player.entity->health = player.entity->maxHealth;
	player.entity->type = PLAYER_TYPE;
	player.entity->rotation = vector3d(0,0,0);
	player.entity->update = player_update;
	player.entity->hurt = player_hurt;
	player.entity->speed = 3;
	player.entity->flip = vector2d(FACE_RIGHT, 0);
	player.entity->scale = vector2d(2, 2);
	player.entity->attackHit = 0;

}

Vector2D player_get_position() {
	Vector2D realPosition;

	realPosition = entity_real_position(player.entity);

	if (player.entity->side == FACE_LEFT)
		realPosition.x -= player.entity->sprite->frame_w / 2;

	return realPosition;
}

void player_free() {
	//if player struct has any pointers free them

	if (player.entity->_inuse)
		entity_free(player.entity);

	memset(&player, 0, sizeof(PlayerEntity));
}

void player_movement(const Uint8* keys) {
	int up, down, left, right;

	up = keys[SDL_SCANCODE_W];
	right = keys[SDL_SCANCODE_D];
	down = keys[SDL_SCANCODE_S];
	left = keys[SDL_SCANCODE_A];

	if (left) {
		player.entity->drawPosition.x -= player.entity->speed;
		if (player.entity->drawPosition.x < 0)
			player.entity->drawPosition.x = 0;

		camera_move(vector2d(-player.entity->speed, 0));

	}
	else if (right) {
		player.entity->drawPosition.x += player.entity->speed;
		if (player.entity->drawPosition.x > 1200 - 2*player.entity->sprite->frame_w)
			player.entity->drawPosition.x = 1200 - 2*player.entity->sprite->frame_w;

		camera_move(vector2d(player.entity->speed, 0));
	}
	if (up) {
		player.entity->drawPosition.y -= player.entity->speed;
		if (player.entity->drawPosition.y < 0 + player.entity->sprite->frame_h)
			player.entity->drawPosition.y = 0 + player.entity->sprite->frame_h;

		camera_move(vector2d(0, -player.entity->speed));
	}
	else if (down) {
		player.entity->drawPosition.y += player.entity->speed;
		if (player.entity->drawPosition.y > 1200 - 5*player.entity->sprite->frame_h)
			player.entity->drawPosition.y = 1200 - 5*player.entity->sprite->frame_h;

		camera_move(vector2d(0, player.entity->speed));
	}
}

void player_update_side(const Uint8* keys) {
	int left, right;

	left = keys[SDL_SCANCODE_A];
	right = keys[SDL_SCANCODE_D];

	if (left && !right) {
		player.entity->side = FACE_LEFT;
		player.entity->flip.x = FACE_RIGHT;
	}
	else if (right && !left) {
		player.entity->side = FACE_RIGHT;
		player.entity->flip.x = FACE_LEFT;
	}
}

void player_change_state(enum player_state state) {
	player.entity->frame = 0;
	player.entity->statePos = 0;
	player.entity->attackHit = 0;
	player.entity->state = state;
}

void player_input(const Uint8* keys) {
	enum player_directional_input raw_input = NO_INPUT;
	enum player_move move = 0;
	int up, down, left, right;
	int punch, kick;
	int startFrame, endFrame;
	SDL_Rect hitbox;

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
	
	switch (player.entity->state)
	{
	case PLAYER_IDLE:
		startFrame = 0;
		endFrame = 9;
		player.entity->frame += 0.05;

		if (player.entity->frame > endFrame || player.entity->frame < startFrame)
			player.entity->frame = startFrame;

		if (move == QC_MOVE && punch) {
			player_change_state(PLAYER_QCFP);
		}
		else if (punch && kick) {
			player_change_state(PLAYER_PK);
		}
		else if (punch) {
			player_change_state(PLAYER_PUNCH);
		}
		else if (kick) {
			player_change_state(PLAYER_KICK);
		}
		else if (right || left || up || down) {
			player_change_state(PLAYER_WALK);
			player_update_side(keys);
			player_movement(keys);
		}
		else {
			
		}
		break;

	case PLAYER_WALK:
		startFrame = 10;
		endFrame = 11;

		player.entity->frame += 0.20;
		if (player.entity->frame > endFrame || player.entity->frame < startFrame)
			player.entity->frame = startFrame;

		player_movement(keys);
		player_update_side(keys);

		if (move == QC_MOVE && punch) {
			player_change_state(PLAYER_QCFP);
		} 
		else if (move == QC_MOVE && kick) {
			player_change_state(PLAYER_QCFK);
		} 
		else if (move == BACK_FORWARD_MOVE && punch) {
			player_change_state(PLAYER_BFP);
		}
		else if (move == BACK_FORWARD_MOVE && kick) {
			player_change_state(PLAYER_BFK);
		}
		else if (punch) {
			player_change_state(PLAYER_PUNCH);
		}
		else if (kick) {
			player_change_state(PLAYER_KICK);
		}
		else if (!right && !left && !up && !down) {
			player_change_state(PLAYER_IDLE);
		} 
		
		break;

		//TODO
	case PLAYER_HURT:
		player_change_state(PLAYER_IDLE);
		break;

	case PLAYER_PUNCH:
		startFrame = 23;
		endFrame = 23;
		player.entity->statePos++;

		gfc_rect_set(hitbox, player.entity->drawPosition.x + player.entity->sprite->frame_w, player.entity->drawPosition.y, player.entity->sprite->frame_w, player.entity->sprite->frame_h);

		if (!player.entity->attackHit) {
			if (player.entity->side == FACE_LEFT) {
				hitbox.x -= 2 * player.entity->sprite->frame_w;
			}

			if (entity_manager_check_collison(hitbox, 20))
				player.entity->attackHit = 1;
		}
	

		if (player.entity->frame < startFrame)
			player.entity->frame = startFrame;

		if (player.entity->statePos > 12)
			player_change_state(PLAYER_IDLE);
		else if (player.entity->statePos < 8 && kick) {
			player_change_state(PLAYER_PK);
		}

		
		
		break;

	case PLAYER_KICK:
		startFrame = 18;
		endFrame = 22;
		player.entity->statePos++;

		gfc_rect_set(hitbox, player.entity->drawPosition.x + player.entity->sprite->frame_w, player.entity->drawPosition.y, player.entity->sprite->frame_w, player.entity->sprite->frame_h);

		if (!player.entity->attackHit) {
			if (player.entity->side == FACE_LEFT) {
				hitbox.x -= 2 * player.entity->sprite->frame_w;
			}

			if (entity_manager_check_collison(hitbox, 20))
				player.entity->attackHit = 1;
		}
		
		if (player.entity->frame < startFrame)
			player.entity->frame = startFrame;

		else if (player.entity->frame == endFrame) {
			
		}
		else {
			player.entity->frame++;
		}

		if (player.entity->statePos > 15)
			player_change_state(PLAYER_IDLE);
		else if (player.entity->statePos < 8 && punch) {
			player_change_state(PLAYER_PK);
		}

		break;

	case PLAYER_QCFP:
		startFrame = 14;
		endFrame = 16;

		player.entity->statePos++;

		gfc_rect_set(hitbox, player.entity->drawPosition.x + player.entity->sprite->frame_w, player.entity->drawPosition.y, player.entity->sprite->frame_w, player.entity->sprite->frame_h);

		if (!player.entity->attackHit) {
			if (player.entity->side == FACE_LEFT) {
				hitbox.x -= 2 * player.entity->sprite->frame_w;
			}

			if (entity_manager_check_collison(hitbox, 20))
				player.entity->attackHit = 1;
		}

		if (player.entity->frame < startFrame)
			player.entity->frame = startFrame;
		else if (player.entity->frame == endFrame) {

		}
		else {
			player.entity->frame++;
		}

		if (player.entity->statePos > 45)
			player_change_state(PLAYER_IDLE);

		break;

	case PLAYER_QCFK:
		startFrame = 30;
		endFrame = 39;

		player.entity->statePos++;

		gfc_rect_set(hitbox, player.entity->drawPosition.x + player.entity->sprite->frame_w, player.entity->drawPosition.y, player.entity->sprite->frame_w, player.entity->sprite->frame_h);

		if (!player.entity->attackHit) {
			if (player.entity->side == FACE_LEFT) {
				hitbox.x -= 2 * player.entity->sprite->frame_w;
			}

			if (entity_manager_check_collison(hitbox, 20))
				player.entity->attackHit = 1;
		}

		if (player.entity->frame < startFrame)
			player.entity->frame = startFrame;
		else if (player.entity->frame == endFrame) {

		}
		else {
			player.entity->frame+=0.25;
		}

		if (player.entity->statePos > 45)
			player_change_state(PLAYER_IDLE);

		break;

	case PLAYER_BFP:
		startFrame = 41;
		endFrame = 47;

		player.entity->statePos++;

		gfc_rect_set(hitbox, player.entity->drawPosition.x + player.entity->sprite->frame_w, player.entity->drawPosition.y, player.entity->sprite->frame_w, player.entity->sprite->frame_h);

		if (!player.entity->attackHit) {
			if (player.entity->side == FACE_LEFT) {
				hitbox.x -= 2 * player.entity->sprite->frame_w;
			}

			if (entity_manager_check_collison(hitbox, 20))
				player.entity->attackHit = 1;
		}

		if (player.entity->frame < startFrame)
			player.entity->frame = startFrame;
		else if (player.entity->frame == endFrame) {

		}
		else {
			player.entity->frame+= 0.5;
		}

		if (player.entity->statePos > 45)
			player_change_state(PLAYER_IDLE);

		break;

	case PLAYER_BFK:
		startFrame = 24;
		endFrame = 29;

		player.entity->statePos++;

		gfc_rect_set(hitbox, player.entity->drawPosition.x + player.entity->sprite->frame_w, player.entity->drawPosition.y, player.entity->sprite->frame_w, player.entity->sprite->frame_h);

		if (!player.entity->attackHit) {
			if (player.entity->side == FACE_LEFT) {
				hitbox.x -= 2 * player.entity->sprite->frame_w;
			}

			if (entity_manager_check_collison(hitbox, 20))
				player.entity->attackHit = 1;
		}

		if (player.entity->frame < startFrame)
			player.entity->frame = startFrame;
		else if (player.entity->frame == endFrame) {

		}
		else {
			player.entity->frame += 0.5;
		}

		if (player.entity->statePos > 45)
			player_change_state(PLAYER_IDLE);

		break;

	case PLAYER_PK:
		startFrame = 48;
		endFrame = 63;

		player.entity->statePos++;

		gfc_rect_set(hitbox, player.entity->drawPosition.x + player.entity->sprite->frame_w, player.entity->drawPosition.y, player.entity->sprite->frame_w, player.entity->sprite->frame_h);

		if (!player.entity->attackHit) {
			if (player.entity->side == FACE_LEFT) {
				hitbox.x -= 2 * player.entity->sprite->frame_w;
			}

			if (entity_manager_check_collison(hitbox, 60))
				player.entity->attackHit = 1;
		}

		if (player.entity->frame < startFrame)
			player.entity->frame = startFrame;
		else if (player.entity->frame == endFrame) {

		}
		else {
			player.entity->frame += 0.25;
		}

		if (player.entity->statePos > 60)
			player_change_state(PLAYER_IDLE);
		break;

	default:
		player_change_state(PLAYER_IDLE);
	}
}

void player_update() {
	const Uint8* keys;
	keys = SDL_GetKeyboardState(NULL); // get the keyboard state for this frame
	player_input(keys);

	player.entity->realPosition = entity_real_position(player.entity);
	gfc_rect_set(player.entity->hurtbox, player.entity->drawPosition.x, player.entity->drawPosition.y, player.entity->sprite->frame_w, player.entity->sprite->frame_h);
}

void player_hurt(Entity* self, int damage) {
	player_change_state(PLAYER_HURT);
	player.entity->health -= damage;
}