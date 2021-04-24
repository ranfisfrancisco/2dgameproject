#include "simple_logger.h"
#include "player.h"
#include "input.h"
#include "camera.h"
#include "director.h"

void player_update(Entity* self);
void player_hurt(Entity* self);
void player_set_hurtbox(SDL_Rect* hurtbox, Vector2D* drawPosition);
SDL_Rect player_generic_hitbox();

static Entity* player = { 0 };

Entity* player_allocate_entity(Vector2D position);

void player_spawn(Vector2D position) {
	if (player != NULL)
		entity_free(player);

	player = player_allocate_entity(position);
}

Entity* player_allocate_entity(Vector2D position) {
	Entity* ent;
	PlayerData* data;

	ent = entity_new(PLAYER_TYPE);
	if (!ent) {
		slog("failed to create entity for player");
		return NULL;
	}

	data = malloc(sizeof(PlayerData));
	if (!data) {
		slog("failed to create data for player");
		return NULL;
	}
	memset(data, 0, sizeof(PlayerData));
	ent->data = data;
	data->attackSound = gfc_sound_load("sounds/punch1.mp3", -1, -1);
	data->hurtSound = gfc_sound_load("sounds/mc_hurt.mp3", -1, -1);

	ent->sprite = gf2d_sprite_load_all("images/kyo_2.png", 128, 144, 13);
	vector2d_copy(ent->drawPosition, position);
	player_set_hurtbox(&ent->hurtbox, &ent->drawPosition);
	ent->maxHealth = 100;
	ent->health = ent->maxHealth;
	ent->rotation = vector3d(0,0,0);
	ent->defaultColorShift = vector4d(255, 255, 255, 255);
	ent->colorShift = ent->defaultColorShift;
	ent->update = player_update;
	ent->hurt = player_hurt;
	ent->baseDamage = 10;
	ent->defaultSpeed = 4;
	ent->speed = ent->defaultSpeed;
	ent->flip = vector2d(FACE_RIGHT, 0);
	ent->scale = vector2d(2, 2);
	ent->attackHit = 0;

	return ent;
}

Vector2D player_get_position() {
	Vector2D realPosition;

	realPosition = entity_real_position(player);

	if (player->side == FACE_LEFT)
		realPosition.x -= player->sprite->frame_w / 2;

	return realPosition;
}

int player_get_health() {
	if (player)
		return player->health;
	return 0;
}

int player_get_max_health() {
	if (player)
		return player->maxHealth;
	return 0;
}

void player_attach_weapon(Entity* ent) {
	slog("Starting Attachment");
	if (!entity_is_weapon(ent->type)) {
		slog("Attempted to attach non-weapon to player");
		return;
	}
	else if (!ent->_inuse) {
		slog("Attempted to attatch entity not in use to player");
		return;
	}
	slog("Checked for type and in use");

	if (((PlayerData*)player->data)->weapon != NULL)
		player_discard_weapon();


	((PlayerData*)player->data)->weapon = ent;
	((PlayerData*)player->data)->weapon->state = 1;

	slog("Done!");
}

void player_discard_weapon() {
	if (((PlayerData*)player->data)->weapon == NULL) {
		return;
	}

	entity_free(((PlayerData*)player->data)->weapon);
	((PlayerData*)player->data)->weapon = NULL;
}

void player_power_up(int frameTime) {
	((PlayerData*)player->data)->powerUpTime = frameTime;
	player->defaultColorShift = vector4d(255, 100, 100, 255);
	player->baseDamage = 20;
}

Vector2D player_get_weapon_position() {
	return player_get_position();
}

