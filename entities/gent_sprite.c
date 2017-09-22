#include "video.h"
#include "gent.h"

void gent_DestroySprite(Entity* entity) {
	sprite_t* s = entity->graphic[0];
	s->y = 0xFF;
	v_FreeSprite(s);
}
