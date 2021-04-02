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
}

int director_get_score() {
    return GAME_VARS.score;
}

int director_get_high_score() {
    return GAME_VARS.highScore;
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
        return "levels/demoLevel.json";
        break;
    case 2:
        return "levels/exampleLevel.json";
        break;
    case 3:
        return "levels/newLevel.json";
        break;
    default:
        slog("Failed to match code to level");
        return NULL;
    }
}

/*int director_load_fight_data(int levelCode) {
    LevelFightData* oldData;
    char* filename;

    oldData = NULL;

    if (GAME_VARS.currentLevel->fightData != NULL) {
        oldData = GAME_VARS.currentLevel->fightData;
    }

    filename = director_int_to_filename(levelCode);
    if (!filename)
        return 0;

    GAME_VARS.currentLevel->fightData = levelFightData_load(filename);
    if (GAME_VARS.currentLevel->fightData == NULL) {
        slog("Failed to load fight data");
        GAME_VARS.currentLevel->fightData = oldData;
        return 0;
    }

    if (oldData) {
        levelFightData_free(oldData);
    }

    return 1;
}*/

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

    if (oldLevel) {
        level_free(oldLevel);
    }

    entity_manager_reset_all();
    GAME_VARS.currentLevelCode = levelCode;
    slog("Loaded Level!");
    return 1;
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

void director_spawn_next_encounter() {
    int encounterCount, spawnCount;

    encounterCount = sj_array_get_count(GAME_VARS.currentLevel->fightData->encounterList);

    if (GAME_VARS.currentLevel->fightData->rowCounter >= encounterCount) {
        GAME_VARS.currentLevel->fightData->rowCounter = -1;
        return;
    }
}

void director_init_game() {
    QUIT_FLAG = 0;
    GAME_VARS.score = 0;
    GAME_VARS.highScore = director_load_score(SCORE_FILE_NAME);

    slog("Loaded High Score: %d", GAME_VARS.highScore);

    init_logger("gf2d.log");
    slog("---==== BEGIN ====---");
    gf2d_graphics_initialize(
        "gf2d",
        1200,
        720,
        1200,
        720,
        vector4d(0, 0, 0, 255),
        0);
    gf2d_graphics_set_frame_delay(16);
    camera_set_dimensions(vector2d(1200, 720));
    camera_set_position(vector2d(0, 0));
    gf2d_sprite_init(1024);
    entity_manager_init(100);
    input_buffer_init();
    font_init(10);
    hud_init();
    SDL_ShowCursor(SDL_DISABLE);

    director_set_level(3);
    
    director_spawn_entity(vector2d(100, 360), PLAYER_TYPE);

    director_spawn_entity(vector2d(300, 30), INTERACTABLE_TRASH_CAN);
    director_spawn_entity(vector2d(400, 160), INTERACTABLE_BOX);
    director_spawn_entity(vector2d(700, 80), INTERACTABLE_METAL_BOX);
    director_spawn_entity(vector2d(700, 160), INTERACTABLE_CAR);
    director_spawn_entity(vector2d(700, 600), INTERACTABLE_EXPLOSIVE);
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

    if (GAME_VARS.currentLevel->fightData->rowCounter != -1 && entity_get_enemy_population() == 0) {
        director_spawn_next_encounter();
    }
    else if (GAME_VARS.currentLevel->fightData->rowCounter == -1 && entity_get_enemy_population() == 0) {
        director_set_level(GAME_VARS.currentLevelCode + 1);

        if (GAME_VARS.currentLevel == NULL) {
            slog("Couldn't get the next level");
            QUIT_FLAG = 1;
        }
    }

    entity_manager_update_entities();
    entity_manager_think_entities();
    level_update(GAME_VARS.currentLevel);

    gf2d_graphics_clear_screen();// clears drawing buffers
    // all drawing should happen betweem clear_screen and next_frame
        //backgrounds drawn first
    level_draw(GAME_VARS.currentLevel);
    entity_manager_draw_entities();
    //entity_debug_draw_hurtboxes();
    hud_draw();

    gf2d_grahics_next_frame();// render current draw frame and skip to the next frame

//        slog("Rendering at %f FPS",gf2d_graphics_get_frames_per_second());

    if (GAME_VARS.score > GAME_VARS.highScore)
        GAME_VARS.highScore = GAME_VARS.score;
    
    return QUIT_FLAG;
}

void director_end_game() {
    director_save_score(SCORE_FILE_NAME);

    input_buffer_free();

    slog("---==== END ====---");
}