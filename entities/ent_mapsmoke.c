#include "game.h"
#include "video.h"
#include "map.h"
#include "entity.h"

#include "tiles.h"
#include "entities.h"
#include "gent.h"

#define param_sprite	((bound_sprite_t*)this->paramp[0])

void updateSmoke(entity_t* this);
void destroySmoke(entity_t* this);

void ent_MapSmoke(entity_t* this, va_list args) {
	int x = va_arg(args, int);
	int y = va_arg(args, int);
	bound_sprite_t* bs;

	this->onDestroy = destroySmoke;

	EREQUIRE(param_sprite = bs =
		map_AllocBoundSprite());

	bs->x = x;
	bs->y = y;
	bs->sprite->tile = SPR_SMOKE0;
	bs->sprite->attrib = 1;

	this->onUpdate = updateSmoke;
}

void updateSmoke(entity_t* this) {
	bound_sprite_t* bs = param_sprite;
	char t = (tickcount >> 4) % 2;

	bs->sprite->tile = t ? SPR_SMOKE0 : SPR_SMOKE1;

	map_UpdateSprite(bs);
}

void destroySmoke(entity_t* this) {
	if(param_sprite)
		map_FreeBoundSprite(param_sprite);
}
