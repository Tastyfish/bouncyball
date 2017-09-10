#include "video.h"
#include "gent.h"

void gent_DestroySprite(Entity* entity) {
	spriteID_t sprid = entity->a;
	SpriteIDToAddress(sprid)->y = 0xFF;
	v_FreeSprite(sprid);
}
