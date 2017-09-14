#include "game.h"
#include "video.h"
#include "input.h"
#include "entity.h"
#include "entities.h"
#include "gent.h"

extern const char NAM_BG;
const char* const pNAM_BG = &NAM_BG;

// param_a will contain whether input is enabled
// param_b is fading

void UpdateStart(Entity*);

void ent_StartScreen(Entity* this) {
	spriteID_t sprID = v_AllocSprite();
	Sprite* sprite;

	if(sprID != 0) {
		// We needed SPRITE 0 for the split screen!!
		__asm__("brk");
	}
	sprite = SpriteIDToAddress(sprID);

	sprite->x = 0;
	sprite->y = 21 * 8 + 6;
	sprite->tileID = 0x50;
	sprite->attrib = 0;

	this->onDestroy = gent_DestroySprite;
	this->onUpdate = UpdateStart;
	this->graphic = (int)sprite;
	this->param_a = 0;

	v_FadeIn(5,
		0x01, 0x10, 0x20,
		0x01, 0x21, 0x31,
		0x17, 0x27, 0x37,
		0x01, 0x27, 0x37);
	this->param_b = 1;
}

void UpdateStart(Entity* this) {
	char ppustat;

	if(this->param_a) {
		input_t i = i_GetStandardInput(INPUT_PLAYER_0);
		if(i & INPUT_START) {
			// massive state change---but after a fade
			v_FadeOut(5,
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
					vb_DecompressNT(0x2000, pNAM_BG);

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

	do {
		__asm__("lda %w\nldy #%o\nsta (sp),y", 0x2002, ppustat);
	} while((ppustat & 0x40) == 0);

	__asm__("lda #%w\nsta %w", 0x90, 0x2000);
}
