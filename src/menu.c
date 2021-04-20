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
	//char** main_menu_texts = { "wow",};
	int main_menu_len = 1;

	int screenWidth, screenHeight;
	char* texts;
	int textLen;
	TextLine text;
	SDL_Rect menuRect;

	screenWidth = director_get_screen_width();
	screenHeight = director_get_screen_height();
	menuRect = gfc_sdl_rect(screenWidth / 2 - screenWidth / 6, screenHeight / (main_menu_len + 1), screenWidth / 3, screenHeight / 7);

	//texts = main_menu_texts;
	textLen = main_menu_len;

	for (int i = 0; i < textLen; i++) {
		//gfc_line_sprintf(text, texts[i]);
		SDL_RenderDrawRect(gf2d_graphics_get_renderer(), &menuRect);
		SDL_RenderFillRect(gf2d_graphics_get_renderer(), &menuRect);
		//font_render(MENU_FONT, text, vector2d(screenWidth / 2 - 200, screenHeight / 2), gfc_color8(255, 0, 0, 255));
	}
}