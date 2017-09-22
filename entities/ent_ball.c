#include "game.h"
#include "entity.h"
#include "video.h"
#include "sound.h"

#include "tiles.h"
#include "sounds.h"

#include "entities.h"
#include "gent.h"

#define param_sprite	(this->graphic[0])
// accel as 1/8 of a pixel
#define param_accelx	(this->paramc[2])
#define param_accely	(this->paramc[3])

void UpdateBall(Entity* this);

void ent_Ball(Entity* this) {
	Sprite* s;
	spriteID_t sprid = v_AllocSprite();
	if(sprid == 0xFF)
		return;
	s = SpriteIDToAddress(sprid);

	param_sprite = s;
	param_accelx = crand(-32, 32); // random direction
	param_accely = crand(-32, 32); // random direction

	s->x = crand(8, 240);
	s->y = crand(8, 224);
	s->tile = SPR_BALL; // ball
	s->attrib = crand(0, 3); // random color

	this->onDestroy = gent_DestroySprite;
	this->onUpdate = UpdateBall;
}

void UpdateBall(Entity* this) {
	Sprite* s = param_sprite;

	// get accel values, also gravity
	signed char accelX = param_accelx;
	signed char accelY = param_accely + 2;

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
	param_accelx = accelX;
	param_accely = accelY;

	// dV
	s->x += (int)accelX / 8;
	s->y += (int)accelY / 8;
}
