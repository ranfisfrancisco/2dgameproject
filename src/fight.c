#include "fight.h"
#include "simple_logger.h"

LevelFightData* levelFightData_load(SJson* levelJson){
	LevelFightData* fightData;
	SJson *fightDataJson;

	if (!levelJson)return NULL;

	fightData = gfc_allocate_array(sizeof(LevelFightData), 1);
	if (fightData == NULL) {
		slog("Failed to allocate fight data");
		return NULL;
	}

	fightDataJson = sj_object_get_value(levelJson, "fightData");
	if (!fightDataJson)
	{
		slog("level json missing fightData object");
		levelFightData_free(fightData);
		return NULL;
	}

	fightData->encounterList = sj_copy(fightDataJson);
	fightData->rowCounter = 0;

	return fightData;
}

LevelFightData* levelFightData_new() {
	LevelFightData* data;

	data = calloc(1, sizeof(LevelFightData));
	if (!data)
		return NULL;

	data->encounterList = sj_array_new();

	return data;
}

void levelFightData_addSpawn(LevelFightData* data, int xpos, int ypos, EntityType type) {
	SJson* list, *spawnItem, *xJson, *yJson, *typeJson;

	list = sj_array_get_nth(data->encounterList, data->rowCounter);
	if (list == NULL) {
		list = sj_array_new();
		sj_array_append(data->encounterList, list);
	}

	spawnItem = sj_object_new();

	xJson = sj_new_int(xpos);
	yJson = sj_new_int(ypos);
	typeJson = sj_new_int(type);

	sj_object_insert(spawnItem, "xPos", xJson);
	sj_object_insert(spawnItem, "yPos", yJson);
	sj_object_insert(spawnItem, "entityType", typeJson);

	sj_array_append(list, spawnItem);
}

void levelFightData_free(LevelFightData* data) {
	if (data == NULL)
		return;

	sj_object_free(data->encounterList);
	free(data);
}