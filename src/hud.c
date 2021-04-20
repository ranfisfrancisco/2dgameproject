#include "simple_logger.h" 
#include "hud.h"
#include "director.h"
#include "font.h"
#include "player.h"

SDL_Rect rect;
Font* font;

void hud_init() {
	rect = gfc_sdl_rect(0, 0, 920, 80);
	font = font_load("fonts/colony_wars.ttf", 16);
}

void hud_draw() {
	TextLine healthText;
	gfc_line_sprintf(healthText, "HEALTH: %d/%d ENEMY #: %d SCORE: %d", player_get_health(), player_get_max_health(), entity_get_enemy_population(), director_get_score());

	//draw rectangle
	SDL_RenderDrawRect(gf2d_graphics_get_renderer(), &rect);
	SDL_RenderFillRect(gf2d_graphics_get_renderer(), &rect);

	font_render(font, healthText, vector2d(32, 32), gfc_color8(255, 0, 0, 255));
}

void hud_draw_level_transition() {
	TextLine text;
	gfc_line_sprintf(text, "NEXT LEVEL!");

	//draw rectangle
	SDL_RenderDrawRect(gf2d_graphics_get_renderer(), &rect);
	SDL_RenderFillRect(gf2d_graphics_get_renderer(), &rect);

	font_render(font, text, vector2d(1200/2, 720/2), gfc_color8(255, 0, 0, 255));
}