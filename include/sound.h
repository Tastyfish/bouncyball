typedef unsigned char sfx_channel_t;
enum {
	SFX_CH0 = (sfx_channel_t)(0*15),
	SFX_CH1 = (sfx_channel_t)(1*15),
	SFX_CH2 = (sfx_channel_t)(2*15),
	SFX_CH3 = (sfx_channel_t)(3*15)
};

typedef unsigned char songid_t;

void __fastcall__ s_Setup(const void* music, const void* sfx);

void __fastcall__ s_PlayMusic(songid_t songid);
void s_StopMusic();
void __fastcall__ s_PauseMusic(bool pause);

void __fastcall__ s_PlaySFX(songid_t sfxid, sfx_channel_t channel);
