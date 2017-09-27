#include <stdarg.h>
#include <stdlib.h>

#include "math.h"
#include "entity.h"
#include "video.h"
#include "sound.h"
#include "collision.h"

#include "tiles.h"
#include "sounds.h"

#include "entities.h"

#define param_sprite	(this->graphic[0])
// accel as 1/8 of a pixel
#define param_accelx	((signed char)this->paramc[2])
#define param_accely	((signed char)this->paramc[3])
// so the shaker can find the balls
#define param_magicid	(this->paramu[2])
#define param_col		((collision_box_t*)this->paramp[3])

#define colparam_accelx	((signed char)currentColEntity->paramc[2])
#define colparam_accely	((signed char)currentColEntity->paramc[3])

void UpdateBall(entity_t* this);
void DestroyBall(entity_t* this);
void CollideBall(collision_box_t* this, unsigned int x, unsigned int y);

// Variables for the collision test
entity_t* currentColEntity;
char useCH = 0;
sfx_channel_t const channels[4] = {SFX_CH0, SFX_CH1, SFX_CH2, SFX_CH3};

void ent_Ball(entity_t* this, va_list args) {
	collision_box_t* col;
	sprite_t* s = v_AllocSprite();
	if(!s)
		return;

	col = col_AllocBox(false, 0, 0, 8, 8);
	if(!col) {
		v_FreeSprite(s);
		return;
	}

	param_sprite = s;
	param_col = col;
	param_accelx = crand(-32, 32); // random direction
	param_accely = crand(-32, 32); // random direction
	param_magicid = 0xBA11;

	s->x = va_arg(args, int);
	s->y = va_arg(args, int);
	s->tile = SPR_BALL; // ball
	s->attrib = crand(0, 3); // random color

	col->onCollide = CollideBall;

	this->onDestroy = DestroyBall;
	this->onUpdate = UpdateBall;
}

void DestroyBall(entity_t* this) {
	sprite_t* s = param_sprite;
	s->y = 0xFF;
	v_FreeSprite(s);
	col_FreeBox(param_col);
}

void UpdateBall(entity_t* this) {
	sprite_t* s = param_sprite;
	collision_box_t* col = param_col;

	// gravity
	param_accely += 2;

	// Contact acceleration will be updated in collision code
	col->x = s->x;
	col->y = s->y;
	col->right = s->x + 8;
	col->bottom = s->y + 8;
	currentColEntity = this;
	col_Test(param_col);

	// dV
	s->x += param_accelx / 8;
	s->y += param_accely / 8;
}

void CollideBall(collision_box_t*, unsigned int nx, unsigned int ny) {
	int accelx = colparam_accelx;
	int accely = colparam_accely;

	// did we completely fall inside? hard reverse then
	if(nx == 4 && ny == 4) {
		colparam_accelx = -accelx;
		colparam_accely = -accely;
		return;
	}

	// Everything is bounding boxes, so it has to be a cardinal contact (except corner cases)
	// normal is adjusted to be center-aligned
	if(reflectcard(&accelx, &accely, nx - 4, ny - 4)) {
		colparam_accelx = accelx * 2 / 3 + 5;
		//colparam_accely = accely * 2 / 3;// + crand(-8, 8);
	} else {
		colparam_accelx = crand(-8, 8);
		colparam_accely = accely * 2 / 3;
	}

	s_PlaySFX(SFX_BOUNCEH, channels[useCH]);
	if(++useCH == 4)
		useCH = 0;
}
