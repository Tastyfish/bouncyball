#include "video.h"
#include "gent.h"

void gent_DestroySprite(Entity* entity) {
	Sprite* s = entity->graphic[0];
	s->y = 0xFF;
	v_FreeSprite(AddressToSpriteID(s));
}
