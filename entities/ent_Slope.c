#include <stdarg.h>
#include <stdlib.h>

#include "math.h"
#include "video.h"
#include "map.h"
#include "entity.h"
#include "sound.h"
#include "collision.h"

#include "tiles.h"
#include "sounds.h"

#include "entities.h"

#define param_col		((collision_box_t*)this->paramp[0])

void DestroySlope(entity_t* this);

void ent_Slope(entity_t* this, va_list args) {
	collision_box_t* col;
	tile_t tile;

	this->onDestroy = DestroySlope;

	col = col_AllocBox(true, va_arg(args, int), va_arg(args, int) + 3, 5, 5);
	if(!col) {
		e_Destroy(this);
		return;
	}
	param_col = col;

	tile = 0x80 + va_arg(args, tile_t);
	map_SetTile(col->x / 8, col->y / 8, tile);

	// check if left-facing slope
	if(tile == TILE_SLOPEL) {
		col->x += 3;
		col->right += 3;
	}
}

void DestroySlope(entity_t* this) {
	if(param_col)
		col_FreeBox(param_col);
}
