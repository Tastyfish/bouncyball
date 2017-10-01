#include "math.h"
#include "video.h"
#include "mmc5.h"
#include "entity.h"
#include "entities.h"
#include "sound.h"
#include "sounds.h"

extern const void* const NAM_BG;
extern const void* const NAM_JOKE;

extern void setup_ingame(void) {
	char i;

	vc_bg_bank = 0;

	v_WaitVBlank();
	vb_DecompressNT(0x2000, NAM_BG);
	v_WaitVBlank();
	vb_DecompressNT(0x2400, NAM_JOKE);

	e_Create(&ent_Shaker);
	e_Create(&ent_NTSmoke, 21*8, 11*8-1);
	e_Create(&ent_NTSmoke, 23*8, 11*8-1);
	e_Create(&ent_NTSmoke, 25*8, 11*8-1);

	for(i = 0; i < 4; i++) {
		e_Create(&ent_Ball, crand(8, 240), crand(8, 224));
	}

	s_PlayMusic(MUSIC_INGAME);
}