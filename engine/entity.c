#include <stddef.h>
#include <stdbool.h>
#include <string.h>
#include "game.h"
#include "entity.h"

#define NUM_ENTITIES 32

Entity entity_table[NUM_ENTITIES];

// Hard wipe all entities, will probably mess things up mid-game
void e_Reset() {
	bzero(entity_table, sizeof(Entity) * NUM_ENTITIES);
}

// Create an entity with given ctor
// Returns the new entity, or null if no slots left
Entity* e_Create(EntityCallback ctor) {
	int i;
	Entity* ent;

	for(i = 0; i < NUM_ENTITIES; i++) {
		ent = &entity_table[i];

		if(ent->onDestroy == NULL) {
			// found a free one
			ctor(ent);
			return ent;
		}
	}

	// didn't find one
	return NULL;
}

// Destroy an entity, calls OnDestroy()
void e_Destroy(Entity* entity) {
	if(entity->onDestroy) {
		entity->onDestroy(entity);
		bzero(entity, sizeof(entity));
	}
}

// Just call an entity's OnUpdate()
void e_Update(Entity* entity) {
	if(entity->onUpdate)
		entity->onUpdate(entity);
}

// Just call an entity's OnDraw()
void e_Draw(Entity* entity) {
	if(entity->onDraw)
		entity->onDraw(entity);
}

// Update as much as we can this round
// True if was interrupted by VBlank, false if just ran out of work
bool e_UpdateTick() {
	static int currentEntity = 0;

	int currentTick = tickcount;
	int startingEntity = currentEntity;

	// go until vblank -- that's drawing time!
	while(currentTick == tickcount) {
		e_Update(&entity_table[currentEntity]);

		// loop around
		if(++currentEntity == NUM_ENTITIES) {
			currentEntity = 0;
		}

		// stop short of updating the same entities twice in the same frame
		if(currentEntity == startingEntity)
			return false;
	}

	return true;
}

// Draw everything -- there's no way to detect the beginning of render phase,
// without clashing with other PPU ops so don't bother
// Always returns false
bool e_DrawTick() {
	int currentEntity;
	for(currentEntity = 0; currentEntity < NUM_ENTITIES; currentEntity++) {
		e_Draw(&entity_table[currentEntity]);
	}

	return false;
}
