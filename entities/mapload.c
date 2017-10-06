#include <stddef.h>
#include "map.h"
#include "entities.h"

extern const mapload_entity_t mapload_entities[] = {
	{0x01, ent_Ball},
	{0x02, ent_Coin},
	//{0x04, ent_LeftBumper},
	//{0x05, ent_LeftBumper},
	//{0x07, ent_UpBumper},
	{0x08, ent_MapSmoke},
	//{0x0A, ent_PushUp},
	{0x0E, ent_Slope},
	{0x1E, ent_Slope},
	{0x2E, ent_Slope},
	//{0x2A, ent_Info},
	{0x46, ent_Smasher}
};

extern const size_t mapload_entity_count = sizeof mapload_entities / sizeof(mapload_entity_t);
