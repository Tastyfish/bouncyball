_GAME_EXPORT = 1
.include "mem.inc"
.include "video.inc"
.include "entity.inc"
.include "game.inc"

.export _g_Init

; void g_Init()
.proc _g_Init
	; turn off rendering until we setup properly
	lda #$00
	sta PPU_MASK
	
	; wait for PPU to start
	sei
	bit PPU_STATUS  ; clear the VBL flag if it was set at reset time
vwait1:
	bit PPU_STATUS
	bpl vwait1     ; at this point, about 27384 cycles have passed
vwait2:
	bit PPU_STATUS
	bpl vwait2     ; at this point, about 57165 cycles have passed
	
	; start vblank nmi
	lda #$00
	sta tickcount
	sta tickcount+1
	sta VBLANK_FLAG
	lda #$80
	sta PPU_CTRL
	
	jsr e_Reset
	
	jsr _v_ClearOAM
	lda #$0F ; make bg black
	jsr _v_SetBGColor
	
	rts
.endproc
