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

#define ACCEL_LIMIT		(48)

#define param_lsprite	((bound_sprite_t*)this->paramp[0])
#define param_rsprite	((bound_sprite_t*)this->paramp[1])
// accel as 1/8 of a pixel
#define param_accelx	((signed char)this->paramc[4])
#define param_accely	((signed char)this->paramc[5])
#define param_col		((collision_box_t*)this->paramp[3])

#define colparam_accelx	((signed char)currentColEntity->paramc[4])
#define colparam_accely	((signed char)currentColEntity->paramc[5])

void UpdateBall(entity_t* this);
void DestroyBall(entity_t* this);
void CollideBall(collision_box_t* this, collision_box_t* other, int x, int y);

// Variables for the collision test
entity_t* currentColEntity;
int ofsX, ofsY;
//char useCH = 0;
//sfx_channel_t const channels[4] = {SFX_CH0, SFX_CH1, SFX_CH2, SFX_CH3};

void ent_Ball(entity_t* this, va_list args) {
	collision_box_t* col;
	bound_sprite_t* ls, * rs;

	this->onDestroy = DestroyBall;

	ls = map_AllocBoundSprite();
	if(!ls) {
		e_Destroy(this);
		return;
	}
	param_lsprite = ls;

	rs = map_AllocBoundSprite();
	if(!rs) {
		e_Destroy(this);
		return;
	}
	param_rsprite = rs;

	col = col_AllocBox(false, va_arg(args, int), va_arg(args, int), 16, 16);
	if(!col) {
		e_Destroy(this);
		return;
	}
	param_col = col;

	param_accelx = 0;
	param_accely = 0;

	ls->x = col->x;
	ls->y = col->y;
	ls->sprite->tile = SPR_BALL_L; // ball
	ls->sprite->attrib = crand(0, 3); // random color
	rs->x = col->x + 8;
	rs->y = col->y;
	rs->sprite->tile = SPR_BALL_R; // ball
	rs->sprite->attrib = ls->sprite->attrib; // same color

	col->onCollide = CollideBall;

	this->onUpdate = UpdateBall;
}

void DestroyBall(entity_t* this) {
	bound_sprite_t* ls = param_lsprite;
	bound_sprite_t* rs = param_rsprite;

	if(ls)
		map_FreeBoundSprite(ls);
	if(rs)
		map_FreeBoundSprite(rs);
	if(param_col)
		col_FreeBox(param_col);
}

void UpdateBall(entity_t* this) {
	bound_sprite_t* ls = param_lsprite;
	bound_sprite_t* rs = param_rsprite;
	collision_box_t* col = param_col;

	input_t i = i_GetStandardInput(INPUT_PLAYER_0);

	if((i & INPUT_LEFT) && param_accelx >= -ACCEL_LIMIT - 1) {
		param_accelx -= 4;
	}
	if((i & INPUT_RIGHT) && param_accelx < ACCEL_LIMIT) {
		param_accelx += 4;
	}
	if((i & INPUT_UP) && param_accely >= -ACCEL_LIMIT - 1) {
		param_accely -= 4;
	}
	if((i & INPUT_DOWN) && param_accely < ACCEL_LIMIT) {
		param_accely += 4;
	}

	// gravity
	param_accely += 3;

	// Contact acceleration will be updated in collision code
	currentColEntity = this;
	ofsX = ofsY = 0;
	col_Test(col);
	g_Yield();
	map_TestColBox(col);
	g_Yield();

	// camera
	if(ls->y < map_refY - 40) {
		map_MoveTo(ls->x, MAX(120, ls->y + 40));
	} else if(ls->y >= map_refY + 40) {
		map_MoveTo(ls->x, MIN(map_height * 8 - 120, ls->y - 40));
	}

	// update sprites -- map_UpdateSprite must be called after camera update
	col->x += ofsX + param_accelx / 8;
	col->y += ofsY + param_accely / 8;
	col->right = col->x + 16;
	col->bottom = col->y + 16;
	rs->x = (ls->x = col->x) + 8;
	rs->y = ls->y = col->y;
	map_UpdateSprite(ls);
	map_UpdateSprite(rs);
}

void CollideBall(collision_box_t*, collision_box_t* other, int nx, int ny) {
	decimal_t accelx = ((decimal_t)colparam_accelx) * 2;
	decimal_t accely = ((decimal_t)colparam_accely) * 2;
	decimal_t dnx, dny;

	// hitting another box that has a param does not cause a reaction
	// or escape if other was deleted
	if(other && (other->param || !other->right)) {
		return;
	}

	nx -= 8;
	ny -= 8;

	// are we actually in the cirle??
	if(vleni(nx, ny) >= 8)
		return;

	// did we completely fall inside? hard reverse then
	if(!(nx || ny)) {
		colparam_accelx = accelx / -2;
		colparam_accely = accely / -2;
		return;
	}

	// knock outside of object
	if(nx)
		ofsX += nx < 0 ? nx + 8 : nx - 8;
	if(ny)
		ofsY += ny < 0 ? ny + 8 : ny - 8;

	// make precise enough to do real math on
	dnx = INT2DEC(nx);
	dny = INT2DEC(ny);

	// The actual vector math to reflect things
	normalized(&dnx, &dny);
	reflectd(&accelx, &accely, dnx, dny);
	colparam_accelx = (signed char)(accelx / (dnx / 2 + 2));
	colparam_accely = (signed char)(accely / (dny / 2 + 2));

	//s_PlaySFX(SFX_BOUNCEH, channels[useCH]);
	//if(++useCH == 4)
	//	useCH = 0;
}
