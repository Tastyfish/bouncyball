#include <stdbool.h>
#include <stdlib.h>

#include "game.h"
#include "video.h"
#include "entity.h"
#include "entities.h"

extern const char NAM_JOKE;
extern const char NAM_DIRECTIONS;
const char* const pNAM_JOKE = &NAM_JOKE;
const char* const pNAM_DIRECTIONS = &NAM_DIRECTIONS;

void main() {
	v_SetPalette(PAL_SPR0, 0x16, 0x06, 0x36);
	v_SetPalette(PAL_SPR1, 0x12, 0x02, 0x32);
	v_SetPalette(PAL_SPR2, 0x19, 0x09, 0x39);
	v_SetPalette(PAL_SPR3, 0x14, 0x04, 0x34);

	v_SetPalette(PAL_BG0, 0x01, 0x10, 0x20);
	v_SetPalette(PAL_BG1, 0x01, 0x21, 0x31);
	v_SetPalette(PAL_BG2, 0x17, 0x27, 0x37);
	v_SetPalette(PAL_BG3, 0x01, 0x27, 0x37);

	// the start screen will make the changes into the actual game
	e_Create(&ent_StartScreen);

	v_WaitVBlank();
	vb_DecompressNT(0x2000, pNAM_DIRECTIONS);
	v_WaitVBlank();
	vb_DecompressNT(0x2400, pNAM_JOKE);
	vb_EnableSprites(true);
	vb_EnableBackgrounds(true);

	g_Run();
}
