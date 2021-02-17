#ifndef __LEVEL_H__
#define __LEVEL_H__

#include "gfc_types.h"
#include "gfc_text.h"

typedef enum {
	TT_Brick
}TileTypes;

typedef struct
{
	TextLine bgimage; /* The background image for the level*/
	TextLine tileSet; /**< sprite for the tileset */
	TileTypes* tileMap; /**< the tiles for the level */
	Uint32 levelWidth; /**< how many tiles per row the level has */
	Uint32 levelHeight;
	Uint32 tileWidth; /**< how many pixels wide the tiles are */
	Uint32 tileHeight;

}Level;

/**
 * @brief allocate and initialize a level
 * @return NULL on error, else a pointer to a new level
*/
Level* level_new();

/**
 * @brief free up a previously loaded level
 * 
 */

void level_free(Level *level);

void level_draw(Level *level);

#endif