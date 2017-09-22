#include "video.h"
#include "entity.h"
#include "entities.h"
#include "sound.h"
#include "sounds.h"

extern const char* const NAM_BG;
extern const char* const NAM_JOKE;

extern void setup_ingame(void) {
	char i;

	v_WaitVBlank();
	vb_DecompressNT(0x2000, NAM_BG);
	v_WaitVBlank();
	vb_DecompressNT(0x2400, NAM_JOKE);

	e_Create(&ent_Shaker);
	for(i = 0; i < 4; i++) {
		e_Create(&ent_Ball);
	}

	s_PlayMusic(MUSIC_INGAME);
}