#include <stdarg.h>
#include <stdlib.h>

#include "math.h"
#include "game.h"
#include "video.h"
#include "input.h"
#include "map.h"
#include "entity.h"
#include "sound.h"
#include "collision.h"

#include "tiles.h"
#include "sounds.h"

#include "entities.h"

#define ACCEL_LIMIT		INT2DEC(6)
#define GRAV_LIMIT		INT2DEC(8)

#define param_lsprite	((bound_sprite_t*)this->paramp[0])
#define param_rsprite	((bound_sprite_t*)this->paramp[1])
#define param_col		((collision_box_t*)this->paramp[2])
#define param_gravcol	((collision_box_t*)this->paramp[3])
#define param_accelx	((decimal_t)this->parami[4])
#define param_accely	((decimal_t)this->parami[5])

#define colparam_accelx	((decimal_t)currentColEntity->parami[4])
#define colparam_accely	((decimal_t)currentColEntity->parami[5])

void UpdateBall(entity_t* this);
void DestroyBall(entity_t* this);
void CollideBall(collision_box_t* this, collision_box_t* other, int x, int y);
void GravBall(collision_box_t* this, collision_box_t* other, int x, int y);

// Variables for the collision test
entity_t* currentColEntity;
int ofsX, ofsY;
//char useCH = 0;
//sfx_channel_t const channels[4] = {SFX_CH0, SFX_CH1, SFX_CH2, SFX_CH3};
bool doGravity;

void ent_Ball(entity_t* this, va_list args) {
	collision_box_t* col, * gcol;
	bound_sprite_t* ls, * rs;
	int x = va_arg(args, int);
	int y = va_arg(args, int);

	this->onDestroy = DestroyBall;

	EREQUIRE(param_lsprite = ls =
		map_AllocBoundSprite());
	EREQUIRE(param_rsprite = rs =
		map_AllocBoundSprite());
	EREQUIRE(param_col = col =
		col_AllocBox(false, x, y, 16, 16));
	EREQUIRE(param_gravcol = gcol =
		col_AllocBox(false, x + 4, y + 16, 8, 1));

	ls->x = x;
	ls->y = y;
	ls->sprite->tile = SPR_BALL_L; // ball
	ls->sprite->attrib = crand(0, 3); // random color
	rs->x = x + 8;
	rs->y = y;
	rs->sprite->tile = SPR_BALL_R; // ball
	rs->sprite->attrib = ls->sprite->attrib; // same color

	col->onCollide = CollideBall;
	gcol->onCollide = GravBall;

	this->onUpdate = UpdateBall;
}

void DestroyBall(entity_t* this) {
	bound_sprite_t* ls = param_lsprite;
	bound_sprite_t* rs = param_rsprite;
	collision_box_t* col = param_col;
	collision_box_t* gcol = param_gravcol;

	if(ls)
		map_FreeBoundSprite(ls);
	if(rs)
		map_FreeBoundSprite(rs);
	if(col)
		col_FreeBox(col);
	if(gcol)
		col_FreeBox(gcol);
}

void UpdateBall(entity_t* this) {
	bound_sprite_t* ls = param_lsprite;
	bound_sprite_t* rs = param_rsprite;
	collision_box_t* col = param_col;
	collision_box_t* gcol = param_gravcol;

	input_t i = i_GetStandardInput(INPUT_PLAYER_0);

	if((i & INPUT_LEFT) && param_accelx >= -ACCEL_LIMIT - 1) {
		param_accelx -= 8;
	}
	if((i & INPUT_RIGHT) && param_accelx < ACCEL_LIMIT) {
		param_accelx += 8;
	}
	if((i & INPUT_UP) && param_accely >= -ACCEL_LIMIT - 1) {
		param_accely -= 8;
	}
	if((i & INPUT_DOWN) && param_accely < ACCEL_LIMIT) {
		param_accely += 8;
	}

	// gravity
	if(param_accely < GRAV_LIMIT) {
		doGravity = true;
		map_TestColBox(gcol);
		//g_Yield();
		if(doGravity)
			param_accely += 6;
	}

	// Contact acceleration will be updated in collision code
	currentColEntity = this;
	ofsX = ofsY = 0;
	map_TestColBox(col);
	//g_Yield();
	col_Test(col);
	//g_Yield();

	// camera
	if(ls->y < map_refY - 40) {
		map_MoveTo(ls->x, MAX(120, ls->y + 40));
	} else if(ls->y >= map_refY + 40) {
		map_MoveTo(ls->x, MIN(map_height * 8 - 120, ls->y - 40));
	}

	// update sprites -- map_UpdateSprite must be called after camera update
	gcol->x = (col->x += ofsX + DEC2INT(param_accelx)) + 4;
	col->y += ofsY + DEC2INT(param_accely);
	gcol->right = (col->right = col->x + 16) - 4;
	gcol->bottom = (gcol->y = (col->bottom = col->y + 16)) + 1;
	rs->x = (ls->x = col->x) + 8;
	rs->y = ls->y = col->y;
	map_UpdateSprite(ls);
	map_UpdateSprite(rs);
}

void CollideBall(collision_box_t*, collision_box_t*, int nx, int ny) {
	decimal_t accelx = colparam_accelx;
	decimal_t accely = colparam_accely;
	decimal_t dnx, dny;

	nx -= 8;
	ny -= 8;

	// are we actually in the cirle??
	if(vleni(nx, ny) >= 8)
		return;

	// did we completely fall inside? hard reverse then
	if(!(nx || ny)) {
		colparam_accelx = -accelx;
		colparam_accely = -accely;
		return;
	}

	// knock outside of object
	if(nx && (!ny || abs(nx) >= abs(ny)))
		ofsX += nx < 0 ? nx + 8 : nx - 8;
	if(ny && (!nx || abs(ny) >= abs(nx)))
		ofsY += ny < 0 ? ny + 8 : ny - 8;

	// make precise enough to do real math on
	dnx = INT2DEC(nx);
	dny = INT2DEC(ny);

	// The actual vector math to reflect things
	normalized(&dnx, &dny);
	reflectd(&accelx, &accely, dnx, dny);
	colparam_accelx = (accelx / (dnx + 1));
	colparam_accely = (accely / (dny + 2));

	//s_PlaySFX(SFX_BOUNCEH, channels[useCH]);
	//if(++useCH == 4)
	//	useCH = 0;
}

void GravBall(collision_box_t*, collision_box_t*, int nx, int) {
	if(nx == 4)
		doGravity = false;
}
