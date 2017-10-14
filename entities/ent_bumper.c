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
#define param_timebase	(this->paramc[6])
#define param_tile		(this->paramc[7])

void destroyBumper(entity_t* this);
void updateBumper(entity_t* this);

void ent_Bumper(entity_t* this, va_list args) {
	int x = va_arg(args, int);
	int y = va_arg(args, int);
	char tile = va_arg(args, char) | 0x80;

	collision_box_t* col;

	this->onDestroy = destroyBumper;

	EREQUIRE(param_col = col =
		col_AllocBox(true, x, y, 8, 16));

	//this->onUpdate = updateSmasher;
	param_x = x;
	param_y = y;
	param_timebase = tickcount / 32;
	param_tile = tile;

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
	char state = (param_tile & BUMPER_STATE_MASK) >> BUMPER_STATE_SHIFT;
	char newState = ((char)(tickcount / 16) - param_timebase) % 6;

	int x, y;
	collision_box_t* col;

	if(newState >= 4)
		newState = 6 - newState;

	if(state == newState)
		return;

	if(!newState)
		param_timebase = tickcount / 16;

	state = (param_tile & ~BUMPER_STATE_MASK) | (newState << BUMPER_STATE_SHIFT);
	param_tile = state;
	x = param_x;
	y = param_y;

	// back half always same except into state 0 and state 1
	if(newState < 2) {
		map_SetTile(x / 8, y / 8, state);
		map_SetTile(x / 8, y / 8 + 1, state + 1);
	}

	if(state & BUMPER_DIR_MASK) {
		// right
		col->right = x + newState * 2 + 8;

		map_SetTile(x / 8 + 1, y / 8, state + 2);
		map_SetTile(x / 8 + 1, y / 8 + 1, state + 3);
	} else  {
		// left
		col->x = x - newState * 2;

		map_SetTile(x / 8 - 1, y / 8, state + 2);
		map_SetTile(x / 8 - 1, y / 8 + 1, state + 3);
	}
}
