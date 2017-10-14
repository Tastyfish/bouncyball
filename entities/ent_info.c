#include <stdarg.h>
#include <stdlib.h>

#include "game.h"
#include "input.h"
#include "video.h"
#include "entity.h"
#include "collision.h"

#include "entities.h"

#define param_col		((collision_box_t*)this->paramp[0])
#define param_step		(this->paramc[2])

#define cparam_step		(((entity_t*)this->param)->paramc[2])

void destroyInfo(entity_t* this);
void collideInfo(collision_box_t* this, collision_box_t*, int, int);

void ent_Info(entity_t* this, va_list args) {
	int x = va_arg(args, int);
	int y = va_arg(args, int);

	collision_box_t* col;

	this->onDestroy = destroyInfo;

	EREQUIRE(param_col = col =
		col_AllocBox(true, x, y, 8, 8));

	col->onCollide = collideInfo;
	col->param = this;
}

void destroyInfo(entity_t* this) {
	collision_box_t* col = param_col;

	if(col)
		col_FreeBox(col);
}

void collideInfo(collision_box_t* this, collision_box_t*, int, int) {
	if(!cparam_step) {
		v_SetBGColor(0x31);
		cparam_step = 1;
	} else {
		v_SetBGColor(0x01);
		e_Destroy(this->param);
	}
}