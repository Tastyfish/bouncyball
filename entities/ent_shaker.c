#include "game.h"
#include "entity.h"
#include "video.h"
#include "input.h"
#include "sound.h"
#include "sounds.h"

#include "entities.h"
#include "gent.h"


// param has base time for joke timer
// graphic_a indicates fading in
// graphic_b indicates if played joke sfx

#define param_timer_base	(this->paramu[0])
#define param_fadein		(this->paramc[2])
#define param_played_joke	(this->paramc[3])

void UpdateShaker(Entity* this);
void DrawShaker(Entity* this);

void ent_Shaker(Entity* this) {
	this->onDestroy = gent_DestroyEmpty;
	this->onUpdate = UpdateShaker;

	param_timer_base = tickcount;
	param_fadein = 1; // 1 means fading in
	param_played_joke = 0; // if played joke sfx yet

	v_FadeIn(5,
		0x01, 0x10, 0x20,
		0x01, 0x21, 0x31,
		0x17, 0x27, 0x37,
		0x01, 0x27, 0x37);
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

	if(i & INPUT_SELECT) {
		// clear the balls
		Entity* ent;
		for(ent = e_Iterate(); ent; e_IterateNext(&ent)) {
			if(ent != this)
				e_Destroy(ent);
		}
	}

	// show the joke
	if(tickcount - param_timer_base >= 60 * 15) {
		x += 256;
		if(!param_played_joke) {
			s_PlaySFX(SFX_JOKE, SFX_CH2);
			param_played_joke = 1;
		}
	}

	// shake EVERYTHING
	if(x != 0 || y != 0) {
		Entity* shakeEnt;
		for(shakeEnt = e_Iterate(); shakeEnt; e_IterateNext(&shakeEnt)) {
			// detect a gent_Sprite naively
			if(shakeEnt->paramc[1] == 0x05) {
				shakeEnt->paramc[2] += x * 4;
				shakeEnt->paramc[3] += y * 4;
			}
		}
	}
	v_BigScrollBackground(x, y);

	if(param_fadein && v_FadeStep())
		param_fadein = 0;
}
