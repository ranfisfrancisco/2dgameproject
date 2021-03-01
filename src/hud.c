#include "simple_logger.h" 
#include "hud.h"
#include "font.h"
#include "player.h"

void hud_draw() {
	Font* font;
	TextLine healthText;

	font = font_load("fonts/colony_wars.ttf", 16);
	gfc_line_sprintf(healthText, "HEALTH: %d/%d", player_get_health(), player_get_max_health());

	font_render(font, healthText, vector2d(32, 32), gfc_color8(255, 0, 0, 255));
}