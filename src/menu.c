#include "simple_logger.h" 
#include "menu.h"
#include "font.h"
#include "director.h"

Font* MENU_FONT;
MenuState MENU_STATE;

void menu_init() {
	MENU_STATE = MENU_STATE_MAIN;
	MENU_FONT = font_load("fonts/DroidSans.ttf", 32);
	slog("Initialized Menu");
}

int menu_update() {
	const Uint8* keys;

	keys = SDL_GetKeyboardState(NULL); // get the keyboard state for this frame

	return -1;
}

void menu_draw() {
	char** main_menu_texts[2];
	main_menu_texts[0] = "one";
	main_menu_texts[1] = "two";
	int main_menu_len = 2;

	int screenWidth, screenHeight;
	char** texts;
	int textLen;
	TextLine text;
	SDL_Rect menuRect;

	screenWidth = director_get_screen_width();
	screenHeight = director_get_screen_height();

	texts = main_menu_texts;
	textLen = main_menu_len;

	for (int i = 0; i < textLen; i++) {
		int x, y;
		x = screenWidth / 2 - screenWidth / 6;
		y = 100 + 150 * i;

		menuRect = gfc_sdl_rect(x,  y, screenWidth / 3, screenHeight / 7);
		gfc_line_sprintf(text, texts[i]);

		SDL_RenderDrawRect(gf2d_graphics_get_renderer(), &menuRect);
		SDL_RenderFillRect(gf2d_graphics_get_renderer(), &menuRect);
		font_render(MENU_FONT, text, vector2d(x + screenWidth / 6 - 60, y + screenHeight/18), gfc_color8(255, 0, 0, 255));
	}
}