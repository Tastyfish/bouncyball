_GAME_EXPORT = 1
.include "mem.inc"
.include "video.inc"
.include "entity.inc"
.include "game.inc"
.include "mmc5.inc"

.export _g_Run

.exportzp _tickcount	:= $6B
.exportzp _VBLANK_FLAG	:= $70
.exportzp ppuscrollx	:= $77
.exportzp ppuscrolly	:= $78
.exportzp ppuctrl		:= $79

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
	sta ppuscrollx
	sta ppuscrolly
	lda #$80
	sta ppuctrl
	sta PPU_CTRL

	; setup CHR
	lda #01
	sta MMC5_CHR_MODE
	lda #00
	sta $5123
	lda #01
	sta $5127
	; setup NT
	lda #$44
	sta MMC5_NT_MAPPING

	jsr _vb_ClearOAM
	lda #$0F ; make bg black
	jsr _v_SetBGColor

	; disable HBLANK initially, until a scanline callback is set
	lda #$00
	sta MMC5_SL_COUNTER
	sta MMC5_IRQ_CTRL

	rts
.endproc

; Run the main game loop---never returns
; void g_Run()
.proc _g_Run
	sei
	jsr _vb_FullCopyOAM
	jsr _e_UpdateTick
	cli
	cmp #0
	bne _g_Run
	jsr _v_WaitVBlank
	jmp _g_Run
.endproc
