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

Font* font_load(const char* filename, int ptsize);
void font_free(Font* font);
void font_render(Font* font, char* text, Vector2D position, Color color);

#endif