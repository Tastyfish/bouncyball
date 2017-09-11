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
	entity->onDraw = DrawShaker;
}

void UpdateShaker(Entity* entity) {
	input_t i = i_GetStandardInput(INPUT_PLAYER_0);

	if(i & INPUT_A) {
		entity->a += (tickcount % 5) - 2;
	} else {
		entity->a = 0;
	}
}

void DrawShaker(Entity* entity) {
	v_BigScrollBackground(entity->a, 0);
}
