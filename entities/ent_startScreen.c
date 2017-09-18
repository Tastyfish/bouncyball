#include "game.h"
#include "video.h"
#include "mmc5.h"
#include "input.h"
#include "entity.h"
#include "entities.h"

extern const char* const NAM_BG;
extern const char* const NAM_JOKE;

// param_a will contain whether input is enabled
// param_b is fading

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
	// CHR bank
	*(char*)(0x2000) = 0x90;
	// Change NT
	mmc5_nt_mapping = 0x01;
}

void UpdateStart(Entity* this) {
	if(this->param_a) {
		input_t i = i_GetStandardInput(INPUT_PLAYER_0);
		if(i & INPUT_START) {
			// massive state change---but after a fade
			v_FadeOut(2,
				0x01, 0x10, 0x20,
				0x01, 0x21, 0x31,
				0x17, 0x27, 0x37,
				0x01, 0x27, 0x37);
			this->param_a = 0;
			this->param_b = 2;
		}
	}

	if(this->param_b) {
		if(v_FadeStep()) {
			Entity* ent;
			int i;
			switch(this->param_b) {
				case 2:
					// the big switch to the real game
					for(ent = e_Iterate(); ent; e_IterateNext(&ent)) {
						e_Destroy(ent);
					}

					v_WaitVBlank();
					vb_DecompressNT(0x2000, NAM_BG);
					v_WaitVBlank();
					vb_DecompressNT(0x2400, NAM_JOKE);

					e_Create(&ent_Shaker);
					for(i = 0; i < 4; i++) {
						e_Create(&ent_Ball);
					}
					return;
				default:
					this->param_a = 1;
					this->param_b = 0;
					break;
			}
		}
	}
}
