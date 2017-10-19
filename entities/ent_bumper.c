#include <stdarg.h>
#include <stddef.h>
#include <stdlib.h>

#include "math.h"
#include "game.h"
#include "entity.h"
#include "video.h"
#include "map.h"
#include "collision.h"

#include "tiles.h"

#include "entities.h"

#define BUMPER_STATE_MASK	(0x30)
#define BUMPER_STATE_SHIFT	(4)
#define BUMPER_DIR_MASK		(0x40)

// sprite only exists on movements
#define param_col		((collision_box_t*)this->paramp[0])
#define param_x			(this->parami[1])
#define param_y			(this->parami[2])
#define param_tick		(this->paramc[6])
#define param_tile		(this->paramc[7])

void destroyBumper(entity_t* this);
void updateBumper(entity_t* this);

void ent_Bumper(entity_t* this, va_list args) {
	int x = va_arg(args, int);
	int y = va_arg(args, int);
	char tile = va_arg(args, char) | 0x80;

	this->onDestroy = destroyBumper;

	EREQUIRE(param_col =
		col_AllocBox(true, x, y, 8, 16));

	//this->onUpdate = updateSmasher;
	param_x = x;
	param_y = y;
	param_tile = tile & ~BUMPER_STATE_MASK;

	map_SetTile(x / 8, y / 8, tile);
	map_SetTile(x / 8, y / 8 + 1, tile + 1);

	this->onUpdate = updateBumper;
}

void destroyBumper(entity_t* this) {
	collision_box_t* col = param_col;

	if(col)
		col_FreeBox(col);
}

void updateBumper(entity_t* this) {
	int x, y;
	collision_box_t* col;
	char state, tile;
	char tick = param_tick;

	if(tick >= 48)
		param_tick = tick = 0;
	else
		param_tick = tick + 1;

	if(tick % 8)
		return;

	state = tick / 8 % 6;

	if(state >= 4)
		state = 6 - state;

	tile = param_tile | (state << BUMPER_STATE_SHIFT);
	x = param_x;
	y = param_y;
	col = param_col;

	// back half always same except into state 0 and state 1
	if(state < 2) {
		map_SetTile(x / 8, y / 8, tile);
		map_SetTile(x / 8, y / 8 + 1, tile + 1);
	}

	if(tile & BUMPER_DIR_MASK) {
		// right
		col->right = x + state * 2 + 8;

		map_SetTile(x / 8 + 1, y / 8, tile + 2);
		map_SetTile(x / 8 + 1, y / 8 + 1, tile + 3);
	} else  {
		// left
		col->x = x - state * 2;

		map_SetTile(x / 8 - 1, y / 8, tile + 2);
		map_SetTile(x / 8 - 1, y / 8 + 1, tile + 3);
	}
}
