#include "simple_logger.h"
#include "entity.h"
#include "camera.h"

typedef struct{
	Entity *entity_list;
	Uint32 max_entities;
	int population[NUM_OF_ENTITY_TYPES];
}EntityManager;

static EntityManager entity_manager = {0};

void entity_manager_init(Uint32 max_entities){
	if (max_entities == 0){
		slog("cannot allocate 0 entities!");
		return;
	}
	if (entity_manager.entity_list != NULL){
		entity_manager_free();
	}
	entity_manager.entity_list = (Entity *)gfc_allocate_array(sizeof(Entity), max_entities);
	if (entity_manager.entity_list == NULL){
		slog("Failed to allocate entity list");
		return;
	}
	entity_manager.max_entities = max_entities;
	atexit(entity_manager_free);
	slog("entity system initialized");
}

void entity_manager_reset_all() {
	if (entity_manager.entity_list == NULL) {
		slog("entity system does not exist");
		return;
	}

	for (int i = 0; i < entity_manager.max_entities; i++) {
		if (entity_manager.entity_list[i]._inuse)
			entity_free(&entity_manager.entity_list[i]);
	}
}

void entity_manager_free(){
	if (entity_manager.entity_list != NULL){
		free(entity_manager.entity_list);
	}
	memset(&entity_manager, 0, sizeof(EntityManager));
	slog("entity system closed");
}

Entity *entity_new(enum entity_type type){
	if (entity_manager.entity_list == NULL){
		slog("entity system does not exist");
		return NULL;
	}
	for (int i=0; i < entity_manager.max_entities; i++){
		if (entity_manager.entity_list[i]._inuse) 
			continue;
		memset(&entity_manager.entity_list[i], 0, sizeof(Entity));
		entity_manager.entity_list[i].colorShift = vector4d(255, 255, 255, 255);
		entity_manager.entity_list[i]._inuse = 1;
		entity_manager.entity_list[i].type = type;

		entity_manager.population[type]++;

		return &entity_manager.entity_list[i];
	}
	slog("No free entities available");
	return NULL;
}

void entity_free(Entity* ent) {
	if (!ent) {
		slog("cannot free a NULL entity");
		return;
	}
	
	if (ent->free) {
		ent->free(ent);
	}
	else {
		gf2d_sprite_free(ent->sprite);
		ent->sprite = NULL;
		ent->_inuse = 0;
	}

	entity_manager.population[ent->type]--;
}

Vector2D entity_real_position(Entity* ent) {
	int realX, realY;

	realX = ent->drawPosition.x + (ent->sprite->frame_w * ent->scale.x) / 2;
	realY = ent->drawPosition.y + (ent->sprite->frame_h * ent->scale.y) / 2;

	return vector2d(realX, realY);
}

int entity_get_population(enum entity_type type) {
	if (type > NUM_OF_ENTITY_TYPES || type < 0)
		return -1;

	return entity_manager.population[type];
}

int entity_get_enemy_population() {
	int sum = 0;
	for (int i = ENEMY_TYPE_1; i <= BOSS_TYPE_2; i++)
		sum += entity_get_population(i);

	return sum;
}

void entity_update(Entity* self) {
	if (!self) return;
	//do any generic update code

	if (self->update != NULL)
		self->update(self);
}

void entity_think(Entity* self) {
	if (!self) return;

	if (self->think != NULL)
		self->think(self);
}

void entity_draw(Entity* ent) {
	if (!ent) {
		slog("Cannot draw NULL entity");
		return;
	}
	if (ent->sprite == NULL) {
		return;
	}
	if (ent->draw)
		ent->draw(ent);
	else {
		Vector2D offset, drawPosition;

		offset = camera_get_offset();
		drawPosition.x = ent->drawPosition.x + offset.x;
		drawPosition.y = ent->drawPosition.y + offset.y;

		//do not render if outside screen bounds
		if (drawPosition.x < -80 || drawPosition.x > 1280 || drawPosition.y < -100 || drawPosition.y > 800) {
			return;
		}

		gf2d_sprite_draw(
			ent->sprite,
			drawPosition,
			&ent->scale,
			NULL,
			&ent->rotation,
			&ent->flip,
			&ent->colorShift,
			ent->frame);
	}
}

