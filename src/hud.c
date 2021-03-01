#include "simple_logger.h" 
#include "hud.h"
#include "font.h"
#include "player.h"

void hud_draw() {
	Font* font;
	SDL_Rect rect;
	TextLine healthText;

	font = font_load("fonts/colony_wars.ttf", 16);
	rect = gfc_sdl_rect(0,0, 720/2 + 20, 80);
	gfc_line_sprintf(healthText, "HEALTH: %d/%d", player_get_health(), player_get_max_health());

	//draw rectangle
	SDL_RenderDrawRect(gf2d_graphics_get_renderer(), &rect);
	SDL_RenderFillRect(gf2d_graphics_get_renderer(), &rect);

	font_render(font, healthText, vector2d(32, 32), gfc_color8(255, 0, 0, 255));
}