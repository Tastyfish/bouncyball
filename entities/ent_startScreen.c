#include "game.h"
#include "video.h"
#include "mmc5.h"
#include "map.h"
#include "input.h"
#include "entity.h"

extern void setup_ingame();
extern const palset_t* const PAL_WATERSHIP;

// paramc_x contains camera x coord
// paramc_state is:
// --0 means input enabled
// --1 means waiting for fade in
// --2 means waiting for fade out
#define param_scanline_callback	(this->paramp[0])
#define param_x					(this->paramc[2])
#define param_state				(this->paramc[3])

void UpdateStart(entity_t*);
void Destroy(entity_t*);
void onSplitscreen(unsigned char y);

void ent_StartScreen(entity_t* this, va_list) {
	this->onDestroy = Destroy;
	this->onUpdate = UpdateStart;
	param_scanline_callback = (void*)vm_AddScanlineCallback(21 * 8, onSplitscreen);
	param_x = 0;

	v_FadeIn(5, PAL_WATERSHIP);
	param_state = 1;
}

void Destroy(entity_t* this) {
	vm_RemoveScanlineCallback((HScanlineCB)param_scanline_callback);
}

extern char mmc5_nt_mapping;
extern char mmc5_bg_bank;

void onSplitscreen(unsigned char) {
	// Directly talking to hardware to allow things to reset to normal on next frames
	// move HUD onto screen
	mmc5_nt_mapping = 0xAA;
	// Set BG bank to characters
	mmc5_bg_bank = 0x01;
	// Force hardware scroll to 0,0
	__asm__("bit $2002");
	*(char*)(0x2005) = 0;
	*(char*)(0x2005) = 0;
}

void UpdateStart(entity_t* this) {
	if(!param_state) {
		input_t i = i_GetStandardInput(INPUT_PLAYER_0);
		if(i & INPUT_START) {
			// massive state change---but after a fade
			v_FadeOut(2);
			param_state = 2;
		}

		if(param_x < 255)
			map_MoveTo(param_x++ + 128, 0);
	} else {
		if(v_FadeStep()) {
			entity_t* ent;
			switch(param_state) {
				case 2:
					// the big switch to the real game
					for(ent = e_Iterate(); ent; e_IterateNext(&ent)) {
						e_Destroy(ent);
					}

					setup_ingame();
					return;
				default:
					param_state = 0;
					break;
			}
		}
	}
}
