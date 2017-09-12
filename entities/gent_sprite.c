#include "video.h"
#include "gent.h"

void gent_DestroySprite(Entity* entity) {
	Sprite* s = (Sprite*)entity->graphic;
	s->y = 0xFF;
	v_FreeSprite(AddressToSpriteID(s));
}
