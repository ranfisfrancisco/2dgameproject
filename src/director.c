#include <SDL.h>

#include "simple_logger.h"
#include "gf2d_graphics.h"
#include "gf2d_sprite.h"
#include "gf2d_draw.h"

#include "director.h"
#include "hud.h"
#include "player.h"
#include "enemy.h"
#include "object.h"
#include "input.h"
#include "camera.h"
#include "level.h"
#include "fight.h"
#include "simple_json.h"

GameVarsStruct GAME_VARS = { 0 };
const Uint8* KEYS; 
int QUIT_FLAG;
const char* SCORE_FILE_NAME = "data/score.json";

void director_add_score(int amount) {
    GAME_VARS.score += amount;
    if (GAME_VARS.score > GAME_VARS.highScore)
        GAME_VARS.highScore = GAME_VARS.score;
}

int director_get_score() {
    return GAME_VARS.score;
}

int director_get_high_score() {
    return GAME_VARS.highScore;
}

Vector2D director_get_screen_size_vector() {
    return GAME_VARS.screenSize;
}

int director_get_screen_width() {
    return GAME_VARS.screenSize.x;
}

int director_get_screen_height() {
    return GAME_VARS.screenSize.y;
}

void director_spawn_entity(Vector2D position, enum entity_type type) {
    if (entity_is_player(type)) {
        player_spawn(position);
    }
    else if (entity_is_enemy(type)) {
        enemy_spawn(position, type);
    }
    else if (entity_is_object(type)) {
        object_spawn(position, type);
    }
}

/**
 * @brief Change the game state
 * @param state GameState to change the game to
 */
void director_change_state(GameState state) {
    switch (state) {
    case GAME_STATE_IN_LEVEL:
        break;
    case GAME_STATE_LEVEL_TRANSITION:
     
        break;
    default:
        slog("Attempted transition to illegal state");
        break;
    }

    GAME_VARS.gameState = state;
    GAME_VARS.gameStateStartTime = clock();
}

void director_save_score(char* fileName) {
    SJson* root;
    SJson* j_int;

    j_int = sj_new_int(GAME_VARS.highScore);
    root = sj_object_new();

    sj_object_insert(root, "highScore", j_int);

    sj_save(root, fileName);

    sj_object_free(j_int);
    sj_object_free(root);
}

int director_load_score(char* fileName) {
    SJson* json, *scoreJson;
    int score;

    score = 0;
    json = sj_load(fileName);

    if (!json)return 0;

    scoreJson = sj_object_get_value(json, "highScore");
    if (!scoreJson) { 
        sj_object_free(json);
        return 0;
    }

    sj_get_integer_value(scoreJson, &score);

    sj_object_free(scoreJson);
    sj_object_free(json);

    return score;
}

void director_snap_camera() {
    if (GAME_VARS.currentLevel != NULL)
        level_update(GAME_VARS.currentLevel);
}

char* director_int_to_filename(int code) {
    switch (code) {
    case 1:
        return "levels/level1.json";
        break;
    case 2:
        return "levels/level2.json";
        break;
    case 3:
        return "levels/level3.json";
        break;
    default:
        slog("Failed to match code to level");
        return NULL;
    }
}

int director_set_level(int levelCode) {
    Level* oldLevel;
    char* filename;

    oldLevel = NULL;

    if (GAME_VARS.currentLevel != NULL) {
        oldLevel = GAME_VARS.currentLevel;
    }

    filename = director_int_to_filename(levelCode);
    if (!filename)
        return 0;

    GAME_VARS.currentLevel = level_load(filename);

    if (GAME_VARS.currentLevel == NULL) {
        slog("Failed to set level");
        GAME_VARS.currentLevel = oldLevel;
        return 0;
    }

    if (oldLevel)
        level_free(oldLevel);

    entity_manager_reset_all();
    director_spawn_entity(vector2d(100, 360), PLAYER_TYPE);
    GAME_VARS.currentLevelCode = levelCode;
    gfc_sound_play(GAME_VARS.currentLevel->bgMusic, -1, 0.5, -1, -1);
    slog("Loaded Level!");
    return 1;
}

void director_spawn_next_encounter() {
    SJson* row;
    int encounterCount, spawnCount;

    encounterCount = sj_array_get_count(GAME_VARS.currentLevel->fightData->encounterList);

    if (GAME_VARS.currentLevel->fightData->rowCounter >= encounterCount) {
        GAME_VARS.currentLevel->fightData->rowCounter = -1;
        return;
    }

    row = sj_array_get_nth(GAME_VARS.currentLevel->fightData->encounterList, GAME_VARS.currentLevel->fightData->rowCounter);
    spawnCount = sj_array_get_count(row);

    printf("SPAWNS %d\n", spawnCount);

    for (int i = 0; i < spawnCount; i++) {
        SJson* spawnItem;
        int entityType, xPos, yPos;

        spawnItem = sj_array_get_nth(row, i);

        sj_get_integer_value(sj_object_get_value(spawnItem, "entityType"), &entityType);
        sj_get_integer_value(sj_object_get_value(spawnItem, "xPos"), &xPos);
        sj_get_integer_value(sj_object_get_value(spawnItem, "yPos"), &yPos);

        director_spawn_entity(vector2d(xPos, yPos), entityType);

        sj_free(spawnItem);
    }

    GAME_VARS.currentLevel->fightData->rowCounter++;

    sj_free(row);
}

