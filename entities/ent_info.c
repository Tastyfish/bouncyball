#include <stdarg.h>
#include <stdlib.h>

#include "game.h"
#include "input.h"
#include "video.h"
#include "entity.h"
#include "collision.h"
#include "xdata.h"

#include "entities.h"

#define param_col		((collision_box_t*)this->paramp[0])
#define param_msg		((const char*)this->paramp[1])
#define param_step		(this->paramc[4])

#define cparam_step		(((entity_t*)this->param)->paramc[4])

#define xparam_msg		((const char*)lastInfo->paramp[1])

void destroyInfo(entity_t* this);
void collideInfo(collision_box_t* this, collision_box_t*, int, int);
void receiveXData(const void* data);

// for receiving xdata---should be the IMMEDIATELY next entity loaded or the map
// was made incorrectly
entity_t* lastInfo;

void ent_Info(entity_t* this, va_list args) {
	int x = va_arg(args, int);
	int y = va_arg(args, int);

	collision_box_t* col;

	this->onDestroy = destroyInfo;

	EREQUIRE(param_col = col =
		col_AllocBox(true, x, y, 8, 8));

	lastInfo = this;
	xdata_SetHandler(&receiveXData);

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

void receiveXData(const void* data) {
	xparam_msg = data;
}