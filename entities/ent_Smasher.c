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

typedef enum {
	S_UP,
	S_DOWNWARD,
	S_DOWN,
	S_UPWARD
} smasher_state_t;

// sprite only exists on movements
#define param_sprite	((bound_sprite_t*)this->paramp[0])
#define param_x			(this->parami[1])
#define param_y			(this->parami[2])
#define param_state		(this->paramc[6])

void destroySmasher(entity_t* this);
void updateSmasher(entity_t* this);

void ent_Smasher(entity_t* this, va_list args) {
	int x = va_arg(args, int);
	int y = va_arg(args, int);

	this->onDestroy = destroySmasher;
	this->onUpdate = updateSmasher;
	param_x = x;
	param_y = y;

	map_SetTile(x / 8, y / 8, SPR_SMASHER0);
}

void destroySmasher(entity_t* this) {
	if(param_sprite)
		map_FreeBoundSprite(param_sprite);
}

void updateSmasher(entity_t* this) {
	bound_sprite_t* bs;

	switch (param_state) {
		case S_UP:
			if(tickcount % (60 * 4) > 60 * 3) {
				map_SetTile(param_x / 8, param_y / 8, TILE_BLANK);

				bs = map_AllocBoundSprite();
				if(!bs) {
					e_Destroy(this);
					return;
				}
				param_sprite = bs;
				bs->sprite->tile = SPR_SMASHER0+0x10;
				bs->sprite->attrib = 0;
				bs->x = param_x;
				bs->y = param_y;
				map_UpdateSprite(bs);
				param_state = S_DOWNWARD;
			}
			break;
		case S_DOWNWARD:
			if(tickcount % (60 * 4) < 60) {
				bs = param_sprite;
				map_FreeBoundSprite(bs);
				bs = NULL;
				map_SetTile(param_x / 8, param_y / 8, SPR_SMASHER0);
				param_state = S_UP;
			} else {
				map_UpdateSprite(param_sprite);
			}
			break;
	}
}
