#ifndef __FIGHT_H__
#define __FIGHT_H__

#include "gfc_vector.h"
#include "simple_json.h"
#include "entity.h"

/*Structure defining data needed to spawn a single entity*/
typedef struct SpawnData_s {
	enum entity_type entityType;
	Vector2D position;
} SpawnData;

/*Structure containing list of SpawnData to be used at once at that stage of the level.*/
typedef struct EncounterData_s {
	SJson* spawnList;
	int nextTrigger;
} EncounterData;

/*Structure containing all the encounters to be used in a level*/
typedef struct LevelFightData_s {
	SJson* encounterList;
} LevelFightData;

/**
 * @brief load combat data from a json file
 * @param filename the file to load
 * @return NULL on error (See the logs) a loaded level otherwise
 */
LevelFightData* levelFightData_load(const char* filename);

/**
 * @brief Free the given level fight data
 * @param data The data to be freed
 */
void levelFightData_free(LevelFightData* data);
#endif