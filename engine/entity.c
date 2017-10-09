#include <stddef.h>
#include <stdbool.h>
#include <string.h>
#include <stdarg.h>
#include "game.h"
#include "entity.h"

#define NUM_ENTITIES 64

entity_t entity_table[NUM_ENTITIES];

#define ENTITY_TABLE_END (entity_table + NUM_ENTITIES)

// Create an entity with given ctor
// Returns the new entity, or null if no slots left
entity_t* e_Create(entity_ctor_t ctor, ...) {
	entity_t* ent = entity_table;
	va_list args;

	for(; ent < ENTITY_TABLE_END; ++ent) {
		if(!ent->type) {
			// found a free one
			ent->type = ctor;
			va_start(args, ctor);
			ctor(ent, args);
			va_end(args);
			return ent;
		}
	}

	// didn't find one
	return NULL;
}

// Destroy an entity, calls OnDestroy()
void e_Destroy(entity_t* entity) {
	entity_cb_t destroy = entity->onDestroy;
	if(destroy) {
		destroy(entity);
	}
	bzero(entity, sizeof(entity_t));
}

// Update as much as we can this round
// True if was interrupted by VBlank, false if just ran out of work
bool e_UpdateTick(void) {
	static entity_t* currentEntity = entity_table;

	entity_t* startingEntity = currentEntity;
	entity_cb_t update;

	VBLANK_FLAG = 0;

	// go until vblank -- that's drawing time!
	while(!VBLANK_FLAG) {
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

		// let IRQ's tick
		g_Yield();
	}

	return true;
}

entity_t* e_Iterate(void) {
	return entity_table;
}

void e_IterateNext(entity_t** e) {
	do {
		if(++(*e) == ENTITY_TABLE_END) {
			*e = NULL;
			return;
		}
	} while(!(*e)->type);
}
