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
extern const palset_t* const PAL_BALLS;
extern entity_t* game_controller_instance;

#define param_score			(this->paramu[0])
#define param_fadein		(this->paramc[2])

void UpdateGC(entity_t* this);

void ent_GameController(entity_t* this, va_list) {
	this->onUpdate = UpdateGC;

	param_score = 0;
	param_fadein = 1; // 1 means fading in

	v_FadeIn(5, PAL_WATERSHIP, PAL_BALLS);
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
