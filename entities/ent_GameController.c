#include "game.h"
#include "math.h"
#include "entity.h"
#include "video.h"
#include "input.h"
#include "sound.h"
#include "collision.h"
#include "map.h"

#include "sounds.h"
#include "entities.h"
#include "gent.h"

extern const palset_t* const PAL_WATERSHIP;

#define param_score			(this->paramu[0])
#define param_fadein		(this->paramc[2])
#define param_y				(this->parami[2])

void UpdateGC(entity_t* this);

void ent_GameController(entity_t* this, va_list) {
	this->onUpdate = UpdateGC;

	param_score = 0;
	param_fadein = 1; // 1 means fading in
	param_y = 0;

	// These will leak, but also the game never leaves here without reseting
	col_AllocBox(true, -16, -16, 32, 272);
	col_AllocBox(true, -16, -16, 288, 32);
	col_AllocBox(true, 240, -16, 32, 272);
	col_AllocBox(true, -16, 224, 288, 32);

	v_FadeIn(5, PAL_WATERSHIP);
}

void UpdateGC(entity_t* this) {
	input_t i = i_GetStandardInput(INPUT_PLAYER_0);

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

	if(param_fadein && v_FadeStep())
		param_fadein = 0;
}
