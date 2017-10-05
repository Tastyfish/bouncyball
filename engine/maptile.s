.include "zeropage.inc"
.include "mem.inc"
.include "video.inc"

.import aslax1, aslax4, tosmoda0, tosicmp0
.import _map_refX, _map_refY, _map_orientation
.import ppubuf_put

.segment "LOWCODE"

.export _map_SetTile

; If we ever add bounds checking (a bit expensive for each tile)
; ; bounds is [refx - 256..refx + 256, refy / 240 * 240..+240]
; ; left
; ldy #2
; lda (sp),y
; tax
; iny
; lda (sp),y
; sta ptr1
; stx ptr1+1
; jsr pushax
; lda _map_refX
; ldx _map_refX+1
; dex ; - 256
; jsr tosicmp
; bmi abort

; right
; lda ptr1
; ldx ptr1+1
; jsr pushax
; lda _map_refX
; ldx _map_refX+1
; inx ; + 256
; jsr tosicmp
; bpl abort


; Set tile in map
; Auto fails if out of bounds of loaded map
; void __fastcall__ map_SetTile(int x, int y, maptile_t tile)
.proc _map_SetTile
	sta tmp4 ; tile

	lda _map_orientation
	beq vertical
	; horizontal

	; (x % 64 < 32 ? 0x2000 : 0x2400) + x % 32 + y % 30 * 32
	ldy #2
	lda (sp),y
	and #63
	cmp #32
	bpl @rightnt
	lda #$20
	bne @ntcont
@rightnt:
	lda #$24
@ntcont:
	sta ptr2+1
	lda #0
	sta ptr2
	beq done

vertical:
	; (y % 60 < 30 ? 0x2000 : 0x2400) + x % 32 + y % 30 * 32
	ldy #1
	lda (sp),y
	tax
	dey
	lda (sp),y
	jsr pushax
	lda #60
	jsr tosmoda0
	cmp #30
	bpl @rightnt
	lda #$20
	bne @ntcont
@rightnt:
	lda #$24
@ntcont:
	sta ptr2+1
	lda #0
	sta ptr2

done:
	; continue the + x % 32 + y % 30 * 32
	lda #30
	jsr tosmoda0
	jsr aslax4
	jsr aslax1
	clc
	adc ptr2
	sta ptr2
	txa
	adc ptr2+1
	sta ptr2+1

	jsr popax
	ldx #0
	and #31
	clc
	adc ptr2
	sta ptr2
	txa
	adc ptr2+1

	tay
	ldx ptr2
	lda tmp4
	jmp ppubuf_put
abort:
	jmp incsp4
.endproc