int player_collison_check(SDL_Rect rect) {
	if (!player) return 0;

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
	Vector2D cameraNewPosition, playerNewPosition;
	SDL_Rect testHurtbox;

	playerNewPosition = player->drawPosition;

	up = keys[SDL_SCANCODE_W];
	right = keys[SDL_SCANCODE_D];
	down = keys[SDL_SCANCODE_S];
	left = keys[SDL_SCANCODE_A];

	//TODO: Redo boundary check
	if (left) {
		playerNewPosition.x -= player->speed;
		//if (player->drawPosition.x < 0)
		//	player->drawPosition.x = 0;

	}
	else if (right) {
		playerNewPosition.x += player->speed;
		//if (player->drawPosition.x > 1200 - 2 * player->sprite->frame_w)
		//	player->drawPosition.x = 1200 - 2 * player->sprite->frame_w;

	}
	if (up) {
		playerNewPosition.y -= player->speed;
		//if (player->drawPosition.y < 0 + player->sprite->frame_h / 2)
		//	player->drawPosition.y = 0 + player->sprite->frame_h / 2;

	}
	else if (down) {
		playerNewPosition.y += player->speed;
		//if (player->drawPosition.y > 1200 - 5 * player->sprite->frame_h)
		//	player->drawPosition.y = 1200 - 5 * player->sprite->frame_h;
	}

	player_set_hurtbox(&testHurtbox, &playerNewPosition);

	//check hurtbox overlap with interactbles
	if (entity_manager_player_interactable_collision(testHurtbox))
		return;

	player->drawPosition = playerNewPosition;

	cameraNewPosition.x = player->drawPosition.x - 1200 / 2;
	cameraNewPosition.y = player->drawPosition.y - 720 / 2;
	camera_set_position(cameraNewPosition);
	director_snap_camera();
}

void player_update_side(const Uint8* keys) {
	int left, right;
	left = keys[SDL_SCANCODE_A];
	right = keys[SDL_SCANCODE_D];

	if (left && !right) {
		if (player->side == FACE_RIGHT) {
			player->side = FACE_LEFT;
			player->flip.x = FACE_RIGHT;
			player->drawPosition.x += player->sprite->frame_w;
		}
	}
	else if (right && !left) {
		if (player->side == FACE_LEFT) {
			player->side = FACE_RIGHT;
			player->flip.x = FACE_LEFT;
			player->drawPosition.x -= player->sprite->frame_w;
		}
	}
}

void player_set_hurtbox(SDL_Rect* hurtbox, Vector2D* drawPosition) {
	if (!player)
		return;

	Vector2D cameraOffset;

	cameraOffset = camera_get_offset();

	if (player->side == FACE_LEFT) {
		gfc_rect_set((*hurtbox), drawPosition->x + cameraOffset.x, drawPosition->y + cameraOffset.y, player->sprite->frame_w, player->sprite->frame_h * 1.3);
	}
	else if (player->side == FACE_RIGHT){
		gfc_rect_set((*hurtbox), drawPosition->x + player->sprite->frame_w + cameraOffset.x, drawPosition->y + cameraOffset.y, player->sprite->frame_w, player->sprite->frame_h * 1.3);
	}
}

SDL_Rect player_generic_hitbox() {
	SDL_Rect hitbox;

	hitbox = player->hurtbox;

	if (player->side == FACE_LEFT) {
		hitbox.x -= player->sprite->frame_w * 0.5;
	}
	hitbox.w *= 1.5;
	return hitbox;
}

int player_is_attack_state(enum player_state state) {
	return (state >= PLAYER_PUNCH && state <= PLAYER_PK);
}

void player_change_state(enum player_state state) {
	player->frame = 0;
	player->statePos = 0;
	player->attackHit = 0;
	player->state = state;
	player->colorShift = player->defaultColorShift;
}

void player_attack_check(SDL_Rect hitbox, int attackPower, int weaponDegradation) {
	if (player->attackHit == 0) {
		if (entity_manager_player_attack_collison(hitbox, attackPower)) {
			player->attackHit = 1;
			gfc_sound_play(((PlayerData*)player->data)->attackSound, 0, 1, -1, -1);

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

		hitbox = player_generic_hitbox();

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

		hitbox = player_generic_hitbox();

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

		hitbox = player_generic_hitbox();

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

		hitbox = player_generic_hitbox();

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

		hitbox = player_generic_hitbox();

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

		hitbox = player_generic_hitbox();

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

		hitbox = player_generic_hitbox();

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
	if (player == NULL)
		return;

	const Uint8* keys;

	if (((PlayerData *)player->data)->powerUpTime > 0) {
		((PlayerData*)player->data)->powerUpTime--;
		if (((PlayerData*)player->data)->powerUpTime <= 0) {
			player->defaultColorShift = vector4d(255, 255, 255, 255);
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
	player_set_hurtbox(&player->hurtbox, &player->drawPosition);
}

void player_hurt(Entity* self, int damage) {
	player_change_state(PLAYER_HURT);
	player->health -= damage;
	gfc_sound_play(((PlayerData*)player->data)->hurtSound, 0, 1, -1, -1);
}
