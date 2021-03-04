#include "simple_logger.h"
#include "player.h"
#include "input.h"
#include "camera.h"

enum player_state {PLAYER_IDLE, PLAYER_HURT, PLAYER_WALK, PLAYER_PUNCH, PLAYER_KICK, PLAYER_QCFP, PLAYER_QCFK, PLAYER_BFP, PLAYER_BFK, PLAYER_PK};

void player_update(Entity* self);
void player_hurt(Entity* self);

static Entity* player = { 0 };

void player_spawn(Vector2D position) {
	PlayerData* data;

	if (player != NULL)
		entity_free(player);

	player = entity_new(PLAYER_TYPE);
	if (!player) {
		slog("failed to create entity for player");
		return;
	}

	data = malloc(sizeof(PlayerData));
	if (!data) {
		slog("failed to create data for player");
		return;
	}
	memset(data, 0, sizeof(PlayerData));
	player->data = data;

	player->sprite = gf2d_sprite_load_all("images/kyo_2.png", 128, 144, 13);
	gfc_rect_set(player->hurtbox, player->drawPosition.x, player->drawPosition.y, player->sprite->frame_w, player->sprite->frame_h);
	vector2d_copy(player->drawPosition, position);
	player->maxHealth = 100;
	player->health = player->maxHealth;
	player->rotation = vector3d(0,0,0);
	player->colorShift = vector4d(255, 255, 255, 255);
	player->update = player_update;
	player->hurt = player_hurt;
	player->baseDamage = 10;
	player->defaultSpeed = 4;
	player->speed = player->defaultSpeed;
	player->flip = vector2d(FACE_RIGHT, 0);
	player->scale = vector2d(2, 2);
	player->attackHit = 0;
}

Vector2D player_get_position() {
	Vector2D realPosition;

	realPosition = entity_real_position(player);

	if (player->side == FACE_LEFT)
		realPosition.x -= player->sprite->frame_w / 2;

	return realPosition;
}

int player_get_health() {
	return player->health;
}

int player_get_max_health() {
	return player->maxHealth;
}

void player_attach_weapon(Entity* ent) {
	if (ent->type != PICKUP_TYPE_KNIFE && ent->type != PICKUP_TYPE_CROWBAR) {
		slog("Attempted to attatch non-weapon to player");
		return;
	}
	else if (!ent->_inuse) {
		slog("Attempted to attatch entity not in use to player");
return;
	}

	if (((PlayerData*)player->data)->weapon != NULL)
		free(((PlayerData*)player->data)->weapon);

	((PlayerData*)player->data)->weapon = ent;
}

void player_power_up(int frameTime) {
	((PlayerData*)player->data)->powerUpTime = frameTime;
	player->colorShift = vector4d(255, 100, 100, 255);
	player->baseDamage = 20;
}

Vector2D player_get_weapon_position() {
	return player_get_position();
}

int player_collison_check(SDL_Rect rect) {
	if (SDL_HasIntersection(&rect, &player->hurtbox))
		return 1;
	return 0;
}

void player_change_health(int amount) {
	player->health += amount;
}

void player_free() {
	//if player struct has any pointers free them
	if (player->data != NULL)
		free(player->data);

	if (player->_inuse)
		entity_free(player);

	memset(&player, 0, sizeof(Entity));
}


//TODO: REWORDK CAMERA SET TO CENTER ON PLAYER THEN CHECK BOUNDS
void player_movement(const Uint8* keys) {
	int up, down, left, right;
	Vector2D setPosition;

	up = keys[SDL_SCANCODE_W];
	right = keys[SDL_SCANCODE_D];
	down = keys[SDL_SCANCODE_S];
	left = keys[SDL_SCANCODE_A];

	//TODO: Redo boundary check
	if (left) {
		player->drawPosition.x -= player->speed;
		//if (player->drawPosition.x < 0)
		//	player->drawPosition.x = 0;

	}
	else if (right) {
		player->drawPosition.x += player->speed;
		//if (player->drawPosition.x > 1200 - 2 * player->sprite->frame_w)
		//	player->drawPosition.x = 1200 - 2 * player->sprite->frame_w;

	}
	if (up) {
		player->drawPosition.y -= player->speed;
		//if (player->drawPosition.y < 0 + player->sprite->frame_h / 2)
		//	player->drawPosition.y = 0 + player->sprite->frame_h / 2;

	}
	else if (down) {
		player->drawPosition.y += player->speed;
		//if (player->drawPosition.y > 1200 - 5 * player->sprite->frame_h)
		//	player->drawPosition.y = 1200 - 5 * player->sprite->frame_h;

	}

	setPosition.x = player->drawPosition.x - 1200/2;
	setPosition.y = player->drawPosition.y - 720 / 2;
	camera_set_position(setPosition);

	slog("Player Position %f %f", player->drawPosition.x, player->drawPosition.y);
	slog("Camera Position %f %f", camera_get_position().x, camera_get_position().y);
}

