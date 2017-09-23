#include "entity.h"

#define ENTITY(name) extern void (name)(entity_t* this, va_list args);

ENTITY(ent_Ball);
ENTITY(ent_Shaker);
ENTITY(ent_StartScreen);
ENTITY(ent_MapSmoke);
ENTITY(ent_NTSmoke);
