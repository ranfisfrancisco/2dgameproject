#include <time.h> 
#include "simple_logger.h" 
#include "gf2d_graphics.h"
#include "menu.h"
#include "director.h"


static Menu* CURRENT_MENU;
static Menu MENU_MAIN;
static Menu MENU_CHANGE;
static Menu MENU_EXIT;
static clock_t MENU_CLOCK_START, MENU_MOVE_START;
double MENU_TIME, MOVE_TIME;

void menu_switch(Menu* menu);

void menu_init() {
	MENU_MAIN.font = font_load("fonts/DroidSans.ttf", 32);
	MENU_MAIN.numOfOptions = 3;
	MENU_MAIN.texts = malloc(sizeof(char*) * MENU_MAIN.numOfOptions);
	MENU_MAIN.texts[0] = "Continue";
	MENU_MAIN.texts[1] = "Change Mode";
	MENU_MAIN.texts[2] = "Quit";
	MENU_MAIN.highlightIndex = 0;
	MENU_MAIN.backMenu = NULL;

	MENU_CHANGE.font = font_load("fonts/DroidSans.ttf", 32);
	MENU_CHANGE.numOfOptions = 3;
	MENU_CHANGE.texts = malloc(sizeof(char*) * MENU_CHANGE.numOfOptions);
	MENU_CHANGE.texts[0] = "Go to Game";
	MENU_CHANGE.texts[1] = "Go to Editor";
	MENU_CHANGE.texts[2] = "Go Back";
	MENU_CHANGE.highlightIndex = 0;
	MENU_CHANGE.backMenu = NULL;

	MENU_EXIT.font = font_load("fonts/DroidSans.ttf", 32);
	MENU_EXIT.numOfOptions = 2;
	MENU_EXIT.texts = malloc(sizeof(char*) * MENU_EXIT.numOfOptions);
	MENU_EXIT.texts[0] = "Return";
	MENU_EXIT.texts[1] = "Really Quit";
	MENU_EXIT.highlightIndex = 0;
	MENU_EXIT.backMenu = NULL;

	menu_switch(&MENU_MAIN);

	slog("Initialized Menus");
}

void menu_open() {
	MENU_MAIN.highlightIndex = 0;
	CURRENT_MENU = &MENU_MAIN;
}

void menu_switch(Menu* menu) {
	CURRENT_MENU = menu;
	MENU_CLOCK_START = clock();
	MENU_MOVE_START = clock();
	MENU_TIME = 0;
	MOVE_TIME = 0;
}

int menu_update() {
	const Uint8* keys;
	MENU_TIME = (double)(clock() - MENU_CLOCK_START) / CLOCKS_PER_SEC;
	MOVE_TIME = (double)(clock() - MENU_MOVE_START) / CLOCKS_PER_SEC;

	keys = SDL_GetKeyboardState(NULL); // get the keyboard state for this frame

	if (keys[SDL_SCANCODE_W] && MOVE_TIME > 0.2) {
		CURRENT_MENU->highlightIndex--;
		MENU_MOVE_START = clock();
	}
	else if (keys[SDL_SCANCODE_S] && MOVE_TIME > 0.2) {
		CURRENT_MENU->highlightIndex++;
		MENU_MOVE_START = clock();
	}
	else if (keys[SDL_SCANCODE_RETURN] && MENU_TIME > 0.2) {
		if (CURRENT_MENU == &MENU_MAIN) {
			if (CURRENT_MENU->highlightIndex == 0) {
				return MENU_ACTION_CLOSE;
			}
			else if (CURRENT_MENU->highlightIndex == 1) {
				menu_switch(&MENU_CHANGE);
			}
			else if (CURRENT_MENU->highlightIndex == 2) {
				menu_switch(&MENU_EXIT);
			}
		}
		else if (CURRENT_MENU == &MENU_CHANGE) {
			if (CURRENT_MENU->highlightIndex == 0) {
				return MENU_ACTION_GOTO_GAME;
			}
			else if (CURRENT_MENU->highlightIndex == 1) {
				return MENU_ACTION_GOTO_EDITOR;
			}
			else if (CURRENT_MENU->highlightIndex == 2) {
				menu_switch(&MENU_MAIN);
			}
		}
		else if (CURRENT_MENU == &MENU_EXIT) {
			if (CURRENT_MENU->highlightIndex == 0) {
				menu_switch(&MENU_MAIN);
			}
			else if (CURRENT_MENU->highlightIndex == 1) {
				return MENU_ACTION_QUIT;
			}
		}
	}

	if (CURRENT_MENU->highlightIndex < 0)
		CURRENT_MENU->highlightIndex = 0;
	else if (CURRENT_MENU->highlightIndex >= CURRENT_MENU->numOfOptions) {
		CURRENT_MENU->highlightIndex = CURRENT_MENU->numOfOptions - 1;
	}

	return MENU_ACTION_CONTINUE;
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

		menuRect = gfc_sdl_rect(x, y, screenWidth / 3, screenHeight / 7);
		gfc_line_sprintf(text, CURRENT_MENU->texts[i]);

		SDL_RenderDrawRect(gf2d_graphics_get_renderer(), &menuRect);
		SDL_RenderFillRect(gf2d_graphics_get_renderer(), &menuRect);
		if (i == CURRENT_MENU->highlightIndex)
			font_render(CURRENT_MENU->font, text, vector2d(x + screenWidth / 6 - 60, y + screenHeight/18), gfc_color8(255, 255, 255, 255));
		else
			font_render(CURRENT_MENU->font, text, vector2d(x + screenWidth / 6 - 60, y + screenHeight / 18), gfc_color8(255, 0, 0, 255));
	}
}
