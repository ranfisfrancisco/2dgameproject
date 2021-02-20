#include "simple_logger.h"
#include "enemy.h"
#include "player.h"

void enemy_think();
void enemy_hurt();

Entity *enemy_spawn(Vector2D position) {
	Entity* ent;

	ent = entity_new();
	if (!ent) {
		slog("failed to create entity for player");
		return NULL;
	}
	ent->sprite = gf2d_sprite_load_all("images/cat.png", 64, 64, 10);
	vector2d_copy(ent->position, position);
	ent->frameRate = 0.1;
	ent->frameCount = 16;
	ent->maxHealth = 100;
	ent->health = ent->maxHealth;
	ent->type = ENEMY_TYPE;
	ent->rotation = vector3d(0, 0, 0);
	//ent->update = player_update;
	ent->think = enemy_think;
	ent->hurt = enemy_hurt;
	ent->speed = 2;
	ent->flip = vector2d(FACE_RIGHT, 0);
	ent->scale = vector2d(4, 4);

	return ent;
}

//TODO: Add randomness to movement
void enemy_think(struct Entity_s* self) {
	Vector2D playerPosition;
	float xchange, ychange;

	playerPosition = player_get_position();
	xchange = 1;
	ychange = 1;

	printf("%f %f \n", playerPosition.x - self->position.x, playerPosition.y - self->position.y);
	
	if (playerPosition.x - self->position.x > 0) {
		self->position.x += xchange;
	}
	else if (playerPosition.x - self->position.x < 0) {
		self->position.x -= xchange;
	}

	if (playerPosition.y - self->position.y > 0) {
		self->position.y += ychange;
	}
	else if (playerPosition.y - self->position.y < 0) {
		self->position.y -= ychange;
	}

}

void enemy_hurt() {

}