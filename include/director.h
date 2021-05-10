#ifndef __DIRECTOR_H__
#define __DIRECTOR_H__

#include <time.h> 
#include "level.h"
#include "fight.h"

typedef enum {
	GAME_STATE_ERROR, GAME_STATE_MENU, GAME_STATE_IN_LEVEL, GAME_STATE_BEAT_GAME, GAME_STATE_LEVEL_LOAD, GAME_STATE_EDITOR, GAME_STATE_EDITOR_LOAD, GAME_STATE_EXITING
} GameState;

typedef struct {
	GameState gameState;
	GameState gameStateBuffer;
	clock_t gameStateStartTime;
	double gameStateElapsedTime;
	Level* currentLevel;
	int currentLevelCode;
	int endGameCode;
	int score;
	int combo;
	clock_t comboLastHitStartTime;
	double comboLastHitElapsedTime;
	Vector2D screenSize;
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
 * @brief Increases combo counter due to landed hit
 * @param amount to increase combo counter by
 * @param hitstreak amount of hits landed without getting hit
 */
void director_add_combo_for_hit(int hits, int hitstreak);

/**
 * @brief Increases combo counter due to chain being performed
 * @param amount to increase combo counter by
 */
void director_add_combo_for_chain(int amount);

/**
 * @brief Snaps the camera to the level bounds
 */
void director_snap_camera();

/**
 * @brief Returns screen size as Vector2D
 */
Vector2D director_get_screen_size_vector();

/**
 * @brief Returns screen width as int
 */
int director_get_screen_width();

/**
 * @brief Returns screen height as int
 */
int director_get_screen_height();

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