_VIDEO_EXPORT = 1
.include "helpers.inc"
.include "mem.inc"
.include "video.inc"

.export _v_ClearOAM, _v_FullCopyOAM, _v_CopySprite, _v_CopyOAM
.export _v_CopyPPU
.export _v_SetBGColor, _v_SetPalette
.export _v_WaitVBlank
.export _v_DisableAll, _v_EnableSprites, _v_EnableBackgrounds

.bss
	ppuMaskCache: .byte 0

.code

; Clear the entire OAM cache and data
; void v_ClearOAM()
.proc _v_ClearOAM
	; fill OAM swap with FF's and copy (disable sprite)
	lda #$00
	sta addr1
	lda #$02
	sta addr1+1
	lda #$FF

	ldy #$00
loop:
	sta (addr1),y
	iny
	bne loop

	; tail call
	jmp _v_FullCopyOAM
.endproc

; Fully copy OAM cache to PPU
; void v_FullCopyOAM()
.proc _v_FullCopyOAM
	lda #$00
	sta OAM_ADDR
	lda #$02
	sta OAM_DMA

	rts
.endproc

; Copy a sprite by ID from cache to PPU OAM
; __fastcall__ void v_CopySprite(byte id)
.proc _v_CopySprite
	; Was originally a wrapper tail-calling _v_CopyOAM but the overhead for that was crazy

	asl
	asl
	sta OAM_ADDR

	sta addr1
	lda #$02
	sta addr1+1

	ldy #$00
loop:
	lda (addr1),y
	sta OAM_DATA
	iny
	cpy #.sizeof(Sprite)
	bne loop

	rts
.endproc

; Copy part of OAM cache to PPU
; __fastcall__ void v_CopyOAM(byte start, byte length)
.proc _v_CopyOAM
start_addr	= 0
	sta sreg ; length here

	; copy $0200+start_addr here as copy SRC
	ldy #start_addr
	lda (sp),y
	sta addr1
	ldy #02
	sty addr1+1

	; set PPU dest
	sta OAM_ADDR

	; copy data
	ldy #$00
loop:
	lda (addr1),y
	sta OAM_DATA
	iny

	cpy sreg
	bne loop

	ldy #1
	jmp addysp
.endproc

; Copy part of PPU cache to PPU
; void v_CopyPPU(void* start, int length)
.proc _v_CopyPPU
length	= 1
start	= 3
	; start -> addr1
	ldy #start
	lda (sp),y
	sta addr1
	dey
	lda (sp),y
	sta addr1+1

	; set PPU dest
	lda PPU_STATUS
	lda addr1+1
	sta PPU_ADDR
	lda addr1
	sta PPU_ADDR

	; set RAM src
	clc
	lda addr1+1
	adc #$03
	sta addr1+1
	; also set up length as end (length += start + $0200)
	clc
	ldy #length
	lda (sp),y
	adc addr1
	sta (sp),y
	dey
	lda (sp),y
	adc addr1+1
	adc #$02
	sta (sp),y
	; copy data
	ldy #$00
loop:
	; copy data
	lda (addr1),y
	sta PPU_DATA
	inc16 addr1

	; check if addr==end
	lda addr1
	ldy #length
	cmp (sp),y
	bne loop
	lda addr1+1
	dey
	cmp (sp),y
	bne loop

	ldy #4
	jmp addysp
.endproc

; Set the global BG color
; __fastcall__ void v_SetBGColor(byte color)
.proc _v_SetBGColor
	pha

	lda PPU_STATUS
	lda #$3F
	sta PPU_ADDR
	lda #$00
	sta PPU_ADDR

	pla
	sta PPU_DATA

	rts
.endproc

; Set the palette color triplet at palette ID #
; __fastcall__ void v_SetPalette(byte pal_id, byte col_1, byte col_2, byte col_3)
.proc _v_SetPalette
pal_id = 2
col_1  = 1
col_2  = 0
	pha ; col_3

	lda PPU_STATUS
	lda #$3F
	sta PPU_ADDR
	ldy #pal_id
	lda (sp),y
	sta PPU_ADDR

	dey
	lda (sp),y
	sta PPU_DATA
	dey
	lda (sp),y
	sta PPU_DATA
	pla
	sta PPU_DATA

	ldy #3
	jmp addysp
.endproc

; Wait for a v blank
; void v_WaitVBlank()
.proc _v_WaitVBlank
	lda tickcount
loop:
	cmp tickcount
	beq loop
	rts
.endproc

; Clear the entire PPU mask
; void v_DisableAll()
.proc _v_DisableAll
	lda #$00
	sta ppuMaskCache
	sta PPU_MASK
	rts
.endproc

; Enable or disable sprites
; void __fastcall__ v_EnableSprites(bool enable)
.proc _v_EnableSprites
	cmp #0
	bne enable
	; disable

	lda ppuMaskCache
	and #(~(PPUM_SPRITES | PPUM_EDGE_SPRITES))&$FF
	bne end

enable:
	lda ppuMaskCache
	ora #PPUM_SPRITES | PPUM_EDGE_SPRITES
	sta ppuMaskCache

end:
	sta ppuMaskCache
	sta PPU_MASK
	rts
.endproc

; Enable or disable background
; void __fastcall__ v_EnableBackgrounds(bool enable)
.proc _v_EnableBackgrounds
	cmp #0
	bne enable
	; disable

	lda ppuMaskCache
	and #(~(PPUM_BACKGROUNDS | PPUM_EDGE_BACKGROUNDS))&$FF
	bne end

enable:
	lda ppuMaskCache
	ora #PPUM_BACKGROUNDS | PPUM_EDGE_BACKGROUNDS
	sta ppuMaskCache

end:
	sta ppuMaskCache
	sta PPU_MASK
	rts
.endproc
