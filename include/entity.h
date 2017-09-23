#include <stdarg.h>
#include <stdbool.h>
#include "video.h"
#include "map.h"

#ifndef _ENTITY_H
#define _ENTITY_H

struct Entity_s;
typedef void (*entity_ctor_t)(struct entity_s* this, va_list args);
typedef void (*entity_cb_t)(struct entity_s* this);

typedef struct entity_s {
	entity_cb_t onDestroy;
	entity_cb_t onUpdate;
	union param {
		char paramc[8];
		int parami[4];
		int paramu[4];
		void* paramp[4];
		sprite_t* graphic[4]; // getting a bit specific, but extremely common
		bound_sprite_t* boundGraphic[4];
	};
} entity_t;


extern entity_t* e_Create(entity_ctor_t ctor, ...);
extern void e_Destroy(entity_t* entity);

extern bool e_UpdateTick(void);

extern entity_t* e_Iterate(void);
extern void e_IterateNext(entity_t** e);

#endif
