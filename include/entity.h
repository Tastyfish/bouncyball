#include <stdarg.h>
#include <stdbool.h>
#include "video.h"
#include "map.h"

#ifndef _ENTITY_H
#define _ENTITY_H

struct Entity_s;
typedef void (*EntityCtor)(struct Entity_s* this, va_list args);
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
		sprite_t* graphic[4]; // getting a bit specific, but extremely common
		bound_sprite_t* boundGraphic[4];
	};
} Entity;


extern Entity* e_Create(EntityCtor ctor, ...);
extern void e_Destroy(Entity* entity);

extern bool e_UpdateTick(void);
extern Entity* e_Collide(int x, int y);

extern Entity* e_Iterate(void);
extern void e_IterateNext(Entity** e);

#endif
