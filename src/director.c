#include <SDL.h>

#include "simple_logger.h"
#include "gf2d_graphics.h"
#include "gf2d_sprite.h"
#include "gf2d_draw.h"

#include "hud.h"
#include "player.h"
#include "enemy.h"
#include "pickup.h"
#include "input.h"
#include "camera.h"
#include "level.h"
#include "simple_json.h"

const char* scoreFileName = "score.json";

Level* CURRENT_LEVEL;
const Uint8* KEYS;
int QUIT_FLAG;
int SCORE;
int HIGH_SCORE;

void director_add_score(int amount) {
    SCORE += amount;
}

int director_get_score() {
    return SCORE;
}

int director_get_high_score() {
    return HIGH_SCORE;
}

void save_score() {
    SJson* root;
    SJson* j_int;

    j_int = sj_new_int(HIGH_SCORE);
    root = sj_object_new();

    sj_object_insert(root, "High Score", j_int);

    sj_save(root, "score.json");

    sj_object_free(j_int);
    sj_object_free(root);
}

int load_score() {
    SJson* json, *scoreJson;
    int score;

    score = 0;
    json = sj_load(scoreFileName);

    if (!json)return 0;

    scoreJson = sj_object_get_value(json, "High Score");
    if (!scoreJson)return 0;

    sj_get_integer_value(scoreJson, &score);

    return score;
}

void director_init_game() {
    QUIT_FLAG = 0;
    SCORE = 0;
    HIGH_SCORE = load_score();
    slog("Loaded High Score: %d", HIGH_SCORE);

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

    CURRENT_LEVEL = level_load("levels/exampleLevel.json");
    player_spawn(vector2d(600, 360));
    enemy_spawn(vector2d(1500, 200), ENEMY_TYPE_1);
   //enemy_spawn(vector2d(600, 200), ENEMY_TYPE_2);
   //enemy_spawn(vector2d(600, 200), ENEMY_TYPE_3);
   //enemy_spawn(vector2d(600, 200), ENEMY_TYPE_4);
   //enemy_spawn(vector2d(600, 200), ENEMY_TYPE_5);

   //TODO:
   pickup_spawn(vector2d(300, 160), INTERACTABLE_TRASH_CAN);
}

int director_run_game() {
    SDL_PumpEvents();   // update SDL's internal event structures
    KEYS = SDL_GetKeyboardState(NULL); // get the keyboard state for this frame
    if (KEYS[SDL_SCANCODE_ESCAPE])QUIT_FLAG = 1; // exit condition

    entity_manager_think_entities();
    entity_manager_update_entities();
    level_update(CURRENT_LEVEL);

    gf2d_graphics_clear_screen();// clears drawing buffers
    // all drawing should happen betweem clear_screen and next_frame
        //backgrounds drawn first
    level_draw(CURRENT_LEVEL);
    // gf2d_sprite_draw_image(sprite, vector2d(0, 0));
    entity_manager_draw_entities();
    hud_draw();

    gf2d_grahics_next_frame();// render current draw frame and skip to the next frame

//        slog("Rendering at %f FPS",gf2d_graphics_get_frames_per_second());

    if (SCORE > HIGH_SCORE)
        HIGH_SCORE = SCORE;
    
    return QUIT_FLAG;
}

void director_end_game() {
    save_score();

    entity_manager_free();
    input_buffer_free();
    slog("---==== END ====---");
}