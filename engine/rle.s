.include "zeropage.inc"
.include "video.inc"

.import popax, ppuMaskCache
.importzp ppuctrl

.export _vb_DecompressNT

; Adapted from RLE decompressor by Shiru (NESASM version)
; void __fastcall__ v_DecompressNT(int ppuAddr, void* ptr)
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

rle_byte:
	lda (ptr1),y
	inc ptr1
	bne @done
	inc ptr1+1
@done:
	rts
.endproc
