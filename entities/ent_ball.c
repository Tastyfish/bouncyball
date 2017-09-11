#include "game.h"
#include "entity.h"
#include "video.h"

#include "ent_ball.h"
#include "gent.h"

// sprite in a (gent_Sprite)
// accel in b
#define P_ACCEL_X_MASK	(0x0F)
#define P_ACCEL_Y_MASK	(0xF0)
#define ACCEL_MIN		(-8)
#define ACCEL_MAX		(7)

void UpdateBall(Entity* entity);

void ent_Ball(Entity* entity) {
	Sprite* s;
	spriteID_t sprid = v_AllocSprite();
	if(sprid == 0xFF)
		return;

	entity->a = sprid;
	entity->b = crand(0, 255); // random direction

	s = SpriteIDToAddress(sprid);
	s->x = crand(8, 240);
	s->y = crand(8, 224);
	s->tileID = 0; // ball
	s->attrib = crand(0, 3); // random color

	entity->onDestroy = gent_DestroySprite;
	entity->onUpdate = UpdateBall;
}

void UpdateBall(Entity* entity) {
	Sprite* s = SpriteIDToAddress(entity->a);

	// get accel values
	signed char accelX = (entity->b & P_ACCEL_X_MASK) - 8;
	signed char accelY = (entity->b >> 4) - 8;

	// 1/4 gravity
	if(tickcount % 5 == 0)
		accelY++;

	// walls
	if(
		(accelX < 0 && s->x <= 8)
		|| (accelX > 0 && s->x >= 240)) {

		// flip X accel
		accelX = -accelX;
	}

	if(
		(accelY < 0 && s->y <= 8)
		|| (accelY > 0 && s->y >= 220)) {

		// flip Y accel
		accelY = -accelY;
	}

	// write accel
	entity->b = (CLAMP(accelX, -8, 7) + 8) | ((CLAMP(accelY, -8, 7) + 8) << 4);

	// dV
	s->x += accelX;
	s->y += accelY;
}
