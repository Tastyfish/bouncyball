.include "zeropage.inc"
.include "mmc5zp.inc"
.include "video.inc"

.import popax, popa, ppuMaskCache, ppubuf_put

.export _vb_DecompressNT
.export _v_DecompressToRAM
.export _v_DecompressQLEChunk

; Adapted from RLE decompressor by Shiru (NESASM version)
; void __fastcall__ vb_DecompressNT(int ppuAddr, void* src)
.proc _vb_DecompressNT
	sta ptr1
	stx ptr1+1

	; This runs so long, disable everything
	lda #0
	sta PPU_MASK

	jsr popax
	bit PPU_STATUS
	stx PPU_ADDR
	sta PPU_ADDR

	ldy #0
	jsr rle_byte
	sta sreg
loop:
	jsr rle_byte
	cmp sreg
	beq rle_start
	sta PPU_DATA
	sta sreg+1
	bne loop
rle_start:
	jsr rle_byte
	cmp #0
	beq done
	tax
	lda sreg+1
rle_loop:
	sta PPU_DATA
	dex
	bne rle_loop
	beq loop
done:
	lda ppuMaskCache
	sta PPU_MASK
	rts
.endproc

; void __fastcall__ v_DecompressToRAM(void* dest, void* src)
.proc _v_DecompressToRAM
	sta ptr1
	stx ptr1+1

	jsr popax
	sta ptr2
	stx ptr2+1

	ldy #0
	jsr rle_byte
	sta sreg
loop:
	jsr rle_byte
	cmp sreg
	beq rle_start
	sta (ptr2),y
	inc ptr2
	bne wcont
	inc ptr2+1
wcont:
	sta sreg+1
	bne loop
	rle_start:
	jsr rle_byte
	cmp #0
	beq done
	tax
	lda sreg+1
rle_loop:
	sta (ptr2),y
	inc ptr2
	bne wcont2
	inc ptr2+1
wcont2:
	dex
	bne rle_loop
	beq loop
done:
	rts
.endproc

.proc rle_byte
	lda (ptr1),y
	inc ptr1
	bne done
	inc ptr1+1
done:
	rts
.endproc

; Load a QLE chunk into a quarter of a nametable
; Quarters are in LTR reading order
; src is a chunk, not a pointer to the header
; Reiteratively adapted from above
; void __fastcall__ v_DecompressQLEChunk(int ppuAddr. char quarter, void* src)
.proc _v_DecompressQLEChunk
	sta ptr1 ; src
	stx ptr1+1
	jsr popa
	.repeat 6
		asl a ; shift over to 00,40,80,C0 so we can do bit N/V tests
	.endrep
	sta tmp3 ; quarter

	jsr popax
	; adjust into starting position for the given quarter
	bit tmp3
	bpl testQX
	.repeat 2
		inx
	.endrep
testQX:
	stx ptr2+1
	bit tmp3
	bvc contQX
	adc #$10
contQX:
	sta ptr2

	; x and y postition
	lda #0
	sta tmp1
	sta tmp2

	; continue as normal, until we've found that we passed X or Y barriers
	ldy #0
	jsr rle_byte
	sta sreg
loop:
	jsr rle_byte
	cmp sreg
	beq rle_start
	sta sreg+1
	jsr wbyte
	beq loop
rle_start:
	jsr rle_byte
	cmp #0
	beq done
	sta tmp4
	lda sreg+1
rle_loop:
	jsr wbyte
	dec tmp4
	bne rle_loop
	beq loop
done:
	rts

wbyte:
	pha
	ldx ptr2
	ldy ptr2+1
	jsr ppubuf_put
	inc ptr2
	bne wbyte_c
	inc ptr2+1
wbyte_c:

	; check if in attrib part
	lda tmp3
	cmp #$FF
	beq wbyte_attrib

	inc tmp1 ; check for X scroll
	lda #$10
	cmp tmp1
	bne wbyte_done

	clc
	lda ptr2
	adc #16
	sta ptr2
	bcc wbyte_xsetc
	inc ptr2+1
wbyte_xsetc:
	lda #0
	sta tmp1

	inc tmp2
	lda tmp2
	bit tmp3
	bmi testy_short_section
	cmp #$10
	bne wbyte_done
	beq wbyte_yset
testy_short_section:
	cmp #$0E
	bne wbyte_done

	; transfer start address & quarter into attrib address
	; (base & 0xFC00) + 32*30 + q*60

wbyte_yset:
	clc
	lda #<(32*30)
	sta ptr2
	lda ptr2+1
	and #$FC
	adc #>(32*30)
	sta ptr2+1
	bit tmp3
	bpl wbyte_y_checkx
	; Y offset
	clc
	lda ptr2
	adc #$20
	sta ptr2
wbyte_y_checkx:
	bit tmp3
	bvc wbyte_yloop_done
	; X offset
	lda ptr2
	adc #$04
	sta ptr2
	bcc wbyte_yloop_done
	inc ptr2+1
wbyte_yloop_done:
	lda #$FF
	sta tmp3 ; represent that we're in attribs now

wbyte_done:
	pla
	ldy #0
	rts

wbyte_attrib:
	inc tmp1 ; check for X scroll
	lda #$04
	cmp tmp1
	bne wbyte_done

	clc
	lda ptr2
	adc #$04
	sta ptr2
	bcc wbyte_attrib_setc
	inc ptr2+1
wbyte_attrib_setc:
	lda #0
	sta tmp1
	beq wbyte_done

.endproc
