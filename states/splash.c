#include "video.h"
#include "mmc5.h"
#include "entity.h"
#include "entities.h"

extern const char* const QLE_SPLASH;
extern const char* const NAM_DIRECTIONS;

extern void setup_splash();

void setup_splash() {
	// figure out the QLE chunk
	const char* qle0 = (const char*)(QLE_SPLASH + *(int*)(QLE_SPLASH + 4));
	const char* qle1 = (const char*)(QLE_SPLASH + *(int*)(QLE_SPLASH + 6));
	const char* qle2 = (const char*)(QLE_SPLASH + *(int*)(QLE_SPLASH + 12));
	const char* qle3 = (const char*)(QLE_SPLASH + *(int*)(QLE_SPLASH + 14));

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

	// make mirroring horizontal
	vm_SetNametableMirroring(0xA4);
	v_DecompressToRAM((char*)0x5C00, NAM_DIRECTIONS);
	v_WaitVBlank();
	vb_DecompressQLEChunk(0x2000, 0, qle0);
	v_WaitVBlank();
	vb_DecompressQLEChunk(0x2000, 1, qle1);
	v_WaitVBlank();
	vb_DecompressQLEChunk(0x2000, 2, qle2);
	v_WaitVBlank();
	vb_DecompressQLEChunk(0x2000, 3, qle3);
}
