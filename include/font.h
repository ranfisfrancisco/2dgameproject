#ifndef __FONT_H__
#define __FONT_H_

#include <SDL_ttf.h>

#include "gfc_vector.h"
#include "gfc_text.h"
#include "gfc_color.h"
typedef struct
{
	Uint32 _refCount;
	TTF_Font* font;
	TextLine filename;
	int ptsize;
}Font;

/**
 * @brief intialize font resource manager
 * @param maxFonts how many fonts to support
 */
void font_init(Uint32 maxFonts);

/**
 * @brief Load a given font
 * @param filename The filename to be opened
 * @oaram ptsize
 * @return Pointer to the new font
 */
Font* font_load(const char* filename, int ptsize);

/**
 * @brief Free a font
 * @param font The font to be freed
 */
void font_free(Font* font);

/**
 * @brief Render a font
 * @param font The font to be rendered
 * @param text The text to be written
 * @param position Vector2D containing the position
 * @param color The color to be used
 */
void font_render(Font* font, char* text, Vector2D position, Color color);

#endif