void player_update_side(const Uint8* keys) {
	int left, right;

	left = keys[SDL_SCANCODE_A];
	right = keys[SDL_SCANCODE_D];

	if (left && !right) {
		player->side = FACE_LEFT;
		player->flip.x = FACE_RIGHT;
	}
	else if (right && !left) {
		player->side = FACE_RIGHT;
		player->flip.x = FACE_LEFT;
	}
}

void player_update_hurtbox() {
	if (player->side == FACE_LEFT) {
		gfc_rect_set(player->hurtbox, player->drawPosition.x - player->sprite->frame_w, player->drawPosition.y, player->sprite->frame_w, player->sprite->frame_h);
	}
	else if (player->side == FACE_RIGHT){
		gfc_rect_set(player->hurtbox, player->drawPosition.x, player->drawPosition.y, player->sprite->frame_w, player->sprite->frame_h);
	}
}

void player_change_state(enum player_state state) {
	player->frame = 0;
	player->statePos = 0;
	player->attackHit = 0;
	player->state = state;
}

void player_attack_check(SDL_Rect hitbox, int attackPower, int weaponDegradation) {
	if (player->attackHit == 0) {
		if (entity_manager_player_attack_collison(hitbox, attackPower)) {
			player->attackHit = 1;

			if (((PlayerData*)player->data)->weapon)
				((PlayerData*)player->data)->weapon->health -= weaponDegradation;
		}
	}
}


