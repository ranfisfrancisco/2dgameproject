#ifndef __DIRECTOR_H__
#define __DIRECTOR_H__

#include "level.h"
#include "fight.h"

typedef struct {
	Level* currentLevel;
	int currentLevelCode;
	int score;
	int highScore;
} GameVarsStruct;

/**
 * @brief Adds amount to score
 * @param amount amount to increase score by
 */
void director_add_score(int amount);

/**
 * @brief Gets the global game score
 * @return The score
 */
int director_get_score();

/**
 * @brief Snaps the camera to the level bounds
 */
void director_snap_camera();

/**
 * @brief Initializes the game
 */
void director_init_game();

/**
 * @brief Run the next game frame
 * @return Returns 1 if game needs to exit, 0 otherwise
 */
int director_run_game();

/**
 * @brief Ends and exits the game
 */
void director_end_game();

#endif