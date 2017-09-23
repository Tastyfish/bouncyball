#include "game.h"
#include "video.h"
#include "entity.h"

#include "tiles.h"
#include "entities.h"
#include "gent.h"

#define param_upper_sprite	(this->graphic[0])
#define param_lower_sprite	(this->graphic[1])
#define param_magicid		(this->paramu[2])
#define param_basex			(this->paramc[6])
#define param_basey			(this->paramc[7])

void updateNTSmoke(entity_t* this);
void destroyNTSmoke(entity_t* this);

void ent_NTSmoke(entity_t* this, va_list args) {
	char x = va_arg(args, int);
	char y = va_arg(args, int);

	sprite_t* ubs = v_AllocSprite();
	sprite_t* lbs = v_AllocSprite();
	if(!ubs)
		return;
	if(!lbs) {
		v_FreeSprite(ubs);
		return;
	}

	this->onUpdate = updateNTSmoke;
	this->onDestroy = destroyNTSmoke;

	ubs->x = x;
	ubs->y = y;
	ubs->tile = SPR_SMOKE0;
	ubs->attrib = 1;

	lbs->x = x;
	lbs->y = y + 8;
	lbs->tile = SPR_SMOKE0 + 0x10;
	lbs->attrib = 1;

	param_upper_sprite = ubs;
	param_lower_sprite = lbs;
	param_magicid = 0x530C;
	param_basex = x;
	param_basey = y;
}

void updateNTSmoke(entity_t* this) {
	sprite_t* ubs = param_upper_sprite;
	sprite_t* lbs = param_lower_sprite;
	char t = (tickcount >> 4) % 2;

	ubs->tile = t ? SPR_SMOKE0 : SPR_SMOKE1;
	lbs->tile = t ? SPR_SMOKE0 + 0x10 : SPR_SMOKE1 + 0x10;
}

void destroyNTSmoke(entity_t* this) {
	v_FreeSprite(param_upper_sprite);
	v_FreeSprite(param_lower_sprite);
}
