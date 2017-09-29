#include "game.h"
#include "video.h"
#include "entity.h"

#include "tiles.h"
#include "entities.h"
#include "gent.h"

#define param_sprite		(this->paramp[0])
#define param_magicid		(this->paramu[2])
#define param_basex			(this->paramc[6])
#define param_basey			(this->paramc[7])

void updateNTSmoke(entity_t* this);
void destroyNTSmoke(entity_t* this);

void ent_NTSmoke(entity_t* this, va_list args) {
	char x = va_arg(args, int);
	char y = va_arg(args, int);
	sprite_t* s;

	this->onDestroy = destroyNTSmoke;

	s = v_AllocSprite();
	if(!s) {
		e_Destroy(this);
		return;
	}
	param_sprite = s;

	s->x = x;
	s->y = y;
	s->tile = SPR_SMOKE0;
	s->attrib = 1;

	param_basex = x;
	param_basey = y;

	this->onUpdate = updateNTSmoke;
}

void updateNTSmoke(entity_t* this) {
	sprite_t* s = param_sprite;
	char t = (tickcount >> 4) % 2;

	s->tile = t ? SPR_SMOKE0 : SPR_SMOKE1;
}

void destroyNTSmoke(entity_t* this) {
	if(param_sprite)
		v_FreeSprite(param_sprite);
}
