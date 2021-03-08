#include <SDL.h>

#include "simple_logger.h"
#include "gf2d_graphics.h"
#include "gf2d_sprite.h"
#include "gf2d_draw.h"

#include "director.h"
#include "hud.h"
#include "player.h"
#include "enemy.h"
#include "pickup.h"
#include "input.h"
#include "camera.h"
#include "level.h"
#include "simple_json.h"

GameVarsStruct game_vars = { 0 };
const Uint8* KEYS; 
int QUIT_FLAG;
const char* SCORE_FILE_NAME = "score.json";

void director_add_score(int amount) {
    game_vars.score += amount;
}

int director_get_score() {
    return game_vars.score;
}

int director_get_high_score() {
    return game_vars.highScore;
}

void save_score(char* fileName) {
    SJson* root;
    SJson* j_int;

    j_int = sj_new_int(game_vars.highScore);
    root = sj_object_new();

    sj_object_insert(root, "High Score", j_int);

    sj_save(root, fileName);

    sj_object_free(j_int);
    sj_object_free(root);
}

int load_score(char* fileName) {
    SJson* json, *scoreJson;
    int score;

    score = 0;
    json = sj_load(fileName);

    if (!json)return 0;

    scoreJson = sj_object_get_value(json, "High Score");
    if (!scoreJson)return 0;

    sj_get_integer_value(scoreJson, &score);

    return score;
}

void director_init_game() {
    QUIT_FLAG = 0;
    game_vars.score = 0;
    game_vars.highScore = load_score(SCORE_FILE_NAME);

    slog("Loaded High Score: %d", game_vars.highScore);

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

    game_vars.currentLevel = level_load("levels/exampleLevel.json");
    player_spawn(vector2d(600, 360));
    enemy_spawn(vector2d(1500, 200), ENEMY_TYPE_1);
   //enemy_spawn(vector2d(600, 200), ENEMY_TYPE_2);
   //enemy_spawn(vector2d(600, 200), ENEMY_TYPE_3);
   //enemy_spawn(vector2d(600, 200), ENEMY_TYPE_4);
   //enemy_spawn(vector2d(600, 200), ENEMY_TYPE_5);

   pickup_spawn(vector2d(300, 160), INTERACTABLE_BOX);
}

int director_run_game() {
    SDL_PumpEvents();   // update SDL's internal event structures
    KEYS = SDL_GetKeyboardState(NULL); // get the keyboard state for this frame
    if (KEYS[SDL_SCANCODE_ESCAPE])QUIT_FLAG = 1; // exit condition

    entity_manager_update_entities();
    entity_manager_think_entities();
    level_update(game_vars.currentLevel);

    gf2d_graphics_clear_screen();// clears drawing buffers
    // all drawing should happen betweem clear_screen and next_frame
        //backgrounds drawn first
    level_draw(game_vars.currentLevel);
    // gf2d_sprite_draw_image(sprite, vector2d(0, 0));
    entity_manager_draw_entities();
    hud_draw();

    gf2d_grahics_next_frame();// render current draw frame and skip to the next frame

//        slog("Rendering at %f FPS",gf2d_graphics_get_frames_per_second());

    if (game_vars.score > game_vars.highScore)
        game_vars.highScore = game_vars.score;
    
    return QUIT_FLAG;
}

void director_end_game() {
    save_score(SCORE_FILE_NAME);

    entity_manager_free();
    input_buffer_free();

    slog("---==== END ====---");
}