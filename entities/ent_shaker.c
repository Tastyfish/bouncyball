#include "game.h"
#include "entity.h"
#include "video.h"
#include "input.h"

#include "entities.h"
#include "gent.h"

void UpdateShaker(Entity* entity);
void DrawShaker(Entity* entity);

void ent_Shaker(Entity* entity) {
	// A is X scroll, B is physics shake
	entity->full = 0;

	entity->onDestroy = gent_DestroyEmpty;
	entity->onUpdate = UpdateShaker;
}

void UpdateShaker(Entity* entity) {
	int x, y;

	input_t i = i_GetStandardInput(INPUT_PLAYER_0);

	if(i & INPUT_LEFT) {
		entity->a--;
	} else if(i & INPUT_RIGHT) {
		entity->a++;
	}
	if(i & INPUT_UP) {
		entity->b--;
	} else if(i & INPUT_DOWN) {
		entity->b++;
	}

	x = entity->a;
	y = entity->b;
	if(x & 0x80)
		x |= 0xFF00;
	if(y & 0x80)
		y |= 0xFF00;

	v_BigScrollBackground(x, y);
}
