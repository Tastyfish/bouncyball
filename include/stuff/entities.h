#include "entity.h"

#define ENTITY(name) extern void (name)(Entity* this);

ENTITY(ent_Ball);
ENTITY(ent_Shaker);
ENTITY(ent_StartScreen);
ENTITY(ent_Smoke);
