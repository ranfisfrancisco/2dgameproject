#include <SDL.h>

#include "simple_logger.h"

#include "gf2d_graphics.h"
#include "gf2d_sprite.h"
#include "gf2d_draw.h"

#include "entity.h"
#include "player.h"
#include "enemy.h"
#include "input.h"
#include "camera.h"
#include "level.h"


int main(int argc, char* argv[])
{
    /*variable declarations*/
    int done = 0;
    const Uint8* keys;
    //Sprite* sprite;
    //Entity* mouse;
    Level* level;

    //Sprite* mouse;
    int mx, my;
    Vector4D mouseColor = { 255,100,255,200 };

    /*program initializtion*/
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
    SDL_ShowCursor(SDL_DISABLE);

    /*demo setup*/
    //sprite = gf2d_sprite_load_image("images/backgrounds/bg_flat.png");
    //mouse = entity_new();

    level = level_load("levels/exampleLevel.json");
    player_spawn(vector2d(600, 360));
    enemy_spawn(vector2d(600, 200));

    /*main game loop*/
    while (!done)
    {
        SDL_PumpEvents();   // update SDL's internal event structures
        keys = SDL_GetKeyboardState(NULL); // get the keyboard state for this frame
        if (keys[SDL_SCANCODE_ESCAPE])done = 1; // exit condition

        /*update things here*/
        SDL_GetMouseState(&mx, &my);
        
        entity_manager_update_entities();
        entity_manager_think_entities();
        level_update(level);

        gf2d_graphics_clear_screen();// clears drawing buffers
        // all drawing should happen betweem clear_screen and next_frame
            //backgrounds drawn first
        level_draw(level);
        // gf2d_sprite_draw_image(sprite, vector2d(0, 0));
        entity_manager_draw_entities();

        gf2d_grahics_next_frame();// render current draw frame and skip to the next frame

//        slog("Rendering at %f FPS",gf2d_graphics_get_frames_per_second());
    }
    entity_manager_free();
    input_buffer_free();
    slog("---==== END ====---");
    return 0;
}
/*eol@eof*/