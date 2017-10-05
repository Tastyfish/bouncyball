.include "zeropage.inc"
.include "mem.inc"

.export _v_DecompressQREChunk
.import rle_byte

.segment "LOWCODE"

; Writes out list of mapload structs to dest. Will blindly overflow it
; void __fastcall__ v_DecompressQREChunk(void* dest, char quarter, const void* src)
.proc _v_DecompressQREChunk
	sta ptr1 ; src
	stx ptr1+1
	jsr popa
	.repeat 6
		asl a ; shift over to 00,40,80,C0 so we can do bit N/V tests
	.endrep
	sta tmp3 ; quarter

	jsr popax
	sta ptr2
	stx ptr2+1

	; x and y postition
	lda #0
	sta tmp1
	sta tmp2

	; continue as normal, until we've found that we passed X or Y barriers
	ldy #0
loop:
	jsr rle_byte
	cmp #0
	bmi rle_start
	sta sreg
	jsr wbyte
	beq loop
rle_start:
	and #$7F
	beq bigByte
	sta tmp4
	lda sreg
rle_loop:
	jsr wbyte
	dec tmp4
	bne rle_loop
	beq loop
bigByte: ; The special 0x80 [full 8-bit value] case
	jsr rle_byte
	cmp #0
	beq done
	sta sreg
	jsr wbyte
	beq loop
done: ; 0x80 0x00 means end of stream
	rts

; struct {
; char enitityID
; char x // relative to NT
; char y // relative to NT
; }
wbyte:
	pha
	cmp #0 ; id 0 means nothing here
	beq wbyte_c
	ldy #0
	sta (ptr2),y
	iny
	lda tmp1
	sta (ptr2),y
	iny
	lda tmp2
	sta (ptr2),y
	clc
	lda ptr2
	adc #3
	sta ptr2
	bcc wbyte_c
	inc ptr2+1
wbyte_c:

	inc tmp1 ; check for X scroll
	lda #$10
	cmp tmp1
	bne wbyte_done

	lda #0
	sta tmp1
	inc tmp2

wbyte_done:
	pla
	ldy #0
	rts
.endproc
