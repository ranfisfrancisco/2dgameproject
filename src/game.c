#include <SDL.h>

#include <math.h>

#include "simple_logger.h"

#include "gf2d_graphics.h"
#include "gf2d_sprite.h"

#include "entity.h"
#include "player.h"

int main(int argc, char* argv[])
{
    /*variable declarations*/
    int done = 0;
    const Uint8* keys;
    Sprite* sprite;
    Entity* mouse;
    Entity* player;
    Vector2D spawnPosition;

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
    gf2d_sprite_init(1024);
    entity_manager_init(100);
    SDL_ShowCursor(SDL_DISABLE);

    /*demo setup*/
    sprite = gf2d_sprite_load_image("images/backgrounds/bg_flat.png");
    //mouse = entity_new();

    spawnPosition = vector2d(0,0);
    player = player_spawn(spawnPosition);

    //mouse->sprite = gf2d_sprite_load_all("images/pointer.png", 32, 32, 16);
    //mouse->health = 100;


    /*main game loop*/
    while (!done)
    {
        SDL_PumpEvents();   // update SDL's internal event structures
        keys = SDL_GetKeyboardState(NULL); // get the keyboard state for this frame
        if (keys[SDL_SCANCODE_ESCAPE])done = 1; // exit condition
        player_input(player, keys);

        /*update things here*/
        SDL_GetMouseState(&mx, &my);

        /*mouse->position.x = mx;
        mouse->position.y = my;
        mouse->frame += 0.1;
        if (mouse->frame >= 16.0) mouse->frame = 0;*/
        
        entity_manager_update_entities();

        gf2d_graphics_clear_screen();// clears drawing buffers
        // all drawing should happen betweem clear_screen and next_frame
            //backgrounds drawn first
        gf2d_sprite_draw_image(sprite, vector2d(0, 0));
        entity_manager_draw_entities();

        gf2d_grahics_next_frame();// render current draw frame and skip to the next frame

//        slog("Rendering at %f FPS",gf2d_graphics_get_frames_per_second());
    }
    entity_manager_free();
    slog("---==== END ====---");
    return 0;
}
/*eol@eof*/