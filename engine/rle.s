.include "zeropage.inc"
.include "video.inc"

.import popax

.export _v_DecompressNT

; Adapted from RLE decompressor by Shiru (NESASM version)
; void __fastcall__ v_DecompressNT(int ppuAddr, void* ptr)
.proc _v_DecompressNT
	sta ptr1
	stx ptr1+1

	jsr popax
	ldy PPU_STATUS
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
	rts

rle_byte:
	lda (ptr1),y
	inc ptr1
	bne @done
	inc ptr1+1
@done:
	rts
.endproc