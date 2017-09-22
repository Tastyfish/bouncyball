#include <stdbool.h>
#include "video.h"

#ifndef _ENTITY_H
#define _ENTITY_H

struct Entity_s;
typedef void (*EntityCallback)(struct Entity_s* this);
typedef bool (*CollideCallback)(struct Entity_s* this, int x, int y);

typedef struct Entity_s {
	EntityCallback onDestroy;
	EntityCallback onUpdate;
	union param {
		char paramc[8];
		int parami[4];
		int paramu[4];
		void* paramp[4];
		Sprite* graphic[4]; // getting a bit specific, but extremely common
	};
} Entity;


extern Entity* e_Create(EntityCallback ctor);
extern void e_Destroy(Entity* entity);

extern bool e_UpdateTick();
extern Entity* e_Collide(int x, int y);

extern Entity* e_Iterate();
extern void e_IterateNext(Entity** e);

#endif