void director_init_game() {
    QUIT_FLAG = 0;
    GAME_VARS.screenSize = vector2d(1200, 720);
    GAME_VARS.score = 0;
    GAME_VARS.highScore = director_load_score(SCORE_FILE_NAME);

    slog("Loaded High Score: %d", GAME_VARS.highScore);

    init_logger("gf2d.log");
    slog("---==== BEGIN ====---");
    gf2d_graphics_initialize(
        "gf2d",
        GAME_VARS.screenSize.x,
        GAME_VARS.screenSize.y,
        GAME_VARS.screenSize.x,
        GAME_VARS.screenSize.y,
        vector4d(0, 0, 0, 255),
        0);
    gf2d_graphics_set_frame_delay(16);
    camera_set_dimensions(GAME_VARS.screenSize);
    camera_set_position(vector2d(0, 0));
    gf2d_sprite_init(1024);
    entity_manager_init(100);
    gfc_audio_init(30, 30, 30, 30, 1, 0);
    input_buffer_init();
    font_init(10);
    hud_init();
    SDL_ShowCursor(SDL_DISABLE);

    director_change_state(GAME_STATE_IN_LEVEL);
    director_set_level(2);

    /*director_spawn_entity(vector2d(300, 30), INTERACTABLE_TRASH_CAN);
    director_spawn_entity(vector2d(400, 160), INTERACTABLE_BOX);
    director_spawn_entity(vector2d(700, 80), INTERACTABLE_METAL_BOX);
    director_spawn_entity(vector2d(700, 160), INTERACTABLE_CAR);
    director_spawn_entity(vector2d(700, 600), INTERACTABLE_EXPLOSIVE);*/
}

int director_run_game() {
    if (GAME_VARS.currentLevel == NULL) {
        slog("Level not loaded! Aborting.");
        QUIT_FLAG = 1;
        return QUIT_FLAG;
    }

    SDL_PumpEvents();   // update SDL's internal event structures
    KEYS = SDL_GetKeyboardState(NULL); // get the keyboard state for this frame
    if (KEYS[SDL_SCANCODE_ESCAPE])QUIT_FLAG = 1; // exit condition
    GAME_VARS.gameStateTime = (double)(clock() - GAME_VARS.gameStateStartTime) / CLOCKS_PER_SEC;

    switch (GAME_VARS.gameState)
    {
    case GAME_STATE_IN_LEVEL:
        if (GAME_VARS.currentLevel->fightData->rowCounter != -1 && entity_get_enemy_population() == 0) {
            director_spawn_next_encounter();
        }
        else if (GAME_VARS.currentLevel->fightData->rowCounter == -1 && entity_get_enemy_population() == 0) {
            GAME_VARS.currentLevelCode++;
            director_change_state(GAME_STATE_LEVEL_TRANSITION);
        }

        entity_manager_update_entities();
        entity_manager_think_entities();
        level_update(GAME_VARS.currentLevel);

        gf2d_graphics_clear_screen();
        level_draw(GAME_VARS.currentLevel);
        entity_manager_draw_entities();
        entity_debug_draw_hurtboxes();
        hud_draw();

        gf2d_grahics_next_frame();// render current draw frame and skip to the next frame
        //slog("Rendering at %f FPS",gf2d_graphics_get_frames_per_second());

        break;

    case GAME_STATE_LEVEL_TRANSITION:
        if (GAME_VARS.gameStateTime > 2) {
            director_set_level(GAME_VARS.currentLevelCode);
            director_change_state(GAME_STATE_IN_LEVEL);

            if (GAME_VARS.currentLevel == NULL) {
                slog("Couldn't get the next level");
                QUIT_FLAG = 1;
                return QUIT_FLAG;
            }
        }

        gf2d_graphics_clear_screen();
        hud_draw_level_transition();
        gf2d_grahics_next_frame();

        break;

    default:
        slog("Unknown Engine State. Aborting");
        QUIT_FLAG = 1;
        return QUIT_FLAG;
    }
    return QUIT_FLAG;
}

void director_end_game() {
    director_save_score(SCORE_FILE_NAME);

    input_buffer_free();

    slog("---==== END ====---");
}