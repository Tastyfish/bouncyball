#include <stddef.h>
#include <stdbool.h>
#include <string.h>
#include "game.h"
#include "entity.h"

#define NUM_ENTITIES 64

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

// Update as much as we can this round
// True if was interrupted by VBlank, false if just ran out of work
bool e_UpdateTick() {
	static char currentI = 0;
	static Entity* currentEntity = entity_table;

	int currentTick = tickcount;
	char startingI = currentI;

	// go until vblank -- that's drawing time!
	while(currentTick == tickcount) {
		if(currentEntity->onUpdate)
			currentEntity->onUpdate(currentEntity);

		// loop around
		++currentEntity;
		if(++currentI == NUM_ENTITIES) {
			currentI = 0;
			currentEntity = entity_table;
		}

		// stop short of updating the same entities twice in the same frame
		if(currentI == startingI)
			return false;
	}

	return true;
}

// Draw everything -- there's no way to detect the beginning of render phase,
// without clashing with other PPU ops so don't bother
// Always returns false
bool e_DrawTick() {
	char i;
	Entity* currentEntity;
	for(i = 0, currentEntity = entity_table; i < NUM_ENTITIES; ++i, ++currentEntity) {
		if(currentEntity->onDraw)
			currentEntity->onDraw(currentEntity);
	}

	return false;
}
