#include "video.h"
#include "mmc5.h"
#include "map.h"
#include "entity.h"
#include "entities.h"
#include "sound.h"
#include "sounds.h"

extern const void* const MAP_TITLE;
extern const void* const NAM_DIRECTIONS;
extern const void* const S_MUSIC;
extern const void* const S_SFX;
extern const palset_t* const PAL_BALLS;

extern void setup_splash(void) {
	s_Setup(S_MUSIC, S_SFX);

	// the start screen will make the changes into the actual game
	e_Create(&ent_StartScreen);

	e_Create(&ent_MapSmoke, 47*8, 9*8-1);
	e_Create(&ent_MapSmoke, 49*8, 9*8-1);
	e_Create(&ent_MapSmoke, 51*8, 9*8-1);

	v_CopySpritePalset(PAL_BALLS);
	vc_bg_bank = 2;

	v_DecompressToRAM((void*)0x5C00, NAM_DIRECTIONS);

	map_Load(MAP_TITLE, 128, 0);
	map_SetOrientation(MO_HORIZONTAL);

	s_PlayMusic(MUSIC_OPENING);
}
