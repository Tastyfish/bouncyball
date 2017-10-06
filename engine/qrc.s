.include "zeropage.inc"
.include "mem.inc"

.import rle_byte
.export _v_DecompressQRCChunk

.segment "LOWCODE"

; Read QRC data into a bit array for col testing
; void __fastcall__ v_DecompressQRCChunk(void* dest, const void* src);
.proc _v_DecompressQRCChunk
	sta ptr1 ; src
	stx ptr1+1
	jsr popax
	sta ptr2 ; dest
	stx ptr2+1

	lda #1
	sta tmp3 ; current bit

	; continue as normal, until we've found that we passed X or Y barriers
	ldy #0
loop:
	jsr rle_byte
	cmp #0
	beq done
	sta tmp2 ; high bit is value, lower 7 is length
	and #$7F
	sta tmp1
	lda tmp2
	.repeat 7
		lsr
	.endrep
bloop:
	jsr wbyte
	dec tmp1
	bne bloop
	beq loop

done: ; 0x80 0x00 means end of stream
	rts

; write out a bit at a time
wbyte:
	pha
	cmp #0
	beq @clear
	lda (ptr2),y
	ora tmp3
	sta (ptr2),y
	bne @adv
@clear:
	lda tmp3
	eor #$FF
	and (ptr2),y
	sta (ptr2),y
@adv:
	asl tmp3 ; move mask to next bit
	bcc @done

	; onto next byte
	lda #1
	sta tmp3
	inc ptr2
	bne @done
	inc ptr2+1

@done:
	pla
	rts

.endproc