void entity_hurt(Entity* ent, int damage) {
	if (!ent) {
		slog("Cannot draw NULL entity");
		return;
	}
	if (ent->hurt != NULL) {
		ent->hurt(ent, damage);
	}
}

void entity_manager_update_entities() {
	int i;

	if (entity_manager.entity_list == NULL) {
		slog("entity system does not exist");
		return NULL;
	}

	for (i = 0; i < entity_manager.max_entities; i++) {
		if (entity_manager.entity_list[i]._inuse == 0) 
			continue;

		entity_update(&entity_manager.entity_list[i]);
	}
}

void entity_manager_think_entities() {
	int i;

	if (entity_manager.entity_list == NULL) {
		slog("entity system does not exist");
		return NULL;
	}

	for (i = 0; i < entity_manager.max_entities; i++) {
		if (entity_manager.entity_list[i]._inuse == 0)
			continue;

		entity_think(&entity_manager.entity_list[i]);
	}
}

void entity_manager_draw_entities() {
	int i;

	if (entity_manager.entity_list == NULL) {
		slog("entity system does not exist");
		return NULL;
	}

	for (i = 0; i < entity_manager.max_entities; i++) {
		if (entity_manager.entity_list[i]._inuse == 0)
			continue;

		entity_draw(&entity_manager.entity_list[i]);
	}
}

int entity_manager_player_attack_collison(SDL_Rect playerHitbox, int damage) {
	int count = 0;

	if (entity_manager.entity_list == NULL) {
		slog("entity system does not exist");
		return NULL;
	}

	for (int i = 0; i < entity_manager.max_entities; i++) {
		if (entity_manager.entity_list[i]._inuse == 0)
			continue;

		if (!entity_is_enemy(entity_manager.entity_list[i].type) && !entity_is_interactable(entity_manager.entity_list[i].type))
			continue;

		if (SDL_HasIntersection(&playerHitbox, &entity_manager.entity_list[i].hurtbox)) {
			entity_hurt(&entity_manager.entity_list[i], damage);
			count++;
		}
	}

	return count;
}

int entity_manager_player_interactable_collision(SDL_Rect hurtbox) {
	if (entity_manager.entity_list == NULL) {
		slog("entity system does not exist");
		return NULL;
	}

	for (int i = 0; i < entity_manager.max_entities; i++) {
		if (entity_manager.entity_list[i]._inuse == 0)
			continue;

		if (!entity_is_interactable(entity_manager.entity_list[i].type))
			continue;

		if (SDL_HasIntersection(&hurtbox, &entity_manager.entity_list[i].hurtbox)) {
			return true;
		}
	}

	return false;
}

int entity_is_player(enum entity_type type) {
	if (type == PLAYER_TYPE)
		return true;
	return false;
}

int entity_is_enemy(enum entity_type type) {
	if (type >= ENEMY_TYPE_1 && type <= BOSS_TYPE_2)
		return true;
	return false;
}

int entity_is_boss(enum entity_type type) {
	if (type >= BOSS_TYPE_1 && type <= BOSS_TYPE_2)
		return true;
	return false;
}

int entity_is_pickup(enum entity_type type) {
	if (type >= PICKUP_TYPE_MEDKIT && type <= PICKUP_TYPE_POWERUP)
		return true;
	return false;
}

int entity_is_weapon(enum entity_type type) {
	if (type >= PICKUP_TYPE_KNIFE && type <= PICKUP_TYPE_CROWBAR)
		return true;
	return false;
}

int entity_is_interactable(enum entity_type type) {
	if (type >= INTERACTABLE_BOX && type <= INTERACTABLE_TRASH_CAN)
		return true;
	return false;
}

int entity_is_object(enum entity_type type) {
	if (type >= PICKUP_TYPE_MEDKIT && type <= INTERACTABLE_TRASH_CAN)
		return true;
	return false;
}

void entity_debug_draw_hurtboxes() {
	for (int i = 0; i < entity_manager.max_entities; i++) {
		if (entity_manager.entity_list[i]._inuse == 0)
			continue;

		SDL_RenderDrawRect(gf2d_graphics_get_renderer(), &entity_manager.entity_list[i].hurtbox);
	}
}