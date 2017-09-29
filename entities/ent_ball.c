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

#define param_lsprite	(this->paramp[0])
#define param_rsprite	(this->paramp[1])
// accel as 1/8 of a pixel
#define param_accelx	((signed char)this->paramc[4])
#define param_accely	((signed char)this->paramc[5])
#define param_col		((collision_box_t*)this->paramp[3])

#define colparam_accelx	((signed char)currentColEntity->paramc[4])
#define colparam_accely	((signed char)currentColEntity->paramc[5])

void UpdateBall(entity_t* this);
void DestroyBall(entity_t* this);
void CollideBall(collision_box_t* this, unsigned int x, unsigned int y);

// Variables for the collision test
entity_t* currentColEntity;
char useCH = 0;
sfx_channel_t const channels[4] = {SFX_CH0, SFX_CH1, SFX_CH2, SFX_CH3};

void ent_Ball(entity_t* this, va_list args) {
	collision_box_t* col;
	sprite_t* ls, * rs;

	this->onDestroy = DestroyBall;

	ls = v_AllocSprite();
	if(!ls) {
		e_Destroy(this);
		return;
	}
	param_lsprite = ls;

	rs = v_AllocSprite();
	if(!rs) {
		e_Destroy(this);
		return;
	}
	param_rsprite = rs;

	col = col_AllocBox(false, 0, 0, 8, 8);
	if(!col) {
		e_Destroy(this);
		return;
	}
	param_col = col;

	param_accelx = crand(-32, 32); // random direction
	param_accely = crand(-32, 32); // random direction

	ls->x = va_arg(args, int);
	ls->y = va_arg(args, int);
	ls->tile = SPR_BALL_L; // ball
	ls->attrib = crand(0, 3); // random color
	rs->x = ls->x;
	rs->y = ls->y;
	rs->tile = SPR_BALL_R; // ball
	rs->attrib = ls->attrib; // same color

	col->onCollide = CollideBall;

	this->onUpdate = UpdateBall;
}

void DestroyBall(entity_t* this) {
	sprite_t* ls = param_lsprite;
	sprite_t* rs = param_rsprite;

	if(ls) {
		ls->y = 0xFF;
		v_FreeSprite(ls);
	}
	if(rs) {
		rs->y = 0xFF;
		v_FreeSprite(rs);
	}
	if(param_col)
		col_FreeBox(param_col);
}

void UpdateBall(entity_t* this) {
	sprite_t* ls = param_lsprite;
	sprite_t* rs = param_rsprite;
	collision_box_t* col = param_col;

	// gravity
	param_accely += 3;

	// Contact acceleration will be updated in collision code
	col->x = ls->x;
	col->y = ls->y;
	col->right = ls->x + 16;
	col->bottom = ls->y + 16;
	currentColEntity = this;
	col_Test(param_col);

	// dV
	ls->x += param_accelx / 8;
	ls->y += param_accely / 8;
	rs->x = ls->x + 8;
	rs->y = ls->y;
}

void CollideBall(collision_box_t*, unsigned int nx, unsigned int ny) {
	int accelx = colparam_accelx;
	int accely = colparam_accely;

	// did we completely fall inside? hard reverse then
	if(nx == 4 && ny == 4) {
		colparam_accelx = -2 * accelx;
		colparam_accely = -2 * accely;
		return;
	}

	// Everything is bounding boxes, so it has to be a cardinal contact (except corner cases)
	// normal is adjusted to be center-aligned
	if(reflectcard(&accelx, &accely, nx - 4, ny - 4)) {
		colparam_accelx = accelx * 2 / 3;
		//colparam_accely = accely * 2 / 3;// + crand(-8, 8);
	} else {
		colparam_accelx = crand(-8, 8) + 4;
		colparam_accely = accely * 2 / 3;
	}

	s_PlaySFX(SFX_BOUNCEH, channels[useCH]);
	if(++useCH == 4)
		useCH = 0;
}
