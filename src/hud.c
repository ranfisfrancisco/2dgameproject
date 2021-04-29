#include "simple_logger.h" 
#include "hud.h"
#include "director.h"
#include "font.h"
#include "player.h"

static SDL_Rect HUD_RECT;
static Font* HUD_FONT;
static Font* LEVEL_TRANSITION_FONT;

void hud_init() {
	HUD_RECT = gfc_sdl_rect(0, 0, 1270, 80);
	HUD_FONT = font_load("fonts/GameRobot.ttf", 48);
	LEVEL_TRANSITION_FONT = font_load("fonts/colony_wars.ttf", 20);
}

void hud_draw() {
	TextLine healthText;
	gfc_line_sprintf(healthText, "HEALTH: %d/%d ENEMY #: %d SCORE: %d", player_get_health(), player_get_max_health(), entity_get_enemy_population(), director_get_score());

	//draw rectangle
	SDL_RenderDrawRect(gf2d_graphics_get_renderer(), &HUD_RECT);
	SDL_RenderFillRect(gf2d_graphics_get_renderer(), &HUD_RECT);

	font_render(HUD_FONT, healthText, vector2d(32, 32), gfc_color8(255, 0, 0, 255));
}

void hud_draw_level_transition() {
	TextLine text;
	gfc_line_sprintf(text, "NEXT LEVEL!");

	//draw rectangle
	SDL_RenderDrawRect(gf2d_graphics_get_renderer(), &HUD_RECT);
	SDL_RenderFillRect(gf2d_graphics_get_renderer(), &HUD_RECT);

	font_render(LEVEL_TRANSITION_FONT, text, vector2d(1200/2 - 200, 720/2), gfc_color8(255, 0, 0, 255));
}

void hud_draw_beat_game() {
	TextLine text;
	gfc_line_sprintf(text, "CONGRATULATIONS!\n YOU BEAT THE GAME");

	//draw rectangle
	SDL_RenderDrawRect(gf2d_graphics_get_renderer(), &HUD_RECT);
	SDL_RenderFillRect(gf2d_graphics_get_renderer(), &HUD_RECT);

	font_render(LEVEL_TRANSITION_FONT, text, vector2d(1200 / 2 - 400, 720 / 2), gfc_color8(255, 0, 0, 255));
}