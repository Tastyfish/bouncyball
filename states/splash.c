#include "video.h"
#include "map.h"
#include "entity.h"
#include "entities.h"
#include "sound.h"
#include "sounds.h"

extern const char* const QLE_SPLASH;
extern const char* const NAM_DIRECTIONS;
extern const void* const S_MUSIC;
extern const void* const S_SFX;

extern void setup_splash();

void setup_splash() {
	s_Setup(S_MUSIC, S_SFX);

	v_SetPalette(PAL_SPR0, 0x16, 0x06, 0x36);
	v_SetPalette(PAL_SPR1, 0x12, 0x02, 0x32);
	v_SetPalette(PAL_SPR2, 0x19, 0x09, 0x39);
	v_SetPalette(PAL_SPR3, 0x14, 0x04, 0x34);

	//v_SetPalette(PAL_BG0, 0x01, 0x10, 0x20);
	//v_SetPalette(PAL_BG1, 0x01, 0x21, 0x31);
	//v_SetPalette(PAL_BG2, 0x17, 0x27, 0x37);
	//v_SetPalette(PAL_BG3, 0x01, 0x27, 0x37);

	// the start screen will make the changes into the actual game
	e_Create(&ent_StartScreen);

	v_DecompressToRAM((char*)0x5C00, NAM_DIRECTIONS);

	//vb_DecompressQLEChunk(0x2000, 0, QLE_SPLASH + 0x14);
	map_Load(QLE_SPLASH, 128, 0);
	map_SetOrientation(MO_HORIZONTAL);

	s_PlayMusic(MUSIC_OPENING);
}
