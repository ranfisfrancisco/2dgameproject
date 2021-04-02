#include "fight.h"
#include "simple_logger.h"

LevelFightData* levelFightData_load(SJson* levelJson){
	LevelFightData* fightData;
	SJson *fightDataJson;

	if (!levelJson)return NULL;

	fightData = (Entity*)gfc_allocate_array(sizeof(LevelFightData), 1);
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

void levelFightData_free(LevelFightData* data) {
	if (data == NULL)
		return;

	sj_object_free(data->encounterList);
	free(data);
}