void player_input(const Uint8* keys) {
	enum player_directional_input raw_input = NO_INPUT;
	enum player_move move = 0;
	int up, down, left, right;
	int punch, kick;
	int startFrame, endFrame;
	int attackPower;
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
	
	switch (player->state)
	{
	case PLAYER_IDLE:
		startFrame = 0;
		endFrame = 9;
		player->frame += 0.05;

		if (player->frame > endFrame || player->frame < startFrame)
			player->frame = startFrame;

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

		player->frame += 0.20;
		if (player->frame > endFrame || player->frame < startFrame)
			player->frame = startFrame;

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
		startFrame = 64;
		endFrame = 64;
		player->statePos++;

		player->frame = startFrame;

		if (player->statePos > 2) {
			player_change_state(PLAYER_IDLE);
		}

		break;

	case PLAYER_PUNCH:
		startFrame = 23;
		endFrame = 23;
		attackPower = player->baseDamage + 10;
		player->statePos++;

		gfc_rect_set(hitbox, player->drawPosition.x + player->sprite->frame_w, player->drawPosition.y, player->sprite->frame_w, player->sprite->frame_h);
		if (player->side == FACE_LEFT) {
			hitbox.x -= 2 * player->sprite->frame_w;
		}

		if (((PlayerData*)player->data)->weapon)
			attackPower += 10;

		player_attack_check(hitbox, attackPower, 20);

		if (player->frame < startFrame)
			player->frame = startFrame;

		if (player->statePos > 12)
			player_change_state(PLAYER_IDLE);
		else if (player->statePos < 8 && kick) {
			player_change_state(PLAYER_PK);
		}

		break;

	case PLAYER_KICK:
		startFrame = 18;
		endFrame = 22;
		attackPower = player->baseDamage + 10;
		player->statePos++;

		gfc_rect_set(hitbox, player->drawPosition.x + player->sprite->frame_w, player->drawPosition.y, player->sprite->frame_w, player->sprite->frame_h);
		if (player->side == FACE_LEFT) {
			hitbox.x -= 2 * player->sprite->frame_w;
		}

		player_attack_check(hitbox, attackPower, 0);

		if (player->frame < startFrame)
			player->frame = startFrame;

		else if (player->frame == endFrame) {
			
		}
		else {
			player->frame++;
		}

		if (player->statePos > 15)
			player_change_state(PLAYER_IDLE);
		else if (player->statePos < 8 && punch) {
			player_change_state(PLAYER_PK);
		}

		break;

	case PLAYER_QCFP:
		startFrame = 14;
		endFrame = 16;
		attackPower = player->baseDamage + 21;

		player->statePos++;

		gfc_rect_set(hitbox, player->drawPosition.x + player->sprite->frame_w, player->drawPosition.y, player->sprite->frame_w, player->sprite->frame_h);
		if (player->side == FACE_LEFT) {
			hitbox.x -= 2 * player->sprite->frame_w;
		}

		if (((PlayerData*)player->data)->weapon)
			attackPower += 10;
		player_attack_check(hitbox, attackPower, 30);

		if (player->frame < startFrame)
			player->frame = startFrame;
		else if (player->frame == endFrame) {

		}
		else {
			player->frame++;
		}

		if (player->statePos > 45)
			player_change_state(PLAYER_IDLE);

		break;

	case PLAYER_QCFK:
		startFrame = 30;
		endFrame = 39;
		attackPower = player->baseDamage + 22;

		player->statePos++;

		gfc_rect_set(hitbox, player->drawPosition.x + player->sprite->frame_w, player->drawPosition.y, player->sprite->frame_w, player->sprite->frame_h);
		if (player->side == FACE_LEFT) {
			hitbox.x -= 2 * player->sprite->frame_w;
		}

		player_attack_check(hitbox, attackPower, 0);

		if (player->frame < startFrame)
			player->frame = startFrame;
		else if (player->frame == endFrame) {

		}
		else {
			player->frame+=0.25;
		}

		if (player->statePos > 45)
			player_change_state(PLAYER_IDLE);

		break;

	case PLAYER_BFP:
		startFrame = 41;
		endFrame = 47;
		attackPower = player->baseDamage + 21;

		player->statePos++;

		gfc_rect_set(hitbox, player->drawPosition.x + player->sprite->frame_w, player->drawPosition.y, player->sprite->frame_w, player->sprite->frame_h);
		if (player->side == FACE_LEFT) {
			hitbox.x -= 2 * player->sprite->frame_w;
		}

		if (((PlayerData*)player->data)->weapon)
			attackPower += 10;

		player_attack_check(hitbox, attackPower, 10);

		if (player->frame < startFrame)
			player->frame = startFrame;
		else if (player->frame == endFrame) {

		}
		else {
			player->frame+= 0.5;
		}

		if (player->statePos > 45)
			player_change_state(PLAYER_IDLE);

		break;

	case PLAYER_BFK:
		startFrame = 24;
		endFrame = 29;
		attackPower = player->baseDamage + 21;

		player->statePos++;

		gfc_rect_set(hitbox, player->drawPosition.x + player->sprite->frame_w, player->drawPosition.y, player->sprite->frame_w, player->sprite->frame_h);
		if (player->side == FACE_LEFT) {
			hitbox.x -= 2 * player->sprite->frame_w;
		}

		player_attack_check(hitbox, attackPower, 0);

		if (player->frame < startFrame)
			player->frame = startFrame;
		else if (player->frame == endFrame) {

		}
		else {
			player->frame += 0.5;
		}

		if (player->statePos > 45)
			player_change_state(PLAYER_IDLE);

		break;

	case PLAYER_PK:
		startFrame = 48;
		endFrame = 63;
		attackPower = player->baseDamage + 50;

		player->statePos++;

		gfc_rect_set(hitbox, player->drawPosition.x + player->sprite->frame_w, player->drawPosition.y, player->sprite->frame_w, player->sprite->frame_h);
		if (player->side == FACE_LEFT) {
			hitbox.x -= 2 * player->sprite->frame_w;
		}

		player_attack_check(hitbox, attackPower, 0);

		if (player->frame < startFrame)
			player->frame = startFrame;
		else if (player->frame == endFrame) {

		}
		else {
			player->frame += 0.25;
		}

		if (player->statePos > 60)
			player_change_state(PLAYER_IDLE);
		break;

	default:
		player_change_state(PLAYER_IDLE);
	}
}

void player_update() {
	const Uint8* keys;

	if (((PlayerData *)player->data)->powerUpTime > 0) {
		((PlayerData*)player->data)->powerUpTime--;
		if (((PlayerData*)player->data)->powerUpTime <= 0) {
			player->colorShift = vector4d(255, 255, 255, 255);
			player->speed = player->defaultSpeed;
			player->maxHealth;
			player->baseDamage = 10;
		}
	}

	if (((PlayerData*)player->data)->weapon)
		if (!((PlayerData*)player->data)->weapon->_inuse)
			((PlayerData*)player->data)->weapon = NULL;

	keys = SDL_GetKeyboardState(NULL); // get the keyboard state for this frame
	player_input(keys);

	player->realPosition = entity_real_position(player);
	player_update_hurtbox();
}

void player_hurt(Entity* self, int damage) {
	player_change_state(PLAYER_HURT);
	player->health -= damage;
}