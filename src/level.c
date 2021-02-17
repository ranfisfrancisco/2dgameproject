#include <stdlib.h>

#include "level.h"

#include "simple_json.h"
#include "simple_logger.h"

Level* level_new() {
	Level* level;
	level = (Level*)malloc(sizeof(Level));
	if (!level) {
		slog("failed to allocate memory for the game level");
		return NULL;
	}
	memset(level, 0, sizeof(Level));
	return level;
}

Level *level_load(const char* filename) {
	Level* level;
	SJson* json, *levelJS;

	if (!filename) {
		slog("filename is NULL, cannot load the level");
		return NULL;
	}

	json = sj_load(filename);
	if (!json) return NULL;

	level = level_new();
	if (!level) {
		sj_free(json);
		return NULL;
	}
	
	levelJS = sj_object_get_value(json, "level");
	if (!levelJS) {
		slog("level json missing levle object");
		level_free(level);
		sj_free(json);
		return NULL;
	}

	//string = sj_get_string_value(sj_object_get_value(json));

	sj_free(json);
	return level;
}

void level_free(Level* level) {
	if (!level) return;

	if (level->tileMap != NULL) {
		free(level->tileSet);
		level->tileMap = NULL;
	}
	gf2d_sprite_free(level->bgimage);
	gf2d_sprite_free(level->tileSet);

	free(level);
}

void level_draw(Level* level) {
	int i;
	if (!level) {
		slog("cannot draw level, NULL pointer provided");
		return;
	}

	//draw background first
	gf2d_sprite_draw_image(level->bgimage, vector2d(0, 0));
	//then draw the tiles

	for (i = 0; i < level->levelWidth * i / level->levelHeight; i++) {
		if (level->tileMap[i] == 0)continue;
		gf2d_sprite_draw(
			level->tileSet,
			vector2d(i % level->levelWidth, i/level->levelHeight),
			NULL,
			NULL,
			NULL,
			NULL,
			NULL,
			level->tileMap[i] - 1);
	}
}