#include "video.h"
#include "map.h"
#include "entity.h"
#include "entities.h"
#include "sound.h"
#include "sounds.h"

extern const char* const QLE_SPLASH;
extern const char* const NAM_DIRECTIONS;
extern const void* const S_MUSIC;
extern const void* const S_SFX;
extern const palset_t* const PAL_BALLS;

extern void setup_splash(void) {
	s_Setup(S_MUSIC, S_SFX);

	v_CopySpritePalset(PAL_BALLS);

	// the start screen will make the changes into the actual game
	e_Create(&ent_StartScreen);

	e_Create(&ent_MapSmoke, 47*8, 9*8-1);
	e_Create(&ent_MapSmoke, 49*8, 9*8-1);
	e_Create(&ent_MapSmoke, 51*8, 9*8-1);

	v_DecompressToRAM((char*)0x5C00, NAM_DIRECTIONS);

	map_Load(QLE_SPLASH, 128, 0);
	map_SetOrientation(MO_HORIZONTAL);

	s_PlayMusic(MUSIC_OPENING);
}
