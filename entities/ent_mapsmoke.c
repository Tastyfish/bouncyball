#include "game.h"
#include "video.h"
#include "map.h"
#include "entity.h"

#include "tiles.h"
#include "entities.h"
#include "gent.h"

#define param_upper_sprite	(this->boundGraphic[0])
#define param_lower_sprite	(this->boundGraphic[1])

void updateSmoke(entity_t* this);
void destroySmoke(entity_t* this);

void ent_MapSmoke(entity_t* this, va_list args) {
	int x = va_arg(args, int);
	int y = va_arg(args, int);

	bound_sprite_t* ubs = map_AllocBoundSprite();
	bound_sprite_t* lbs = map_AllocBoundSprite();
	if(!ubs)
		return;
	if(!lbs) {
		map_FreeBoundSprite(ubs);
		return;
	}

	this->onUpdate = updateSmoke;
	this->onDestroy = destroySmoke;

	ubs->x = x;
	ubs->y = y;
	ubs->sprite->tile = SPR_SMOKE0;
	ubs->sprite->attrib = 1;

	lbs->x = x;
	lbs->y = y + 8;
	lbs->sprite->tile = SPR_SMOKE0 + 0x10;
	lbs->sprite->attrib = 1;

	param_upper_sprite = ubs;
	param_lower_sprite = lbs;
}

void updateSmoke(entity_t* this) {
	bound_sprite_t* ubs = param_upper_sprite;
	bound_sprite_t* lbs = param_lower_sprite;
	char t = (tickcount >> 4) % 2;

	ubs->sprite->tile = t ? SPR_SMOKE0 : SPR_SMOKE1;
	lbs->sprite->tile = t ? SPR_SMOKE0 + 0x10 : SPR_SMOKE1 + 0x10;

	map_UpdateSprite(ubs);
	map_UpdateSprite(lbs);
}

void destroySmoke(entity_t* this) {
	map_FreeBoundSprite(param_upper_sprite);
	map_FreeBoundSprite(param_lower_sprite);
}
