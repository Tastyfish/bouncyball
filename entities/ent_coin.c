#include <stdarg.h>
#include <stdlib.h>

#include "game.h"
#include "video.h"
#include "map.h"
#include "entity.h"
#include "sound.h"
#include "collision.h"

#include "tiles.h"
#include "sounds.h"

#include "entities.h"

#define param_sprite	((bound_sprite_t*)this->paramp[0])
#define param_col		((collision_box_t*)this->paramp[1])

void DestroyCoin(entity_t* this);
void UpdateCoin(entity_t* this);
void CollideCoin(collision_box_t* this, collision_box_t* other, int nx, int ny);

void ent_Coin(entity_t* this, va_list args) {
	bound_sprite_t* s;
	collision_box_t* col;

	this->onDestroy = DestroyCoin;

	EREQUIRE(param_sprite = s =
		map_AllocBoundSprite());
	EREQUIRE(param_col = col =
		col_AllocBox(true, va_arg(args, int), va_arg(args, int), 8, 8));

	s->x = col->x;
	s->y = col->y;
	s->sprite->tile = SPR_COIN0;
	s->sprite->attrib = 3; // yellow

	col->onCollide = CollideCoin;
	col->param = this;

	this->onUpdate = UpdateCoin;
}

void DestroyCoin(entity_t* this) {
	bound_sprite_t* s = param_sprite;
	collision_box_t* c = param_col;

	if(s)
		map_FreeBoundSprite(s);
	if(c)
		col_FreeBox(c);
}

void UpdateCoin(entity_t* this) {
	// just rotate graphic
	bound_sprite_t* s = param_sprite;
	char anistep = tickcount / 8 % 6;

	s->sprite->tile = SPR_COIN0 + (anistep < 4 ? anistep : 6 - anistep) * 0x10;
	map_UpdateSprite(s);
}

void CollideCoin(collision_box_t* this, collision_box_t*, int, int) {
	// Collect the coin
	s_PlaySFX(SFX_JOKE, SFX_CH2);
	e_Destroy(this->param);
}
