#include "game.h"
#include "entity.h"
#include "video.h"
#include "input.h"

#include "entities.h"
#include "gent.h"

void UpdateShaker(Entity* entity);
void DrawShaker(Entity* entity);

void ent_Shaker(Entity* entity) {
	entity->onDestroy = gent_DestroyEmpty;
	entity->onUpdate = UpdateShaker;
}

void UpdateShaker(Entity* this) {
	int x, y;

	input_t i = i_GetStandardInput(INPUT_PLAYER_0);

	if(i & (INPUT_LEFT|INPUT_RIGHT)) {
		x = (tickcount % 9) - 4;
	} else {
		x = 0;
	}
	if(i & (INPUT_UP|INPUT_DOWN)) {
		y = (tickcount % 9) - 4;
	} else {
		y = 0;
	}

	if(i & INPUT_A) {
		e_Create(&ent_Ball);
	}

	if(i & INPUT_START) {
		// clear the balls
		Entity* ent;
		for(ent = e_Iterate(); ent; e_IterateNext(&ent)) {
			if(ent != this)
				e_Destroy(ent);
		}
	}

	// show the joke
	if(tickcount >= 60 * 5) {
		x += 256;
	}

	// shake EVERYTHING
	if(x != 0 || y != 0) {
		Entity* shakeEnt;
		for(shakeEnt = e_Iterate(); shakeEnt; e_IterateNext(&shakeEnt)) {
			// detect a gent_Sprite naively
			if(shakeEnt->graphic_b == 0x05) {
				shakeEnt->param_a += x * 4;
				shakeEnt->param_b += y * 4;
			}
		}
	}
	v_BigScrollBackground(x, y);
}
