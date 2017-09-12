#include <stddef.h>
#include <stdbool.h>
#include <string.h>
#include "game.h"
#include "entity.h"

#define NUM_ENTITIES 64

Entity entity_table[NUM_ENTITIES];

#define ENTITY_TABLE_END (entity_table + NUM_ENTITIES)

// Hard wipe all entities, will probably mess things up mid-game
void e_Reset() {
	bzero(entity_table, sizeof(entity_table));
}

// Create an entity with given ctor
// Returns the new entity, or null if no slots left
Entity* e_Create(EntityCallback ctor) {
	Entity* ent;

	for(ent = entity_table; ent < ENTITY_TABLE_END; ent++) {
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
	static Entity* currentEntity = entity_table;

	int currentTick = tickcount;
	Entity* startingEntity = currentEntity;
	EntityCallback update;

	// go until vblank -- that's drawing time!
	while(currentTick == tickcount) {
		update = currentEntity->onUpdate;
		if(update)
			update(currentEntity);

		// loop around
		if(++currentEntity == ENTITY_TABLE_END) {
			currentEntity = entity_table;
		}

		// stop short of updating the same entities twice in the same frame
		if(currentEntity == startingEntity)
			return false;
	}

	return true;
}

// Check collisions
Entity* e_Collide(int x, int y) {
	Entity* currentEntity;
	CollideCallback collide;
	for(currentEntity = entity_table; currentEntity < ENTITY_TABLE_END; ++currentEntity) {
		collide = currentEntity->onCollide;
		if(collide)
			if(collide(currentEntity, x, y))
				return currentEntity;
	}

	return NULL;
}

Entity* e_Iterate() {
	return entity_table;
}

Entity* e_IterateNext(Entity** e) {
	if(++(*e) == ENTITY_TABLE_END)
		*e = NULL;
}
