#include "simple_logger.h"
#include "entity.h"

typedef struct{
	Entity *entity_list;
	Uint32 max_entities;
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

void entity_manager_free(){
	if (entity_manager.entity_list != NULL){
		free(entity_manager.entity_list);
	}
	memset(&entity_manager, 0, sizeof(EntityManager));
	slog("entity system closed");
}

Entity *entity_new(){
	if (entity_manager.entity_list == NULL){
		slog("entity system does not exist");
		return NULL;
	}
	for (int i=0; i < entity_manager.max_entities; i++){
		if (entity_manager.entity_list[i]._inuse) 
			continue;
		memset(&entity_manager.entity_list[i], 0, sizeof(Entity));
		entity_manager.entity_list[i]._inuse = 1;
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
		gf2d_sprite_draw(
			ent->sprite,
			ent->position,
			&ent->scale,
			NULL,
			&ent->rotation,
			&ent->flip,
			NULL,
			ent->frame);
	}
}