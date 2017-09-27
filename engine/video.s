_VIDEO_EXPORT = 1
.include "helpers.inc"
.include "mem.inc"
.include "zeropage.inc"
.include "video.inc"
.include "mmc5zp.inc"

.importzp _tickcount
.import _bzero
.import tosdiva0, ppubuf_put, ppubuf_flush

.export ppuMaskCache

.export _vb_ClearOAM, _vb_FullCopyOAM, _vb_CopySprite, _vb_CopyOAM
.export _vb_CopyPPU
.export _v_SetBGColor, _v_SetPalette, _v_CopyPalette, _v_CopyBackgroundPalset, _v_CopySpritePalset
.export _v_WaitVBlank
.export _vb_DisableAll, _vb_EnableSprites, _vb_EnableBackgrounds
.export _v_ScrollBackground, _v_BigScrollBackground

.export _v_AllocSprite, _v_FreeSprite

NUM_SPRITES = 64

.bss
	ppuMaskCache:	.res 1
	sprite_atable:	.res NUM_SPRITES

.segment "INIT"

.interruptor flush_vblank, 20
.proc flush_vblank
	jsr ppubuf_flush
	jsr _vb_FullCopyOAM

	; reset the video counter
	lda #$20
	sta PPU_ADDR
	lda #$00
	sta PPU_ADDR

	; Set scroll
	lda _mmc_ctrl
	sta PPU_CTRL
	lda _mmc_scrollx
	sta PPU_SCROLL
	lda _mmc_scrolly
	sta PPU_SCROLL
	rts
.endproc

.code

; Clear the entire OAM cache and data
; void vb_ClearOAM()
.proc _vb_ClearOAM
	; fill OAM swap with FF's and copy (disable sprite)
	lda #$00
	sta ptr1
	lda #$05
	sta ptr1+1
	lda #$FF

	ldy #$00
loop:
	sta (ptr1),y
	iny
	bne loop

	; tail call
	jmp _vb_FullCopyOAM
.endproc

; Fully copy OAM cache to PPU
; void v_FullCopyOAM()
.proc _vb_FullCopyOAM
	lda #$00
	sta OAM_ADDR
	lda #$05
	sta OAM_DMA

	rts
.endproc

; Copy a sprite by ID from cache to PPU OAM
; __fastcall__ void v_CopySprite(byte id)
.proc _vb_CopySprite
	; Was originally a wrapper tail-calling _v_CopyOAM but the overhead for that was crazy

	asl
	asl
	sta OAM_ADDR

	sta ptr1
	lda #$05
	sta ptr1+1

	ldy #$00
loop:
	lda (ptr1),y
	sta OAM_DATA
	iny
	cpy #.sizeof(Sprite)
	bne loop

	rts
.endproc

; Copy part of OAM cache to PPU
; __fastcall__ void v_CopyOAM(byte start, byte length)
.proc _vb_CopyOAM
start_addr	= 0
	sta tmp1 ; length here

	; copy $0200+start_addr here as copy SRC
	ldy #start_addr
	lda (sp),y
	sta ptr1
	ldy #05
	sty ptr1+1

	; set PPU dest
	sta OAM_ADDR

	; copy data
	ldy #$00
loop:
	lda (ptr1),y
	sta OAM_DATA
	iny

	cpy tmp1
	bne loop

	jmp incsp1
.endproc

; Copy part of PPU cache to PPU
; void v_CopyPPU(void* start, int length)
.proc _vb_CopyPPU
length	= 1
start	= 3
	; start -> ptr1
	ldy #start
	lda (sp),y
	sta ptr1
	dey
	lda (sp),y
	sta ptr1+1

	; set PPU dest
	bit PPU_STATUS
	lda ptr1+1
	sta PPU_ADDR
	lda ptr1
	sta PPU_ADDR

	; set RAM src
	clc
	lda ptr1+1
	adc #$03
	sta ptr1+1
	; also set up length as end (length += start + $0200)
	clc
	ldy #length
	lda (sp),y
	adc ptr1
	sta (sp),y
	dey
	lda (sp),y
	adc ptr1+1
	adc #$02
	sta (sp),y
	; copy data
	ldy #$00
loop:
	; copy data
	lda (ptr1),y
	sta PPU_DATA
	inc16 ptr1

	; check if addr==end
	lda ptr1
	ldy #length
	cmp (sp),y
	bne loop
	lda ptr1+1
	dey
	cmp (sp),y
	bne loop

	jmp incsp4
.endproc

; Set the global BG color
; __fastcall__ void v_SetBGColor(color_t color)
.proc _v_SetBGColor
	ldy #$3F
	ldx #$00
	jmp ppubuf_put
.endproc

