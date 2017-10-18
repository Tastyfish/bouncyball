#include "entity.h"

#define ENTITY(name) extern void (name)(entity_t* this, va_list args);

ENTITY(ent_Ball);
ENTITY(ent_Shaker);
ENTITY(ent_StartScreen);
ENTITY(ent_MapSmoke);
ENTITY(ent_NTSmoke);
ENTITY(ent_GameController);
ENTITY(ent_PushUp);
ENTITY(ent_Smasher);
ENTITY(ent_Coin);
ENTITY(ent_Bumper);
ENTITY(ent_UpBumper);
ENTITY(ent_Info);
ENTITY(ent_Slope);
ENTITY(ent_XData);