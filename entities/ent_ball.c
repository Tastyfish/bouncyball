#include "game.h"
#include "entity.h"
#include "video.h"

#include "ent_ball.h"
#include "gent.h"

#define PF_MOVE_LEFT (1)
#define PF_MOVE_UP (2)
#define PF_MOVE_MASK (PF_MOVE_LEFT|PF_MOVE_UP)

void UpdateBall(Entity* entity);

void ent_Ball(Entity* entity) {
	Sprite* s;
	spriteID_t sprid = v_AllocSprite();
	if(sprid == 0xFF)
		return;

	entity->a = sprid;
	entity->b = crand(0, 3);

	s = SpriteIDToAddress(sprid);
	s->x = crand(1, 248);
	s->y = crand(8, 224);
	s->tileID = 0; // ball
	s->attrib = crand(0, 3); // random color

	entity->onDestroy = gent_DestroySprite;
	entity->onUpdate = UpdateBall;
}

void UpdateBall(Entity* entity) {
	Sprite* s = SpriteIDToAddress(entity->a);

	// movement
	if(entity->b & PF_MOVE_LEFT) {
		s->x--;

		if(s->x <= 1)
			entity->b &= ~PF_MOVE_LEFT;
	} else {
		s->x++;

		if(s->x >= 248)
			entity->b |= PF_MOVE_LEFT;
	}

	if(entity->b & PF_MOVE_UP) {
		s->y--;

		if(s->y <= 8)
			entity->b &= ~PF_MOVE_UP;
	} else {
		// acceleration in the upper nibble of B
		s->y += (entity->b >> 2);

		if(entity->b < 0xFC)
			entity->b += 4;

		if(s->y >= 224) {
			entity->b |= PF_MOVE_UP;
			entity->b &= PF_MOVE_MASK;
		}
	}
}
