#include "math.h"
#include "video.h"
#include "mmc5.h"
#include "map.h"
#include "sound.h"

#include "entities.h"
#include "sounds.h"

extern void setup_ingame(const map_header_t* map) {
	vc_bg_bank = 0;

	map_Load(map);
	map_SetOrientation(MO_VERTICAL);

	e_Create(&ent_GameController);

	s_PlayMusic(MUSIC_INGAME);
}
