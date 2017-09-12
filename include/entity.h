#include <stdbool.h>

#ifndef _ENTITY_H
#define _ENTITY_H

struct Entity_s;
typedef void (*EntityCallback)(struct Entity_s* this);
typedef bool (*CollideCallback)(struct Entity_s* this, int x, int y);

typedef struct Entity_s {
	EntityCallback onDestroy;
	EntityCallback onUpdate;
	CollideCallback onCollide;
	union param {
		int full;
		struct half {
			unsigned char a;
			unsigned char b;
		};
	};
} Entity;


extern void e_Reset();
extern Entity* e_Create(EntityCallback ctor);
extern void e_Destroy(Entity* entity);

extern bool e_UpdateTick();

extern Entity* e_Collide(int x, int y);

#endif
