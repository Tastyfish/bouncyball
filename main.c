#include <stdbool.h>
#include <stdlib.h>
#include "game.h"
#include "video.h"
#include "entity.h"
#include "ent_ball.h"

extern void g_Init();

void main() {
	int i;

	g_Init();
	v_WaitVBlank();

	v_SetPalette(PAL_SPR0, 0x16, 0x06, 0x36);
	v_SetPalette(PAL_SPR1, 0x12, 0x02, 0x32);
	v_SetPalette(PAL_SPR2, 0x19, 0x09, 0x39);
	v_SetPalette(PAL_SPR3, 0x17, 0x07, 0x37);

	for(i = 0; i < 64; i++) {
		e_Create(&ent_Ball);
	}

	v_EnableSprites(true);

	while(true) {
		v_FullCopyOAM();
		//e_DrawTick();

		if(!e_UpdateTick())
			v_WaitVBlank();
	}
}
