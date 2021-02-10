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
		entity_manager.entity_list[i]._inuse = 1;
		memset(&entity_manager.entity_list[i], 0, sizeof(Entity));
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
	gf2d_sprite_free(ent->sprite);
	ent->sprite = NULL;
	ent->_inuse = 0;
}

void entity_draw(Entity* ent) {
	if (!ent) {
		slog("Cannot draw NULL entity");
		return;
	}
	if (ent->sprite == NULL) {
		return;
	}
	gf2d_sprite_draw(
		ent->sprite,
		ent->position,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		ent->frame);
}