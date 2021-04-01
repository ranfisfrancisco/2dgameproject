#include "fight.h"

LevelFightData* levelFightData_load(const char* filename) {
	LevelFightData* fightData = (Entity*)gfc_allocate_array(sizeof(LevelFightData), 1);
}

void levelFightData_free(LevelFightData* data) {
	if (data == NULL)
		return;

	sj_object_free(data->encounterList);
	free(data);
}