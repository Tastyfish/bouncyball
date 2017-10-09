_GAME_EXPORT = 1
.include "mem.inc"
.include "video.inc"
.include "entity.inc"
.include "game.inc"
.include "mmc5.inc"
.include "mmc5zp.inc"

.import _vbm_SetNametableMirroring
.export _g_Run, _g_Yield

.exportzp _tickcount	:= $6B

.segment "ONCE"
.constructor _g_Init, 20
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
	sta _tickcount
	sta _tickcount+1
	sta VBLANK_FLAG
	sta _vc_scrollx
	sta _vc_scrolly
	lda #$A0 ; enable nmi & tall sprites
	sta _vc_ctrl
	sta PPU_CTRL

	; setup CHR
	lda #01
	sta _mmc5_chr_mode
	lda #00
	sta _vc_chr_lbank
	sta _vc_chr_ubank
	sta _vc_bg_bank
	; setup NT as diagnal mirroring
	lda #%00010100
	jsr _vbm_SetNametableMirroring

	jsr _vb_ClearOAM
	lda #$0F ; make bg black
	jsr _v_SetBGColor

	; disable HBLANK initially, until a scanline callback is set
	lda #$00
	sta _mmc5_sl_counter
	sta _mmc5_irq_ctrl

	rts
.endproc

.segment "LOWCODE"

; Run the main game loop---never returns
; void g_Run()
.proc _g_Run
	sei
	jsr _e_UpdateTick
	cli
	cmp #0
	bne _g_Run
	jsr _v_WaitVBlank
	jmp _g_Run
.endproc

; Let IRQ's fire
; void g_Sleep(void)
.proc _g_Yield
	cli
	.repeat 3
		nop
	.endrep
	sei
	rts
.endproc
