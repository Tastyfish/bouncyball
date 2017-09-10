#include <stdbool.h>
#include <stdlib.h>
#include "game.h"
#include "video.h"
#include "entity.h"

extern void g_Init();

void CreateBall(Entity* entity);
void DestroyBall(Entity* entity);
void UpdateBall(Entity* entity);

void main() {
	int i;

	g_Init();
	v_WaitVBlank();

	v_SetPalette(PAL_SPR0, 0x16, 0x06, 0x36);
	v_SetPalette(PAL_SPR1, 0x12, 0x02, 0x32);
	v_SetPalette(PAL_SPR2, 0x19, 0x09, 0x39);
	v_SetPalette(PAL_SPR3, 0x17, 0x07, 0x37);

	for(i = 0; i < 32; i++) {
		e_Create(&CreateBall);
	}

	v_EnableSprites(true);

	while(true) {
		v_WaitVBlank();

		v_FullCopyOAM();
		e_DrawTick();

		e_UpdateTick();
	}
}

void CreateBall(Entity* entity) {
	Sprite* s;
	spriteID_t sprid = v_AllocSprite();
	if(sprid == 0xFF)
		return;

	entity->a = sprid;
	entity->b = 0;

	s = SpriteIDToAddress(sprid);
	s->x = crand(1, 248);
	s->y = crand(8, 224);
	s->tileID = 0; // ball
	s->attrib = crand(0, 3); // random color

	entity->onDestroy = DestroyBall;
	entity->onUpdate = UpdateBall;
}

void DestroyBall(Entity* entity) {
	spriteID_t sprid = entity->a;
	SpriteIDToAddress(sprid)->y = 0xFF;
	v_FreeSprite(sprid);
}

void UpdateBall(Entity* entity) {
	Sprite* s = SpriteIDToAddress(entity->a);

	// movement
	if(entity->b & 1)
		s->x--;
	else
		s->x++;

	if(entity->b & 2)
		s->y--;
	else
		s->y++;

	// walls
	if(s->x <= 1) {
		entity->b &= ~1;
	} else if(s->x >= 248) {
		entity->b |= 1;
	}
	if(s->y <= 8) {
		entity->b &= ~2;
	} else if(s->y >= 224) {
		entity->b |= 2;
	}
}
