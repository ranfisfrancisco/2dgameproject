#include "simple_logger.h" 
#include "menu.h"
#include "font.h"
#include "director.h"

Font* MENU_FONT;
Menu* CURRENT_MENU;
Menu MENU_MAIN;

void menu_init() {
	CURRENT_MENU = &MENU_MAIN;
	MENU_FONT = font_load("fonts/DroidSans.ttf", 32);

	MENU_MAIN.numOfOptions = 2;
	MENU_MAIN.texts[0] = "Continue";
	MENU_MAIN.texts[1] = "Quit";
	MENU_MAIN.highlightIndex = 0;

	slog("Initialized Menus");
}

void menu_open() {
	MENU_MAIN.highlightIndex = 0;
	CURRENT_MENU = &MENU_MAIN;
}

int menu_update() {
	const Uint8* keys;

	keys = SDL_GetKeyboardState(NULL); // get the keyboard state for this frame

	if (keys[SDL_SCANCODE_W])
		CURRENT_MENU->highlightIndex--;
	else if (keys[SDL_SCANCODE_S])
		CURRENT_MENU->highlightIndex++;

	if (CURRENT_MENU->highlightIndex < 0)
		CURRENT_MENU->highlightIndex = 0;
	else if (CURRENT_MENU->highlightIndex >= CURRENT_MENU->numOfOptions) {
		CURRENT_MENU->highlightIndex = CURRENT_MENU->numOfOptions - 1;
	}

	return -1;
}

void menu_draw() {
	int screenWidth, screenHeight;
	TextLine text;
	SDL_Rect menuRect;

	screenWidth = director_get_screen_width();
	screenHeight = director_get_screen_height();

	for (int i = 0; i < CURRENT_MENU->numOfOptions; i++) {
		int x, y;
		x = screenWidth / 2 - screenWidth / 6;
		y = 100 + 150 * i;

		menuRect = gfc_sdl_rect(x,  y, screenWidth / 3, screenHeight / 7);
		gfc_line_sprintf(text, CURRENT_MENU->texts[i]);

		SDL_RenderDrawRect(gf2d_graphics_get_renderer(), &menuRect);
		SDL_RenderFillRect(gf2d_graphics_get_renderer(), &menuRect);
		if (i == CURRENT_MENU->highlightIndex)
			font_render(MENU_FONT, text, vector2d(x + screenWidth / 6 - 60, y + screenHeight/18), gfc_color8(255, 255, 255, 255));
		else
			font_render(MENU_FONT, text, vector2d(x + screenWidth / 6 - 60, y + screenHeight / 18), gfc_color8(255, 0, 0, 255));
	}
}
