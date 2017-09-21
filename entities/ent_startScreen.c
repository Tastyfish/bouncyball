#include "game.h"
#include "video.h"
#include "mmc5.h"
#include "map.h"
#include "input.h"
#include "entity.h"

extern void setup_ingame();

// param_a contains camera x coord
// param_b is state
// --0 means input enabled
// --1 means waiting for fade in
// --2 means waiting for fade out
// graphic is scanline callback

void UpdateStart(Entity*);
void Destroy(Entity*);
void onSplitscreen(unsigned char y);

void ent_StartScreen(Entity* this) {
	this->onDestroy = Destroy;
	this->onUpdate = UpdateStart;
	this->graphic = (int)vm_AddScanlineCallback(21 * 8, onSplitscreen);
	this->param_a = 0;

	v_FadeIn(5,
		0x01, 0x10, 0x20,
		0x01, 0x21, 0x31,
		0x17, 0x27, 0x37,
		0x01, 0x27, 0x37);
	this->param_b = 1;
}

void Destroy(Entity* this) {
	vm_RemoveScanlineCallback((HScanlineCB)this->graphic);
}

extern char mmc5_nt_mapping;

void onSplitscreen(unsigned char) {
	// Directly talking to hardware to allow things to reset to normal on next frames
	// 2nd CHR bank
	*(char*)(0x2000) = 0x90;
	// move HUD onto screen
	mmc5_nt_mapping = 0xAA;
	// Force hardware scroll to 0,0
	__asm__("bit $2002");
	*(char*)(0x2005) = 0;
	*(char*)(0x2005) = 0;
}

void UpdateStart(Entity* this) {
	if(!this->param_b) {
		input_t i = i_GetStandardInput(INPUT_PLAYER_0);
		if(i & INPUT_START) {
			// massive state change---but after a fade
			v_FadeOut(2,
				0x01, 0x10, 0x20,
				0x01, 0x21, 0x31,
				0x17, 0x27, 0x37,
				0x01, 0x27, 0x37);
			this->param_b = 2;
		}

		if(this->param_a < 255)
			map_MoveTo(this->param_a++ + 128, 0);
	} else {
		if(v_FadeStep()) {
			Entity* ent;
			switch(this->param_b) {
				case 2:
					// the big switch to the real game
					for(ent = e_Iterate(); ent; e_IterateNext(&ent)) {
						e_Destroy(ent);
					}

					setup_ingame();
					return;
				default:
					this->param_b = 0;
					break;
			}
		}
	}
}
