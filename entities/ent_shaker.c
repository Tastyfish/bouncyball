#include "game.h"
#include "math.h"
#include "entity.h"
#include "video.h"
#include "input.h"
#include "sound.h"
#include "collision.h"

#include "sounds.h"
#include "entities.h"
#include "gent.h"

extern const palset_t* const PAL_WATERSHIP;

// param has base time for joke timer
// graphic_a indicates fading in
// graphic_b indicates if played joke sfx

#define param_timer_base	(this->paramu[0])
#define param_fadein		(this->paramc[2])
#define param_played_joke	(this->paramc[3])
#define param_shakex		(this->parami[2])
#define param_shakey		(this->parami[3])

void UpdateShaker(entity_t* this);
void DrawShaker(entity_t* this);

void ent_Shaker(entity_t* this, va_list) {
	this->onUpdate = UpdateShaker;

	param_timer_base = tickcount;
	param_fadein = 1; // 1 means fading in
	param_played_joke = 0; // if played joke sfx yet
	param_shakex = param_shakey = 0x7FFF;

	// These will leak, but also the game never leaves here without reseting
	col_AllocBox(true, -16, -16, 32, 272);
	col_AllocBox(true, -16, -16, 288, 32);
	col_AllocBox(true, 240, -16, 32, 272);
	col_AllocBox(true, -16, 224, 288, 32);

	v_FadeIn(5, PAL_WATERSHIP);
}

void UpdateShaker(entity_t* this) {
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
		e_Create(&ent_Ball, crand(8, 240), crand(8, 224));
	}

	if(i & INPUT_SELECT) {
		// clear the balls
		entity_t* ent;
		for(ent = e_Iterate(); ent; e_IterateNext(&ent)) {
			if(ent->type == &ent_Ball)
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
	if(param_shakex != x || param_shakey != y) {
		entity_t* shakeEnt;
		sprite_t* sprite;
		for(shakeEnt = e_Iterate(); shakeEnt; e_IterateNext(&shakeEnt)) {
			if(shakeEnt->type == &ent_Ball) {
				// shake balls
				shakeEnt->paramc[4] += x * 4;
				shakeEnt->paramc[5] += y * 4;
			} else if(shakeEnt->type == &ent_NTSmoke) {
				// move smoke around
				sprite = shakeEnt->paramp[0];
				sprite->x = shakeEnt->paramc[6] - x;
				sprite->y = shakeEnt->paramc[7] - y;
			}
		}
		v_BigScrollBackground(x, y);
		param_shakex = x;
		param_shakey = y;
	}

	if(param_fadein && v_FadeStep())
		param_fadein = 0;
}
