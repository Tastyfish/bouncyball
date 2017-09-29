#include <stdarg.h>
#include <stdbool.h>

#ifndef _ENTITY_H
#define _ENTITY_H

struct Entity_s;
typedef void (*entity_ctor_t)(struct entity_s* this, va_list args);
typedef void (*entity_cb_t)(struct entity_s* this);

typedef struct entity_s {
	entity_ctor_t type; // the constructor that created it, for identification
	entity_cb_t onDestroy; // called by e_Destroy()
	entity_cb_t onUpdate; // called at most once a frame, less if there's too much work
	union param {
		char paramc[8];
		int parami[4];
		int paramu[4];
		void* paramp[4];
	};
} entity_t;


extern entity_t* e_Create(entity_ctor_t ctor, ...);
extern void e_Destroy(entity_t* entity);

extern bool e_UpdateTick(void);

extern entity_t* e_Iterate(void);
extern void e_IterateNext(entity_t** e);

#endif