; Set the palette color triplet at palette ID #
; __fastcall__ void v_SetPalette(palid_t pal_id, byte col_1, byte col_2, byte col_3)
.proc _v_SetPalette
pal_id = 2
col_1  = 1
col_2  = 0
	pha ; col_3

	; ptr1 will hold PPU pos
	lda #$3F
	sta ptr1+1
	ldy #pal_id
	lda (sp),y
	sta ptr1

	; write each value out
	dey
	lda (sp),y
	ldx ptr1
	ldy ptr1+1
	jsr ppubuf_put
	inc ptr1

	ldy #col_2
	lda (sp),y
	ldx ptr1
	ldy ptr1+1
	jsr ppubuf_put
	inc ptr1

	pla
	ldx ptr1
	ldy ptr1+1
	jsr ppubuf_put

	jmp incsp3
.endproc

; Copy a palette to a hw palette
; void __fastcall__ v_CopyPalette(palid_t pal_id, pal_t source)
.proc _v_CopyPalette
	sta ptr1 ; source
	stx ptr1+1
	jsr popa
	sta tmp1; pal_id
	lda #0
	sta tmp2 ; source idx

loop:
	ldy tmp2
	lda (ptr1),y
	ldx tmp1
	ldy #$3F
	jsr ppubuf_put
	inc tmp1
	inc tmp2
	lda tmp2
	cmp #3
	bne loop

	rts
.endproc

; Copy in all the palette values for the BG from a source
; void __fastcall__ v_CopyBackgroundPalset(palset_t source);
.proc _v_CopyBackgroundPalset
	ldy #$00
	sty tmp1
	jmp internalCopyPalset
.endproc

; void __fastcall__ v_CopySpritePalset(palset_t source);
.proc _v_CopySpritePalset
	ldy #$10
	sty tmp1
	jmp internalCopyPalset
.endproc

.proc internalCopyPalset
	sta ptr1 ; source
	stx ptr1+1

	lda #0
	sta tmp2 ; src idx

loop:
	; skip BG indexes (every 0 out of 4)
	lda tmp2
	tay
	and #$03
	beq skip_write

	lda (ptr1),y
	ldx tmp1
	ldy #$3F
	jsr ppubuf_put

skip_write:
	inc tmp1
	inc tmp2
	lda tmp2
	cmp #$10
	bne loop

	rts
.endproc

; Wait for a v blank
; void v_WaitVBlank()
.proc _v_WaitVBlank
	lda _tickcount
loop:
	cmp _tickcount
	beq loop
	rts
.endproc

; Clear the entire PPU mask
; void v_DisableAll()
.proc _vb_DisableAll
	lda #$00
	sta ppuMaskCache
	sta PPU_MASK
	rts
.endproc

; Enable or disable sprites
; void __fastcall__ v_EnableSprites(bool enable)
.proc _vb_EnableSprites
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
.proc _vb_EnableBackgrounds
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

; Set the background scroll to a specific value
; void __fastcall__ v_ScrollBackground(unsigned char x, unsigned char y);
.proc _v_ScrollBackground
	sta _mmc_scrolly
	jsr popa
	sta _mmc_scrollx
	rts
.endproc

; Set the background scroll, allowing for negative values to scoll things to the right sensibly
; void __fastcall__ v_BigScrollBackground(int x, int y)
.proc _v_BigScrollBackground
	sta _mmc_scrolly
	sta tmp1
	stx tmp2
	jsr popax
	sta _mmc_scrollx

	; Set MSB
	txa
	and #1
	beq x0
	lda #$81
	bne conty
x0:
	lda #$80
conty:
	sta tmp3
	lda tmp1
	ldx tmp2
	jsr pushax
	lda #240
	jsr tosdiva0
	and #1
	beq y0
	lda tmp3
	ora #$02
	bne contout
y0:
	lda tmp3
contout:
	sta _mmc_ctrl

	rts
.endproc

; Allocate an unused sprite id
; sprite_t v_AllocSprite()
.proc _v_AllocSprite
	; first, find a cleared one

	ldy #0
loop:
	lda sprite_atable,y
	beq found

	iny
	cpy #NUM_SPRITES
	bne loop

	; didn't find one
	lda #0
	tax
	rts

found:
	lda #1
	sta sprite_atable,y
	tya

	; Convert to address
	asl
	asl
	ldx #$05

	rts
.endproc

; Free allocated sprite id
; void __fastcall__ v_FreeSprite(sprite_t sprite)
.proc _v_FreeSprite
	cpx #$05
	bne invalid
	; Convert to ID
	lsr
	lsr
	tay
	lda #0
	sta sprite_atable,y
invalid:
	rts
.endproc
