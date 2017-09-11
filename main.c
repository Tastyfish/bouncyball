#include <stdbool.h>
#include <stdlib.h>
#include "game.h"
#include "video.h"
#include "entity.h"
#include "ent_ball.h"

extern void g_Init();
extern void __fastcall__ v_DecompressNT(int ppuAddr, void* ptr);
extern char NAM_BG;

void main() {
	int i;

	g_Init();
	v_WaitVBlank();

	v_SetPalette(PAL_SPR0, 0x16, 0x06, 0x36);
	v_SetPalette(PAL_SPR1, 0x12, 0x02, 0x32);
	v_SetPalette(PAL_SPR2, 0x19, 0x09, 0x39);
	v_SetPalette(PAL_SPR3, 0x14, 0x04, 0x34);

	v_SetPalette(PAL_BG0, 0x01, 0x10, 0x20);
	v_SetPalette(PAL_BG1, 0x01, 0x21, 0x31);
	v_SetPalette(PAL_BG2, 0x17, 0x27, 0x37);
	v_SetPalette(PAL_BG3, 0x01, 0x27, 0x37);

	v_DecompressNT(0x2000, &NAM_BG);

	for(i = 0; i < 32; i++) {
		e_Create(&ent_Ball);
	}

	v_EnableSprites(true);
	v_EnableBackgrounds(true);

	while(true) {
		v_FullCopyOAM();
		//e_DrawTick();

		if(!e_UpdateTick())
			v_WaitVBlank();
	}
}
