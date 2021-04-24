#include <time.h> 
#include "editor.h"
#include "simple_logger.h" 
#include "entity.h"
#include "camera.h"
#include "player.h"
#include "enemy.h"
#include "object.h"
#include "level.h"

static Editor editor = { 0 };

void editor_update_movement(const Uint8* keys);
void editor_set_shadow(enum entity_type type);
void editor_place_entity();
void editor_end_encounter();
void editor_save();
Entity* editor_spawn_entity(Vector2D position, enum entity_type type);

Entity* editor_spawn_entity(Vector2D position, enum entity_type type) {
	if (entity_is_player(type)) {
		return player_allocate_entity(position);
	}
	else if (entity_is_enemy(type)) {
		return enemy_spawn(position, type);
	}
	else if (entity_is_object(type)) {
		return object_spawn(position, type);
	}
	return NULL;
}

void editor_set_shadow(enum entity_type type) {
	if (type >= NUM_OF_ENTITY_TYPES || type < 1)
		return;

	editor.currentType = type;

	if (editor.shadow) {
		if (editor.shadow->die) {
			editor.shadow->die(editor.shadow);
		}
		else {
			entity_free(editor.shadow);
		}
	}

	editor.shadow = editor_spawn_entity(editor.position, type);
}

void editor_init() {
	editor.fightData = levelFightData_new();
	editor_set_shadow(PLAYER_TYPE);
	editor.clock_start = clock();
}

void editor_update() {
	const Uint8* keys;

	editor.time_elapsed = (double)(clock() - editor.clock_start) / CLOCKS_PER_SEC;

	keys = SDL_GetKeyboardState(NULL);
	editor_update_movement(keys);

	if (editor.time_elapsed > 0.2) {
		if (keys[SDL_SCANCODE_UP]) {
			editor_set_shadow(editor.currentType + 1);
			editor.clock_start = clock();
		}
		else if (keys[SDL_SCANCODE_DOWN]) {
			editor_set_shadow(editor.currentType - 1);
			editor.clock_start = clock();
		}
		else if (keys[SDL_SCANCODE_E]) {
			editor_place_entity();
			editor.clock_start = clock();
		}
		else if (keys[SDL_SCANCODE_T]) {
			editor_end_encounter();
			editor.clock_start = clock();
		}
		else if (keys[SDL_SCANCODE_U]) {
			editor_save();
			editor.clock_start = clock();
		}
	}
}

void editor_update_movement(const Uint8* keys) {
	int up, down, left, right;
	Vector2D cameraNewPosition, newPosition;
	SDL_Rect testHurtbox;

	newPosition = editor.position;
	
	up = keys[SDL_SCANCODE_W];
	right = keys[SDL_SCANCODE_D];
	down = keys[SDL_SCANCODE_S];
	left = keys[SDL_SCANCODE_A];

	//TODO: Redo boundary check
	if (left) {
		newPosition.x -= 4;
		//if (player->drawPosition.x < 0)
		//	player->drawPosition.x = 0;

	}
	else if (right) {
		newPosition.x += 4;
	}
	if (up) {
		newPosition.y -= 4;
	}
	else if (down) {
		newPosition.y += 4;
	}

	editor.position = newPosition;
	if (editor.shadow != NULL) {
		editor.shadow->drawPosition = editor.position;
	}

	cameraNewPosition.x = editor.position.x - 1200 / 2;
	cameraNewPosition.y = editor.position.y - 720 / 2;
	camera_set_position(cameraNewPosition);
	director_snap_camera();
}


void editor_place_entity() {
	editor_spawn_entity(editor.position, editor.currentType);
	levelFightData_addSpawn(editor.fightData, editor.position.x, editor.position.y, editor.currentType);
}

void editor_end_encounter() {
	editor.fightData->rowCounter++;
}

void editor_save() {
	SJson* fileJson, *levelJS;

	fileJson = sj_load("levels/editorSkeleton.json");

	levelJS = sj_object_get_value(fileJson, "level");
	sj_object_insert(levelJS, "fightData", editor.fightData->encounterList);

	sj_save(fileJson, "levels/editor.json");
	sj_free(fileJson); // causes crash on next editor save because encounterList is destroyed as well!
}