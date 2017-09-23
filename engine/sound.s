.export _s_Setup
.export _s_PlayMusic, _s_StopMusic, _s_PauseMusic
.export _s_PlaySFX

.export FamiToneUpdate

.include "zeropage.inc"
.include "./ext/famitone2.s"
.import popax, popa


.segment "INIT"

.interruptor update_sound, 1
.proc update_sound
	jmp FamiToneUpdate
.endproc

.code

; void __fastcall__ s_Setup(void* music, void* sfx);
.proc _s_Setup
	sta ptr1
	stx ptr1+1
	jsr popax
	stx tmp1
	ldy tmp1
	tax
	lda #1
	jsr FamiToneInit
	ldx ptr1
	ldy ptr1+1
	jmp FamiToneSfxInit
.endproc

; void __fastcall__ s_PlayMusic(char songid);
.proc _s_PlayMusic
	jmp FamiToneMusicPlay
.endproc

; void s_StopMusic();
.proc _s_StopMusic
	jmp FamiToneMusicStop
.endproc

; void __fastcall__ pauseMusic(bool pause);
.proc _s_PauseMusic
	jmp FamiToneMusicPause
.endproc

; void __fastcall__ playSFX(char sfxid, SFXChannel channel);
.proc _s_PlaySFX
	tax
	jsr popa
	jmp FamiToneSfxPlay
.endproc
