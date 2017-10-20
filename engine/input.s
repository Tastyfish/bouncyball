.include "zeropage.inc"

.export _i_GetStandardInput

.segment "LOWCODE"

; Get input from player 0 or 1
; byte __fastcall__ i_GetStandardInput(byte player)
.proc _i_GetStandardInput
	; player is lower byte of $4016 or $4017

	sta ptr1
	lda #$40
	sta ptr1+1

	; Reset controller
	ldy #0
	lda #1
	sta (ptr1),y
	tya
	sta (ptr1),y

	; X is bit #, tmp1 is returned flags
	sta tmp1
	ldx #8
loop:
	lda (ptr1),y
	ror
	rol tmp1
	dex
	bne loop

	lda tmp1
	rts
.endproc
