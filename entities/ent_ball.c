#include "game.h"
#include "entity.h"
#include "video.h"
#include "sound.h"
#include "sounds.h"

#include "entities.h"
#include "gent.h"

// sprite ptr in graphic (gent_Sprite)
// accel in param_a and param_b as 1/8 of a pixel

void UpdateBall(Entity* entity);

void ent_Ball(Entity* entity) {
	Sprite* s;
	spriteID_t sprid = v_AllocSprite();
	if(sprid == 0xFF)
		return;
	s = SpriteIDToAddress(sprid);

	entity->graphic = (int)s;
	entity->param_a = crand(-32, 32); // random direction
	entity->param_b = crand(-32, 32); // random direction

	s->x = crand(8, 240);
	s->y = crand(8, 224);
	s->tileID = 0x80; // ball
	s->attrib = crand(0, 3); // random color

	entity->onDestroy = gent_DestroySprite;
	entity->onUpdate = UpdateBall;
}

void UpdateBall(Entity* entity) {
	Sprite* s = (Sprite*)entity->graphic;

	// get accel values, also gravity
	signed char accelX = entity->param_a;
	signed char accelY = entity->param_b + 2;

	// walls
	if(
		(accelX < 0 && s->x <= 8)
		|| (accelX > 0 && s->x >= 240)) {

		// flip X accel and bump randomly
		accelX = accelX * 2 / -3 + 2;
		accelY += crand(-4, 4);

		s_PlaySFX(SFX_BOUNCEH, SFX_CH0);
	}

	if(
		(accelY < 0 && s->y <= 8)
		|| (accelY > 0 && s->y >= 220)) {

		// flip Y accel and bump randomly
		accelY = accelY * 2 / -3;
		accelX += crand(-4, 4);

		s_PlaySFX(SFX_BOUNCEL, SFX_CH1);
	}

	// write accel
	entity->param_a = accelX;
	entity->param_b = accelY;

	// dV
	s->x += (int)accelX / 8;
	s->y += (int)accelY / 8;
}
