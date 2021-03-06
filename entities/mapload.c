#include <stddef.h>
#include "map.h"
#include "entities.h"

extern const mapload_entity_t mapload_entities[] = {
	{0x01, ent_Ball},
	{0x02, ent_Coin},
	{0x04, ent_Bumper},
	//{0x07, ent_UpBumper},
	//{0x0A, ent_PushUp},
	{0x0E, ent_Slope},
	{0x1E, ent_Slope},
	{0x2A, ent_Info},
	{0x2E, ent_Slope},
	{0x44, ent_Bumper},
	{0x48, ent_Smasher},
	{0x4A, ent_MapSmoke},

	{0x60, ent_XData},
	{0x61, ent_XData},
	{0x62, ent_XData},
	{0x63, ent_XData},
	{0x70, ent_XData},
	{0x71, ent_XData},
	{0x72, ent_XData},
	{0x73, ent_XData},
};

extern const size_t mapload_entity_count = sizeof mapload_entities / sizeof(mapload_entity_t);
